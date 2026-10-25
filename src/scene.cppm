export module scene;

import core;
import hit_record;
import camera;
import bvh;
import std;

import primitives.plane;
import primitives.disk;
import primitives.sphere;
import primitives.triangle_mesh;

export using SceneObject = std::variant<Plane, Disk, Sphere, TriangleMesh>;

export class Scene
{
public:
  explicit Scene() : m_objects{} {}

  void add(SceneObject obj) { m_objects.push_back(std::move(obj)); }

  void build_bvh()
  {
    if (m_objects.empty())
      return;

    m_bvh = build_recursive(0, static_cast<i32>(m_objects.size()));
  }

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min = 1e-4f, f32 t_max = MAX_FLOAT_32) const
  {
    if (!m_bvh)
      return std::nullopt;

    std::optional<HitRecord> best = std::nullopt;
    f32 closest = t_max;

    intersect_node(m_bvh.get(), ray, t_min, closest, best);
    return best;

    //     for (const auto& obj : m_objects)
    //     {
    //       std::visit([&](const auto& o) {
    //         if (auto hit = o.intersect(ray, t_min, closest))
    //         {
    //           closest = hit->t;
    //
    //           Vector3f n = hit->n;
    //           if (dot(n, ray.d) > 0.f)
    //             n = -n;
    //
    //           best = HitRecord{ hit->t, hit->p, n, o.reflectance };
    //         }
    //       }, obj);
    //     }
    //     return best;
  }

private:
  BoundingBox3 bbox_of(i32 prim) const { return std::visit([](const auto& o){ return o.bounding_box(); }, m_objects.at(prim)); }
  Point3f centroid_of(i32 prim) const { return std::visit([](const auto& o){ return o.centroid(); }, m_objects.at(prim)); }

  std::unique_ptr<BVHNode> build_recursive(i32 first, i32 count)
  {
    auto node = std::make_unique<BVHNode>();

    // 1) Calculate the node's AABB
    BoundingBox3 b;
    for (i32 i = first; i < first + count; ++i)
      b.expand(bbox_of(i));
    node->aabb = b;

    // 2) is leaf?
    constexpr auto leaf_size = 4;
    if (count <= leaf_size)
    {
      node->primitive_idx  = first;
      node->num_primitives = count;
      return node;
    }

    // 3) get the longest axis and split
    Vector3f extent = node->aabb.max - node->aabb.min;
    i32 axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;
    f32 split_pos = node->aabb.min[axis] + 0.5f * extent[axis];

    // 4) partizione in-place di m_objects[first..first+count)
    i32 i = first;
    i32 j = first + count - 1;
    while (i <= j)
    {
      if (centroid_of(i)[axis] < split_pos)
      {
        ++i;
      }
      else
      {
        std::swap(m_objects[i], m_objects[j]);
        --j;
      }
    }

    i32 left_count  = i - first;
    i32 right_count = count - left_count;

    // 5) split degenere → foglia con tutte le primitive
    if (left_count == 0 || right_count == 0)
    {
      node->primitive_idx  = first;
      node->num_primitives = count;
      return node;
    }

    // 6) ricorri
    node->left  = build_recursive(first, left_count);
    node->right = build_recursive(first + left_count, right_count);
    return node;
  }

  void intersect_node(const BVHNode* node,
                      const Ray& ray,
                      f32 t_min,
                      f32& closest,
                      std::optional<HitRecord>& best) const
  {
    // 1) test AABB: se il raggio non entra nel box, salta il sottoalbero
    if (!node->aabb.intersect(ray, t_min, closest)) return;

    if (node->is_leaf())
    {
      for (i32 i = 0; i < node->num_primitives; ++i)
      {
        const SceneObject& obj = m_objects[node->primitive_idx + i];
        if (auto hit = std::visit([&](const auto& o){ return o.intersect(ray, t_min, closest); }, obj))
        {
          closest = hit->t;
          best    = hit;
        }
      }
      return;
    }

    // 3) nodo interno: ricorri sui figli
    intersect_node(node->left.get(),  ray, t_min, closest, best);
    intersect_node(node->right.get(), ray, t_min, closest, best);
  }

  std::vector<SceneObject> m_objects;
  std::unique_ptr<BVHNode> m_bvh;
};









//   void bvh_subdivide(i32 node_idx, i32 first, i32 count)
//   {
//     auto& node = m_bvh[node_idx];
//
//     // (1) AABB of the node given as the union of the primitives
//     auto b = calculate_node_aabb(first, count);
//     node.aabb = b;
//
//     // (2) is leaf? Stop
//     if (count <= m_leaf_size)
//     {
//       node.first_prim = first;
//       node.prim_count = count;
//       return;
//     }
//
//     // (3) split axis: the longest axis of the node’s AABB
//     Vector3f extent = node.aabb.max - node.aabb.min;
//     i32 axis = 0;
//     if (extent.y > extent.x) axis = 1;
//     if (extent.z > extent[axis]) axis = 2;
//
//     // split position: halfway along the AABB along that axis
//     f32 split_pos = node.aabb.min[axis] + 0.5f * extent[axis];
//
//     // (4) partitioning in-place (quicksort style)
//     //    left:   centroid[axis] < split_pos
//     //    right:  centroid[axis] >= split_pos
//     i32 i = first;
//     i32 j = first + count - 1;
//     while (i <= j)
//     {
//       if (centroid_of(m_prim_order[i])[axis] < split_pos)
//       {
//         ++i;
//       }
//       else
//       {
//         std::swap(m_prim_order[i], m_prim_order[j]);
//         --j;
//       }
//     }
//     i32 left_count  = i - first;
//     i32 right_count = count - left_count;
//
//     // (5) degenerate split: all primitives on one side. Convert it to a leaf node using all primitives (safe fallback)
//     if (left_count == 0 || right_count == 0)
//     {
//       node.first_prim = first;
//       node.prim_count = count;
//       return;
//     }
//
//     // (6) The two children are always stored consecutively in memory
//     i32 left_child = static_cast<i32>(m_bvh.size());
//     m_bvh.emplace_back(); // left_child
//     m_bvh.emplace_back(); // right_child = left_child + 1
//
//     node.left       = left_child;
//     node.first_prim = -1;
//     node.prim_count = 0; // this is not a leaf
//
//     // (7) recursion
//     bvh_subdivide(left_child,      first,              left_count);
//     bvh_subdivide(left_child + 1,  first + left_count, right_count);
//   }
