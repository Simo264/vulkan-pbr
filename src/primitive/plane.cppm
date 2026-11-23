export module primitive:plane;

import :surface;

import core;
import std;

import camera;
import hit_record;
import material;

export struct Plane
{
  explicit Plane() = default;
  explicit Plane(Point3f p, Vector3f n, Material mat = Material{}, Color3f Le = Color3f{0.f}, bool two_sided = false) :
    center{p},
    normal{normalize(n)},
    surface{mat, Le, two_sided, 0.f} // infinite: no area
  {}

  Point3f  center{0.f}; // the world position
  Vector3f normal{0.f}; // surface normal
  Surface  surface{};   // material, area, Le
};

export std::optional<HitRecord> intersect_primitive(const Plane& plane, Ray ray, f32 t_min, f32 t_max)
{
  f32 denom = dot(plane.normal, ray.d);

  // if ray is parallel to the plane -> no intersection
  if (std::abs(denom) < 1e-4)
    return std::nullopt;

  f32 t = dot(plane.center - ray.o, plane.normal) / denom;
  if (t < t_min || t > t_max)
    return std::nullopt;

  // denom < 0  ->  ray hits the front face (ray.d points INTO the surface)
  // denom > 0  ->  ray hits the back face
  Color3f Le{0.f};
  if(plane.surface.two_sided || denom < 0.f)
    Le = plane.surface.Le;

  Point3f p = ray(t);
  Vector3f n = (denom < 0.f) ? plane.normal : -plane.normal;

  HitRecord hit{
    .t=t,
    .p=p,
    .n=n,
    .mat=plane.surface.mat,
    .Le=Le
  };
  return hit;
}
