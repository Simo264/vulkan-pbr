export module material:conductor;

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

  BSDFSample s{
    .wi = wi,
    .f = F / cos_i,
    .pdf = 1.f,
    .cos_i = cos_i,
    .valid = true
  };
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
  Color3f f = F * (Dv * Gv) / (4.f * cos_i * cos_o);
  f32 pdf = distrib.D(wo, wm) / (4.f * cos_om);

  BSDFSample s{
    .wi = wi,
    .f = f,
    .pdf = pdf,
    .cos_i = cos_i,
    .valid = true
  };
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

export Color3f eval_material(Conductor mat, Vector3f wo, Vector3f wi)
{
  // Opposite hemispheres: return 0 (a conductor does not transmit).
  if (!same_hemisphere(wo, wi)) return Color3f{0.f};

  TrowbridgeReitzDistribution distrib{mat.alpha_x, mat.alpha_y};

  // BRDF evaluation always returns zero in the smooth case,
  // since the specular peak is considered unreachable by other sampling methods.
  // A fundamental issue with perfect specular scattering distributions is that
  // they contain Dirac delta terms, which preclude BSDF evaluation
  if (distrib.effectively_smooth()) return Color3f{0.f};   // smooth -> zero

  // Rough case: Torrance-Sparrow microfacet model,
  //    f = D(wm) * F(wo . wm) * G(wo, wi) / (4 * |cos_i| * |cos_o|)
  // - wm is the half vector: the only microfacet normal that can reflect wo into wi.
  // - D is the distribution of microfacet normals (Trowbridge-Reitz),
  // - F is the Fresnel, evaluated at the angle between wo and wm,
  // - G is the masking-shadowing function between microfacets.

  Vector3f wh = wo + wi;
  if(length_squared(wh) < 1e-4f)
    return Color3f{0.f}; // wi = -wo, degenerate

  wh = normalize(wh);
  f32 cos_om = std::min(dot(wo, wh), 1.f);
  f32 cos_i  = std::abs(wi.z);
  f32 cos_o  = std::abs(wo.z);
  if(cos_i <= 0.f || cos_o <= 0.f)
    return Color3f{0.f};

  Color3f F  = fresnel_conductor(cos_om, mat.eta, mat.k);
  f32 Dv = distrib.D(wh);
  f32 Gv = distrib.G(wo, wi);
  return Color3f{(F * Dv * Gv) / (4.f * cos_i * cos_o)};
}
