export module lights:point_light;

import :light_sample;

import core;
import std;

export struct PointLight
{
  explicit PointLight(Point3f p, f32 intensity) : position{p}, intensity{intensity} {}

  Point3f position;
  f32 intensity;
};

export Color3f L(const PointLight&, Point3f, Vector3f, Vector3f)
{
  // A point light has no surface.
  // There is no `pe` at which to evaluate it, and no `Le` with finite units at any such point.
  return Color3f{0.f};
}

export LightSample sample_Li(const PointLight& light, Point3f p, RandomGenerator& rng)
{
  Vector3f d = light.position - p;
  f32 dist = length(d);
  if (dist < 1e-6f)
    return LightSample{}; // pdf = 0, will be skipped

  LightSample s;
  s.dist  = dist;
  s.wi    = d / dist;              // = normalize(d)
  s.L     = Color3f{light.intensity / (dist*dist)}; // l.intensity / (dist*dist);
  s.pdf   = 1.f;
  return s;
}
