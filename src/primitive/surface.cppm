export module primitive:surface;

import core;
import std;

import material;

export struct Surface
{
  Material mat{};        // the surface material
  Color3f Le{0.f};       // emitted radiance (independent of material)
  bool two_sided{false}; // default false: emits only on the +normal side
  f32 area{0.f};         // 0 = invalid area light

  Color3f power() const
  {
    if(Le == Color3f{0.f}) return Color3f{0.f};

    if(two_sided)
      return 2.f * PI * area * Le;
    return PI * area * Le;
  }
};
