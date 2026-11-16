export module lights;

import core;
import std;

export class PointLight
{
public:
  explicit PointLight(Point3f p = Point3f{0.f,0.f,0.f}, f32 intensity = 1.f) :
    position{p}, intensity{intensity}
  {}

  Point3f position;
  f32 intensity;
};

// export class DirectionalLight{};
// export class SpotLight{};
// export class AreaLight{};

export using LightObject = std::variant<PointLight /* DirectionalLight,SpotLight,AreaLight */>;

export struct LightSample
{
  Vector3f wi{};    // the direction (world space) along which light arrives at the point
  f32      L{};     // the amount of radiance leaving the light toward the receiving point;
  f32      pdf{};   // the PDF value for the light sample
  f32      dist{};  // distance from light
};

export LightSample sample_li(PointLight l, Point3f p)
{
  LightSample s;
  Vector3f d = l.position - p;
  f32 dist = length(d);

  s.dist = dist;
  s.wi = d / dist; // = normalize(d)
  s.L = l.intensity / (dist); // l.intensity / (dist*dist);
  s.pdf = 1.f;
  return s;
}
