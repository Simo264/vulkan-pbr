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
  std::shared_ptr<ImageHDR> radiance_map{}; // HDR image that represents the distribution of incident radiance in a real-world environment
  Transform render_from_light{};            // The transform is a rotation that orients the environment map.
  f32 r{0.f};                               // radius of the sphere that surrounds the scene, used to compute the distance to the light source
};

// Maps a point (u, v) in [0,1]^2 to a unit direction on the sphere.
// The mapping is ‘equal-area’: equal areas in the square correspond to equal solid angles on the sphere.
// This forms the basis for sampling the environment uniformly with respect to the solid angle.
Vector3f equalarea_square_to_sphere(Point2f p)
{
  f32 u = 2.f * p.x - 1.f; // from [-1, 1]^2 to [0,1]^2
  f32 v = 2.f * p.y - 1.f; // from [-1, 1]^2 to [0,1]^2

  f32 up = std::abs(u);
  f32 vp = std::abs(v);

  // "signed distance" tells us whether the point is in the inner diamond
  //    |u|+|v| < 1     -> north hemisphere
  //    |u|+|v| > 1     -> south hemisphere
  f32 signed_distance = 1 - (up + vp);
  f32 d = std::abs(signed_distance);  // is the distance from the edge of diamond
  f32 r = 1 - d;                      // r=0 -> center;   r=1 -> on the edge
  f32 sqr_r = r*r;

  // phi = ((vp - up)/r + 1) * pi/4
  f32 phi = (r == 0.f ? 1.f : (vp - up) / r + 1.f) * PI / 4.f;

  // z = 1 - r^2.
  f32 z = std::copysign(1.f - sqr_r, signed_distance);

  // (x,y) coordinate are given from projection of the disk of radius r onto the hemisphere
  f32 cos_phi = std::copysign(std::cos(phi), u);
  f32 sin_phi = std::copysign(std::sin(phi), v);

  return Vector3f{
    cos_phi * r * std::sqrt(std::max(0.f, 2.f - sqr_r)),
    sin_phi * r * std::sqrt(std::max(0.f, 2.f - sqr_r)),
  z};
}

// This is the inverse of equalarea_square_to_sphere().
// This function maps a unit direction `d` (d.x, d.y, d.z) on the sphere to a point (u,v) in [0,1]^2.
// This mapping is "equal-area": an infinitesimal area dA on the square corresponds to a solid angle onto the sphere.
// dw = 4PI dA
//
// This is fundamental for sampling the environment uniformly with respect to the solid angle.
Point2f equalarea_sphere_to_square(Vector3f d)
{
  f32 x_abs = std::abs(d.x);
  f32 y_abs = std::abs(d.y);
  f32 z_abs = std::abs(d.z);

  // recover r from |z| = 1 − r^2
  f32 r = std::sqrt(std::max(0.f, 1.f - z_abs));

  // phi = atan2(|y|, |x|).
  f32 phi = 0.f;
  if(r != 0.f)
    phi = std::atan2(y_abs, x_abs);

  f32 signed_distance = 1.f - r;
  if(d.z < 0.f)
    signed_distance = r - 1.f;

  // phi      = ((vp - up)/r + 1) * PI/4
  // up + vp  = 1 - signed_distance
  f32 up = ((1.f - signed_distance) - r * (4.f * phi / PI - 1.f)) / 2.f;
  f32 vp = ((1.f - signed_distance) + r * (4.f * phi / PI - 1.f)) / 2.f;

  f32 u = std::copysign(up, d.x);
  f32 v = std::copysign(vp, d.y);

  // map from [-1,1]^2 to [0,1]^2.
  return Point2f{ (u + 1.f) / 2.f, (v + 1.f) / 2.f };
}

// Returns the radiance emitted by the environment in the direction w.
// The w direction is the direction of the ray that has left the scene
// (e.g. a ray reflected by a mirror or refracted by glass).
// The environment is defined by a radiance map (ImageHDR) in octahedral equal-area parameterisation.
export Color3f L(const ImageInfiniteLight& light, Point3f /*pe*/, Vector3f /*ne*/, Vector3f w)
{
  // We transform w from the rendering space to the light space using the inverse of render_from_light
  Vector3f w_light = normalize(inverse(light.render_from_light)(w));

  // Let’s convert the direction into (u, v) coordinates within the square [0,1]^2 using the equal-area projection.
  Point2f uv = equalarea_sphere_to_square(w_light);

  // Look up the nearest pixel.
  i32 width = light.radiance_map->width;
  i32 height = light.radiance_map->height;
  i32 x = std::clamp(static_cast<i32>(uv.x * width),  0, width  - 1);
  i32 y = std::clamp(static_cast<i32>(uv.y * height), 0, height - 1);
  i32 idx = (y * width + x) * light.radiance_map->channels;
  return Color3f{light.radiance_map->pixels.at(idx + 0),
                 light.radiance_map->pixels.at(idx + 1),
                 light.radiance_map->pixels.at(idx + 2)};
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
