export module materials;

export import :bsdf_sample;
export import :diffuse;
export import :conductor;

import std;

export using Material = std::variant<Diffuse>;
