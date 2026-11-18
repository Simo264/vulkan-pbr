export module primitives:sphere;

import :primitive_sample;
import :surface;

import core;
import camera;
import hit_record;
import materials;
import std;

export struct Sphere
{
  explicit Sphere() = default;
  explicit Sphere(Point3f center, f32 radius, Material mat = Material{}, Color3f Le = Color3f{0.f}, bool two_sided = false) :
    center{center},
    radius{radius},
    surface{mat, Le, two_sided, 4.f * PI * radius * radius}
  {}

  Point3f  center{0.f}; // the world position
  f32      radius{1.f}; // sphere radius
  Surface  surface{};   // material, area, Le
};

export std::optional<HitRecord> intersect_primitive(const Sphere& sphere, Ray ray, f32 t_min, f32 t_max)
{
  Vector3f oc = ray.o - sphere.center;
  f32 b = dot(oc, ray.d);
  f32 c = dot(oc, oc) - sphere.radius * sphere.radius;
  f32 disc = b * b - c;

  if (disc < 0.f)
    return std::nullopt;

  f32 sqrt_d = std::sqrt(disc);

  // Find the nearest root that lies in the acceptable range.
  f32 t = -b - sqrt_d;
  if (t < t_min || t > t_max)
  {
    t = -b + sqrt_d;
    if (t < t_min || t > t_max)
      return std::nullopt;
  }

  Point3f p = ray(t);
  Vector3f n = (p - sphere.center) / sphere.radius;

  // denom < 0  ->  ray hits the front face (ray.d points INTO the surface)
  // denom > 0  ->  ray hits the back face
  f32 denom = dot(n, ray.d);
  Color3f Le{0.f};
  if(sphere.surface.two_sided || denom < 0.f)
    Le = sphere.surface.Le;

  return HitRecord{t,p,n,sphere.surface.mat,Le};
}

export PrimitiveSample sample_primitive_area(const Sphere& sphere, RandomGenerator& rng)
{
  // Every point on the unit sphere S^2 can be written with the spherical coordinates: (theta, phi)
  // Drawing theta and phi independently and uniformly looks harmless, but it packs far too many points near the poles.
  // The fix is to stop parametrizing by theta and switch to z=cos_theta.
  // In the coordinates (z, phi), area is exactly uniform.
  // Z ~ unif(-1, 1)
  // Phi ~ unif(0, 2pi)
  // P = (sqrt(1-z^2) * cos_phi, sqrt(1-z^2) * sin_phi, Z)

  f32 u1 = rng.uniform(); // [0,1)
  f32 u2 = rng.uniform(); // [0,1)
  f32 z  = 1.f - 2.f * u1;    // z ~ Uniform(-1,1)
  f32 phi = 2.f * PI * u2;  // phi ~ uniform[0, 2π)

  f32 r_xy = std::sqrt(std::max(0.f, 1.f - z*z));
  Vector3f n = Vector3f{ r_xy * std::cos(phi), r_xy * std::sin(phi), z };
  Point3f p = sphere.center + sphere.radius * n;
  f32 pdf_area = 1.f / sphere.surface.area;
  return PrimitiveSample{ p, n, pdf_area };
}
