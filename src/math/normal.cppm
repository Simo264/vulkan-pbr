
export module math:normal; // export normal submodule

import :vector;
import types;

template<Arithmetic T>
struct Normal3
{
  T x{}, y{}, z{};

  constexpr Normal3& operator+=(const Normal3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
  constexpr Normal3& operator-=(const Normal3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
  constexpr Normal3& operator*=(T s) noexcept              { x *= s;    y *= s;    z *= s;    return *this; }
  constexpr Normal3& operator/=(T s) noexcept              { x /= s;    y /= s;    z /= s;    return *this; }

  [[nodiscard]] constexpr T dot(const Normal3& o) const noexcept { return x * o.x + y * o.y + z * o.z; }
  [[nodiscard]] constexpr T dot(const Vector3<T>& v) const noexcept { return x * v.x + y * v.y + z * v.z; }
};

export {
  using Normal3f = Normal3<f32>;


  template<Arithmetic T>
  [[nodiscard]] constexpr Normal3<T> operator+(Normal3<T> a, const Normal3<T>& b) noexcept { a += b; return a; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Normal3<T> operator-(Normal3<T> a, const Normal3<T>& b) noexcept { a -= b; return a; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Normal3<T> operator*(Normal3<T> n, T s) noexcept { n *= s; return n; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Normal3<T> operator*(T s, Normal3<T> n) noexcept { n *= s; return n; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Normal3<T> operator/(Normal3<T> n, T s) noexcept { n /= s; return n; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Normal3<T> operator-(Normal3<T> n) noexcept { return { -n.x, -n.y, -n.z }; }

  template<Arithmetic T>
  [[nodiscard]] constexpr T dot(const Normal3<T>& n, const Vector3<T>& v) noexcept { return n.dot(v); }

  template<Arithmetic T>
  [[nodiscard]] constexpr T sq_length(const Normal3<T>& n) noexcept { return n.x * n.x + n.y * n.y + n.z * n.z; }
  template<Arithmetic T>
  [[nodiscard]] constexpr T length(const Normal3<T>& n) noexcept { return std::sqrt(sq_length(n)); }
  template<Arithmetic T>
  [[nodiscard]] constexpr Normal3<T> normalize(const Normal3<T>& n) noexcept { return n * (T(1) / length(n)); }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector3<T> cross(const Vector3<T>& v, const Normal3<T>& n) noexcept { return { v.y * n.z - v.z * n.y, v.z * n.x - v.x * n.z, v.x * n.y - v.y * n.x }; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector3<T> cross(const Normal3<T>& n, const Vector3<T>& v) noexcept { return { n.y * v.z - n.z * v.y, n.z * v.x - n.x * v.z, n.x * v.y - n.y * v.x }; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Normal3<T> face_forward(const Normal3<T>& n, const Vector3<T>& v) noexcept { return dot(n, v) < T(0) ? n : -n; }
};
