export module sampling;

import core;
import std;

// Concentric Disk Sampling. A Low Distortion Map Between Disk and Square, Shirley & Chiu 1997 presents
// the idea for warping the unit square into that of a unit circle.
// This gives a uniform distribution of samples over the disk in Cartesian space.
// Given a disk centered origin (0,0) with radius r
export Point2f sample_uniform_disk_concentric(f32 radius, RandomGenerator& rng)
{
  f32 x{0.f}, y{0.f};

  // Draw two uniform random numbers in the range [0,1)
  f32 r1 = rng.uniform();
  f32 r2 = rng.uniform();

  // Avoid a potential divide by zero
  if (r1 == 0.f && r2 == 0.f)
    return Point2f{0.f, 0.f};

  // Initial mapping
  f32 phi{0.f};

  f32 a = (2.f * r1) - 1.f;
  f32 b = (2.f * r2) - 1.f;

  // Uses squares instead of absolute values
  f32 a2 = a*a;
  f32 b2 = b*b;
  if (a2 + b2 < 1e-6f)
    return Point2f{0.f, 0.f};

  if (a2 > b2)
  {
    // Top half
    radius *= a;
    phi = PI_OVER_4 * (b/a);
  }
  else
  {
    // Bottom half
    radius *= b;
    phi = PI_OVER_2 - (PI_OVER_4 * (a/b));
  }

  // Map the distorted Polar coordinates (phi,radius)
  // into the Cartesian (x,y) space
  x = std::cos(phi) * radius;
  y = std::sin(phi) * radius;
  return Point2f{x,y};
}

// Malley’s Method. To sample direction vectors from a cosine-weighted distribution,
// uniformly sample points on the unit disk and project them up to the unit hemisphere.
export Vector3f sample_cosine_hemisphere(RandomGenerator& rng)
{
  // Sample uniformly on the unit disk (via concentric mapping)
  Point2f d = sample_uniform_disk_concentric(1.f, rng);
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

// Draws a direction uniformly at random on the unit sphere S^2, with density
// P(w) = 1/(4pi) with respect to solid angle.
export Vector3f sample_uniform_sphere(RandomGenerator& rng)
{
  // Every point on the unit sphere S^2 can be written with the spherical coordinates: (theta, phi)
  // Drawing theta and phi independently and uniformly looks harmless, but it packs far too many points near the poles.
  // The fix is to stop parametrizing by theta and switch to z=cos_theta.
  // In the coordinates (z, phi), area is exactly uniform.
  // Z ~ unif(-1, 1)
  // Phi ~ unif(0, 2pi)
  // P = (sqrt(1-z^2) * cos_phi, sqrt(1-z^2) * sin_phi, Z)

  f32 u1 = rng.uniform();
  f32 u2 = rng.uniform();
  f32 z = 1.f - 2.f * u1; // z ~ unif(-1, 1)
  f32 phi = 2.f * PI * u2; // phi ~ Uniform(0, 2pi)
  f32 r = std::sqrt(std::max(0.f, 1.f - z*z)); // radius of the circle at height z
  return Vector3f{r * std::cos(phi), r * std::sin(phi), z};
}
