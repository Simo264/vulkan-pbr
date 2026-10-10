import std;

import types;
import math;
import camera;
import color;
import geometry;

i32 main()
{
  constexpr auto image_width = 640;
  constexpr auto image_height = 360;
  constexpr auto fov = 60.f; // in degrees

  Camera cam{
    Point3f {0.f, 0.f, 0.f},   // at the origin
    Vector3f{0.f, 0.f, 1.f},   // forward
    fov,
    image_width,
    image_height
  };

  Sphere sphere{ Point3f{0.f, 0.f, 3.f}, 1.0f };

  for (auto j = 0; j < image_height; ++j)
  {
    for (auto i = 0; i < image_width; ++i)
    {
      constexpr Color3f white {1.0f, 1.0f, 1.0f};
      constexpr Color3f sky   {0.5f, 0.7f, 1.0f};
      constexpr Color3f red   {1.0f, 0.1f, 0.1f};

      Ray r = cam.generate_ray(i, j);

      f32 t_sky = 0.5f * (r.d.y + 1.f);
      Color3f c = lerp(white, sky, t_sky);

      if (sphere.intersect(r) > 0.f)
        c = red;

      cam.film.at(i, j) = c;
    }
  }


  std::ofstream out{"output.ppm", std::ios::binary};
  if (!out)
  {
    std::println(std::cerr, "Cannot open output.ppm");
    return 1;
  }

  out << "P6\n" << image_width << ' ' << image_height << "\n255\n";
  auto to255 = [](f32 v) -> std::uint8_t {
    v = std::clamp(v, 0.f, 1.f);
    return static_cast<std::uint8_t>(255.f * v);
  };

  for (i32 j = 0; j < image_height; ++j)
  {
    for (i32 i = 0; i < image_width; ++i)
    {
      const Color3f c = clamp(cam.film.at(i, j), 0.f, 1.f);
      out.put(static_cast<char>(to255(c.r)));
      out.put(static_cast<char>(to255(c.g)));
      out.put(static_cast<char>(to255(c.b)));
    }
  }
  std::println("Wrote output.ppm ({}x{})", image_width, image_height);
  return 0;
}
