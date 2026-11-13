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
