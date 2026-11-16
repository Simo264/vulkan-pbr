export module materials:diffuse;

import core;
import bsdf;
import sampling;
import std;

// The Lambertian model describes a perfect diffuse surface that scatters incident illumination equally in all directions.
// It is a reasonable approximation to many real-world surfaces such as paper or matte paint.
export struct Diffuse
{
  Color3f albedo{0.9f}; // reflactance
};

// Here, the material CHOOSES wi (importance sampling) and returns
// a BSDFSample (wi, f, pdf, cos_i). It is used when the direction is ours to pick,
// i.e. to continue a path (indirect lighting).
export BSDFSample sample_material(Diffuse mat, Vector3f wo, RandomGenerator& rng)
{
  Vector3f wi = sample_cosine_hemisphere(rng);
  if (!same_hemisphere(wo, wi))
    return BSDFSample{};

  f32 cos_i = wi.z;
  if (cos_i <= 0.f)
    return BSDFSample{};

  BSDFSample s;
  s.wi      = wi;
  s.cos_i   = cos_i;
  s.f       = mat.albedo / PI;
  s.pdf     = cos_i / PI;
  s.valid   = true;
  return s;
}

// Evaluates the BSDF f(wo, wi) for a given pair of directions, both expressed
// in the local shading frame (normal = +z). It returns only the value of f.
// Here, wi is GIVEN from outside (e.g. the direction toward a light) and 
// the material only answers "how much light is scattered from wi to wo?".
// No random numbers and no pdf are involved.
export Color3f eval_material(Diffuse mat, Vector3f wo, Vector3f wi)
{
  if (!same_hemisphere(wo, wi))
    return Color3f{0.f};

  //   Lambertian reflection scatters light equally in all directions of the
  //   hemisphere, so the BRDF is the constant f = albedo / PI. The division by PI
  //   ensures energy conservation: the integral of f * cos over the hemisphere
  //   equals albedo.
  //   Reflection only: if wo and wi lie in opposite hemispheres, return 0.
  return mat.albedo / PI; // This is the BRDF function for Diffuse material
}
