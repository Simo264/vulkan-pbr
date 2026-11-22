export module lights:uniform_infinite_light;

import :light_sample;

import core;
import primitives;
import sampling;
import std;

// UniformInfiniteLight represents environmental lighting at infinity: a light source that surrounds the entire scene
// and emits the same radiance Le in every direction. Unlike a point light or a DiffuseAreaLight, it has no position,
// no shape, and no surface; it is a directional radiance field.
export struct UniformInfiniteLight
{
  UniformInfiniteLight() = default;
  UniformInfiniteLight(Color3f Le, f32 r) :
    Le{Le},
    r{r},
    power{4 * PI*PI * r*r * Le} // 4 * pi^2 * scene_radius^2 * Lemit
  {}

  Color3f Le{};       // constant emitted radiance
  Color3f power{};    // total emitted flux
  f32 r{};            // the radius of a bounding sphere enclosing the entire scene
};

export Color3f L(const UniformInfiniteLight& light, Point3f, Vector3f, Vector3f)
{
  // Uniform emission in every direction; pe, ne, w are irrelevant
  return light.Le;
}

export LightSample sample_Li(const UniformInfiniteLight& light, Point3f ps, RandomGenerator& rng)
{
  // Returns a direction sampled uniformly over the full sphere
  // and the radiance emitted along it, for the shading point `ps`.
  // There is no surface point `pe` to choose and no area-to-solid-angle Jacobian to apply:
  // the pdf is already a solid-angle pdf, p(w) = 1/(4 pi)

  Vector3f wi   = sample_uniform_sphere(rng);         // full S^2, uniform
  f32      pdf  = 1.0f / (4.0f * PI);                 // solid-angle pdf
  f32      dist = 2.0f * light.r;                     // certainly outside the scene
  Color3f  Li   = L(light, Point3f{}, Vector3f{}, Vector3f{});  // = light.Le
  return LightSample{ wi, Li, pdf, dist };
}
