export module bvh:bbox;

import core;
import camera;
import std;

export class BoundingBox3
{
public:
  explicit BoundingBox3() = default;
  explicit BoundingBox3(const Point3f& a, const Point3f& b) :
    min{ std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) },
    max{ std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) }
  {}

  bool is_empty() const { return min == max; }

  // Expand the box to include point p.
  BoundingBox3& expand(const Point3f& p) noexcept
  {
    min.x = std::min(min.x, p.x);
    min.y = std::min(min.y, p.y);
    min.z = std::min(min.z, p.z);
    max.x = std::max(max.x, p.x);
    max.y = std::max(max.y, p.y);
    max.z = std::max(max.z, p.z);
    return *this;
  }

  // Expand the box to include another box b.
  BoundingBox3& expand(const BoundingBox3& b)
  {
    min.x = std::min(min.x, b.min.x);
    min.y = std::min(min.y, b.min.y);
    min.z = std::min(min.z, b.min.z);
    max.x = std::max(max.x, b.max.x);
    max.y = std::max(max.y, b.max.y);
    max.z = std::max(max.z, b.max.z);
    return *this;
  }

  Point3f centroid() const
  {
    return Point3f{ 0.5f * (min.x + max.x),
                    0.5f * (min.y + max.y),
                    0.5f * (min.z + max.z) };
  }

  std::optional<f32> intersect(const Ray& ray, f32 t_min = 1e-4, f32 t_max = MAX_FLOAT_32) const
  {
    f32 t0 = t_min;
    f32 t1 = t_max;

    f32 inv_dx = 1.f / ray.d.x;
    f32 inv_dy = 1.f / ray.d.y;
    f32 inv_dz = 1.f / ray.d.z;

    // X
    {
      f32 t_near = (min.x - ray.o.x) * inv_dx;
      f32 t_far  = (max.x - ray.o.x) * inv_dx;
      if (t_near > t_far) std::swap(t_near, t_far);
      t0 = std::max(t0, t_near);
      t1 = std::min(t1, t_far);
      if (t0 > t1) return std::nullopt;
    }
    // Y
    {
      f32 t_near = (min.y - ray.o.y) * inv_dy;
      f32 t_far  = (max.y - ray.o.y) * inv_dy;
      if (t_near > t_far) std::swap(t_near, t_far);
      t0 = std::max(t0, t_near);
      t1 = std::min(t1, t_far);
      if (t0 > t1) return std::nullopt;
    }
    // Z
    {
      f32 t_near = (min.z - ray.o.z) * inv_dz;
      f32 t_far  = (max.z - ray.o.z) * inv_dz;
      if (t_near > t_far) std::swap(t_near, t_far);
      t0 = std::max(t0, t_near);
      t1 = std::min(t1, t_far);
      if (t0 > t1) return std::nullopt;
    }
    return t0;
  }

  Point3f min{};
  Point3f max{};
};
