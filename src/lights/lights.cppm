export module lights;

export import :light_sample;
export import :point_light;
export import :diffuse_area_light;

import core;
import std;

export using Light = std::variant<PointLight, DiffuseAreaLight>;

// Its purpose is to return the radiance emitted by the light at a particular point `pe` in a particular direction `w`.
// Here, `ne` is the light’s geometric normal, the one that defines the emitting side.
export Color3f L(const DiffuseAreaLight& light, Point3f pe, Vector3f ne, Vector3f w);
export Color3f L(const PointLight&       light, Point3f pe, Vector3f ne, Vector3f w);

export LightSample sample_Li(const DiffuseAreaLight& light, Point3f ps, RandomGenerator& rng);
export LightSample sample_Li(const PointLight&       light, Point3f ps, [[maybe_unused]]RandomGenerator& rng);