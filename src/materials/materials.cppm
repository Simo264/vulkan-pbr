export module materials;

export import :diffuse;
export import :conductor;

import std;

// export struct BSDFSample
// {
//   Vector3f wi{};  // sampled incoming direction, in shading space
//   Color3f  f{};   // BSDF value f(wo, wi)
//   f32      pdf{}; // probability density of sampling wi given wo
// };

export using Material = std::variant<Diffuse, Conductor>;
