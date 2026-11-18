export module scene;

import core;
import camera;
import hit_record;
import primitives;
import lights;
import std;

export class Scene
{
public:
  explicit Scene() : m_objects{} {}

  auto& objects() const { return m_objects; }
  auto& lights() const { return m_lights; }

  void add(Primitive obj) { m_objects.push_back(std::move(obj)); }
  void add(Light light)
  {
    std::visit([&](auto& l){
      using T = std::decay_t<decltype(l)>;

      // if `light` is DiffuseAreaLight
      if constexpr (std::is_same_v<T, DiffuseAreaLight>)
      {
        f32 area = std::visit([&](const auto& prim){ return prim.surface.area; }, l.shape);
        if (area <= 0.f) // this shape cannot be an area light
          return;

        m_objects.push_back(l.shape); // shape already carries { Le, two_sided }
      }
      else if constexpr (std::is_same_v<T, PointLight>)
      { /* if `light` object is a PointLight, then do nothing. Only push into the `m_lights` vector */ }
    }, light);

    m_lights.push_back(std::move(light));
  }

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
          best = HitRecord{ hit->t, hit->p, hit->n, hit->mat, hit->Le };
        }
      }, obj);
    }
    return best;
  }

private:
  std::vector<Primitive> m_objects;
  std::vector<Light> m_lights;
};
