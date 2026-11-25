export module scene;

import core;
import std;

import camera;
import hit_record;
import primitive;
import light_source;

export class Scene
{
public:
  explicit Scene() : m_objects{} {}

  auto& objects() const { return m_objects; }
  auto& lights() const { return m_lights; }
  auto& environment_light() const { return m_environment_light; }

  void add_object(Primitive obj) { m_objects.push_back(obj); }
  void add_light(PointLight light) { m_lights.push_back(light); }
  void add_light(DiffuseAreaLight l)
  {
    f32 area = std::visit([&](const auto& s){ return s.surface.area; }, l.shape);
    if (area <= 0.f) // this shape cannot be an area light
    {
      std::println("Warning: Tried to add a DiffuseAreaLight with zero area. Ignoring.");
      return;
    }

    m_objects.push_back(l.shape); // shape already carries { Le, two_sided }
    m_lights.push_back(l);
  }
  void set_environment(InfiniteLightSource l) { m_environment_light = l; }

  std::optional<HitRecord> intersect(Ray ray, f32 t_min = 1e-4f, f32 t_max = MAX_FLOAT_32) const
  {
    std::optional<HitRecord> best = std::nullopt;
    f32 closest = std::min(t_max, ray.t_max);

    for (const auto& obj : m_objects)
    {
      std::visit([&](const auto& obj) {
        if (auto hit = intersect_primitive(obj, ray, t_min, closest))
        {
          closest = hit->t;
          best = hit;
        }
      }, obj);
    }
    return best;
  }

private:
  std::vector<Primitive> m_objects;
  std::vector<LightSource> m_lights;
  std::optional<InfiniteLightSource> m_environment_light; // optional: a single uniform infinite light for the entire scene
};
