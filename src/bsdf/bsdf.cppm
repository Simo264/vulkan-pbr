export module bsdf;

export import :geometry;
export import :fresnel;
export import :trowbridge_reitz;

export struct BSDFSample
{
  BSDFSample() : wi{}, f{}, pdf{}, cos_i{}, valid{ false }
  {}

  Vector3f wi;      // local-space direction
  Color3f  f;       // The BSDF function: f = f_r + f_t
  f32      pdf;     // useful in Monte Carlo integration
  f32      cos_i;   // cosine between surface normal and the incident beam
  bool     valid;   // set false at any early-out
};
