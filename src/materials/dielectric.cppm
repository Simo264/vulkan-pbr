export module materials:dielectric;

import core;
import std;

export struct Dielectric
{
  f32 eta{ 1.5f };  // The material's IOR (e.g. glass = 1.5)
};

// Fresnel reflectance for a smooth dielectric interface (real-valued IORs on both sides).
// Returns Fr in [0, 1]. Handles TIR internally (returns 1.f when sin^2(theta_t) >= 1).
export f32 real_fresnel_dielectric(f32 cos_theta_i, f32 eta_t, f32 eta_i)
{
  // Snell's law and Fresnel equations:
  //   sin^2(theta_t) = (eta_i / eta_t)^2 * sin^2(theta_i)
  //   cos(theta_t)   = sqrt(1 - sin^2(theta_t))
  //   r_parallel     = (eta_t * cos(theta_i) - eta_i * cos(theta_t))
  //                  / (eta_t * cos(theta_i) + eta_i * cos(theta_t))
  //   r_perp         = (eta_i * cos(theta_i) - eta_t * cos(theta_t))
  //                  / (eta_i * cos(theta_i) + eta_t * cos(theta_t))
  //   Fr             = 0.5 * (r_parallel^2 + r_perp^2)
  cos_theta_i = std::clamp(cos_theta_i, 0.f, 1.f);
  f32 sin2_i    = 1.f - cos_theta_i * cos_theta_i;
  f32 eta_ratio = eta_i / eta_t;
  f32 sin2_t    = eta_ratio * eta_ratio * sin2_i;
  if (sin2_t >= 1.f) return 1.f;  // TIR
  f32 cos_t = std::sqrt(1.f - sin2_t);

  f32 r_par  = (eta_t * cos_theta_i - eta_i * cos_t)
              / (eta_t * cos_theta_i + eta_i * cos_t);
  f32 r_perp = (eta_i * cos_theta_i - eta_t * cos_t)
              / (eta_i * cos_theta_i + eta_t * cos_t);
  return 0.5f * (r_par * r_par + r_perp * r_perp);
}

// Computes the transmitted (refracted) ray direction for a smooth
// dielectric interface, via Snell's law.
// Returns the transmitted ray direction (also pointing away from the surface,
// on the opposite side), or std::nullopt on Total Internal Reflection (TIR).
export std::optional<Vector3f> refract(const Vector3f& wo, f32 eta_rel)
{
  // Formulas:
  //   sin^2(theta_i) = wo.x^2 + wo.y^2
  //   sin^2(theta_t) = sin^2(theta_i) / eta_rel^2
  //   cos(theta_t)   = sqrt(1 - sin^2(theta_t))
  //   TIR when sin^2(theta_t) >= 1
  //   wi.x = -wo.x / eta_rel
  //   wi.y = -wo.y / eta_rel
  //   wi.z = -copysign(cos(theta_t), wo.z)     (crosses to the opposite side)
  //
  // Snell's law:
  //   eta_i * sin(theta_i) = eta_t * sin(theta_t)
  //   sin(theta_t) = sin(theta_i) / eta_rel
  f32 sin2_i = wo.x * wo.x + wo.y * wo.y;
  f32 sin2_t = sin2_i / (eta_rel * eta_rel);
  if (sin2_t >= 1.f) return std::nullopt;  // Total Internal Reflection
  f32 cos_t = std::sqrt(1.f - sin2_t);
  return Vector3f{
    -wo.x / eta_rel,
    -wo.y / eta_rel,
    -std::copysign(cos_t, wo.z)
  };
}
