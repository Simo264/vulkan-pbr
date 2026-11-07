export module materials:conductor;

import core;
import std;




// For conductors such as metals, the IOR becomes a complex number:
//  tilde(eta) = eta - ik
//
// where
// - eta: is the usual refractive part, controlling phase velocity and refraction
// - k:   is the absorption coefficient, controlling how rapidly light decays inside the material
//
// The complex IOR still strongly affects the reflected light, which is why gold, silver, copper, and aluminum
// have different colors and reflection behavior.
export struct Conductor
{
  Color3f eta{ 1.f, 1.f, 1.f }; // material's IOR per channel (r g b)
  Color3f k  { 0.f, 0.f, 0.f }; // absorption coefficient per channel (r g b)
};

export f32 fresnel_conductor_channel(f32 cos_i, f32 eta, f32 k)
{
  using Complex = std::complex<f32>;
  cos_i = std::clamp(cos_i, 0.f, 1.f);

  // tilde(eta) = eta - ik
  Complex eta_t{ eta, -k };

  // sin^2 theta_i
  f32 sin2_i = 1.f - cos_i * cos_i;

  // Snell: sin theta_t = sin theta_i / tilde(eta)
  //        sin^2 theta_t = sin^2 theta_i / tilde(eta)^2
  Complex sin2_t = sin2_i / (eta_t * eta_t);

  // cos theta_t = sqrt(1 - sin^2 theta_t)
  Complex cos_t = std::sqrt(Complex{ 1.f, 0.f } - sin2_t);

  // r_parallel = (tilde(eta) cos_i - cos_t) / (tilde(eta) cos_i + cos_t)
  Complex eta_cos_i = eta_t * cos_i;
  Complex r_parallel = (eta_cos_i - cos_t) / (eta_cos_i + cos_t);

  // r_perp = (cos_i - tilde(eta) cos_t) / (cos_i + tilde(eta) cos_t)
  Complex eta_cos_t = eta_t * cos_t;
  Complex r_perp = (cos_i - eta_cos_t) / (cos_i + eta_cos_t);

  // F_r = 1/2 (|r_parallel|^2 + |r_perp|^2)
  // std::norm(z) = |z|^2
  return 0.5f * (std::norm(r_parallel) + std::norm(r_perp));
}

export Color3f fresnel_conductor(f32 cos_i, const Color3f& eta, const Color3f& k)
{
    return Color3f{
        fresnel_conductor_channel(cos_i, eta.r, k.r),
        fresnel_conductor_channel(cos_i, eta.g, k.g),
        fresnel_conductor_channel(cos_i, eta.b, k.b)
    };
}
