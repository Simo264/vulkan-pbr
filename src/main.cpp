import std;

import core;
import scene;
import camera;
import primitives;
import bsdf;
import sampling;
import materials;

constexpr auto image_width        = 900;
constexpr auto image_height       = 720;
constexpr auto samples_per_pixel  = 512;
constexpr auto max_depth          = 50;
constexpr auto num_threads        = 8;
constexpr auto tile_size          = 32; // 32x32
constexpr auto tiles_x = (image_width + tile_size - 1) / tile_size;
constexpr auto tiles_y = (image_height + tile_size - 1) / tile_size;
constexpr auto n_tiles = tiles_x * tiles_y;


Color3f compute_shading(const Ray&       r,
                        const Scene&     scene,
                        RandomGenerator& rng,
                        i32              depth)
{
  static auto sky_color = [](Vector3f d){ f32 t = 0.5f * (d.y + 1.f); return lerp(Color3f{1.f, 1.f, 1.f}, Color3f{0.5f, 0.7f, 1.f}, t); };

  if (depth <= 0) return Color3f{0.f};

  auto hit = scene.intersect(r);
  if (!hit)
    return sky_color(r.d);

  // Build the shading frame: an orthonormal basis (s, t, n) centered at the hit point with n = surface normal.
  // All BSDF math happens in this local space, where the normal is the +z axis.
  ShadingFrame frame{hit->n};

  Vector3f wo       = frame.to_local(-r.d); // in local shading frame
  Vector3f wi       = Vector3f{0.f};        // in local shading frame
  Color3f brdf      = Color3f{0.f};
  f32 pdf           = 0.f;
  f32 cos_theta_i   = 0.f;                  // cosine between surface normal and the incident beam

  std::visit([&](const auto& mat) {
    using T = std::decay_t<decltype(mat)>;
    if constexpr (std::is_same_v<T, Diffuse>)
    {
      wi = sample_cosine_hemisphere(rng);
      if(!same_hemisphere(wo, wi)) return;

      cos_theta_i = wi.z;
      if (cos_theta_i <= 0.f) return;

      brdf = mat.albedo / PI;   // f_r(p,w_i,w_o) = R/pi
      pdf  = cos_theta_i / PI;  // p(w)           = costheta / pi
    }
    else if constexpr (std::is_same_v<T, Conductor>)
    {
      TrowbridgeReitzDistribution distrib{ mat.alpha_x, mat.alpha_y };

      // handle smooth case
      if(distrib.effectively_smooth())
      {
        wi = reflect(wo, Vector3f{0.f, 0.f, 1.f}); // specular direction
        if(!same_hemisphere(wo, wi)) return;

        cos_theta_i = std::abs(wi.z);
        if(cos_theta_i <= 0.f) return; // fail

        Color3f F = fresnel_conductor(cos_theta_i, mat.eta, mat.k);
        brdf = F / cos_theta_i;   // delta BRDF
        pdf  = 1.f;               // delta convention
      }
      // handle roughness
      else
      {
        Vector3f wm = distrib.sample_wm(wo, rng);
        wi = reflect(wo, wm);
        if(!same_hemisphere(wo, wi)) return;

        f32 cos_i = std::abs(wi.z);
        f32 cos_o = std::abs(wo.z);
        if(cos_i <= 0.f || cos_o <= 0.f) return;

        f32 abs_dot = std::abs(dot(wo ,wm));
        Color3f F = fresnel_conductor(abs_dot, mat.eta, mat.k);
        brdf = distrib.D(wm) * F * distrib.G(wo, wi) / (4 * cos_i * cos_o);
        pdf  = distrib.D(wo, wm) / (4 * abs_dot);
        cos_theta_i = cos_i;
      }
    }
    else if constexpr (std::is_same_v<T, Dielectric>)
    {
      constexpr auto IOR_air = 1.0f;

      // the beam comes from outside? wo.z > 0
      // the beam comes from inside? wo.z < 0
      bool entering = wo.z > 0.f;
      f32 eta_i = entering ? IOR_air : mat.eta; // IOR of the reflected medium.
      f32 eta_t = entering ? mat.eta : IOR_air; // IOR of the transmitted medium.
      f32 eta_rel = eta_t / eta_i;
      f32 cos_i = std::abs(wo.z);

      f32 Fr = fresnel_dielectric(cos_i, eta_t, eta_i);
      f32 T  = 1.f - Fr;

      // Fresnel-based sampling
      bool do_reflect = (rng.uniform() < Fr) || (Fr >= 1.f);
      if (do_reflect) // handle reflection
      {
        wi = reflect(wo, Vector3f{0.f, 0.f, 1.f}); // specular direction
        cos_theta_i = std::abs(wi.z);
        brdf = Color3f{Fr / cos_theta_i};
        pdf  = Fr; // = 1 in case of total internal reflection
      }
      else // handle refraction
      {
        auto refr = refract(wo, Vector3f{0.f, 0.f, 1.f}, eta_rel);
        if (!refr) return; // Total internal reflection

        wi = *refr;
        cos_theta_i = std::abs(wi.z);
        f32 eta_ratio_sq = (eta_i / eta_t) * (eta_i / eta_t);
        brdf = Color3f{(T * eta_ratio_sq) / cos_theta_i};
        pdf  = T;
      }
    }

  }, hit->mat);

  if (pdf <= 0.f || cos_theta_i <= 0.f) return Color3f{0.f}; // safety

  Vector3f wi_world = frame.to_world(wi);
  Point3f  p_offset = hit->p + wi_world * 1e-4f;
  Ray incoming_ray{p_offset, wi_world};

  Color3f Li = compute_shading(incoming_ray, scene, rng, depth - 1);
  // Monte Carlo Estimator. General estimator with any PDF formula:
  // F_n = 1/n sum_i^n f(X_i)/p(X_i)
  Color3f Lo = (brdf * cos_theta_i * Li) / pdf;
  return Lo;
}

