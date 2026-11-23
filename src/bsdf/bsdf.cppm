export module bsdf;

export import :fresnel;
export import :trowbridge_reitz;

export struct BSDFSample
{
  Vector3f wi{0.f, 0.f, 0.f};      // local-space direction
  Color3f  f{0.f, 0.f, 0.f};       // The BSDF function: f = f_r + f_t
  f32      pdf{0.f};     // useful in Monte Carlo integration
  f32      cos_i{0.f};   // cosine between surface normal and the incident beam
  bool     valid{false};   // set false at any early-out
};

export bool same_hemisphere(Vector3f w, Vector3f wp)
{
  return w.z * wp.z > 0;
}
