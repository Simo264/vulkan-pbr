export module hit_record;

import core;

export struct HitRecord
{
  f32      t{};           // ray intersection
  Point3f  p{};           // hit point
  Vector3f n{};           // surface normal vector
  Color3f  reflectance{}; // the material property
};

export struct TriangleHit
{
  f32 t{};
  f32 b0{}, b1{}, b2{}; // barycentric coordinates
};
