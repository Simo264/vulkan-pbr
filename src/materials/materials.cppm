export module materials;

export import :diffuse;
export import :conductor;
export import :dielectric;

export using Material = std::variant<Diffuse, Conductor, Dielectric>;
