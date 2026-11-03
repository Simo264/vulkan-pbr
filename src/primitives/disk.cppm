export module primitives:disk;

import core;
import camera;
import hit_record;
import materials;
import std;

export class Disk
{
public:
  explicit Disk(Point3f p, 
                Vector3f n, 
                f32 radius, 
                Material mat) : center{p}, normal{n}, radius{radius}, mat{mat} {}

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min, f32 t_max) const
  {
    f32 denom = dot(normal, ray.d);
    if (std::abs(denom) < 1e-8f) return std::nullopt; // parallel to the plane

    f32 t = dot(center - ray.o, normal) / denom;
    if (t < t_min || t > t_max) return std::nullopt;

    Point3f p = ray(t);

    Vector3f d = p - center;
    if (dot(d, d) > radius * radius) return std::nullopt;

    HitRecord hit;
    hit.t = t;
    hit.p = p;
    hit.n = (denom < 0.f) ? normal : -normal;
    hit.mat = mat;
    return hit;
  }

  Point3f center{};
  Vector3f normal{};
  f32 radius;
  Material mat{};
};
