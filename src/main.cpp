import std;

import types;
import math;
import camera;
import color;
import geometry;

constexpr Color3f sphere_reflectance{ 0.8f, 0.3f, 0.3f };
constexpr Color3f disk_reflectance  { 0.7f, 0.7f, 0.7f };

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

struct ShadingSample
{
  Color3f  weight;
  Ray      reflected_ray;
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

std::optional<ShadingSample> sample_diffuse(const Point3f&              p,
                                            const Vector3f&             n,
                                            const Vector3f&             ray_dir,
                                            const Color3f&              reflectance,
                                            UniformRandomGenerator&     rng)
{
  // shading frame coordinate system (s,t,n)
  ShadingFrame frame = ShadingFrame::from_normal(n);
  Vector3f wo_local = frame.to_local(-ray_dir);

  // sample w_i direction uniformly
  Vector2f u{ rng.uniform(), rng.uniform() };
  Vector3f wi_local = sample_cosine_hemisphere(u);

  // BRDF, PDF, coseno
  Color3f f_r       = lambert_brdf(wo_local, wi_local, reflectance);
  f32     pdf       = lambert_pdf (wo_local, wi_local);
  f32     cos_theta = wi_local.z;

  if (pdf <= 0.f)
    return std::nullopt;

  // Monte Carlo
  Color3f weight = f_r * (cos_theta / pdf);

  Vector3f wi_world = frame.to_world(wi_local);
  Ray reflected_ray{ p + 1e-4f * n, wi_world };
  return ShadingSample{ weight, reflected_ray };
}

Color3f ray_color(const Ray&                r,
                  std::span<Sphere, 3>      spheres,
                  const Disk&               disk,
                  UniformRandomGenerator&   rng,
                  i32                       depth)
{
  if (depth <= 0)
    return Color3f{0.f};

  // helper used for shading + recursion
  auto lambertian_shading = [&](const Point3f& p, Vector3f n, const Color3f& reflectance)
  {
    if (dot(n, r.d) > 0.f)
      n = -n;

    auto s = sample_diffuse(p, n, r.d, reflectance, rng);
    if (!s) return Color3f{0.f};

    Color3f L_i = ray_color(s->reflected_ray, spheres, disk, rng, depth - 1);
    return s->weight * L_i;
  };

  for(auto sphere : spheres)
  {
    if (auto hit = sphere.intersect(r))
      return lambertian_shading(hit->p, hit->n, sphere_reflectance);
  }
  if (auto hit = disk.intersect(r))
    return lambertian_shading(hit->p, hit->n, disk_reflectance);

  f32 t = 0.5f * (r.d.y + 1.f);
  return lerp(Color3f{1.f, 1.f, 1.f}, Color3f{0.5f, 0.7f, 1.f}, t);
}

i32 main()
{
  constexpr auto image_width = 640;
  constexpr auto image_height = 360;
  Camera cam{
    Point3f {0.f, 0.f, -2.f},   // at the origin
    Vector3f{0.f, 0.f, 1.f},    // forward
    60.f,                       // FOV in degrees,
    image_width,
    image_height
  };

  auto spheres = std::array<Sphere, 3>{
    Sphere{ Point3f{0.f, 0.f, 3.f} },
    Sphere{ Point3f{3.f, 0.f, 3.f} },
    Sphere{ Point3f{-3.f, 0.f, 3.f} },
  };
  Disk disk{ Point3f{0.f, -1.f, 0.f}, Vector3f{0.f, 1.f, 0.f}, 50.f };

  UniformRandomGenerator rng{ 12345u };

  constexpr auto samples_per_pixel = 32;
  for (auto j = 0; j < image_height; ++j)
  {
    for (auto i = 0; i < image_width; ++i)
    {
      Color3f c{0.f};
      for (i32 sample = 0; sample < samples_per_pixel; sample++)
      {
        constexpr auto max_depth = 5;
        f32 sx = rng.uniform();
        f32 sy = rng.uniform();
        Ray r = cam.generate_ray(i, j, sx, sy);
        c += ray_color(r, std::span(spheres), disk, rng, max_depth);
      }
      c = c * (1.f / samples_per_pixel);
      cam.film.at(i, j) = c;
    }
  }

  constexpr auto output_file = "output.ppm";
  std::ofstream out{output_file, std::ios::binary};
  if (!out)
  {
    std::println(std::cerr, "Cannot open {}", output_file);
    return 1;
  }

  out << "P6\n" << image_width << ' ' << image_height << "\n255\n";
  auto to255 = [](f32 v) -> u8 {
    v = std::clamp(v, 0.f, 1.f);
    return static_cast<u8>(255.f * v);
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
  std::println("Wrote {} ({}x{})", output_file, image_width, image_height);
  return 0;
}
