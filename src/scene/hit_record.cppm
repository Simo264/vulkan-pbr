export module scene:hit_record;

import types;
import math;
import color;

export struct HitRecord
{
  f32      t{};           // ray intersection
  Point3f  p{};           // hit point
  Vector3f n{};           // surface normal vector
  Color3f  reflectance{}; // the material property
};
