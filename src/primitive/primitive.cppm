export module primitive;

export import :primitive_sample;
export import :surface;
export import :plane;
export import :sphere;
export import :cube;
export import :disk;

import core;
import std;

import hit_record;

export using Primitive = std::variant<Disk,
                                      Sphere
                                      /*Plane*/
                                      /*Cube*/>;

export std::optional<HitRecord> intersect_primitive(const Disk&    disk,   Ray ray, f32 t_min, f32 t_max);
export std::optional<HitRecord> intersect_primitive(const Sphere&  sphere, Ray ray, f32 t_min, f32 t_max);

// Sample a point on the entire surface uniformly with respect to area
export PrimitiveSample sample_surface_area(const Disk&   disk,   RandomGenerator& rng);
export PrimitiveSample sample_surface_area(const Sphere& sphere, RandomGenerator& rng);
