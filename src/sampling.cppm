export module sampling;

import core;
import std;

// Shirley–Chiu concentric mapping. Produces a point uniformly distributed by area on the unit disk. 
// Required by sample_cosine_hemisphere, where the geometric interpretation of the mapping 
// (radial strips of the unit square mapped to radial strips of the disk) is what makes the cosine-weighted 
// distribution emerge naturally.
export Point2f sample_uniform_disk_concentric(RandomGenerator& rng)
{
  f32 u1 = rng.uniform();
  f32 u2 = rng.uniform();
  Point2f u = Point2f{u1, u2};

  // map u to [-1, +1]^2 and handle degeneracy at the origin
  Point2f u_offset = 2.f * u - Vector2f{1.f, 1.f};
  if (u_offset.x == 0.f && u_offset.y == 0.f)
    return Point2f{0.f, 0.f};

  // Apply concentric mapping to point
  f32 theta, r;
  if (std::abs(u_offset.x) > std::abs(u_offset.y))
  {
    r = u_offset.x;
    theta = PI_OVER_4 * (u_offset.y / u_offset.x);
  }
  else
  {
    r = u_offset.y;
    theta = PI_OVER_2 - PI_OVER_4 * (u_offset.x / u_offset.y);
  }
  return r * Point2f{std::cos(theta), std::sin(theta)};
}

// Samples a direction wi on the local upper hemisphere with density proportional to cos_theta
export Vector3f sample_cosine_hemisphere(RandomGenerator& rng)
{
  // Sample uniformly on the disk (via concentric mapping)
  Point2f d = sample_uniform_disk_concentric(rng);
  // wi = (dx, dy, sqrt(1 - dx2 - dx2))
  f32 z = std::sqrt(std::max(0.f, 1.f - (d.x * d.x) - (d.y * d.y)));
  return Vector3f{d.x, d.y, z};
}

// Uniform-in-area point on the unit disk, with polar parametrization. 
// Required by the Heitz 2018 VNDF sampler in TrowbridgeReitzDistribution::sample_wm, 
// where the algorithm depends specifically on this parametrization (not the concentric one).
export Point2f sample_uniform_disk_polar(RandomGenerator& rng)
{
  f32 u1 = rng.uniform();
  f32 u2 = rng.uniform();

  f32 r = std::sqrt(u1);
  f32 theta = 2 * PI * u2;
  return Point2f{r * std::cos(theta), r * std::sin(theta)};
}
