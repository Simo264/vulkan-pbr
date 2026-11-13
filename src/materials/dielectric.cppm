export module materials:dielectric;

import core;
import bsdf;
import std;

// The Dielectric class includes any substance (whether gaseous, liquid, or solid) that acts as an electric insulator,
// including glass, water, mineral oil, and air.
export struct Dielectric
{
  f32 eta{ 1.5f }; // The material's IOR (e.g. glass = 1.5)

  f32 alpha_x{ 0.0001f };   // roughness; 0 -> smooth (delta)
  f32 alpha_y{ 0.0001f };   // if (alpha_y = alpha_x) -> isotropic
};

// Index matched (eta = 1): no optical boundary, ray passes straight through
BSDFSample sample_index_matched(Vector3f wo)
{
  BSDFSample s;

  Vector3f wi = -wo;
  f32 cos_i = std::abs(wi.z);
  if (cos_i <= 0.f)
    return BSDFSample{};

  s.wi      = wi;
  s.cos_i   = cos_i;
  s.f       = Color3f{1.f / cos_i}; // trivial: passes Li through unchanged
  s.pdf     = 1.f;
  s.valid   = true;
  return s;
}

BSDFSample sample_smooth(Vector3f wo, f32 eta_i, f32 eta_t, f32 eta_rel, RandomGenerator& rng)
{
  f32 cos_i = std::abs(wo.z);
  if (cos_i <= 0.f)
    return BSDFSample{};

  BSDFSample s;

  f32 F = fresnel_dielectric(cos_i, eta_t, eta_i);
  f32 T = 1.f - F;

  bool do_reflect = (F >= 1.f) || (rng.uniform() < F);
  if (do_reflect)
  {
    Vector3f wi = reflect(wo, Vector3f{0.f, 0.f, 1.f}); // specular direction
    f32 cos_wi = std::abs(wi.z);
    if (cos_wi <= 0.f)
      return BSDFSample{};

    s.wi    = wi;
    s.cos_i = cos_wi;
    s.f     = Color3f{F / cos_wi};
    s.pdf   = F; // = 1 in case of total internal reflection
    s.valid = true;
  }
  else
  {
    auto refr = refract(wo, Vector3f{0.f, 0.f, 1.f}, eta_rel);
    if (!refr)
      return BSDFSample{};

    Vector3f wi = *refr;
    f32 cos_wi = std::abs(wi.z);
    if (cos_wi <= 0.f)
      return BSDFSample{};

    s.wi    = wi;
    s.cos_i = cos_wi;
    s.f     = Color3f{T / (eta_rel * eta_rel * cos_wi)};
    s.pdf   = T;
    s.valid = true;
  }
  return s;
}

BSDFSample sample_rough(Vector3f wo, f32 eta_i, f32 eta_t, f32 eta_rel, TrowbridgeReitzDistribution distrib, RandomGenerator& rng)
{
  Vector3f wm = distrib.sample_wm(wo, rng);
  f32 cos_om = dot(wo, wm);                  // signed: negative when wo is inside
  f32 abs_cos_om = std::abs(cos_om);
  if (abs_cos_om <= 0.f) 
    return BSDFSample{};
  
  f32 F = fresnel_dielectric(std::min(abs_cos_om, 1.f), eta_t, eta_i);
  f32 T = 1.f - F;

  // Balanced sampling; force reflection at per-microfacet TIR
  f32 pr = 0.25f + 0.5f * F;
  if (F >= 1.f)
    pr = 1.f;
  f32 pt = 1.f - pr;

  BSDFSample s;

  bool do_reflect = (F >= 1.f) || (rng.uniform() < pr);
  if (do_reflect) // Sample reflection at rough dielectric interface
  {
    Vector3f wi = reflect(wo, wm);
    if (!same_hemisphere(wo, wi))
      return BSDFSample{};

    f32 cos_i = std::abs(wi.z);
    f32 cos_o = std::abs(wo.z);
    if (cos_i <= 0.f || cos_o <= 0.f)
      return BSDFSample{};

    f32 Dv = distrib.D(wm);
    f32 Gv = distrib.G(wo, wi);

    s.wi    = wi;
    s.cos_i = cos_i;
    s.f     = Color3f{F * Dv * Gv / (4.f * cos_i * cos_o)};
    s.pdf   = distrib.D(wo, wm) / (4.f * abs_cos_om) * pr;
    s.valid = true;
  }
  else // Sample transmission at rough dielectric interface
  {
    Vector3f n = (wo.z > 0.f) ? wm : -wm;
    auto refr = refract(wo, n, eta_rel);
    if (!refr)
      return BSDFSample{};

    Vector3f wi = *refr;
    if (same_hemisphere(wo, wi)) // must cross macro-surface
      return BSDFSample{};

    f32 cos_i = std::abs(wi.z);
    f32 cos_o = std::abs(wo.z);
    if (cos_i <= 0.f || cos_o <= 0.f) return s;

    f32 cos_im = dot(wi, wm); // signed, should be < 0
    if (cos_im * cos_om >= 0.f)
      return BSDFSample{}; // wi must be on opposite side of wm
    f32 abs_cos_im = std::abs(cos_im);

    f32 denom    = cos_im + cos_om / eta_rel;  // signed
    f32 denom_sq = denom * denom;
    if (denom_sq <= 0.f)
      return BSDFSample{};

    f32 Dv = distrib.D(wm);
    f32 Gv = distrib.G(wo, wi);

    s.wi    = wi;
    s.cos_i = cos_i;
    s.f     = Color3f{(Dv * T * Gv * abs_cos_im * abs_cos_om) / (cos_i * cos_o * denom_sq * eta_rel * eta_rel)};
    s.pdf   = distrib.D(wo, wm) * abs_cos_im / denom_sq * pt;
    s.valid = true;
  }
  return s;
}

export BSDFSample sample_material(Dielectric mat, Vector3f wo, RandomGenerator& rng)
{
  constexpr auto IOR_air = 1.0f;

  bool index_matched = std::abs(mat.eta - IOR_air) < 1e-4f;
  if (index_matched) // When the IOR is 1 on both sides, the interface is optically invisible
  {
    BSDFSample s = sample_index_matched(wo);
    return s;
  }

  // the beam comes from outside? wo.z > 0
  // the beam comes from inside? wo.z < 0
  bool entering = wo.z > 0.f;
  f32 eta_i  = entering ? IOR_air : mat.eta;
  f32 eta_t  = entering ? mat.eta : IOR_air;
  f32 eta_rel = eta_t / eta_i;

  TrowbridgeReitzDistribution distrib{mat.alpha_x, mat.alpha_y};
  BSDFSample s;

  // handle smooth dielectric
  if (distrib.effectively_smooth())
    s = sample_smooth(wo, eta_i, eta_t, eta_rel, rng);
  // handle rough dielectric
  else
    s = sample_rough(wo, eta_i, eta_t, eta_rel, distrib, rng);
  return s;
}
