export module primitive:primitive_sample;

import core;

export struct PrimitiveSample
{
  Point3f  p; // world-space point on the surface
  Vector3f n; // geometric normal at p
  f32 pdf_area;
};
