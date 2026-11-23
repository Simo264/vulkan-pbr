export module hit_record;

import core;
import material;

export struct HitRecord
{
  f32 t{0.f};        // ray intersection: r = o + td
  Point3f p{0.f, 0.f, 0.f};    // the surface point
  Vector3f n{0.f, 0.f, 1.f};   // the surface normal
  Material mat{};
  Color3f Le{0.f, 0.f, 0.f};   // emitted radiance (independent of material)
};
