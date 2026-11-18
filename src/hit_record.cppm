export module hit_record;

import core;
import materials;

export struct HitRecord
{
  HitRecord(f32 t, Point3f p, Vector3f n, Material mat, Color3f Le) :
   t{t}, p{p}, n{n}, mat{mat}, Le{Le}
  {}

  f32 t{};        // ray intersection: r = o + td
  Point3f p{};    // the surface point
  Vector3f n{};   // the surface normal
  Material mat{};
  Color3f Le{};   // emitted radiance (independent of material)
};
