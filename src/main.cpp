import std;

import math;

int main()
{
  Vector3f vec{1, 1, 1};
  std::println("{}, {}, {}", vec.x, vec.y, vec.z);

  Vector3f vec2 = Vector3f{0.5, 0.5, 0.2} + vec;
  std::println("{}, {}, {}", vec2.x, vec2.y, vec2.z);

  Normal3f n{1, 0, 0};
  std::println("{}, {}, {}", n.x, n.y, n.z);
}
