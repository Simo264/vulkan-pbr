export module primitives:disk;

import :primitive_sample;
import :surface;

import core;
import camera;
import hit_record;
import materials;
import sampling;
import std;

export struct Disk
{
  explicit Disk() = default;
  explicit Disk(Point3f p, Vector3f n, f32 radius, Material mat = Material{}, Color3f Le = Color3f{0.f}, bool two_sided = false) :
    center{p},
    normal{normalize(n)},
    radius{radius},
    surface{mat, Le, two_sided, PI * radius * radius}
  {}

  Point3f  center{0.f};           // the world position
  Vector3f normal{0.f, 1.f, 0.f}; // the surface normal
  f32      radius{1.f};           // disk radius
  Surface  surface{};             // material, area, Le
};

export std::optional<HitRecord> intersect_primitive(const Disk& disk, Ray ray, f32 t_min, f32 t_max)
{
  f32 denom = dot(disk.normal, ray.d);
  if (std::abs(denom) < 1e-8f)
    return std::nullopt; // parallel to the plane

  f32 t = dot(disk.center - ray.o, disk.normal) / denom;
  if (t < t_min || t > t_max)
    return std::nullopt;

  Point3f p = ray(t);
  Vector3f d = p - disk.center;
  if (dot(d, d) > disk.radius * disk.radius)
    return std::nullopt;

  // denom < 0  ->  ray hits the front face (ray.d points INTO the surface)
  // denom > 0  ->  ray hits the back face
  Color3f Le{0.f};
  if(disk.surface.two_sided || denom < 0.f)
    Le = disk.surface.Le;

  Vector3f n = (denom < 0.f) ? disk.normal : -disk.normal;
  return HitRecord{t, p, n, disk.surface.mat, Le};
}

export PrimitiveSample sample_surface_area(const Disk& disk, RandomGenerator& rng)
{
  Point2f sample = sample_uniform_disk_concentric(disk.radius, rng);

  TangentFrame frame{disk.normal}; // tangent, bitangent, normal
  // map from local 2D to world 3D
  Point3f p_e  = disk.center + frame.to_world({sample.x, sample.y, 0.f});
  Vector3f n_e = disk.normal;
  f32 pdf_area = 1.f / disk.surface.area;
  return PrimitiveSample{p_e, n_e, pdf_area};
}
