export module material:dielectric;

import core;
import bsdf;
import std;

constexpr auto IOR_air = 1.0f;

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
  Vector3f wi = -wo;
  f32 cos_i = std::abs(wi.z);
  if (cos_i <= 0.f)
    return BSDFSample{};

  BSDFSample s{
    .wi      = wi,
    .f       = Color3f{1.f / cos_i}, // trivial: passes Li through unchanged
    .pdf     = 1.f,
    .cos_i   = cos_i,
    .valid   = true
  };
  return s;
}

BSDFSample sample_smooth(Vector3f wo, f32 eta_i, f32 eta_t, f32 eta_rel, RandomGenerator& rng)
{
  f32 cos_i = std::abs(wo.z);

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

    s = BSDFSample{
      .wi      = wi,
      .f       = Color3f{F / cos_wi},
      .pdf     = F,
      .cos_i   = cos_wi,
      .valid   = true
    };
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

    Color3f f = Color3f{T / (eta_rel * eta_rel * cos_wi)};

    s = BSDFSample{
      .wi      = wi,
      .f       = f,
      .pdf     = T,
      .cos_i   = cos_wi,
      .valid   = true
    };
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
    Color3f f = Color3f{F * Dv * Gv / (4.f * cos_i * cos_o)};
    f32 pdf = distrib.D(wo, wm) / (4.f * abs_cos_om) * pr;

    s = BSDFSample{
      .wi    = wi,
      .f     = f,
      .pdf   = pdf,
      .cos_i = cos_i,
      .valid = true
    };
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
    Color3f f = Color3f{(Dv * T * Gv * abs_cos_im * abs_cos_om) / (cos_i * cos_o * denom_sq * eta_rel * eta_rel)};
    f32 pdf = distrib.D(wo, wm) * abs_cos_im / denom_sq * pt;

    s = BSDFSample{
      .wi    = wi,
      .f     = f,
      .pdf   = pdf,
      .cos_i = cos_i,
      .valid = true
    };
  }
  return s;
}

export BSDFSample sample_material(Dielectric mat, Vector3f wo, RandomGenerator& rng)
{
  bool index_matched = std::abs(mat.eta - IOR_air) < 1e-4f;
  if (index_matched) // When the IOR is 1 on both sides, the interface is optically invisible
  {
    BSDFSample s = sample_index_matched(wo);
    return s;
  }

  // the beam comes from outside? wo.z > 0
  // the beam comes from inside? wo.z < 0
  f32 cos_i = wo.z;
  bool entering = cos_i > 0.f;
  f32 eta_i  = entering ? IOR_air : mat.eta;
  f32 eta_t  = entering ? mat.eta : IOR_air;
  f32 eta_rel = eta_t / eta_i;

  TrowbridgeReitzDistribution distrib{mat.alpha_x, mat.alpha_y};
  BSDFSample s;

  // handle smooth dielectric
  if (distrib.effectively_smooth())
  {
    s = sample_smooth(wo, eta_i, eta_t, eta_rel, rng);
    s.is_specular = true; // smooth -> specular
  }
  // handle rough dielectric
  else
    s = sample_rough(wo, eta_i, eta_t, eta_rel, distrib, rng);
  return s;
}

export Color3f eval_material(Dielectric mat, Vector3f wo, Vector3f wi)
{
  // Index-matched interface (eta = 1) -> return 0: there is no surface to scatter at.
  bool index_matched = std::abs(mat.eta - IOR_air) < 1e-4f;
  if(index_matched)
    return Color3f{0.f};

  TrowbridgeReitzDistribution distrib{mat.alpha_x, mat.alpha_y};

  // BRDF evaluation always returns zero in the smooth case,
  // since the specular peak is considered unreachable by other sampling methods.
  // A fundamental issue with perfect specular scattering distributions is that
  // they contain Dirac delta terms, which preclude BSDF evaluation
  if (distrib.effectively_smooth())
    return Color3f{0.f};  // both reflection and refraction are Diract Delta distribution

  f32 cos_o = std::abs(wo.z);
  f32 cos_i = std::abs(wi.z);
  if(cos_o == 0.f || cos_i == 0.f)
    return Color3f{0.f};

  // The sign of wo.z tells whether the ray is entering or leaving the medium,
  // which fixes eta_i, eta_t and the relative IOR.
  // The wi's hemisphere selects the branch
  bool entering = wo.z > 0.f;
  f32 eta_i  = entering ? IOR_air : mat.eta;
  f32 eta_t  = entering ? mat.eta : IOR_air;
  f32 eta_rel = eta_t / eta_i;

  Color3f bsdf{0.f};

  // handle reflection (same hemisphere)
  if(same_hemisphere(wo, wi))
  {
    Vector3f wh_raw = wo + wi;
    if(length_squared(wh_raw) < 1e-4f)
      return Color3f{0.f};
    Vector3f wh = normalize(wh_raw);

    f32 c_o = dot(wo, wh);
    if(c_o <= 0.f)
      return Color3f{0.f};

    // Same microfacet formula as the conductor
    f32 F = fresnel_dielectric(std::min(c_o, 1.f), eta_t, eta_i);
    f32 D = distrib.D(wh);
    f32 G = distrib.G(wo, wi);
    bsdf  = Color3f{F * D * G / (4 * cos_o * cos_i)};
  }
  // handle transmission (opposite hemispheres)
  else
  {
    Vector3f wh_raw = -(eta_i * wo + eta_t * wi); // Walter generalized half-vector
    if(length_squared(wh_raw) < 1e-4f)
      return Color3f{0.f};

    Vector3f wh = normalize(wh_raw);
    if (wh.z < 0.f)
      wh = -wh; // face-forward to +z

    f32 c_o = dot(wo, wh);
    f32 c_i = dot(wi, wh);
    if (c_i * wi.z < 0.f || c_o * wo.z < 0.f) // Microfacets facing away from wo or wi are rejected.
      return Color3f{0.f};

    f32 abs_c_o = std::abs(c_o);
    f32 abs_c_i = std::abs(c_i);
    f32 F = fresnel_dielectric(std::min(abs_c_o, 1.f), eta_t, eta_i);
    f32 T = 1 - F;
    f32 D = distrib.D(wh);
    f32 G = distrib.G(wo, wi);
    f32 denom = c_i + c_o / eta_rel;
    if(std::abs(denom) < 1e-4f)
      return Color3f{0.f};

    bsdf = Color3f{(D * T * G * abs_c_i * abs_c_o) / (cos_i * cos_o * denom * denom * eta_rel * eta_rel)};
  }
  return bsdf;
}
