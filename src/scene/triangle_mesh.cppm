export module scene:triangle_mesh;

import :hit_record;

import types;
import math;
import color;
import camera;
import std;

struct TriangleHit
{
  f32 t{};
  f32 b0{}, b1{}, b2{};  // barycentric coordinates
};
std::optional<TriangleHit> ray_triangle(const Ray& ray, const Point3f& p0, const Point3f& p1, const Point3f& p2, f32 t_min, f32 t_max);

export class TriangleMesh
{
public:
  TriangleMesh(Color3f reflectance = {0.3, 0.3, 0.8}) : reflectance{reflectance} {}
  TriangleMesh(const TriangleMesh&) = default;
  TriangleMesh(TriangleMesh&&) noexcept = default;
  TriangleMesh& operator=(const TriangleMesh&) = default;
  TriangleMesh& operator=(TriangleMesh&&) noexcept = default;

  i32 triangle_count() const { return static_cast<i32>(indices.size()); }

  TriangleMesh& scale(const Vector3f& s)
  {
    for (auto& p : positions) { p.x *= s.x; p.y *= s.y; p.z *= s.z; }
    for (auto& n : normals) { n.x /= s.x; n.y /= s.y; n.z /= s.z; n = normalize(n); }
    return *this;
  }
  TriangleMesh& translate(const Vector3f& t)
  {
    for (auto& p : positions) { p.x += t.x; p.y += t.y; p.z += t.z; }
    return *this;
  }
  TriangleMesh& rotate_x(f32 deg)
  {
    f32 a = to_radians(deg);
    f32 c = std::cos(a);
    f32 s = std::sin(a);

    for (auto& p : positions) { f32 y = p.y; f32 z = p.z; p.y = c * y - s * z; p.z = s * y + c * z; }
    for (auto& n : normals)   { f32 y = n.y; f32 z = n.z; n.y = c * y - s * z; n.z = s * y + c * z; }
    return *this;
  }
  TriangleMesh& rotate_y(f32 deg)
  {
    f32 a = to_radians(deg);
    f32 c = std::cos(a);
    f32 s = std::sin(a);

    for (auto& p : positions) { f32 x = p.x; f32 z = p.z; p.x =  c * x + s * z; p.z = -s * x + c * z; }
    for (auto& n : normals)   { f32 x = n.x; f32 z = n.z; n.x =  c * x + s * z; n.z = -s * x + c * z; }
    return *this;
  }
  TriangleMesh& rotate_z(f32 deg)
  {
    f32 a = to_radians(deg);
    f32 c = std::cos(a);
    f32 s = std::sin(a);

    for (auto& p : positions) { f32 x = p.x; f32 y = p.y; p.x = c * x - s * y; p.y = s * x + c * y; }
    for (auto& n : normals)   { f32 x = n.x; f32 y = n.y; n.x = c * x - s * y; n.y = s * x + c * y; }
    return *this;
  }
  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min = 1e-4f, f32 t_max = MAX_FLOAT_32) const
  {
    std::optional<HitRecord> best;
    f32 closest = t_max;

    auto has_normals = !normals.empty();

    for (auto i = 0ul; i < triangle_count(); ++i)
    {
      const Vector3i& idx = indices[i];
      const Point3f&  p0  = positions[idx.x];
      const Point3f&  p1  = positions[idx.y];
      const Point3f&  p2  = positions[idx.z];

      auto tri_hit = ray_triangle(ray, p0, p1, p2, t_min, closest);
      if (!tri_hit) continue;

      closest = tri_hit->t;

      HitRecord hit;
      hit.t = tri_hit->t;
      hit.p = ray(tri_hit->t);

      // normal vector: smooth id per-vertex normals are defined, geometric otherwise
      if (has_normals)
      {
        const Vector3f& n0 = normals[idx.x];
        const Vector3f& n1 = normals[idx.y];
        const Vector3f& n2 = normals[idx.z];
        hit.n = normalize(tri_hit->b0 * n0 +
                          tri_hit->b1 * n1 +
                          tri_hit->b2 * n2);
      }
      else
      {
        Vector3f e1 = p1 - p0;
        Vector3f e2 = p2 - p0;
        hit.n = normalize(cross(e1, e2));
      }

      if (dot(hit.n, ray.d) > 0.f) hit.n = -hit.n;

      hit.reflectance = reflectance;
      best = hit;
    }

    return best;
  }

  std::vector<Point3f>   positions{};
  std::vector<Vector3f>  normals{};
  std::vector<Point2f>   uvs{};
  std::vector<Vector3i>  indices{};  // for each triangle: (i0, i1, i2)
  Color3f reflectance{};
};

std::optional<TriangleHit> ray_triangle(const Ray&     ray,
                                        const Point3f& p0,
                                        const Point3f& p1,
                                        const Point3f& p2,
                                        f32            t_min,
                                        f32            t_max)
{
  // 1) Translate
  Vector3f p0t = p0 - ray.o;
  Vector3f p1t = p1 - ray.o;
  Vector3f p2t = p2 - ray.o;

  // 2) Permute (dominant axis) ---
  f32 adx = std::abs(ray.d.x);
  f32 ady = std::abs(ray.d.y);
  f32 adz = std::abs(ray.d.z);
  i32 kz = 0;
  if (ady > adx) kz = 1;
  if (adz > (kz == 0 ? adx : ady)) kz = 2;
  i32 kx = (kz + 1) % 3;
  i32 ky = (kx + 1) % 3;

  f32 dx = ray.d[kx];
  f32 dy = ray.d[ky];
  f32 dz = ray.d[kz];

  // --- 3) Shear ---
  f32 sx = dx / dz;
  f32 sy = dy / dz;
  f32 sz = 1.f / dz;

  // after sheare + scaling
  f32 a_x = p0t[kx] - sx * p0t[kz];
  f32 a_y = p0t[ky] - sy * p0t[kz];
  f32 a_z = p0t[kz] * sz;

  f32 b_x = p1t[kx] - sx * p1t[kz];
  f32 b_y = p1t[ky] - sy * p1t[kz];
  f32 b_z = p1t[kz] * sz;

  f32 c_x = p2t[kx] - sx * p2t[kz];
  f32 c_y = p2t[ky] - sy * p2t[kz];
  f32 c_z = p2t[kz] * sz;

  // 4) Edge functions
  f32 e0 = b_x * c_y - b_y * c_x;
  f32 e1 = c_x * a_y - c_y * a_x;
  f32 e2 = a_x * b_y - a_y * b_x;

  // edge test: same signs
  if ((e0 < 0.f || e1 < 0.f || e2 < 0.f) &&
      (e0 > 0.f || e1 > 0.f || e2 > 0.f))
      return std::nullopt;

  f32 det = e0 + e1 + e2;
  if (std::abs(det) < 1e-12f) return std::nullopt;

  // 5) Scaled t
  f32 t_scaled = e0 * a_z + e1 * b_z + e2 * c_z;

  // 6) t e baricentriche
  f32 inv_det = 1.f / det;
  f32 t = t_scaled * inv_det;

  // 7) Conservative t > 0
  if (t < t_min || t > t_max) return std::nullopt;

  TriangleHit hit;
  hit.t = t;
  hit.b0 = e0 * inv_det;
  hit.b1 = e1 * inv_det;
  hit.b2 = e2 * inv_det;
  return hit;
}
