export module core.math:utils;

import core.types;

export constexpr f32 PI = 3.14159265358979323846;

export constexpr f32 to_degrees(f32 radians) { return radians * 180.0f / PI; }
export constexpr f32 to_radians(f32 degrees) { return degrees * PI / 180.0f; }
