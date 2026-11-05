export module hit_record;

import core;
import materials;

export struct HitRecord
{
  f32      t{};      // ray intersection
  Point3f  p{};      // hit point
  Vector3f n{};      // point it towards the beam
  Material mat{};
};

// export struct TriangleHit
// {
//   f32 t{};
//   f32 b0{}, b1{}, b2{}; // barycentric coordinates
// };
