export module primitives.triangle;

import core;
import hit_record;
import camera;
import std;

export class Triangle
{
public:
  explicit Triangle(const Point3f& p0, const Point3f& p1, const Point3f& p2) : p0{p0}, p1{p1}, p2{p2} {}

  std::optional<TriangleHit> intersect(const Ray& ray, f32 t_min, f32 t_max) const
  {
    Vector3f edge1 = p1 - p0;
    Vector3f edge2 = p2 - p0;
    Vector3f h     = cross(ray.d, edge2);
    f32 a          = dot(edge1, h);

    if (a > -1e-6f && a < 1e-6f) return std::nullopt;

    f32 f      = 1.f / a;
    Vector3f s = ray.o - p0;
    f32 u      = f * dot(s, h);
    if (u < 0.f || u > 1.f) return std::nullopt;

    Vector3f q = cross(s, edge1);
    f32 v      = f * dot(ray.d, q);
    if (v < 0.f || u + v > 1.f) return std::nullopt;

    f32 t = f * dot(edge2, q);
    if (t < t_min || t > t_max) return std::nullopt;

    TriangleHit hit;
    hit.t = t;
    hit.b0 = 1.f - u - v;
    hit.b1 = u;
    hit.b2 = v;
    return hit;
  }

  Point3f p0{}, p1{}, p2{};
};
