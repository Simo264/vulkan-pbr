export module materials:bsdf_sample;

import core;

export struct BSDFSample
{
  Vector3f wi{};  // sampled incoming direction, in shading space
  Color3f  f{};   // BSDF value f(wo, wi)
  f32      pdf{}; // probability density of sampling wi given wo
};
