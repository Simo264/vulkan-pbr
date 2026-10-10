export module math:point; // export point submodule

import :vector;
import types;

template<Arithmetic T>
struct Point2
{
  T x{}, y{};

  constexpr Point2& operator+=(const Vector2<T>& v) noexcept { x += v.x; y += v.y; return *this; }
  constexpr Point2& operator-=(const Vector2<T>& v) noexcept { x -= v.x; y -= v.y; return *this; }
  constexpr Point2& operator*=(T s) noexcept { x *= s; y *= s; return *this; }
  constexpr Point2& operator/=(T s) noexcept { x /= s; y /= s; return *this; }
};

template<Arithmetic T>
struct Point3
{
  T x{}, y{}, z{};

  constexpr Point3& operator+=(const Vector3<T>& v) noexcept { x += v.x; y += v.y; z += v.z; return *this; }
  constexpr Point3& operator-=(const Vector3<T>& v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; }
  constexpr Point3& operator*=(T s) noexcept { x *= s; y *= s; z *= s; return *this; }
  constexpr Point3& operator/=(T s) noexcept { x /= s; y /= s; z /= s; return *this; }
};

export {
  template <Arithmetic T>
  constexpr Point2<T> operator+(Point2<T> p, const Vector2<T>& v) noexcept { p += v; return p; }
  template <Arithmetic T>
  constexpr Point2<T> operator+(const Vector2<T>& v, Point2<T> p) noexcept { p += v; return p; }
  template <Arithmetic T>
  constexpr Point2<T> operator-(Point2<T> p, const Vector2<T>& v) noexcept { p -= v; return p; }
  template <Arithmetic T>
  constexpr Vector2<T> operator-(const Point2<T>& a, const Point2<T>& b) noexcept { return Vector2<T>{a.x - b.x, a.y - b.y}; }
  template <Arithmetic T>
  constexpr Point2<T> operator*(Point2<T> p, T s) noexcept { p *= s; return p; }
  template <Arithmetic T>
  constexpr Point2<T> operator*(T s, Point2<T> p) noexcept { p *= s; return p; }
  template <Arithmetic T>
  constexpr Point2<T> operator/(Point2<T> p, T s) noexcept { p /= s; return p; }
  template <Arithmetic T>
  constexpr bool operator==(const Point2<T>& a, const Point2<T>& b) noexcept { return a.x == b.x && a.y == b.y; }

  template <Arithmetic T>
  constexpr Point3<T> operator+(Point3<T> p, const Vector3<T>& v) noexcept { p += v; return p; }
  template <Arithmetic T>
  constexpr Point3<T> operator+(const Vector3<T>& v, Point3<T> p) noexcept { p += v; return p; }
  template <Arithmetic T>
  constexpr Point3<T> operator-(Point3<T> p, const Vector3<T>& v) noexcept { p -= v; return p; }
  template <Arithmetic T>
  constexpr Vector3<T> operator-(const Point3<T>& a, const Point3<T>& b) noexcept { return Vector3<T>{a.x - b.x, a.y - b.y, a.z - b.z}; }
  template <Arithmetic T>
  constexpr Point3<T> operator*(Point3<T> p, T s) noexcept { p *= s; return p; }
  template <Arithmetic T>
  constexpr Point3<T> operator*(T s, Point3<T> p) noexcept { p *= s; return p; }
  template <Arithmetic T>
  constexpr Point3<T> operator/(Point3<T> p, T s) noexcept { p /= s; return p; }
  template <Arithmetic T>
  constexpr bool operator==(const Point3<T>& a, const Point3<T>& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z; }

  template <Arithmetic T>
  constexpr T distance_squared(const Point3<T>& a, const Point3<T>& b) noexcept { return length_squared(a - b); }
  template <Arithmetic T>
  constexpr T distance(const Point3<T>& a, const Point3<T>& b) noexcept { return length(a - b); }
  template <Arithmetic T>
  constexpr Point3<T> lerp(T t, const Point3<T>& p0, const Point3<T>& p1) noexcept { return Point3<T>{ lerp(t, p0.x, p1.x), lerp(t, p0.y, p1.y), lerp(t, p0.z, p1.z) }; }

  template <Arithmetic T>
  constexpr T distance_squared(const Point2<T>& a, const Point2<T>& b) noexcept { return length_squared(a - b); }
  template <Arithmetic T>
  constexpr T distance(const Point2<T>& a, const Point2<T>& b) noexcept { return length(a - b); }
  template <Arithmetic T>
  constexpr Point2<T> lerp(T t, const Point2<T>& p0, const Point2<T>& p1) noexcept { return Point2<T>{ lerp(t, p0.x, p1.x), lerp(t, p0.y, p1.y) }; }

  using Point2f = Point2<f32>;
  using Point2d = Point2<f64>;
  using Point2i = Point2<i32>;
  using Point3f = Point3<f32>;
  using Point3d = Point3<f64>;
  using Point3i = Point3<i32>;
};
