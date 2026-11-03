export module materials:diffuse;

import :bsdf_sample;

import core;
import std;

// Lambertian diffuse material
export struct Diffuse
{
  Color3f albedo{0.9f};
};

// The hemispherical-directional reflectance
export Color3f rho(const Diffuse& mat,
                   [[maybe_unused]] const Vector3f& wo,
                   [[maybe_unused]] std::span<const Point2f> u)
{
  return mat.albedo;   // closed form, no Monte Carlo needed
}


// The BRDF is constant: f_r(wi, wo) = R / pi
export Color3f brdf(const Diffuse& mat,
                    const Vector3f& wo,
                    const Vector3f& wi)
{
  if (wo.z <= 0.f || wi.z <= 0.f) return Color3f{0.f};
  return mat.albedo / PI;
}


// the PDF by which you selected the sample direction: p(wi) = (cos theta_i) / (pi)
export f32 pdf([[maybe_unused]] const Diffuse& mat,
               const Vector3f& wo,
               const Vector3f& wi)
{
  if (wo.z <= 0.f || wi.z <= 0.f) return 0.f;
  return wi.z / PI;
}

// sample wi direction on the hemisphere
export BSDFSample sample(const Diffuse& mat,
                         const Vector3f& wo,
                         const Point2f& u,
                         [[maybe_unused]] RandomGenerator& rng)
{
  // It must be in the upper hemisphere
  if (wo.z <= 0.f) return {};

  // sample wi using cosine-weighted hemisphere sampling
  auto cosine_weighted_hemisphere_sampling = [](const Point2f& u) -> Vector3f {
    f32 r     = std::sqrt(u.x);
    f32 phi   = 2.f * PI * u.y;
    f32 x     = r * std::cos(phi);
    f32 y     = r * std::sin(phi);
    f32 z     = std::sqrt(std::max(0.f, 1.f - u.x));
    return Vector3f{x, y, z};
  };

  Vector3f wi = cosine_weighted_hemisphere_sampling(u);
  if (wi.z <= 0.f) return {};

  return BSDFSample {
    wi,
    brdf(mat, wo, wi),
    pdf(mat, wo, wi),
    true
  };
}
