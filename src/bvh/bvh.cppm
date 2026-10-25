export module bvh;
export import :bbox;

import std;

export struct BVHNode
{
  BoundingBox3 aabb{};

  std::unique_ptr<BVHNode> left;
  std::unique_ptr<BVHNode> right;

  i32 primitive_idx{ -1 };
  i32 num_primitives{ 0 };

  bool is_leaf() const noexcept { return num_primitives > 0; }
};
