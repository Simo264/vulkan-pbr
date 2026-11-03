export module primitives:plane;

import core;
import camera;
import hit_record;
import materials;
import std;

export class Plane
{
public:
  explicit Plane(Point3f p, 
                 Vector3f n,
                 Material mat) : center{p}, normal{n}, mat{mat} {}

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min, f32 t_max) const
  {
    f32 denom = dot(normal, ray.d);

    // if ray is parallel to the plane -> no intersection
    if (std::abs(denom) < 1e-4) return std::nullopt;

    f32 t = dot(center - ray.o, normal) / denom;
    if (t < t_min || t > t_max) return std::nullopt;

    HitRecord hit;
    hit.t = t;
    hit.p = ray(t);
    hit.n = (denom < 0.f) ? normal : -normal;
    hit.mat = mat;
    return hit;
  }

  Point3f center{};
  Vector3f normal{};
  Material mat{};
};
