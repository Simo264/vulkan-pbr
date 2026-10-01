
export module math:point; // export point submodule

import :vector;
import types;

// ==============================
// Point 2
// ==============================
template<Arithmetic T>
struct Point2
{
  T x{}, y{};

  constexpr Point2& operator+=(const Vector2<T>& v) noexcept { x += v.x; y += v.y; return *this; }
  constexpr Point2& operator-=(const Vector2<T>& v) noexcept { x -= v.x; y -= v.y; return *this; }

  [[nodiscard]] constexpr Vector2<T> operator-(const Point2& o) const noexcept { return { x - o.x, y - o.y }; }
};

// ==============================
// Point 3
// ==============================
template<Arithmetic T>
struct Point3
{
  T x{}, y{}, z{};

  constexpr Point3& operator+=(const Vector3<T>& v) noexcept { x += v.x; y += v.y; z += v.z; return *this; }
  constexpr Point3& operator-=(const Vector3<T>& v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; }

  [[nodiscard]] constexpr Vector3<T> operator-(const Point3& o) const noexcept { return { x - o.x, y - o.y, z - o.z }; }
};

export {
  using Point2f = Point2<f32>;
  using Point2d = Point2<f64>;
  using Point2i = Point2<i32>;
  using Point3f = Point3<f32>;
  using Point3d = Point3<f64>;
  using Point3i = Point3<i32>;

  template<Arithmetic T>
  [[nodiscard]] constexpr Point2<T> operator+(Point2<T> p, const Vector2<T>& v) noexcept { p += v; return p; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point2<T> operator+(const Vector2<T>& v, Point2<T> p) noexcept { p += v; return p; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point2<T> operator-(Point2<T> p, const Vector2<T>& v) noexcept { p -= v; return p; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point3<T> operator+(Point3<T> p, const Vector3<T>& v) noexcept { p += v; return p; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point3<T> operator+(const Vector3<T>& v, Point3<T> p) noexcept { p += v; return p; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point3<T> operator-(Point3<T> p, const Vector3<T>& v) noexcept { p -= v; return p; }


  template<Arithmetic T>
  [[nodiscard]] constexpr T distance_sq(const Point2<T>& a, const Point2<T>& b) noexcept { return sq_length(b - a); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T distance_sq(const Point3<T>& a, const Point3<T>& b) noexcept { return sq_length(b - a); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T distance(const Point2<T>& a, const Point2<T>& b) noexcept { return length(b - a); }
  template<Arithmetic T>
  [[nodiscard]] constexpr T distance(const Point3<T>& a, const Point3<T>& b) noexcept { return length(b - a); }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point2<T> min(const Point2<T>& a, const Point2<T>& b) noexcept { return { std::min(a.x, b.x), std::min(a.y, b.y) }; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point3<T> min(const Point3<T>& a, const Point3<T>& b) noexcept { return { std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) }; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point2<T> max(const Point2<T>& a, const Point2<T>& b) noexcept { return { std::max(a.x, b.x), std::max(a.y, b.y) }; }
  template<Arithmetic T>
  [[nodiscard]] constexpr Point3<T> max(const Point3<T>& a, const Point3<T>& b) noexcept { return { std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) }; }

};
