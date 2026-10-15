export module scene;

export import :hit_record;
export import :sphere;
export import :disk;

import types;
import math;
import color;

export using SceneObject = std::variant<Sphere, Disk>;

export class Scene
{
public:
  Scene() = default;

  void add(SceneObject obj) { m_objects.push_back(std::move(obj)); }

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min = 1e-4f, f32 t_max = MAX_FLOAT_32) const
  {
    std::optional<HitRecord> best = std::nullopt;
    f32 closest = t_max;
    for (const auto& obj : m_objects)
    {
      std::visit([&](const auto& o) {
        if (auto hit = o.intersect(ray, t_min, closest))
        {
          closest = hit->t;

          Vector3f n = hit->n;
          if (dot(n, ray.d) > 0.f)
            n = -n;

          best = HitRecord{ hit->t, hit->p, n, o.reflectance };
        }
      }, obj);
    }
    return best;
  }

private:
  std::vector<SceneObject> m_objects;
};
