import std;

import core;
import scene;
import camera;
import primitives;
import bsdf;
import materials;
import stb;

constexpr auto image_width        = 900;
constexpr auto image_height       = 720;
constexpr auto samples_per_pixel  = 512;
constexpr auto max_depth          = 20;
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
  Vector3f wo = frame.to_local(-r.d); // in local shading frame

  BSDFSample s = std::visit([&](const auto& m) { return sample_material(m, wo, rng); }, hit->mat);
  if (!s.valid)
    return Color3f{0.f};

  auto wi = s.wi;
  auto cos_i = s.cos_i;
  auto f = s.f;
  auto pdf = s.pdf;

  Vector3f wi_world = frame.to_world(wi);
  f32 side = dot(wi_world, hit->n) > 0.f ? 1.f : -1.f;
  Point3f p_offset = hit->p + hit->n * (side * 1e-4f);
  Ray incoming_ray{p_offset, wi_world};

  Color3f Li = compute_shading(incoming_ray, scene, rng, depth - 1);
  // Monte Carlo Estimator. General estimator with any PDF formula:
  // F_n = 1/n sum_i^n f(X_i)/p(X_i)
  Color3f Lo = (f * cos_i * Li) / pdf;
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

void render_to_png(Camera& cam, std::string_view filename, i32 image_width, i32 image_height)
{
  constexpr auto channels = 3;

  std::vector<u8> pixels(image_width * image_height * channels);

  auto to_byte = [](f32 value) -> u8 {
    value = std::clamp(value, 0.f, 1.f);
    return static_cast<u8>(255.f * value);
  };

  for (i32 j = 0; j < image_height; ++j)
  {
    for (i32 i = 0; i < image_width; ++i)
    {
      Color3f color = clamp(cam.film.at(i, j), 0.f, 1.f);
      usize index = (j * image_width + i) * channels;
      pixels[index + 0] = to_byte(color.r);
      pixels[index + 1] = to_byte(color.g);
      pixels[index + 2] = to_byte(color.b);
    }
  }

  if (!stb::save_image(filename, pixels, image_width, image_height))
    throw std::runtime_error("Failed to save PNG image");
}

i32 main()
{
  Camera camera{
    Point3f{0.f, 4.f, -10.f},   // position
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
    Diffuse{ .albedo = Color3f{0.75f} }
  });

  scene.add(Sphere{
    Point3f{-3.f, 0.f, 4.f}, 1.0f,
    Diffuse{ .albedo = Color3f{0.75f, 0.15f, 0.15} }
  });
  scene.add(Sphere{
    Point3f{0.f, 0.f, 4.f}, 1.0f,
    Conductor{
      .eta = Color3f{0.155, 0.116, 0.138}, // polished silver
      .k   = Color3f{4.83, 3.99, 2.48},
      .alpha_x = 0.0001f, .alpha_y = 0.0001f
    }
  });
  scene.add(Sphere{
    Point3f{3.f, 0.f, 4.f}, 1.0f,
    Conductor{
      .eta = Color3f{0.155, 0.116, 0.138}, // polished silver
      .k   = Color3f{4.83, 3.99, 2.48},
      .alpha_x = 0.05f, .alpha_y = 0.20f
    }
  });
  scene.add(Sphere{
    Point3f{-3.f, 0.f, 0.f}, 1.0f,
    Dielectric{
      .eta = 1.5,
      .alpha_x = 0.0001f, .alpha_y = 0.0001f
    }
  });
  scene.add(Sphere{
    Point3f{0.f, 0.f, 0.f}, 1.0f,
    Dielectric{
      .eta = 1.5,
      .alpha_x = 0.1f, .alpha_y = 0.1f
    }
  });

  //   scene.add(Sphere{
  //     Point3f{-3.f, 0.f, 0.f}, 1.0f,
  //     Dielectric{
  //       .eta = 1.5,
  //       .alpha_x = 0.0001f, .alpha_y = 0.0001f
  //     }
  //   });
  //
  //   scene.add(Sphere{
  //     Point3f{0.f, 0.f, 0.f}, 1.0f,
  //     Dielectric{
  //       .eta = 2.42f,
  //       .alpha_x = 0.0001f, .alpha_y = 0.0001f
  //     }
  //   });

  // scene.add(Sphere{
  //   Point3f{-3.f, 0.f, 0.f}, 1.0f,
  //   Conductor{
  //     .eta = Color3f{0.155, 0.116, 0.138}, // polished silver
  //     .k   = Color3f{4.83, 3.99, 2.48},
  //     .alpha_x = 0.0001f, .alpha_y = 0.0001f
  //   }
  // });
  // scene.add(Sphere{
  //   Point3f{0.f, 0.f, 0.f}, 1.0f,
  //   Conductor{
  //     .eta = Color3f{0.155, 0.116, 0.138}, // anisotropic brushed metal.
  //     .k   = Color3f{4.83, 3.99, 2.48},
  //     .alpha_x = 0.05f, .alpha_y = 0.15f
  //   }
  // });
  // scene.add(Sphere{
  //   Point3f{3.f, 0.f, 0.f}, 1.0f,
  //   Conductor{
  //     .eta = Color3f{0.155, 0.116, 0.138}, // clearly rough gold.
  //     .k   = Color3f{4.83, 3.99, 2.48},
  //     .alpha_x = 0.1f, .alpha_y = 0.1f
  //   }
  // });


  // auto cube = Cube{
  //   Point3f{3.f, 0.f, 4.f},
  //   Conductor{
  //     .eta = Color3f{2.91, 2.95, 2.88},
  //     .k = Color3f{3.07, 2.88, 2.74},
  //     .alpha_x = 0.02f, .alpha_y = 0.02f,
  //   }
  // };
  // cube.set_transform(translate(Point3f{3.f, 0.f, 4.f}) * rotate_y(45.f) * scale(Vector3f{2.f, 2.f, 2.f}));
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
  render_to_png(camera, "output.png", image_width, image_height);
  return 0;
}
