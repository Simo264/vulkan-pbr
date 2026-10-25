export module primitives.triangle_mesh;

import primitives.triangle;
import core;
import hit_record;
import bvh;
import camera;
import std;

struct MeshBVHNode
{
  BoundingBox3 aabb{};
  std::unique_ptr<MeshBVHNode> left;
  std::unique_ptr<MeshBVHNode> right;
  i32 first_prim{-1};
  i32 num_primitives{0};
  bool is_leaf() const noexcept { return num_primitives > 0; }
};

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

    // Recalculate bbox after the transformation
    update_bbox();

    return *this;
  }

  void build_mesh_bvh()
  {
    if (indices.empty())
      return;

    bvh = build_node(0, static_cast<i32>(indices.size()));
  }

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min, f32 t_max) const
  {
    if (!bvh) 
      return std::nullopt;

    std::optional<HitRecord> best;
    f32 closest = t_max;
    intersect_node(bvh.get(), ray, t_min, closest, best);
    return best;
    
    //     std::optional<HitRecord> best;
    //     f32 closest = t_max;
    // 
    //     auto has_normals = !normals.empty();
    // 
    //     for (auto i = 0ul; i < triangle_count(); ++i)
    //     {
    //       const Vector3i& idx = indices[i];
    //       const Point3f& p0 = positions[idx.x];
    //       const Point3f& p1 = positions[idx.y];
    //       const Point3f& p2 = positions[idx.z];
    //       Triangle tri { p0,p1,p2 };
    // 
    //       auto tri_hit = tri.intersect(ray, t_min, closest);
    //       if (!tri_hit) continue;
    // 
    //       closest = tri_hit->t;
    // 
    //       HitRecord hit;
    //       hit.t = tri_hit->t;
    //       hit.p = ray(tri_hit->t);
    // 
    //       // normal vector: smooth id per-vertex normals are defined, geometric otherwise
    //       if (has_normals)
    //       {
    //         const Vector3f& n0 = normals[idx.x];
    //         const Vector3f& n1 = normals[idx.y];
    //         const Vector3f& n2 = normals[idx.z];
    //         hit.n = normalize(tri_hit->b0 * n0 + tri_hit->b1 * n1 + tri_hit->b2 * n2);
    //       }
    //       else
    //       {
    //         const Vector3f e1 = p1 - p0;
    //         const Vector3f e2 = p2 - p0;
    //         hit.n = normalize(cross(e1, e2));
    //       }
    // 
    //       if (dot(hit.n, ray.d) > 0.f) hit.n = -hit.n;
    // 
    //       hit.reflectance = reflectance;
    //       best = hit;
    //     }
    // 
    //     return best;
  }

  void update_bbox()
  {
    bbox = BoundingBox3{};
    for (const auto& p : positions)
      bbox.expand(p);
  }

  BoundingBox3 bounding_box() const { return bbox; }
  Point3f      centroid()     const { return bbox.centroid(); }

  std::vector<Point3f>          positions{};
  std::vector<Vector3f>         normals{};
  std::vector<Point2f>          uvs{};
  std::vector<Vector3i>         indices{};  // for each triangle: (i0, i1, i2)
  Color3f                       reflectance{};
  BoundingBox3                  bbox;
  std::unique_ptr<MeshBVHNode>  bvh;
  static constexpr i32          mesh_leaf_size = 4;

private:

  BoundingBox3 triangle_bbox(i32 tri_idx) const 
  {
    const Vector3i& idx = indices[tri_idx];
    BoundingBox3 b;
    b.expand(positions[idx.x]);
    b.expand(positions[idx.y]);
    b.expand(positions[idx.z]);
    return b;
  }
  
  f32 centroid_axis(i32 tri_idx, int axis) const 
  {
    const Vector3i& idx = indices[tri_idx];
    return (positions[idx.x][axis]
          + positions[idx.y][axis]
          + positions[idx.z][axis]) * (1.f / 3.f);
  }
  
  std::unique_ptr<MeshBVHNode> build_node(i32 first, i32 count)
  {
    auto node = std::make_unique<MeshBVHNode>();
    
    // 1) AABB del nodo: unione dei bbox dei triangoli nel range
    BoundingBox3 b;
    for (i32 i = first; i < first + count; ++i)
      b.expand(triangle_bbox(i));
    node->aabb = b;

    // 2) foglia
    if (count <= mesh_leaf_size) 
    {
      node->first_prim     = first;
      node->num_primitives = count;
      return node;
    }

    // 3) asse più lungo dell'AABB
    Vector3f extent = node->aabb.max - node->aabb.min;
    i32 axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;

    // split al centro dell'AABB lungo quell'asse
    f32 split_pos = node->aabb.min[axis] + 0.5f * extent[axis];

    // 4) partizione in-place per centroide < split_pos
    i32 i = first;
    i32 j = first + count - 1;
    while (i <= j) 
    {
      if (centroid_axis(i, axis) < split_pos) 
      {
        ++i;
      } 
      else 
      {
        std::swap(indices[i], indices[j]);
        --j;
      }
    }

    i32 left_count  = i - first;
    i32 right_count = count - left_count;

    // 5) split degenere → foglia con tutte le primitive (safe fallback)
    if (left_count == 0 || right_count == 0) 
    {
      node->first_prim     = first;
      node->num_primitives = count;
      return node;
    }

    // 6) ricorri
    node->left  = build_node(first,             left_count);
    node->right = build_node(first + left_count, right_count);
    return node;
  }

  void intersect_node(const MeshBVHNode* node, const Ray& ray, f32 t_min, f32& closest, std::optional<HitRecord>& best) const
  {
    // 1) test AABB: scarta il sottoalbero se il raggio non ci entra
    if (!node->aabb.intersect(ray, t_min, closest)) return;

    // 2) foglia: testa i triangoli del range
    if (node->is_leaf()) 
    {
      const bool has_normals = !normals.empty();
      for (i32 i = 0; i < node->num_primitives; ++i) 
      {
        const Vector3i& idx = indices[node->first_prim + i];

        Triangle tri{ positions[idx.x], positions[idx.y], positions[idx.z] };

        auto tri_hit = tri.intersect(ray, t_min, closest);
        if (!tri_hit) 
          continue;

        closest = tri_hit->t;

        HitRecord hit;
        hit.t = tri_hit->t;
        hit.p = ray(tri_hit->t);
        if (has_normals) 
        {
          hit.n = normalize(tri_hit->b0 * normals[idx.x]
                          + tri_hit->b1 * normals[idx.y]
                          + tri_hit->b2 * normals[idx.z]);
        } 
        else
        {
          Vector3f e1 = positions[idx.y] - positions[idx.x];
          Vector3f e2 = positions[idx.z] - positions[idx.x];
          hit.n = normalize(cross(e1, e2));
        }

        if (dot(hit.n, ray.d) > 0.f) hit.n = -hit.n;

        hit.reflectance = reflectance;
        best = hit;
      }
      return;
    }

    // 3) nodo interno: ricorri
    intersect_node(node->left.get(),  ray, t_min, closest, best);
    intersect_node(node->right.get(), ray, t_min, closest, best);
  }
  
};
