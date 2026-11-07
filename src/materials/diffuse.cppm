export module materials:diffuse;

import core;
import std;

// The Diffuse/Lambertian model means that the surface appears to have the same brightness from any viewing direction:
// the outgoing radiance does not depend on w_o. The BRDF is constant:
//
//  f_r(p, w_i, w_o) = R / pi
//
export struct Diffuse
{
  Color3f albedo{0.9f}; // reflactance
};

// generates a random point within a unit disk:
//  x^2 + y^2 <= 1
// The distribution of points on the disk is uniform across the area.
Point2f concentric_sample_disk(RandomGenerator& rng)
{
  f32 u1 = rng.uniform();
  f32 u2 = rng.uniform();

  f32 ox = 2.f * u1 - 1.f;
  f32 oy = 2.f * u2 - 1.f;

  if (ox == 0.f && oy == 0.f) return Point2f{0.f, 0.f};

  f32 r, theta;
  if (std::abs(ox) > std::abs(oy))
  {
    r = ox;
    theta = (PI_OVER_4) * (oy / ox);
  }
  else
  {
    r = oy;
    theta = PI_OVER_2 - PI_OVER_4 * (ox / oy);
  }
  return Point2f{r * std::cos(theta), r * std::sin(theta)};
}

// generates a direction w_i using cosine-weighted hemisphere sampling
export Vector3f sample_cosine_hemisphere(RandomGenerator& rng)
{
  // uniform sampling of the disk
  Point2f d = concentric_sample_disk(rng);

  // Equation of the unit sphere:
  //  x^2 + y^2 + z^2 = 1
  //
  // Solve for z:
  //  z = sqrt(1 - x^2 - y^2)
  //
  // It is used to lift a point (x,y) from the disk onto the surface of the hemisphere.
  //
  // Note: by sampling the disk uniformly, you obtain a distribution across the hemisphere
  //       that favours directions close to the normal.
  f32 z = std::sqrt(std::max(0.f, 1.f - (d.x * d.x) - (d.y * d.y)));
  return Vector3f{d.x, d.y, z};
}
