export module math:utils;

import types;
import std;

export constexpr f32 PI = 3.14159265358979323846;

export constexpr f32 to_degrees(f32 radians) noexcept { return radians * 180.0f / PI; }
export constexpr f32 to_radians(f32 degrees) noexcept { return degrees * PI / 180.0f; }
