export module scene;

import core;
import camera;
import hit_record;
import primitives;
import lights;
import std;

export using SceneObject = std::variant<Plane, Disk, Sphere, Cube>;

export class Scene
{
public:
  explicit Scene() : m_objects{} {}

  auto& objects() const { return m_objects; }
  auto& lights() const { return m_lights; }

  void add(SceneObject obj) { m_objects.push_back(std::move(obj)); }
  void add(LightObject light) { m_lights.push_back(std::move(light)); }

  std::optional<HitRecord> intersect(Ray ray, f32 t_min = 1e-4f, f32 t_max = MAX_FLOAT_32) const
  {
    std::optional<HitRecord> best = std::nullopt;
    f32 closest = t_max;
    for (const auto& obj : m_objects)
    {
      std::visit([&](const auto& o) {
        if (auto hit = o.intersect(ray, t_min, closest))
        {
          closest = hit->t;
          best = HitRecord{ hit->t, hit->p, hit->n, hit->mat };
        }
      }, obj);
    }
    return best;
  }

private:
  std::vector<SceneObject> m_objects;
  std::vector<LightObject> m_lights;
};
