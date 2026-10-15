export module scene:disk;

import :hit_record;

import types;
import math;
import camera;

import std;

export class Disk
{
public:
  explicit constexpr Disk(Point3f center = Point3f{0.f},
                          Vector3f n = Vector3f{0.f, 1.f, 0.f},
                          f32 r = 1.0f,
                          Color3f reflectance = {0.7f, 0.7f, 0.7f})
  : center{center}, normal{n}, radius{r}, reflectance{reflectance} {}

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min = 1e-4, f32 t_max = MAX_FLOAT_32) const noexcept
  {
    f32 denom = dot(normal, ray.d);

    // if ray is parallel to the disk's plane: no intersection
    if (std::abs(denom) < 1e-4f)
      return std::nullopt;

    f32 t = dot(normal, center - ray.o) / denom;
    if (t < t_min || t > t_max)
      return std::nullopt;

    Point3f p = ray(t);
    Vector3f d = p - center;
    if (dot(d, d) > radius * radius)
      return std::nullopt;

    HitRecord hit{};
    hit.t = t;
    hit.p = p;
    hit.n = (denom < 0.f) ? normal : -normal;
    hit.reflectance = reflectance;
    return hit;
  }

  Point3f  center{};
  Vector3f normal{};
  f32 radius;
  Color3f reflectance{};
};
