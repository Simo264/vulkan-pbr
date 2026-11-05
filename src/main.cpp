import std;

import core;
import scene;
import camera;
import gltf_loader;
import primitives;
import materials;

struct ShadingFrame
{
  Vector3f s, t, n;

  static ShadingFrame from_normal(const Vector3f& n)
  {
    ShadingFrame frame;
    frame.n = n;

    f32 sign = std::copysign(1.f, n.z);
    f32 a = -1.f / (sign + n.z);
    f32 b = n.x * n.y * a;
    frame.s = Vector3f{ 1.f + sign * n.x * n.x * a, sign * b, -sign * n.x };
    frame.t = Vector3f{ b, sign + n.y * n.y * a, -n.y };
    return frame;
  }

  // world to local
  Vector3f to_local(const Vector3f& x) const { return { dot(x, s), dot(x, t), dot(x, n) }; }
  // local to world
  Vector3f to_world(const Vector3f& x) const { return x.x * s + x.y * t + x.z * n; }
};

Color3f ray_color(const Ray&       r,
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
  ShadingFrame frame = ShadingFrame::from_normal(hit->n);
  Vector3f wo_local = frame.to_local(-r.d);

  Vector3f wi_local{};
  f32 cos_theta{0.f};
  Color3f brdf{0.f};
  f32 pdf{0.f};

  std::visit([&](const auto& mat) {
    using T = std::decay_t<decltype(mat)>;
    if constexpr (std::is_same_v<T, Diffuse>)
    {
      wi_local = sample_cosine_hemisphere(rng);
      cos_theta = wi_local.z;

      brdf = mat.albedo / PI;   // f_r(p,w_i,w_o) = R/pi
      pdf  = cos_theta / PI;    // p(w)           = costheta / pi
    }
  }, hit->mat);

  if (cos_theta <= 0.f)
    return Color3f{0.f};

  Vector3f wi_world = frame.to_world(wi_local);
  Point3f p_offset = hit->p + hit->n * 1e-4f;
  Ray incoming_ray{p_offset, wi_world};

  Color3f Li = ray_color(incoming_ray, scene, rng, depth - 1);
  // Monte Carlo Estimator
  Color3f Lo = (brdf * cos_theta * Li) / pdf;
  return Lo;
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
  constexpr auto image_width = 900;
  constexpr auto image_height = 720;

  Camera camera{
    Point3f{0.f, 2.f, -6.f},   // position
    Point3f{0.f, 0.f, 5.f},    // look at
    60.f,                      // FOV in degrees,
    image_width, image_height
  };

  // ==============================
  // Set up scene
  // ==============================

  Scene scene;
  scene.add(Disk{
    Point3f{0.f, -1.f, 0.f}, Vector3f{0.f, 1.f, 0.f}, 5.f, Diffuse{Color3f{0.9f}}
  });
  scene.add(Sphere{
    Point3f{0.f, 0.f, 0.f}, 1.0f, Diffuse{Color3f{0.7, 0.2f, 0.2f}}
  });
  scene.add(Sphere{
    Point3f{-3.f, 0.f, 0.f}, 1.0f, Diffuse{Color3f{0.7, 0.2f, 0.2f}}
  });

  auto cube = Cube{
    Point3f{3.f, 0.f, 0.f}, Diffuse{Color3f{0.3f, 0.7f, 0.9f}}
  };
  cube.set_transform(translate(Point3f{3.f, 0.f, 0.f}) * rotate_y(45.f) * scale(Vector3f{2.f, 2.f, 2.f}));
  scene.add(cube);

  // ==============================
  // Prepare for rendering
  // ==============================

  constexpr auto samples_per_pixel  = 32;
  constexpr auto max_depth          = 5;
  constexpr auto num_threads        = 8;
  constexpr auto tile_size          = 32; // 32x32
  constexpr auto tiles_x = (image_width + tile_size - 1) / tile_size;
  constexpr auto tiles_y = (image_height + tile_size - 1) / tile_size;
  constexpr auto n_tiles = tiles_x * tiles_y;
  auto render_tile = [&](i32 tile_idx, Camera& cam, RandomGenerator& rng) {
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
          c += ray_color(r, scene, rng, max_depth);
        }
        c = c / static_cast<f32>(samples_per_pixel);
        cam.film.at(i, j) = c;
      }
    }
  };

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

        render_tile(tile_idx, camera, rng);
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
