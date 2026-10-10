export module math:transform;

import :utils;
import :matrix;
import :vector;
import :point;
import std;

export struct Transform
{
  Transform() : M{ Matrix4f::identity() }, M_inv{ Matrix4f::identity() } {}
  Transform(const Matrix4f& M) : M{ M }, M_inv{ inverse(M) } {}
  Transform(const Matrix4f& M, const Matrix4f& M_inv) : M{ M }, M_inv{ M_inv } {}

  // apply to Point. Full 4x4 with w = 1, then perspective divide if needed.
  constexpr Point3f operator()(const Point3f& p) const noexcept
  {
    const f32 xp = M.m[0][0]*p.x + M.m[0][1]*p.y + M.m[0][2]*p.z + M.m[0][3];
    const f32 yp = M.m[1][0]*p.x + M.m[1][1]*p.y + M.m[1][2]*p.z + M.m[1][3];
    const f32 zp = M.m[2][0]*p.x + M.m[2][1]*p.y + M.m[2][2]*p.z + M.m[2][3];
    const f32 wp = M.m[3][0]*p.x + M.m[3][1]*p.y + M.m[3][2]*p.z + M.m[3][3];

    if (wp == 1.0f)
      return Point3f{ xp, yp, zp };
    return Point3f{ xp / wp, yp / wp, zp / wp };
  }
  // apply to Vector. Only the upper-left 3x3, translation is ignored (w = 0).
  constexpr Vector3f operator()(const Vector3f& v) const noexcept
  {
    return Vector3f{
      M.m[0][0]*v.x + M.m[0][1]*v.y + M.m[0][2]*v.z,
      M.m[1][0]*v.x + M.m[1][1]*v.y + M.m[1][2]*v.z,
      M.m[2][0]*v.x + M.m[2][1]*v.y + M.m[2][2]*v.z
    };
  }
  // apply to Normal. Uses transpose(inverse(M)) = M_inv indexed in transposed order.
  constexpr Normal3f operator()(const Normal3f& n) const noexcept
  {
    return Normal3f{
      M_inv.m[0][0]*n.x + M_inv.m[1][0]*n.y + M_inv.m[2][0]*n.z,
      M_inv.m[0][1]*n.x + M_inv.m[1][1]*n.y + M_inv.m[2][1]*n.z,
      M_inv.m[0][2]*n.x + M_inv.m[1][2]*n.y + M_inv.m[2][2]*n.z
    };
  }

  constexpr bool is_identity() const noexcept { return M == Matrix4f::identity(); }

  Matrix4f M;
  Matrix4f M_inv;
};


export {
  Transform operator*(const Transform& a, const Transform& b) noexcept { return Transform{ a.M * b.M, b.M_inv * a.M_inv }; }
  bool operator==(const Transform& a, const Transform& b) noexcept { return a.M == b.M; }
  bool operator!=(const Transform& a, const Transform& b) noexcept { return !(a == b);}
  Transform inverse(const Transform& t) noexcept { return Transform{ t.M_inv, t.M }; }

  Transform scale(f32 sx, f32 sy, f32 sz) noexcept
  {
    Matrix4f m = Matrix4f::identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;

    Matrix4f m_inv = Matrix4f::identity();
    m_inv.m[0][0] = 1.0f / sx;
    m_inv.m[1][1] = 1.0f / sy;
    m_inv.m[2][2] = 1.0f / sz;

    return Transform{ m, m_inv };
  }
  Transform scale(f32 s) noexcept { return scale(s, s, s); }
  Transform scale(const Vector3f& v) noexcept { return scale(v.x, v.y, v.z); }

  Transform translate(f32 tx, f32 ty, f32 tz) noexcept
  {
    Matrix4f m = Matrix4f::identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;

    Matrix4f m_inv = Matrix4f::identity();
    m_inv.m[0][3] = -tx;
    m_inv.m[1][3] = -ty;
    m_inv.m[2][3] = -tz;

    return Transform{ m, m_inv };
  }
  Transform translate(const Vector3f& v) noexcept { return translate(v.x, v.y, v.z); }
  Transform translate(const Point3f& p) noexcept  { return translate(p.x, p.y, p.z); }

  Transform rotate_x(f32 angle_degrees) noexcept
  {
    const f32 rad = to_radians(angle_degrees);
    const f32 s   = std::sin(rad);
    const f32 c   = std::cos(rad);

    Matrix4f m = Matrix4f::identity();
    m.m[1][1] =  c;
    m.m[1][2] = -s;
    m.m[2][1] =  s;
    m.m[2][2] =  c;

    // The inverse of a pure rotation is its transpose.
    return Transform{ m, transpose(m) };
  }
  Transform rotate_y(f32 angle_degrees) noexcept
  {
    const f32 rad = to_radians(angle_degrees);
    const f32 s   = std::sin(rad);
    const f32 c   = std::cos(rad);

    Matrix4f m = Matrix4f::identity();
    m.m[0][0] =  c;
    m.m[0][2] =  s;
    m.m[2][0] = -s;
    m.m[2][2] =  c;

    return Transform{ m, transpose(m) };
  }
  Transform rotate_z(f32 angle_degrees) noexcept
  {
    const f32 rad = to_radians(angle_degrees);
    const f32 s   = std::sin(rad);
    const f32 c   = std::cos(rad);

    Matrix4f m = Matrix4f::identity();
    m.m[0][0] =  c;
    m.m[0][1] = -s;
    m.m[1][0] =  s;
    m.m[1][1] =  c;

    return Transform{ m, transpose(m) };
  }
}
