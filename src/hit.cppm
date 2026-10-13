export module hit;

import types;
import math;

export struct HitRecord
{
  f32      t{}; // ray intersection
  Point3f  p{}; // hit point
  Vector3f n{}; // surface normal vector
};
