export module materials;

import core;
import std;

export struct Diffuse
{
  Color3f albedo{0.9f}; // reflactance
};

export struct Dielectric
{
  f32 eta{ 1.5f }; // The material's IOR (e.g. glass = 1.5)
};

export struct Conductor
{
  Color3f eta{ 1.f, 1.f, 1.f }; // material's IOR per channel (r g b)
  Color3f k  { 0.f, 0.f, 0.f }; // absorption coefficient per channel (r g b)
  // roughness
  f32 alpha_x{ 0.05f };   // roughness along x (tangent)
  f32 alpha_y{ 0.05f };   // if (alpha_y = alpha_x) -> isotropic
};

export using Material = std::variant<Diffuse, Conductor, Dielectric>;
