export module materials;

export import :diffuse;
export import :conductor;
export import :dielectric;

import core;
import std;

export using Material = std::variant<Diffuse, Conductor, Dielectric>;

// Here, the material CHOOSES wi (importance sampling) and returns
// a BSDFSample (wi, f, pdf, cos_i). It is used when the direction is ours to pick,
// i.e. to continue a path (indirect lighting).
export BSDFSample sample_material(Diffuse mat, Vector3f wo, RandomGenerator& rng);
export BSDFSample sample_material(Conductor mat, Vector3f wo, RandomGenerator& rng);
export BSDFSample sample_material(Dielectric mat, Vector3f wo, RandomGenerator& rng);

// Evaluates the BSDF f(wo, wi) for a given pair of directions, both expressed
// in the local shading frame (normal = +z). It returns only the value of f.
// Here, wi is GIVEN from outside (e.g. the direction toward a light) and
// the material only answers "how much light is scattered from wi to wo?".
// No random numbers and no pdf are involved.
export Color3f eval_material(Diffuse mat, Vector3f wo, Vector3f wi);
export Color3f eval_material(Conductor mat, Vector3f wo, Vector3f wi);
export Color3f eval_material(Dielectric mat, Vector3f wo, Vector3f wi);
