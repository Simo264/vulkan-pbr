export module geometry:sphere;

import types;
import math;
import camera;
import std;

export class Sphere
{
public:
  Sphere(Point3f center = Point3f{0.f, 0.f, 0.f}, f32 radius = 1.0f) : center{center}, radius{radius} {}

  f32 intersect(const Ray& ray) const noexcept
  {
    Vector3f oc = ray.o - center;

    f32 b = dot(oc, ray.d);
    f32 c = dot(oc, oc) - radius * radius;
    f32 disc = b * b - c;

    if (disc < 0.f)
      return -1.f;

    f32 sqrt_d = std::sqrt(disc);

    f32 t = -b - sqrt_d;
    if (t > 1e-4f)
      return t;

    t = -b + sqrt_d;
    if (t > 1e-4f)
      return t;

    return -1.f;
  }

  Point3f center{};
  f32 radius{};
};
