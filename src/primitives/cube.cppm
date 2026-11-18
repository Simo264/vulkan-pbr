export module primitives:cube;

import :surface;

import core;
import camera;
import hit_record;
import materials;
import std;

export struct Cube
{
  explicit Cube() = default;
  explicit Cube(Point3f center, Material mat = Material{}, Color3f Le = Color3f{0.f}, bool two_sided = false) :
    object_to_world{translate(center)},
    world_to_object{inverse(object_to_world)},
    surface{mat, Le, two_sided, 6.f}  // unit cube: 6 faces of 1×1
  {}

  void set_transform(Transform t)
  {
    object_to_world = t;
    world_to_object = inverse(t);

    // NOTE: assumes uniform scale; recompute area if you support non-uniform
    f32 scale = /* extract uniform scale from t */ 1.f;
    surface.area = 6.f * scale * scale;
  }

  Transform object_to_world{};
  Transform world_to_object{}; // pre-calculated: inverse(object_to_world)
  Surface   surface{};
};

export std::optional<HitRecord> intersect_primitive(const Cube& cube, Ray ray, f32 t_min, f32 t_max)
{
  // 1) ray in object space
  Point3f  lo = cube.world_to_object(ray.o);
  Vector3f ld = cube.world_to_object(ray.d);

  // 2) Slab test su [-0.5, 0.5]
  f32 t_near = -std::numeric_limits<f32>::infinity();
  f32 t_far  =  std::numeric_limits<f32>::infinity();
  i32 enter_axis = -1;
  bool enter_positive = false;

  for (i32 a = 0; a < 3; ++a)
  {
    f32 inv = 1.f / ld[a];
    f32 t0 = (-0.5f - lo[a]) * inv;
    f32 t1 = ( 0.5f - lo[a]) * inv;
    bool positive = false;

    if (t0 > t1)
    {
      std::swap(t0, t1);
      positive = true;
    }

    if (t0 > t_near)
    {
      t_near = t0;
      enter_axis = a;
      enter_positive = positive;
    }
    t_far = std::min(t_far, t1);
    if (t_near > t_far)
      return std::nullopt;
  }

  // 3) Range check
  if (t_near < t_min || t_near > t_max)
    return std::nullopt;

  Point3f p = ray(t_near);

  Vector3f local_n{0.f, 0.f, 0.f};
  local_n[enter_axis] = enter_positive ? 1.f : -1.f;

  Vector3f n = cube.object_to_world(Vector3f{local_n.x,local_n.y,local_n.z});
  n = normalize(n);
  if (dot(n, ray.d) > 0.f)
    n = -n;
  return HitRecord{t_near, p, n, cube.surface.mat, Color3f{0.f} }; // TODO: Le=0.f
}
