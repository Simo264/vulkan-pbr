export module camera:ray; // export submodule ray

import core.types;
import core.math;
import std;

// A ray (r) is a semi-infinite line specified by its origin (o) and direction (d).
export struct Ray
{
  Ray() : o{ Point3f{0.f, 0.f, 0.f} }, d{ Vector3f{0.f, 0.f, 1.f} }, t_max{ MAX_FLOAT_32 }  {}
  Ray(const Point3f& o, const Vector3f& d, f32 t_max = MAX_FLOAT_32) : o{o}, d{d}, t_max{t_max} {}

  // Returns the point along the ray at parameter t: o + t * d.
  constexpr Point3f operator()(f32 t) const { return o + t * d; }

  Point3f o{};    // origin
  Vector3f d{};   // direction
  f32 t_max{};    // maximum distance
};
