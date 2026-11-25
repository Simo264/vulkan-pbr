import std;

import core;
import scene;
import camera;
import primitive;
import bsdf;
import material;
import hit_record;
import light_source;
import image;
import stb;

constexpr auto image_output       = "output.png";
constexpr auto image_width        = 900;
constexpr auto image_height       = 720;
constexpr auto samples_per_pixel  = 32;
constexpr auto max_depth          = 5;
constexpr auto num_threads        = 10;
constexpr auto tile_size          = 32; // 32x32
constexpr auto tiles_x            = (image_width + tile_size - 1) / tile_size;
constexpr auto tiles_y            = (image_height + tile_size - 1) / tile_size;
constexpr auto n_tiles            = tiles_x * tiles_y;

bool is_occluded(const Scene& scene, Point3f p, Vector3f n, Vector3f wi_world, f32 distance)
{
  f32 side = std::copysign(1.f, dot(wi_world, n));
  Point3f origin = p + n * (side * 1e-3f);
  f32 t_max_shadow = distance * (1.f - 1e-3f);   // 0.1 % short of `distance`

  Ray shadow_ray{origin, wi_world, t_max_shadow};

  auto hit = scene.intersect(shadow_ray);
  if (hit) return true; // in shadow
  return false;
}

Color3f direct_illumination(const Scene& scene, const LightSample& ls, const HitRecord& hit, const TangentFrame& frame, const Vector3f& wo)
{
  if (ls.pdf == 0.f)          return Color3f{0.f}; // invalid / unimplemented stub
  if (ls.L   == Color3f{0.f}) return Color3f{0.f}; // black light

  Vector3f wi = frame.to_local(ls.wi);   // world -> local
  Color3f bsdf = std::visit([&](const auto& m) { return eval_material(m, wo, wi); }, hit.mat);
  if(bsdf == Color3f{0.f}) return Color3f{0.f};

  if(is_occluded(scene, hit.p, hit.n, ls.wi, ls.dist))
     return Color3f{0.f};

  // Monte Carlo Estimator. General estimator with any PDF formula:
  // F_n = 1/n sum_i^n f(X_i)/p(X_i)
  f32 cos_i = std::abs(wi.z);
  Color3f Li = ls.L;
  f32 pdf = ls.pdf;
  return (bsdf * cos_i * Li) / pdf;
}

