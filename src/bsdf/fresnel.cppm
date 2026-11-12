export module bsdf:fresnel;

import core;
import std;


// Computes the unpolarized Fresnel reflectance Fr in [0,1] for one RGB channel of a conductor, given the complex IOR:
// tilde(eta) = eta - ik
export f32 fresnel_conductor_channel(f32 cos_i, f32 eta, f32 k)
{
  cos_i = std::clamp(cos_i, 0.f, 1.f);

  using Complex = std::complex<f32>;
  Complex eta_t{ eta, -k };
  f32 sin2_i = 1.f - cos_i * cos_i;
  Complex sin2_t = sin2_i / (eta_t * eta_t);
  Complex cos_t = std::sqrt(Complex{ 1.f, 0.f } - sin2_t);
  Complex eta_cos_i = eta_t * cos_i;
  Complex r_parallel = (eta_cos_i - cos_t) / (eta_cos_i + cos_t);
  Complex eta_cos_t = eta_t * cos_t;
  Complex r_perp = (cos_i - eta_cos_t) / (cos_i + eta_cos_t);
  return 0.5f * (std::norm(r_parallel) + std::norm(r_perp));
}

// Per-channel wrapper: applies complex_fresnel_channel to R, G, B with the corresponding (eta, k) values.
// This is what produces colored metals (gold warm, silver neutral, copper reddish).
export Color3f fresnel_conductor(f32 cos_i, Color3f eta, Color3f k)
{
  return Color3f{
    fresnel_conductor_channel(cos_i, eta.r, k.r),
    fresnel_conductor_channel(cos_i, eta.g, k.g),
    fresnel_conductor_channel(cos_i, eta.b, k.b)
  };
}

// Fresnel reflectance for a dielectric interface with real IORs on both sides.
// Handles Total Internal Reflection internally by returning 1.f when sin^2(theta_t) >= 1
export f32 fresnel_dielectric(f32 cos_theta_i, f32 eta_t, f32 eta_i)
{
  cos_theta_i   = std::clamp(cos_theta_i, 0.f, 1.f);
  f32 sin2_i    = 1.f - cos_theta_i * cos_theta_i;
  f32 eta_ratio = eta_i / eta_t;
  f32 sin2_t    = eta_ratio * eta_ratio * sin2_i;
  if (sin2_t >= 1.f)
    return 1.f;  // Total Internal Reflection
  f32 cos_t = std::sqrt(1.f - sin2_t);

  f32 r_par  = (eta_t * cos_theta_i - eta_i * cos_t)
              / (eta_t * cos_theta_i + eta_i * cos_t);
  f32 r_perp = (eta_i * cos_theta_i - eta_t * cos_t)
              / (eta_i * cos_theta_i + eta_t * cos_t);
  return 0.5f * (r_par * r_par + r_perp * r_perp);
}
