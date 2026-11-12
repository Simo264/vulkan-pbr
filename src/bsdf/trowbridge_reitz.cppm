export module bsdf:trowbridge_reitz;

import core;
import sampling;
import std;

export class TrowbridgeReitzDistribution
{
public:
  TrowbridgeReitzDistribution(f32 alpha_x, f32 alpha_y) : alpha_x{alpha_x}, alpha_y{alpha_y} {}

  // When alpha_x=alpha_y the model becomes isotropic
  bool isotropic() const { return alpha_x == alpha_y; }

  // When alpha_x, alpha_y ~ 0 the ellipsoid has been stretched to such a degree that it essentially collapses into a flat surface,
  // and the aggregate BSDF approximates a perfect specular material.
  bool effectively_smooth() const { return std::max(alpha_x, alpha_y) < 1e-3f; }

  // Trowbridge-Reitz (GGX) Normal Distribution Function D(omega)
  f32 D(Vector3f wm) const
  {
    f32 cos2_theta = wm.z * wm.z;
    if (cos2_theta <= 0.f) return 0.f;
    f32 cos4_theta = cos2_theta*cos2_theta;

    f32 alpha2_x = alpha_x * alpha_x;
    f32 alpha2_y = alpha_y * alpha_y;

    // e = tan^2(theta) * (cos^2(phi)/ax^2 + sin^2(phi)/ay^2), without computing theta or phi
    f32 e = (wm.x * wm.x) / alpha2_x + (wm.y * wm.y) / alpha2_y;
    e /= cos2_theta;
    f32 t = 1.f + e;
    return 1.f / (PI * alpha_x * alpha_y * cos4_theta * t * t);
  }

  // Distribution of Visible Normals D_omega(omega_m)
  f32 D(Vector3f w, Vector3f wm) const
  {
    f32 abs_cos = std::abs(w.z);
    if (abs_cos <= 0.f) return 0.f;
    f32 abs_dot = std::abs(dot(w, wm));
    return G1(w) / abs_cos * D(wm) * abs_dot;
  }

  // The Masking Function (Smith approximation)
  f32 G1(Vector3f w) const { return 1.f / (1.f + lambda(w)); }
  // Smith height-correlated masking-shadowing function
  f32 G(Vector3f wo, Vector3f wi) const { return 1.f / (1.f + lambda(wo) + lambda(wi)); }

  Vector3f sample_wm(Vector3f w, RandomGenerator& rng)
  {
    // Step 1: transform the GGX ellipsoid into a hemisphere
    Vector3f wh = normalize(Vector3f{alpha_x * w.x, alpha_y * w.y, w.z});
    if (wh.z < 0)
    wh = -wh;

    // Step 2: find orthonormal basis for visible normal sampling
    Vector3f T1 = (wh.z < 0.99999f) ? normalize(cross(Vector3f{0.f, 0.f, 1.f}, wh)) : Vector3f{1.f, 0.f, 0.f};
    Vector3f T2 = cross(wh, T1);

    // Step 3: generate uniformly distributed points on the unit disk
    Point2f p = sample_uniform_disk_polar(rng);

    // Step 4: warp hemispherical projection for visible normal sampling
    f32 h = std::sqrt(1.f - p.x * p.x);
    p.y = std::lerp(h, p.y, (1.f + wh.z) / 2.f);

    // Step 5: reproject to hemisphere and transform normal to ellipsoid
    f32 pz = std::sqrt(std::max<f32>(0, 1 - length_squared(Vector2f{p.x, p.y})));
    Vector3f nh = p.x * T1 + p.y * T2 + pz * wh;
    return normalize(Vector3f{alpha_x * nh.x, alpha_y * nh.y, std::max<f32>(1e-6f, nh.z)});
  }

  f32 alpha_x, alpha_y;

private:
  // The model computes an effective roughness using the viewing direction’s azimuth phi.
  f32 effective_alpha(Vector3f w) const
  {
    f32 sin2_theta = std::max(0.f, 1.f - w.z * w.z);
    if (sin2_theta <= 0.f) return alpha_x; // looking along the normal: phi is arbitrary

    f32 alpha2_x = alpha_x * alpha_x;
    f32 alpha2_y = alpha_y * alpha_y;

    // cos^2(phi) = w.x^2 / sin^2(theta),  sin^2(phi) = w.y^2 / sin^2(theta)
    f32 alpha2 = (alpha2_x * w.x * w.x + alpha2_y * w.y * w.y) / sin2_theta;
    return std::sqrt(alpha2);
  }

  // Smith approximation
  f32 lambda(Vector3f w) const
  {
    f32 cos2_theta = w.z * w.z;
    if (cos2_theta <= 0.f) return 0.f;  // tan(theta) is infinite

    f32 sin2_theta = std::max(0.f, 1.f - cos2_theta);
    f32 tan2_theta = sin2_theta / cos2_theta;

    f32 alpha = effective_alpha(w);
    f32 alpha2 = alpha*alpha;
    return (std::sqrt(1.f + alpha2 * tan2_theta) - 1.f) / 2.f;
  }
};
