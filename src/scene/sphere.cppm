export module scene:sphere;

import :hit_record;

import core.types;
import core.math;
import core.color;
import camera;
import std;

export class Sphere
{
public:
  explicit constexpr Sphere(Point3f   center = Point3f{0.f, 0.f, 0.f},
                            f32       radius = 1.0f,
                            Color3f   reflectance = Color3f{1.0f})
  : center{center}, radius{radius}, reflectance{reflectance} {}

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min = 1e-4, f32 t_max = MAX_FLOAT_32) const noexcept
  {
    Vector3f oc = ray.o - center;

    f32 b = dot(oc, ray.d);
    f32 c = dot(oc, oc) - radius * radius;
    f32 disc = b * b - c;

    if (disc < 0.f)
      return std::nullopt;

    f32 sqrt_d = std::sqrt(disc);

    // Find the nearest root that lies in the acceptable range.
    f32 t = -b - sqrt_d;
    if (t < t_min || t > t_max)
    {
      t = -b + sqrt_d;
      if (t < t_min || t > t_max)
        return std::nullopt;
    }

    HitRecord hit{};
    hit.t = t;
    hit.p = ray(t);
    hit.n = (hit.p - center) / radius;
    hit.reflectance = reflectance;
    return hit;
  }

  Point3f center{};
  f32 radius{};
  Color3f reflectance{};
};
