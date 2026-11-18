export module core.math:frame;

import :vector;

import std;

// Building an Orthonormal Basis, Revisited (2017) by:
// Tom Duff, James Burgess, Per Christensen, Christophe Hery, Andrew Kensler Max Liani, and Ryusuke Villemin
export struct TangentFrame
{
  TangentFrame(Vector3f n)
  {
    f32 sign = std::copysign(1.0f, n.z);
    f32 a = -1.0f / (sign + n.z);
    f32 b = n.x * n.y * a;
    t = Vector3f{1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x};
    s = Vector3f{b, sign + n.y * n.y * a, -n.y};
    this->n = n;
  }

  // world to local
  Vector3f to_local(Vector3f v) { return { dot(v, s), dot(v, t), dot(v, n) }; }
  // local to world
  Vector3f to_world(Vector3f v) { return v.x * s + v.y * t + v.z * n; }

  Vector3f n; // normal
  Vector3f t; // tangent
  Vector3f s; // bitangent
};
