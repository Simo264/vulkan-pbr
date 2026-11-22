export module core.math:vector;

import core.types;
import std;

template<Arithmetic T>
struct Vector2
{
  T x{}, y{};

  Vector2() = default;
  Vector2(T c) : x{c}, y{c} {}
  Vector2(T x, T y) : x{x}, y{y} {}
  constexpr Vector2& operator+=(const Vector2& o) { x += o.x; y += o.y; return *this; }
  constexpr Vector2& operator-=(const Vector2& o) { x -= o.x; y -= o.y; return *this; }
  constexpr Vector2& operator*=(T s)              { x *= s;   y *= s;   return *this; }
  constexpr Vector2& operator/=(T s)              { x /= s;   y /= s;   return *this; }

  constexpr T  operator[](i32 i) const { switch (i) { case 0: return x; default: return y; } }
  constexpr T& operator[](i32 i)       { switch (i) { case 0: return x; default: return y; } }
};

template<Arithmetic T>
struct Vector3
{
  T x{}, y{}, z{};

  Vector3() = default;
  Vector3(T c) : x{c}, y{c}, z{c} {}
  Vector3(T x, T y, T z) : x{x}, y{y}, z{z} {}
  constexpr Vector3& operator+=(const Vector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
  constexpr Vector3& operator-=(const Vector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
  constexpr Vector3& operator*=(T s)              { x *= s;   y *= s;   z *= s;   return *this; }
  constexpr Vector3& operator/=(T s)              { x /= s;   y /= s;   z /= s;   return *this; }
  constexpr T  operator[](i32 i) const { switch (i) { case 0: return x; case 1: return y; default: return z; } }
  constexpr T& operator[](i32 i)       { switch (i) { case 0: return x; case 1: return y; default: return z; } }
};

template<Arithmetic T>
struct Vector4
{
  T x{}, y{}, z{}, w{};

  Vector4() = default;
  Vector4(T c) : x{c}, y{c}, z{c}, w{c} {}
  Vector4(T x, T y, T z, T w) : x{x}, y{y}, z{z}, w{w} {}
  constexpr Vector4& operator+=(const Vector4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
  constexpr Vector4& operator-=(const Vector4& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
  constexpr Vector4& operator*=(T s)              { x *= s;   y *= s;   z *= s;   w *= s;   return *this; }
  constexpr Vector4& operator/=(T s)              { x /= s;   y /= s;   z /= s;   w /= s;   return *this; }
  constexpr T  operator[](i32 i) const { switch (i) { case 0: return x; case 1: return y; case 2: return z; default: return w; } }
  constexpr T& operator[](i32 i)       { switch (i) { case 0: return x; case 1: return y; case 2: return z; default: return w; } }
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

  template <Arithmetic T>
  constexpr Vector2<T> operator+(Vector2<T> a, const Vector2<T>& b) { a += b; return a; }
  template <Arithmetic T>
  constexpr Vector2<T> operator-(Vector2<T> a, const Vector2<T>& b) { a -= b; return a; }
  template <Arithmetic T>
  constexpr Vector2<T> operator-(const Vector2<T>& v) { return Vector2<T>{-v.x, -v.y}; }
  template <Arithmetic T>
  constexpr Vector2<T> operator*(Vector2<T> v, T s) { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector2<T> operator*(T s, Vector2<T> v) { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector2<T> operator/(Vector2<T> v, T s) { v /= s; return v; }
  template <Arithmetic T>
  constexpr bool operator==(const Vector2<T>& a, const Vector2<T>& b) { return a.x == b.x && a.y == b.y; }
  template <Arithmetic T>
  constexpr bool operator!=(const Vector2<T>& a, const Vector2<T>& b) { return !(a == b); }

  template <Arithmetic T>
  constexpr Vector3<T> operator+(Vector3<T> a, const Vector3<T>& b) { a += b; return a; }
  template <Arithmetic T>
  constexpr Vector3<T> operator-(Vector3<T> a, const Vector3<T>& b) { a -= b; return a; }
  template <Arithmetic T>
  constexpr Vector3<T> operator-(const Vector3<T>& v) { return Vector3<T>{-v.x, -v.y, -v.z}; }
  template <Arithmetic T>
  constexpr Vector3<T> operator*(Vector3<T> v, T s) { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector3<T> operator*(T s, Vector3<T> v) { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector3<T> operator/(Vector3<T> v, T s) { v /= s; return v; }
  template <Arithmetic T>
  constexpr bool operator==(const Vector3<T>& a, const Vector3<T>& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
  template <Arithmetic T>
  constexpr bool operator!=(const Vector3<T>& a, const Vector3<T>& b) { return !(a == b); }

  template <Arithmetic T>
  constexpr Vector4<T> operator+(Vector4<T> a, const Vector4<T>& b) { a += b; return a; }
  template <Arithmetic T>
  constexpr Vector4<T> operator-(Vector4<T> a, const Vector4<T>& b) { a -= b; return a; }
  template <Arithmetic T>
  constexpr Vector4<T> operator-(const Vector4<T>& v) { return Vector4<T>{-v.x, -v.y, -v.z, -v.w}; }
  template <Arithmetic T>
  constexpr Vector4<T> operator*(Vector4<T> v, T s) { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector4<T> operator*(T s, Vector4<T> v) { v *= s; return v; }
  template <Arithmetic T>
  constexpr Vector4<T> operator/(Vector4<T> v, T s) { v /= s; return v; }
  template <Arithmetic T>
  constexpr bool operator==(const Vector4<T>& a, const Vector4<T>& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
  template <Arithmetic T>
  constexpr bool operator!=(const Vector4<T>& a, const Vector4<T>& b) { return !(a == b); }

  template <Arithmetic T>
  constexpr T dot(const Vector2<T>& a, const Vector2<T>& b) { return a.x * b.x + a.y * b.y; }
  template <Arithmetic T>
  constexpr T dot(const Vector3<T>& a, const Vector3<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
  template <Arithmetic T>
  constexpr T dot(const Vector4<T>& a, const Vector4<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
  template <Arithmetic T>
  constexpr T abs_dot(const Vector2<T>& a, const Vector2<T>& b) { const T d = dot(a, b); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr T abs_dot(const Vector3<T>& a, const Vector3<T>& b) { const T d = dot(a, b); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr T abs_dot(const Vector4<T>& a, const Vector4<T>& b) { const T d = dot(a, b); return d < T{0} ? -d : d; }
  template <Arithmetic T>
  constexpr Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>{ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }
  template <Arithmetic T>
  constexpr T length_squared(const Vector2<T>& v) { return v.x * v.x + v.y * v.y; }
  template <Arithmetic T>
  constexpr T length_squared(const Vector3<T>& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
  template <Arithmetic T>
  constexpr T length_squared(const Vector4<T>& v) { return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w; }
  template <Arithmetic T>
  constexpr T length(const Vector2<T>& v) { return std::sqrt(length_squared(v)); }
  template <Arithmetic T>
  constexpr T length(const Vector3<T>& v) { return std::sqrt(length_squared(v)); }
  template <Arithmetic T>
  constexpr T length(const Vector4<T>& v) { return std::sqrt(length_squared(v)); }
  template <Arithmetic T>
  constexpr Vector2<T> normalize(const Vector2<T>& v) { const T len = length(v); return Vector2<T>{v.x / len, v.y / len}; }
  template <Arithmetic T>
  constexpr Vector3<T> normalize(const Vector3<T>& v) { const T len = length(v); return Vector3<T>{v.x / len, v.y / len, v.z / len}; }
  template <Arithmetic T>
  constexpr Vector4<T> normalize(const Vector4<T>& v) { const T len = length(v); return Vector4<T>{v.x / len, v.y / len, v.z / len, v.w / len}; }
  template <Arithmetic T>
  constexpr T lerp(T t, T a, T b) { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr Vector2<T> lerp(T t, const Vector2<T>& a, const Vector2<T>& b) { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr Vector3<T> lerp(T t, const Vector3<T>& a, const Vector3<T>& b) { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr Vector4<T> lerp(T t, const Vector4<T>& a, const Vector4<T>& b) { return (T{1} - t) * a + t * b; }
  template <Arithmetic T>
  constexpr T min_component(const Vector2<T>& v) { return v.x < v.y ? v.x : v.y; }
  template <Arithmetic T>
  constexpr T min_component(const Vector3<T>& v) { const T xy = v.x < v.y ? v.x : v.y; return xy < v.z ? xy : v.z; }
  template <Arithmetic T>
  constexpr T min_component(const Vector4<T>& v) { const T xy = v.x < v.y ? v.x : v.y; const T zw = v.z < v.w ? v.z : v.w; return xy < zw ? xy : zw; }
  template <Arithmetic T>
  constexpr T max_component(const Vector2<T>& v) { return v.x > v.y ? v.x : v.y; }
  template <Arithmetic T>
  constexpr T max_component(const Vector3<T>& v) { const T xy = v.x > v.y ? v.x : v.y; return xy > v.z ? xy : v.z; }
  template <Arithmetic T>
  constexpr T max_component(const Vector4<T>& v) { const T xy = v.x > v.y ? v.x : v.y; const T zw = v.z > v.w ? v.z : v.w; return xy > zw ? xy : zw; }
  template <Arithmetic T>
  constexpr Vector2<T> abs(const Vector2<T>& v) { return Vector2<T>{ v.x < T{0} ? -v.x : v.x, v.y < T{0} ? -v.y : v.y }; }
  template <Arithmetic T>
  constexpr Vector3<T> abs(const Vector3<T>& v) { return Vector3<T>{ v.x < T{0} ? -v.x : v.x, v.y < T{0} ? -v.y : v.y, v.z < T{0} ? -v.z : v.z }; }
  template <Arithmetic T>
  constexpr Vector4<T> abs(const Vector4<T>& v) { return Vector4<T>{ v.x < T{0} ? -v.x : v.x, v.y < T{0} ? -v.y : v.y, v.z < T{0} ? -v.z : v.z, v.w < T{0} ? -v.w : v.w }; }
  template <Arithmetic T>
  constexpr T clamp(T val, T low, T high) { if (val < low) return low; if (val > high) return high; return val; }
  template <Arithmetic T>
  constexpr Vector2<T> clamp(const Vector2<T>& v, T low, T high) { return Vector2<T>{ clamp(v.x, low, high), clamp(v.y, low, high) }; }
  template <Arithmetic T>
  constexpr Vector3<T> clamp(const Vector3<T>& v, T low, T high) { return Vector3<T>{ clamp(v.x, low, high), clamp(v.y, low, high), clamp(v.z, low, high) }; }
  template <Arithmetic T>
  constexpr Vector4<T> clamp(const Vector4<T>& v, T low, T high) { return Vector4<T>{ clamp(v.x, low, high), clamp(v.y, low, high), clamp(v.z, low, high), clamp(v.w, low, high) }; }
  template <Arithmetic T>
  constexpr Vector3<T> permute(const Vector3<T>& v, usize x, usize y, usize z) { const T c[3] = {v.x, v.y, v.z}; return Vector3<T>{c[x], c[y], c[z]}; }


  constexpr Vector3f reflect(Vector3f wo, Vector3f n)
  {
    // The standard formula for calculating the reflection vector r
    // of an incident vector wi off a surface with normal vector n is:
    //    r = wi - 2(wi dot n)n
    // where n is the normalized unit normal vector

    return -wo + 2.f * dot(wo, n) * n;
  }

  // Returns the refracted direction or std::nullopt on Total Internal Reflection (TIR).
  // Inputs:
  //   wo      : outgoing direction
  //   wm      : surface normal
  //   eta_rel : eta_transmitted / eta_incident.
  constexpr std::optional<Vector3f> refract(Vector3f wo, Vector3f n, f32 eta_rel)
  {
    f32 cos_i  = dot(wo, n);
    f32 sin2_i = std::max(0.f, 1.f - cos_i * cos_i);
    f32 sin2_t = sin2_i / (eta_rel * eta_rel);

    if (sin2_t >= 1.f)
      return std::nullopt; // Total Internal Reflection

    f32 cos_t  = std::sqrt(1.f - sin2_t);
    return Vector3f{-wo / eta_rel + (cos_i / eta_rel - std::copysign(cos_t, cos_i)) * n};
  }
};
