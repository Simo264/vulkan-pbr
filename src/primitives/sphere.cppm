export module primitives:sphere;

import core;
import camera;
import hit_record;
import materials;
import std;

export class Sphere
{
public:
  explicit Sphere(Point3f center,
                  f32 radius,
                  Material mat) : center{center}, radius{radius}, mat{mat} {}

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min, f32 t_max) const
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
    hit.mat = mat;
    return hit;
  }

  Point3f center{};
  f32 radius{};
  Material mat{};
};
