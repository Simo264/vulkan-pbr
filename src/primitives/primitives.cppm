export module primitives;

export import :primitive_sample;
export import :surface;
export import :plane;
export import :sphere;
export import :cube;
export import :disk;

import core;
import hit_record;
import std;

export using Primitive = std::variant<Plane, Disk, Sphere, Cube>;

export std::optional<HitRecord> intersect_primitive(const Plane&  plane,  Ray ray, f32 t_min, f32 t_max);
export std::optional<HitRecord> intersect_primitive(const Disk&   disk,   Ray ray, f32 t_min, f32 t_max);
export std::optional<HitRecord> intersect_primitive(const Sphere& sphere, Ray ray, f32 t_min, f32 t_max);
export std::optional<HitRecord> intersect_primitive(const Cube&   cube,   Ray ray, f32 t_min, f32 t_max);

// Uniform Points on a Disk
export PrimitiveSample sample_primitive_area(const Disk&   disk,  RandomGenerator& rng);
// Uniform Points on a Sphere
export PrimitiveSample sample_primitive_area(const Sphere& sphere,RandomGenerator& rng);

export PrimitiveSample sample_primitive_area(const Plane& plane, RandomGenerator& rng)
{
  /* TODO */
  std::source_location loc = std::source_location::current();
  std::println("NOT IMPLEMENTED: {}() at {}:{}", loc.function_name(), loc.file_name(), loc.line());
  std::terminate();
  return PrimitiveSample{ Point3f{}, Vector3f{}, 0.f };
}

export PrimitiveSample sample_primitive_area(const Cube& cube, RandomGenerator& rng)
{
  /* TODO */
  std::source_location loc = std::source_location::current();
  std::println("NOT IMPLEMENTED: {}() at {}:{}", loc.function_name(), loc.file_name(), loc.line());
  std::terminate();
  return PrimitiveSample{ Point3f{}, Vector3f{}, 0.f };
}
