export module camera;

export import :ray;

import types;
import math;
import color;

import std;

export struct Film
{
  Film() = default;
  Film(i32 res_x, i32 res_y, f32 hw, f32 hh) :
    res_x{res_x},
    res_y{res_y},
    half_w{hw},
    half_h{hh},
    pixels(static_cast<usize>(res_x) * static_cast<usize>(res_y), Color3f{0.f, 0.f, 0.f})
  {}

  void add_sample(i32 x, i32 y, const Color3f& c) { pixels.at(static_cast<usize>(y) * res_x + x) += c; }

  Color3f& at(i32 x, i32 y) { return pixels.at(static_cast<usize>(y) * res_x + x); }

  // resolution
  i32 res_x{}, res_y{};

  // dimension of the film
  f32 half_w{}, half_h{};

  // color buffer
  std::vector<Color3f> pixels;
};

export class Camera
{
public:
  Camera(const Point3f&  pos,
         const Point3f&  target,
         f32             fov_degrees,
         i32             res_x,
         i32             res_y,
         const Vector3f& world_up = {0.f, 1.f, 0.f})
      : position{pos}, view_dir{normalize(target - pos)}
  {
    right = normalize(cross(world_up, view_dir));
    up    = cross(view_dir, right);

    f32 aspect = static_cast<f32>(res_x) / static_cast<f32>(res_y);
    f32 half_h = std::tan(to_radians(fov_degrees) * 0.5f);
    f32 half_w = half_h * aspect;

    film = Film{res_x, res_y, half_w, half_h};
  }

  Ray generate_ray(f32 px, f32 py, f32 sx = 0.5f, f32 sy = 0.5f) const
  {
    f32 u = (px + sx) / static_cast<f32>(film.res_x);
    f32 v = (py + sy) / static_cast<f32>(film.res_y);
    f32 ndc_x =  2.f * u - 1.f;
    f32 ndc_y =  1.f - 2.f * v;

    Point3f p_film = position
                     + view_dir
                     + (ndc_x * film.half_w) * right
                     + (ndc_y * film.half_h) * up;

    return Ray{ position, normalize(p_film - position) };
  }

  Point3f position{};
  Vector3f view_dir{}, right{}, up{}; // orthogonal basis
  Film film{};
};
