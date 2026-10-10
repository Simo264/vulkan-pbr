module;
#include <cmath>

export module math:vector; // export vector submodule
import types;

template<Arithmetic T>
struct Vector2
{
  T x{}, y{};
  constexpr Vector2& operator+=(const Vector2& o) noexcept { x += o.x; y += o.y; return *this; }
  constexpr Vector2& operator-=(const Vector2& o) noexcept { x -= o.x; y -= o.y; return *this; }
  constexpr Vector2& operator*=(T s) noexcept              { x *= s;    y *= s;    return *this; }
  constexpr Vector2& operator/=(T s) noexcept              { x /= s;    y /= s;    return *this; }
};

template<Arithmetic T>
struct Vector3
{
  T x{}, y{}, z{};
  constexpr Vector3& operator+=(const Vector3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
  constexpr Vector3& operator-=(const Vector3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
  constexpr Vector3& operator*=(T s) noexcept              { x *= s;    y *= s;    z *= s;    return *this; }
  constexpr Vector3& operator/=(T s) noexcept              { x /= s;    y /= s;    z /= s;    return *this; }
};

template<Arithmetic T>
struct Vector4
{
  T x{}, y{}, z{}, w{};
  constexpr Vector4& operator+=(const Vector4& o) noexcept { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
  constexpr Vector4& operator-=(const Vector4& o) noexcept { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
  constexpr Vector4& operator*=(T s) noexcept              { x *= s;    y *= s;    z *= s;    w *= s;    return *this; }
  constexpr Vector4& operator/=(T s) noexcept              { x /= s;    y /= s;    z /= s;    w /= s;    return *this; }
};

template<Arithmetic T>
struct Normal3
{
  T x{}, y{}, z{};

  constexpr Normal3& operator+=(const Normal3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
  constexpr Normal3& operator-=(const Normal3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
  constexpr Normal3& operator*=(T s) noexcept              { x *= s;    y *= s;    z *= s;    return *this; }
  constexpr Normal3& operator/=(T s) noexcept              { x /= s;    y /= s;    z /= s;    return *this; }
};


export {
  template <Arithmetic T>
  constexpr Vector2<T> operator+(Vector2<T> a, const Vector2<T>& b) noexcept { a += b; return a; }
  template <Arithmetic T>
  constexpr Vector2<T> operator-(Vector2<T> a, const Vector2<T>& b) noexcept { a -= b; return a; }
  template <Arithmetic T>
  constexpr Vector2<T> operator-(const Vector2<T>& v) noexcept { return Vector2<T>{-v.x, -v.y}; }
  template <Arithmetic T>
  constexpr Vector2<T> operator*(Vector2<T> v, T s) noexcept { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector2<T> operator*(T s, Vector2<T> v) noexcept { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector2<T> operator/(Vector2<T> v, T s) noexcept { v /= s; return v; }
  template <Arithmetic T>
  constexpr bool operator==(const Vector2<T>& a, const Vector2<T>& b) noexcept { return a.x == b.x && a.y == b.y; }
  template <Arithmetic T>
  constexpr bool operator!=(const Vector2<T>& a, const Vector2<T>& b) noexcept { return !(a == b); }

  template <Arithmetic T>
  constexpr Vector3<T> operator+(Vector3<T> a, const Vector3<T>& b) noexcept { a += b; return a; }
  template <Arithmetic T>
  constexpr Vector3<T> operator-(Vector3<T> a, const Vector3<T>& b) noexcept { a -= b; return a; }
  template <Arithmetic T>
  constexpr Vector3<T> operator-(const Vector3<T>& v) noexcept { return Vector3<T>{-v.x, -v.y, -v.z}; }
  template <Arithmetic T>
  constexpr Vector3<T> operator*(Vector3<T> v, T s) noexcept { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector3<T> operator*(T s, Vector3<T> v) noexcept { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector3<T> operator/(Vector3<T> v, T s) noexcept { v /= s; return v; }
  template <Arithmetic T>
  constexpr bool operator==(const Vector3<T>& a, const Vector3<T>& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z; }
  template <Arithmetic T>
  constexpr bool operator!=(const Vector3<T>& a, const Vector3<T>& b) noexcept { return !(a == b); }

  template <Arithmetic T>
  constexpr Vector4<T> operator+(Vector4<T> a, const Vector4<T>& b) noexcept { a += b; return a; }
  template <Arithmetic T>
  constexpr Vector4<T> operator-(Vector4<T> a, const Vector4<T>& b) noexcept { a -= b; return a; }
  template <Arithmetic T>
  constexpr Vector4<T> operator-(const Vector4<T>& v) noexcept { return Vector4<T>{-v.x, -v.y, -v.z, -v.w}; }
  template <Arithmetic T>
  constexpr Vector4<T> operator*(Vector4<T> v, T s) noexcept { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector4<T> operator*(T s, Vector4<T> v) noexcept { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector4<T> operator/(Vector4<T> v, T s) noexcept { v /= s; return v; }
  template <Arithmetic T>
  constexpr bool operator==(const Vector4<T>& a, const Vector4<T>& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
  template <Arithmetic T>
  constexpr bool operator!=(const Vector4<T>& a, const Vector4<T>& b) noexcept { return !(a == b); }

  template <Arithmetic T>
  constexpr T dot(const Vector2<T>& a, const Vector2<T>& b) noexcept { return a.x * b.x + a.y * b.y; }
  template <Arithmetic T>
  constexpr T dot(const Vector3<T>& a, const Vector3<T>& b) noexcept { return a.x * b.x + a.y * b.y + a.z * b.z; }
  template <Arithmetic T>
  constexpr T dot(const Vector4<T>& a, const Vector4<T>& b) noexcept { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
  template <Arithmetic T>
  constexpr T dot(const Normal3<T>& a, const Normal3<T>& b) noexcept { return a.x * b.x + a.y * b.y + a.z * b.z; }
  template <Arithmetic T>
  constexpr T dot(const Normal3<T>& n, const Vector3<T>& v) noexcept { return n.x * v.x + n.y * v.y + n.z * v.z; }
  template <Arithmetic T>
  constexpr T dot(const Vector3<T>& v, const Normal3<T>& n) noexcept { return v.x * n.x + v.y * n.y + v.z * n.z; }
  template <Arithmetic T>
  constexpr T abs_dot(const Vector2<T>& a, const Vector2<T>& b) noexcept { const T d = dot(a, b); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr T abs_dot(const Vector3<T>& a, const Vector3<T>& b) noexcept { const T d = dot(a, b); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr T abs_dot(const Vector4<T>& a, const Vector4<T>& b) noexcept { const T d = dot(a, b); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr T abs_dot(const Normal3<T>& a, const Normal3<T>& b) noexcept { const T d = dot(a, b); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr T abs_dot(const Normal3<T>& n, const Vector3<T>& v) noexcept { const T d = dot(n, v); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr T abs_dot(const Vector3<T>& v, const Normal3<T>& n) noexcept { const T d = dot(v, n); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) noexcept { return Vector3<T>{ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }
  template <Arithmetic T>
  constexpr T length_squared(const Vector2<T>& v) noexcept { return v.x * v.x + v.y * v.y; }
  template <Arithmetic T>
  constexpr T length_squared(const Vector3<T>& v) noexcept { return v.x * v.x + v.y * v.y + v.z * v.z; }
  template <Arithmetic T>
  constexpr T length_squared(const Vector4<T>& v) noexcept { return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w; }
  template <Arithmetic T>
  constexpr T length_squared(const Normal3<T>& n) noexcept { return n.x * n.x + n.y * n.y + n.z * n.z; }
  template <Arithmetic T>
  constexpr T length(const Vector2<T>& v) noexcept { return std::sqrt(length_squared(v)); }
  template <Arithmetic T>
  constexpr T length(const Vector3<T>& v) noexcept { return std::sqrt(length_squared(v)); }
  template <Arithmetic T>
  constexpr T length(const Vector4<T>& v) noexcept { return std::sqrt(length_squared(v)); }
  template <Arithmetic T>
  constexpr T length(const Normal3<T>& n) noexcept { return std::sqrt(length_squared(n)); }
  template <Arithmetic T>
  constexpr Vector2<T> normalize(const Vector2<T>& v) noexcept { const T len = length(v); return Vector2<T>{v.x / len, v.y / len}; }
  template <Arithmetic T>
  constexpr Vector3<T> normalize(const Vector3<T>& v) noexcept { const T len = length(v); return Vector3<T>{v.x / len, v.y / len, v.z / len}; }
  template <Arithmetic T>
  constexpr Vector4<T> normalize(const Vector4<T>& v) noexcept { const T len = length(v); return Vector4<T>{v.x / len, v.y / len, v.z / len, v.w / len}; }
  template <Arithmetic T>
  constexpr Normal3<T> normalize(const Normal3<T>& n) noexcept { const T len = length(n); return Normal3<T>{n.x / len, n.y / len, n.z / len}; }
  template <Arithmetic T>
  constexpr T lerp(T t, T a, T b) noexcept { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr Vector2<T> lerp(T t, const Vector2<T>& a, const Vector2<T>& b) noexcept { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr Vector3<T> lerp(T t, const Vector3<T>& a, const Vector3<T>& b) noexcept { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr Vector4<T> lerp(T t, const Vector4<T>& a, const Vector4<T>& b) noexcept { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr Normal3<T> lerp(T t, const Normal3<T>& a, const Normal3<T>& b) noexcept { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr T min_component(const Vector2<T>& v) noexcept { return v.x < v.y ? v.x : v.y; }
  template <Arithmetic T>
  constexpr T min_component(const Vector3<T>& v) noexcept { const T xy = v.x < v.y ? v.x : v.y; return xy < v.z ? xy : v.z; }
  template <Arithmetic T>
  constexpr T min_component(const Vector4<T>& v) noexcept { const T xy = v.x < v.y ? v.x : v.y; const T zw = v.z < v.w ? v.z : v.w; return xy < zw ? xy : zw; }
  template <Arithmetic T>
  constexpr T max_component(const Vector2<T>& v) noexcept { return v.x > v.y ? v.x : v.y; }
  template <Arithmetic T>
  constexpr T max_component(const Vector3<T>& v) noexcept { const T xy = v.x > v.y ? v.x : v.y; return xy > v.z ? xy : v.z; }
  template <Arithmetic T>
  constexpr T max_component(const Vector4<T>& v) noexcept { const T xy = v.x > v.y ? v.x : v.y; const T zw = v.z > v.w ? v.z : v.w; return xy > zw ? xy : zw; }
  template <Arithmetic T>
  constexpr Vector2<T> abs(const Vector2<T>& v) noexcept { return Vector2<T>{ v.x < T{0} ? -v.x : v.x, v.y < T{0} ? -v.y : v.y }; }
  template <Arithmetic T>
  constexpr Vector3<T> abs(const Vector3<T>& v) noexcept { return Vector3<T>{ v.x < T{0} ? -v.x : v.x, v.y < T{0} ? -v.y : v.y, v.z < T{0} ? -v.z : v.z }; }
  template <Arithmetic T>
  constexpr Vector4<T> abs(const Vector4<T>& v) noexcept { return Vector4<T>{ v.x < T{0} ? -v.x : v.x, v.y < T{0} ? -v.y : v.y, v.z < T{0} ? -v.z : v.z, v.w < T{0} ? -v.w : v.w }; }
  template <Arithmetic T>
  constexpr T clamp(T val, T low, T high) noexcept { if (val < low) return low; if (val > high) return high; return val; }
  template <Arithmetic T>
  constexpr Vector2<T> clamp(const Vector2<T>& v, T low, T high) noexcept { return Vector2<T>{ clamp(v.x, low, high), clamp(v.y, low, high) }; }
  template <Arithmetic T>
  constexpr Vector3<T> clamp(const Vector3<T>& v, T low, T high) noexcept { return Vector3<T>{ clamp(v.x, low, high), clamp(v.y, low, high), clamp(v.z, low, high) }; }
  template <Arithmetic T>
  constexpr Vector4<T> clamp(const Vector4<T>& v, T low, T high) noexcept { return Vector4<T>{ clamp(v.x, low, high), clamp(v.y, low, high), clamp(v.z, low, high), clamp(v.w, low, high) }; }
  template <Arithmetic T>
  constexpr Vector3<T> permute(const Vector3<T>& v, std::size_t x, std::size_t y, std::size_t z) noexcept { const T c[3] = {v.x, v.y, v.z}; return Vector3<T>{c[x], c[y], c[z]}; }

  using Vector2f = Vector2<f32>;
  using Vector2d = Vector2<f64>;
  using Vector2i = Vector2<i32>;
  using Vector3f = Vector3<f32>;
  using Vector3d = Vector3<f64>;
  using Vector3i = Vector3<i32>;
  using Vector4f = Vector4<f32>;
  using Vector4d = Vector4<f64>;
  using Vector4i = Vector4<i32>;

  using Normal3f = Normal3<f32>;
};
