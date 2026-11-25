export module light_source:image_infinite_light;

import core;
import std;

import image;

export import :light_sample;

// `ImageInfiniteLight` is a useful infinite light variation that uses an Image to define the directional distribution of emitted radiance.
// Given an image that represents the distribution of incident radiance in a real-world environment (*environment map*),
// this light can be used to render objects under the same illumination.
export struct ImageInfiniteLight
{
  std::shared_ptr<ImageHDR> radiance_map; // HDR image that represents the distribution of incident radiance in a real-world environment
  f32 r;  // radius of the sphere that surrounds the scene, used to compute the distance to the light source
  // rotation     -> Mat3x3 (light -> render)
  // distrib      -> PiecewiseConstant2D  (full distribution)
  // compensated  -> PiecewiseConstant2D  (compensated distribution)
};

Vector3f uv_to_direction(Point2f uv)
{
  return Vector3f{};
}

Point2f direction_to_uv(Vector3f w)
{
  return Point2f{};
}

export Color3f L(const ImageInfiniteLight&   light, Point3f pe, Vector3f ne, Vector3f w)
{
  return Color3f{0.f};
}

export LightSample sample_Li(const ImageInfiniteLight&  light, Point3f ps, RandomGenerator& rng)
{
  LightSample ls{
    .wi = Vector3f{0.f},
    .L  = Color3f{0.f},
    .pdf = 0.f,
    .dist = 0.f
  };
  return ls;
}