Color3f compute_shading(Ray r, const Scene& scene, RandomGenerator& rng, i32 depth)
{
  if (depth <= 0) return Color3f{0.f};

  auto hit = scene.intersect(r);
  if (!hit)
  {
    //static auto sky_color = [](Vector3f d){ f32 t = 0.5f * (d.y + 1.f); return lerp(Color3f{1.f, 1.f, 1.f}, Color3f{0.5f, 0.7f, 1.f}, t); };
    //return sky_color(r.d);

    if (const auto& env = scene.environment_light())
    {
      auto light = env.value();
      auto emission = std::visit([&](const auto& l) { return L(l, r.o, Vector3f{0,0,1}, r.d); }, light);
      return emission;
    }
    return Color3f{0.f};
  }

  // Build the shading frame: an orthonormal basis (s, t, n) centered at the hit point with n = surface normal.
  // All BSDF math happens in this local space, where the normal is the +z axis.
  TangentFrame frame{hit->n};
  Vector3f wo = frame.to_local(-r.d); // in local shading frame

  Color3f Lo_direct = Color3f{0.f};
  Color3f Lo_indirect = Color3f{0.f};

  // Explicit light sampling: point and area lights
  for(const auto& light : scene.lights())
  {
    LightSample ls = std::visit([&](const auto& l) { return sample_Li(l, hit->p, rng); }, light); // wi, Li, pdf, distance
    Lo_direct += direct_illumination(scene, ls, *hit, frame, wo);
  }

  // Continue the path with one BSDF sample
  BSDFSample bs = std::visit([&](const auto& m) { return sample_material(m, wo, rng); }, hit->mat); // wi, f, pdf, cos_i
  if(bs.valid && bs.pdf > 0.f && bs.f != Color3f{0.f})
  {
    Vector3f wi_world = frame.to_world(bs.wi);
    f32 side = std::copysign(1.f, dot(wi_world, hit->n));
    Point3f origin = hit->p + hit->n * (side * 1e-3f);
    Ray new_ray{origin, wi_world, MAX_FLOAT_32};
    Color3f Li = compute_shading(new_ray, scene, rng, depth - 1);
    Lo_indirect += (bs.f * bs.cos_i * Li) / bs.pdf;
  }

  return hit->Le + Lo_direct + Lo_indirect;
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

void render_to_image(Camera& cam)
{
  auto filename = ::image_output; // global constant
  auto width    = ::image_width;  // global constant
  auto height   = ::image_height; // global constant
  constexpr auto channels = 3;

  std::vector<u8> pixels(width * height * channels);

  auto to_byte = [](f32 value) -> u8 {
    value = std::clamp(value, 0.f, 1.f);
    return static_cast<u8>(255.f * value);
  };

  for (i32 j = 0; j < height; ++j)
  {
    for (i32 i = 0; i < width; ++i)
    {
      Color3f color = clamp(cam.film.at(i, j), 0.f, 1.f);
      i32 index = (j * width + i) * channels;
      pixels[index + 0] = to_byte(color.r);
      pixels[index + 1] = to_byte(color.g);
      pixels[index + 2] = to_byte(color.b);
    }
  }

  ImageLDR image{
    .pixels   = std::move(pixels),
    .width    = width,
    .height   = height,
    .channels = channels
  };

  if (!stb::save_image_rgb8(filename, image))
    throw std::runtime_error("Failed to save PNG image");
}

i32 main()
{
  Camera camera{
    Point3f{0.f, 5.f, -10.f}, // position
    Point3f{0.f, 2.f, 0.f},   // look at
    50.f,                     // FOV in degrees,
    image_width, image_height
  };

  std::shared_ptr<ImageHDR> hdr_image;
  if(auto res = stb::load_image_rgb32("hdr/meadow.hdr"))
  {
    ImageHDR img = res.value();
    std::println("Loaded HDR environment map: {}x{} pixels, {} channels", img.width, img.height, img.channels);
    hdr_image = std::make_shared<ImageHDR>(std::move(img));
  }
  else
    throw std::runtime_error("Failed to load HDR environment map");

  // ==============================
  // Set up scene
  // ==============================

  Disk disk_emitter{
    Point3f{0.f, 4.5f, 0.f},      // center position
    Vector3f{0.f, -1.f, 0.f},      // normal vector
    1.f,                          // radius
    Diffuse{Color3f{0.f}},         // material: zero albedo -> no reflection
    Color3f{1.f} * 8.f,           // Le (white)
    true                           // two_sided
  };

  Sphere sphere_emitter{
    Point3f{ 0.f, 4.f, 0.f },          // center
    0.75f,                              // radius
    Diffuse{Color3f{0.f}},             // material: zero albedo -> pure emitter
    Color3f{1.0f} * 8,                  // Le
    true                               // two_sided
  };

  PointLight point_light{
    Point3f{0.0f, 4.f, 0.f}, // position
    30.f                     // intensity
  };

  UniformInfiniteLight uniform_env_map{
    Color3f{0.5},    // Le ambient light
    50.f                          // Radius of the sphere that surrounds the scene
  };

  ImageInfiniteLight image_env_map{
    .radiance_map = hdr_image,  // HDR environment map
    .r = 50.f                 // Radius of the sphere that surrounds the scene
  };

  Scene scene;

  //scene.add_light(point_light);
  //scene.add_light(DiffuseAreaLight{disk_emitter});
  //scene.add_light(DiffuseAreaLight{sphere_emitter});
  //scene.set_environment(uniform_env_map);
  scene.set_environment(image_env_map);

  scene.add_object(Disk{
    Point3f{0.f, -1.f, 0.f},
    Vector3f{0.f, 1.f, 0.f},
    15.f,
    Diffuse{.albedo = Color3f{0.8f, 0.9f, 1.f}}
  });
  scene.add_object(Sphere{
    Point3f{3.f, 0.f, 3.f},
    1.f,
    Diffuse{.albedo = Color3f{0.75f, 0.15f, 0.15}} // red diffuse
  });
  scene.add_object(Sphere{
    Point3f{-3.5f, 0.f, 1.f},
    1.f,
    Conductor{
      .eta = Color3f{0.155, 0.116, 0.138}, // polished silver
      .k   = Color3f{4.83, 3.99, 2.48},
      .alpha_x = 0.01f, .alpha_y = 0.01f
    }
  });
  scene.add_object(Sphere{
    Point3f{0.f, 0.f, 0.f},
    1.f,
    Dielectric{
      .eta = 1.5f, // smooth glass
      .alpha_x = 0.01f, .alpha_y = 0.01f
    }
  });

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
  render_to_image(camera);
  return 0;
}
