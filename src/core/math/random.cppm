export module core.math:random;

import core.types;
import std;

export struct RandomGenerator
{
  explicit RandomGenerator(u32 seed) : engine{seed} {}

  // Produces random floating-point values x uniformly distributed on the interval [a,b), 
  // that is, distributed according to the probability density function:
  // P(x, a|b) = 1 /(b-a)
  f32 uniform(f32 a = 0.f, f32 b = 1.f) { return std::uniform_real_distribution<f32>{a, b}(engine); }

  std::mt19937 engine;
};
