export module materials;

export import :bsdf_sample;
export import :diffuse;

import std;

// export struct Conductor{};
// export struct Dielectric{};
export using Material = std::variant<Diffuse /* Conductor, Lambertian */>;
