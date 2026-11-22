export module lights:diffuse_area_light;

import :light_sample;

import core;
import primitives;
import sampling;
import std;

// Compared with the point light, an AreaLight is defined by two things:
// 1. shape: the geometric surface that emits light, such as a disk, rectangle or sphere
// 2. emitted radiance distribution: which describes how much light the source emits from each point and in each direction
// For DiffuseAreaLight, the directional distribution is uniform: each point emits equally in all directions on its emitting side.
// This is why it is called diffuse: it behaves like a Lambertian emitter.
export struct DiffuseAreaLight
{
  DiffuseAreaLight(Primitive shape)
  {
    this->power = std::visit([&](const auto& s){ return s.surface.power(); }, shape);
    this->shape = std::move(shape);
  }

  Primitive shape{}; // shape: the geometric surface that emits light (disk, sphere, quad, ecc.)
  Color3f power{};   // power (per-channel) Phi = pi * area * Le * scale (times 2 if two_sided)
};

export Color3f L(const DiffuseAreaLight& light, Point3f pe, Vector3f ne, Vector3f w)
{
  auto value = std::visit([&](const auto& s) {
    if (!s.surface.two_sided && dot(ne, w) <= 0.f)
      return Color3f{0.f};
    return s.surface.Le;
  }, light.shape);

  return value;
}

export LightSample sample_Li(const DiffuseAreaLight& light, Point3f ps, RandomGenerator& rng)
{
  // TODO: implement spherical-cap sampling for Sphere emitters
  //       (and visible-region sampling for Disk emitters).
  //       The cap sampler should return a solid-angle pdf and skip the
  //       area -> solid-angle Jacobian below. For now, use full-surface
  //       sampling, which is unbiased but higher variance.

  PrimitiveSample sample = std::visit([&](const auto& shape){ // pe, ne, pdf_area
    return sample_surface_area(shape, rng);
  }, light.shape);

  Point3f pe = sample.p; // the emission point
  Vector3f ne = sample.n;

  Vector3f to_light = pe - ps; // direction from the shaded point to the emitted point
  f32 dist = length(to_light); //  distance from light
  if (dist < 1e-6f) return LightSample{}; // degenerate

  f32 d2 = dist*dist;

  Vector3f wi = to_light / dist; // normalize
  f32 cos_e = dot(ne, -wi); // angle at the light
  f32 abs_cos = std::abs(cos_e);
  if (abs_cos < 1e-8f) return LightSample{};  // grazing

  Color3f Le = L(light, pe, ne, -wi); // the radiance emitted by the light source
  if(Le == Color3f{0.f}) return LightSample{};

  f32 pdf = sample.pdf_area * d2 / abs_cos; // area -> solid angle
  return LightSample { wi, Le, pdf, dist };
}
