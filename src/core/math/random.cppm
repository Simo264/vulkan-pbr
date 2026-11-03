export module core.math:random;

import core.types;
import std;

export struct RandomGenerator
{
  explicit RandomGenerator(u32 seed) : engine{seed} {}

  f32 uniform(f32 a = 0.f, f32 b = 1.f) { return std::uniform_real_distribution<f32>{a, b}(engine); }

  std::mt19937 engine;
};
