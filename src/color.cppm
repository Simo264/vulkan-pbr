export module color;

import types;
import std;

export{

  struct Color3f
  {
    constexpr Color3f() noexcept = default;
    constexpr Color3f(f32 r, f32 g, f32 b) noexcept : r{r}, g{g}, b{b} {}

    explicit constexpr Color3f(f32 v) noexcept : r{v}, g{v}, b{v} {}

    constexpr Color3f& operator+=(const Color3f& o) noexcept { r += o.r; g += o.g; b += o.b; return *this; }
    constexpr Color3f& operator-=(const Color3f& o) noexcept { r -= o.r; g -= o.g; b -= o.b; return *this; }
    constexpr Color3f& operator*=(const Color3f& o) noexcept { r *= o.r; g *= o.g; b *= o.b; return *this; }
    constexpr Color3f& operator/=(const Color3f& o) noexcept { r /= o.r; g /= o.g; b /= o.b; return *this; }

    constexpr Color3f& operator*=(f32 s) noexcept { r *= s; g *= s; b *= s; return *this; }
    constexpr Color3f& operator/=(f32 s) noexcept { const f32 inv = 1.f / s; r *= inv; g *= inv; b *= inv; return *this; }

    f32 r{}, g{}, b{};
  };

  constexpr Color3f operator+(Color3f a, const Color3f& b) noexcept { return a += b; }
  constexpr Color3f operator-(Color3f a, const Color3f& b) noexcept { return a -= b; }
  constexpr Color3f operator*(Color3f a, const Color3f& b) noexcept { return a *= b; }
  constexpr Color3f operator/(Color3f a, const Color3f& b) noexcept { return a /= b; }

  constexpr Color3f operator-(const Color3f& c) noexcept { return {-c.r, -c.g, -c.b}; }

  constexpr Color3f operator*(Color3f c, f32 s) noexcept { return c *= s; }
  constexpr Color3f operator*(f32 s, Color3f c) noexcept { return c *= s; }
  constexpr Color3f operator/(Color3f c, f32 s) noexcept { return c /= s; }

  constexpr Color3f clamp(const Color3f& c, f32 lo = 0.f, f32 hi = 1.f) noexcept
  {
    return {
      std::min(std::max(c.r, lo), hi),
      std::min(std::max(c.g, lo), hi),
      std::min(std::max(c.b, lo), hi)
    };
  }

  constexpr Color3f lerp(const Color3f& a, const Color3f& b, f32 t) noexcept { return a + t * (b - a); }
};
