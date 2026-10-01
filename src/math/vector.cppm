module;
#include <type_traits>
#include <cmath>

export module math:vector; // export vector submodule
import types;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// ==============================
// Vector 2
// ==============================
template<Arithmetic T>
struct Vector2
{
  T x{}, y{};

  constexpr Vector2& operator+=(const Vector2& o) noexcept { x += o.x; y += o.y; return *this; }
  constexpr Vector2& operator-=(const Vector2& o) noexcept { x -= o.x; y -= o.y; return *this; }
  constexpr Vector2& operator*=(T s) noexcept              { x *= s;    y *= s;    return *this; }
  constexpr Vector2& operator/=(T s) noexcept              { x /= s;    y /= s;    return *this; }

  [[nodiscard]] constexpr T dot(const Vector2& o) const noexcept { return x * o.x + y * o.y; }
};

// ==============================
// Vector 3
// ==============================
template<Arithmetic T>
struct Vector3
{
  T x{}, y{}, z{};

  constexpr Vector3& operator+=(const Vector3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
  constexpr Vector3& operator-=(const Vector3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
  constexpr Vector3& operator*=(T s) noexcept              { x *= s;    y *= s;    z *= s;    return *this; }
  constexpr Vector3& operator/=(T s) noexcept              { x /= s;    y /= s;    z /= s;    return *this; }

  [[nodiscard]] constexpr T dot(const Vector3& o) const noexcept { return x * o.x + y * o.y + z * o.z; }
  [[nodiscard]] constexpr Vector3 cross(const Vector3& o) const noexcept { return { y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x }; }
};

// ==============================
// Vector 4
// ==============================
template<Arithmetic T>
struct Vector4
{
  T x{}, y{}, z{}, w{};

  constexpr Vector4& operator+=(const Vector4& o) noexcept { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
  constexpr Vector4& operator-=(const Vector4& o) noexcept { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
  constexpr Vector4& operator*=(T s) noexcept              { x *= s;    y *= s;    z *= s;    w *= s;    return *this; }
  constexpr Vector4& operator/=(T s) noexcept              { x /= s;    y /= s;    z /= s;    w /= s;    return *this; }

  [[nodiscard]] constexpr T dot(const Vector4& o) const noexcept { return x * o.x + y * o.y + z * o.z + w * o.w; }
};

export {
  using Vector2f = Vector2<f32>;
  using Vector2d = Vector2<f64>;
  using Vector2i = Vector2<i32>;
  using Vector3f = Vector3<f32>;
  using Vector3d = Vector3<f64>;
  using Vector3i = Vector3<i32>;
  using Vector4f = Vector4<f32>;
  using Vector4d = Vector4<f64>;
  using Vector4i = Vector4<i32>;

  template<typename Vec>
  [[nodiscard]] constexpr Vec operator+(Vec a, const Vec& b) noexcept { a += b; return a; }
  template<typename Vec>
  [[nodiscard]] constexpr Vec operator-(Vec a, const Vec& b) noexcept { a -= b; return a; }
  template<typename Vec, Arithmetic T>
  [[nodiscard]] constexpr Vec operator*(Vec v, T s) noexcept { v *= s; return v; }
  template<typename Vec, Arithmetic T>
  [[nodiscard]] constexpr Vec operator*(T s, Vec v) noexcept { v *= s; return v; }
  template<typename Vec, Arithmetic T>
  [[nodiscard]] constexpr Vec operator/(Vec v, T s) noexcept { v /= s; return v; }
  template<typename Vec>
  [[nodiscard]] constexpr Vec operator-(Vec v) noexcept
  {
    v.x = -v.x;
    if constexpr (requires { v.y; }) v.y = -v.y;
    if constexpr (requires { v.z; }) v.z = -v.z;
    if constexpr (requires { v.w; }) v.w = -v.w;
    return v;
  }


  template<Arithmetic T>
  [[nodiscard]] constexpr T dot(const Vector3<T>& a, const Vector3<T>& b) noexcept { return a.dot(b); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T sq_length(const Vector2<T>& v) noexcept { return v.dot(v); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T sq_length(const Vector3<T>& v) noexcept { return v.dot(v); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T sq_length(const Vector4<T>& v) noexcept { return v.dot(v); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T length(const Vector2<T>& v) noexcept { return std::sqrt(sq_length(v)); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T length(const Vector3<T>& v) noexcept { return std::sqrt(sq_length(v)); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T length(const Vector4<T>& v) noexcept { return std::sqrt(sq_length(v)); }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector2<T> normalize(const Vector2<T>& v) noexcept { return v * (T(1) / length(v)); }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector3<T> normalize(const Vector3<T>& v) noexcept { return v * (T(1) / length(v)); }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector4<T> normalize(const Vector4<T>& v) noexcept { return v * (T(1) / length(v)); }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector3<T> min(const Vector3<T>& a, const Vector3<T>& b) noexcept { return { std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) }; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector3<T> max(const Vector3<T>& a, const Vector3<T>& b) noexcept { return { std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) }; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector3<T> abs(const Vector3<T>& v) noexcept { return { std::abs(v.x), std::abs(v.y), std::abs(v.z) }; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Vector3<T> lerp(const Vector3<T>& a, const Vector3<T>& b, T t) noexcept { return a + (b - a) * t; }
};