void render_tile(i32 tile_idx, const Scene& scene, Camera& cam, RandomGenerator& rng)
{
  auto tx = tile_idx % tiles_x;
  auto ty = tile_idx / tiles_x;
  auto x0 = tx * tile_size;
  auto y0 = ty * tile_size;
  auto x1 = std::min(x0 + tile_size, image_width); // clamp for last tile
  auto y1 = std::min(y0 + tile_size, image_height);
  for (auto j = y0; j < y1; ++j)
  {
    for (auto i = x0; i < x1; ++i)
    {
      Color3f c{0.f};
      for (auto s = 0; s < samples_per_pixel; ++s)
      {
        Ray r = cam.generate_ray(static_cast<f32>(i), static_cast<f32>(j), rng.uniform(), rng.uniform());
        c += compute_shading(r, scene, rng, max_depth);
      }
      c = c / static_cast<f32>(samples_per_pixel);
      cam.film.at(i, j) = c;
    }
  }
}

void save_image(Camera& cam, std::string_view filename, i32 image_width, i32 image_height)
{
  std::ofstream out{filename.data(), std::ios::binary};

  out << "P6\n" << image_width << ' ' << image_height << "\n255\n";
  auto to255 = [](f32 v) -> byte {
    v = std::clamp(v, 0.f, 1.f);
    return static_cast<byte>(255.f * v);
  };

  for (auto j = 0; j < image_height; ++j)
  {
    for (auto i = 0; i < image_width; ++i)
    {
      Color3f c = clamp(cam.film.at(i, j), 0.f, 1.f);
      out.put(static_cast<char>(to255(c.r)));
      out.put(static_cast<char>(to255(c.g)));
      out.put(static_cast<char>(to255(c.b)));
    }
  }
  std::println("Saved image: {} ({}x{})", filename, image_width, image_height);
}

