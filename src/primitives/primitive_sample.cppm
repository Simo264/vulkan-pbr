export module primitives:primitive_sample;

import core;

export struct PrimitiveSample
{
  PrimitiveSample(Point3f p, Vector3f n, f32 pdf_area) : p{p}, n{n}, pdf_area{pdf_area} {}

  Point3f  p; // world-space point on the surface
  Vector3f n; // geometric normal at p
  f32 pdf_area;
};
