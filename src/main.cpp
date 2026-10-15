import std;

import types;
import math;
import camera;
import color;
import scene;
import gltf_loader;

constexpr auto image_width = 640;
constexpr auto image_height = 360;

struct UniformRandomGenerator
{
  explicit UniformRandomGenerator(u32 seed) : engine{seed} {}
  f32 uniform(f32 a = 0.f, f32 b = 1.f) { return std::uniform_real_distribution<f32>{a, b}(engine); }

  std::mt19937 engine;
};

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
  Vector3f to_local(const Vector3f& x) const noexcept { return { dot(x, s), dot(x, t), dot(x, n) }; }

  // local to world
  Vector3f to_world(const Vector3f& x) const noexcept { return x.x * s + x.y * t + x.z * n; }
};

Vector3f sample_uniform_hemisphere(const Vector2f& u)
{
  // u.x, u.y in [0, 1)
  f32 cos_theta = 1.f - u.x; // z
  f32 sin_theta = std::sqrt(std::max(0.f, 1.f - cos_theta * cos_theta));
  f32 phi       = 2.f * PI * u.y;
  return Vector3f {
    sin_theta * std::cos(phi),   // x
    sin_theta * std::sin(phi),   // y
    cos_theta                    // z
  };
}

Vector3f sample_cosine_hemisphere(const Vector2f& u) noexcept
{
  f32 cos_theta = std::sqrt(u.x);
  f32 sin_theta = std::sqrt(std::max(0.f, 1.f - u.x));
  f32 phi       = 2.f * PI * u.y;
  return Vector3f{
    sin_theta * std::cos(phi),
    sin_theta * std::sin(phi),
    cos_theta
  };
}

Color3f lambert_brdf(const Vector3f& wo, const Vector3f& wi, const Color3f& reflectance)
{
  // if both w_i and w_o lie in the same emisphere calculate: R/pi, otherwise return 0
  if (wo.z <= 0.f || wi.z <= 0.f)
    return Color3f{0.f};

  return reflectance / PI;
}

f32 lambert_pdf(const Vector3f& wo, const Vector3f& wi)
{
  // if both w_i and w_o lie in the same emisphere calculate the PDF, otherwise return 0
  if (wo.z <= 0.f || wi.z <= 0.f)
    return 0.f;

  // return 1.f / (2.f * PI);
  return wi.z / PI; // (cos theta) / (pi)
}

Color3f ray_color(const Ray&              r,
                  const Scene&            scene,
                  UniformRandomGenerator& rng,
                  i32                     depth)
{
  if (depth <= 0)
    return Color3f{0.f};

  auto sky_color = [](Vector3f d){ f32 t = 0.5f * (d.y + 1.f); return lerp(Color3f{1.f, 1.f, 1.f}, Color3f{0.5f, 0.7f, 1.f}, t); };

  auto hit = scene.intersect(r);
  if (!hit)
    return sky_color(r.d);

  // shading frame coordinate system (s,t,n)
  ShadingFrame frame = ShadingFrame::from_normal(hit->n);
  Vector3f wo_local = frame.to_local(-r.d);

  // sample w_i direction uniformly
  Vector2f u{ rng.uniform(), rng.uniform() };
  Vector3f wi_local = sample_cosine_hemisphere(u);

  // BRDF, PDF, coseno
  Color3f f_r       = lambert_brdf(wo_local, wi_local, hit->reflectance);
  f32     pdf       = lambert_pdf (wo_local, wi_local);
  f32     cos_theta = wi_local.z;

  // Monte Carlo
  Color3f weight = f_r * (cos_theta / pdf);

  Vector3f wi_world = frame.to_world(wi_local);
  Ray reflected_ray{ hit->p + 1e-4f * hit->n, wi_world };
  return weight * ray_color(reflected_ray, scene, rng, depth - 1);
}


void save_image(Camera& cam, std::string_view filename)
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
    Point3f{0.f, 2.f, -1.f},   // position
    Point3f{0.f, 0.f, 5.f},    // look at
    50.f,                      // FOV in degrees,
    image_width, image_height
  };

  UniformRandomGenerator rng{ 12345u };

  Scene scene;
  scene.add(Disk{ Point3f{0.f, -1.f, 0.f}, Vector3f{0.f, 1.f, 0.f}, 50.f });
  scene.add(Sphere{ Point3f{0.f, 0.f, 5.f} });

  auto mesh = load_gltf_model("models/cube.glb");
  mesh.scale({1.0f});
  mesh.rotate_y(45.f);
  mesh.translate({3.f, 0.f, 5.f});
  scene.add(std::move(mesh));

  // if(auto mesh = load_mesh("models/cylinder.glb"))
  // {
  //   mesh->scale({4.0, 1.0f, 4.0f});
  //   mesh->rotate_y(0.f);
  //   mesh->translate({-3.f, 0.f, 5.f});
  //   mesh->reflectance = {0.8f, 0.8f, 0.8f};
  //   scene.add(std::move(*mesh));
  // }


  constexpr auto samples_per_pixel = 8;
  for (auto j = 0; j < image_height; ++j)
  {
    if (j % std::max(1, image_height / 20) == 0 || j == image_height - 1)
    {
      f32 pct = 100.f * static_cast<f32>(j + 1) / static_cast<f32>(image_height);
      i32 done = (j + 1) * image_width;
      constexpr auto total = image_width * image_height;
      std::println("{} / {} ({:.1f}%)", done, total, pct);
    }

    for (auto i = 0; i < image_width; ++i)
    {
      Color3f c{0.f};
      for (i32 sample = 0; sample < samples_per_pixel; sample++)
      {
        constexpr auto max_depth = 5;
        f32 sx = rng.uniform();
        f32 sy = rng.uniform();
        Ray r = camera.generate_ray(i, j, sx, sy);
        c += ray_color(r, scene, rng, max_depth);
      }
      c = c * (1.f / samples_per_pixel);
      camera.film.at(i, j) = c;
    }
  }

  save_image(camera, "output_tmp.ppm");
  return 0;
}
