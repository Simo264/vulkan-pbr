export module light_source;

export import :light_sample;
export import :point_light;
export import :diffuse_area_light;
export import :uniform_infinite_light;
export import :image_infinite_light;

import core;
import std;

export using LightSource = std::variant<PointLight,
                                        DiffuseAreaLight,
                                        UniformInfiniteLight,
                                        ImageInfiniteLight>;

export using InfiniteLightSource = std::variant<UniformInfiniteLight, ImageInfiniteLight>;

// Returns the radiance emitted by the light at point `p_emitted` (pe) in direction w.
// The `n_emitted` (ne) is the light’s geometric normal at `pe`
export Color3f L(const PointLight&           light, Point3f pe, Vector3f ne, Vector3f w);
export Color3f L(const DiffuseAreaLight&     light, Point3f pe, Vector3f ne, Vector3f w);
export Color3f L(const UniformInfiniteLight& light, Point3f pe, Vector3f ne, Vector3f w);
export Color3f L(const ImageInfiniteLight&   light, Point3f pe, Vector3f ne, Vector3f w);

// Picks one representative point `pe` on the light’s surface and returns the light contribution toward `p_surface` (ps).
export LightSample sample_Li(const PointLight&            light, Point3f ps, RandomGenerator& rng);
export LightSample sample_Li(const DiffuseAreaLight&      light, Point3f ps, RandomGenerator& rng);
export LightSample sample_Li(const UniformInfiniteLight&  light, Point3f ps, RandomGenerator& rng);
export LightSample sample_Li(const ImageInfiniteLight&  light, Point3f ps, RandomGenerator& rng);
