export module materials:conductor;

import core;
import bsdf;
import std;

// The Conductor class includes metals and metal alloys, but also semi-metals like graphite
export struct Conductor
{
  Color3f eta{ 1.f, 1.f, 1.f }; // material's IOR per channel (r g b)
  Color3f k  { 0.f, 0.f, 0.f }; // absorption coefficient per channel (r g b)
  // roughness
  f32 alpha_x{ 0.0001f };   // roughness; 0 -> smooth
  f32 alpha_y{ 0.0001f };   // if (alpha_y = alpha_x) -> isotropic
};

BSDFSample sample_smooth(Conductor mat, Vector3f wo)
{
  Vector3f wi = reflect(wo, Vector3f{0.f, 0.f, 1.f});
  if (!same_hemisphere(wo, wi))
    return BSDFSample{};

  f32 cos_i = std::abs(wi.z);
  if (cos_i <= 0.f)
    return BSDFSample{};

  Color3f F = fresnel_conductor(cos_i, mat.eta, mat.k);

  BSDFSample s;
  s.wi      = wi;
  s.cos_i   = cos_i;
  s.f       = F / cos_i;
  s.pdf     = 1.f;
  s.valid   = true;
  return s;
}

BSDFSample sample_rough(Conductor mat, Vector3f wo, TrowbridgeReitzDistribution distrib, RandomGenerator& rng)
{
  Vector3f wm = distrib.sample_wm(wo, rng);
  f32 cos_om  = dot(wo, wm);
  if (cos_om <= 0.f)
    return BSDFSample{};   // safety; VNDF guarantees > 0

  Vector3f wi = reflect(wo, wm);
  if (!same_hemisphere(wo, wi))
    return BSDFSample{};

  f32 cos_i = std::abs(wi.z);
  f32 cos_o = std::abs(wo.z);
  if (cos_i <= 0.f || cos_o <= 0.f)
    return BSDFSample{};

  Color3f F = fresnel_conductor(cos_om, mat.eta, mat.k);

  f32 Dv = distrib.D(wm);        // NDF (1-arg) — in the BRDF
  f32 Gv = distrib.G(wo, wi);    // height-correlated Smith

  BSDFSample s;
  s.wi      = wi;
  s.cos_i   = cos_i;
  s.f       = F * (Dv * Gv) / (4.f * cos_i * cos_o);
  s.pdf     = distrib.D(wo, wm) / (4.f * cos_om);
  s.valid   = true;
  return s;
}

export BSDFSample sample_material(Conductor mat, Vector3f wo, RandomGenerator& rng)
{
  TrowbridgeReitzDistribution distrib{mat.alpha_x, mat.alpha_y};
  BSDFSample s;

  // handle smooth case
  if (distrib.effectively_smooth())
    s = sample_smooth(mat, wo);
  // handle roughness
  else
    s = sample_rough(mat, wo, distrib, rng);
  return s;
}
