export module primitives.triangle_mesh;

import primitives.triangle;
import core;
import hit_record;
import camera;
import std;

export class TriangleMesh
{
public:
  TriangleMesh(Color3f reflectance = Color3f{1.0f}) : reflectance{reflectance} {}

  i32 triangle_count() const { return static_cast<i32>(indices.size()); }

  TriangleMesh& apply_transformation(const Transform& t)
  {
    for (auto& p : positions)
      p = t(p);

    for (auto& n : normals)
    {
      Normal3f nn{n.x, n.y, n.z};
      Normal3f tn = t(nn);
      n = normalize(Vector3f{tn.x, tn.y, tn.z});
    }

    return *this;
  }

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min, f32 t_max) const
  {
    std::optional<HitRecord> best;
    f32 closest = t_max;

    auto has_normals = !normals.empty();

    for (auto i = 0ul; i < triangle_count(); ++i)
    {
      const Vector3i& idx = indices[i];
      const Point3f& p0 = positions[idx.x];
      const Point3f& p1 = positions[idx.y];
      const Point3f& p2 = positions[idx.z];
      Triangle tri { p0,p1,p2 };

      auto tri_hit = tri.intersect(ray, t_min, closest);
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
        hit.n = normalize(tri_hit->b0 * n0 + tri_hit->b1 * n1 + tri_hit->b2 * n2);
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

  std::vector<Point3f>          positions{};
  std::vector<Vector3f>         normals{};
  std::vector<Point2f>          uvs{};
  std::vector<Vector3i>         indices{};  // for each triangle: (i0, i1, i2)
  Color3f                       reflectance{};
};