i32 main()
{
  Camera camera{
    Point3f{0.f, 5.f, -9.f},   // position
    Point3f{0.f, 0.f, 5.f},    // look at
    35.f,                      // FOV in degrees,
    image_width, image_height
  };

  // ==============================
  // Set up scene
  // ==============================

  Scene scene;
  scene.add(Disk{
    Point3f{0.f, -1.f, 0.f}, Vector3f{0.f, 1.f, 0.f}, 10.f,
    Diffuse{.albedo = Color3f{0.9f}}
  });

  // conductors

  scene.add(Sphere{
    Point3f{-3.f, 0.f, 0.f}, 1.0f,
    Conductor{
      .eta = Color3f{0.155, 0.116, 0.138}, // polished silver
      .k   = Color3f{4.83, 3.99, 2.48},
      .alpha_x = 0.0001f, .alpha_y = 0.0001f
    }
  });
  scene.add(Sphere{
    Point3f{0.f, 0.f, 0.f}, 1.0f,
    Conductor{
      .eta = Color3f{0.155, 0.116, 0.138}, // anisotropic brushed metal.
      .k   = Color3f{4.83, 3.99, 2.48},
      .alpha_x = 0.05f, .alpha_y = 0.15f
    }
  });
  scene.add(Sphere{
    Point3f{3.f, 0.f, 0.f}, 1.0f,
    Conductor{
      .eta = Color3f{0.155, 0.116, 0.138}, // clearly rough gold.
      .k   = Color3f{4.83, 3.99, 2.48},
      .alpha_x = 0.1f, .alpha_y = 0.1f
    }
  });

  // dielectrics

  scene.add(Sphere{
    Point3f{-3.f, 0.f, 4.f}, 1.0f,
    Dielectric{
      .eta = 1.5f // clear glass
    }
  });
  scene.add(Sphere{
    Point3f{0.f, 0.f, 4.f}, 1.0f,
    Dielectric{
      .eta = 1.77f // spahhire
    }
  });
  scene.add(Sphere{
    Point3f{3.f, 0.f, 4.f}, 1.0f,
    Dielectric{
      .eta = 2.417f // Diamond
    }
  });


  // auto cube = Cube{
  //   Point3f{3.f, 0.f, 0.f},
  //   Conductor{.eta = Color3f{0.14f, 0.15f, 0.18f}, .k = Color3f{3.19f, 3.30f, 3.50f}} // silver metal
  // };
  // cube.set_transform(translate(Point3f{3.f, 0.f, 0.f}) * rotate_y(45.f) * scale(Vector3f{2.f, 2.f, 2.f}));
  // scene.add(cube);

  // ==============================
  // Prepare for rendering
  // ==============================

  auto t_start = std::chrono::steady_clock::now();
  std::atomic<i32> tiles_done{0};
  std::atomic<i32> next_tile{0};
  std::atomic<i32> last_pct{-1};
  std::mutex print_mutex;
  auto report_progress = [&]() {
    auto done = tiles_done.load(std::memory_order_relaxed);
    auto pct  = 100 * done / n_tiles;

    if (pct == last_pct.load(std::memory_order_relaxed) && done != n_tiles)
      return;

    std::lock_guard lock(print_mutex);
    if (pct == last_pct.load(std::memory_order_relaxed) && done != n_tiles)
      return;
    last_pct.store(pct, std::memory_order_relaxed);

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<f64>(now - t_start).count();
    auto eta = (done > 0) ? elapsed * (n_tiles - done) / done : 0.0;
    std::println("[{:>3}%] {}/{} tiles  {:.1f}s elapsed, {:.1f}s ETA", pct, done, n_tiles, elapsed, eta);
  };

  std::vector<std::jthread> workers;
  workers.reserve(num_threads);
  for (auto t = 0; t < num_threads; ++t)
  {
    workers.emplace_back([&, t](std::stop_token st) {
      RandomGenerator rng{ 12345u + static_cast<u32>(t) * 1000u };
      while (!st.stop_requested())
      {
        auto tile_idx = next_tile.fetch_add(1, std::memory_order_relaxed);
        if (tile_idx >= n_tiles)
          break;

        render_tile(tile_idx, scene, camera, rng);
        tiles_done.fetch_add(1, std::memory_order_relaxed);
        report_progress();
      }
    });
  }

  for (auto& w : workers)
    w.join();

  auto t_end = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration<f64>(t_end - t_start).count();
  std::println("Rendered in {:.2f} s", elapsed);
  save_image(camera, "output.ppm", image_width, image_height);
  return 0;
}
