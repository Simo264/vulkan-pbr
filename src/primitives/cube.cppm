export module primitives.cube;

import core;
import camera;
import hit_record;
import std;

export class Cube
{
public:
  explicit Cube(Point3f center, Color3f reflectance = Color3f{1.0f}) :
    object_to_world{translate(center)}, world_to_object{inverse(object_to_world)}, reflectance{reflectance}
  {}

  void set_transform(const Transform& t)
  {
    object_to_world = t;
    world_to_object = inverse(t);
  }

  std::optional<HitRecord> intersect(const Ray& ray, f32 t_min, f32 t_max) const
  {
    // 1) Raggio in object space
    Point3f  lo = world_to_object(ray.o);   // punto: con traslazione
    Vector3f ld = world_to_object(ray.d);   // direzione: SOLO 3x3, NON normalizzare

    // 2) Slab test su [-0.5, 0.5]
    f32 t_near = -std::numeric_limits<f32>::infinity();
    f32 t_far  =  std::numeric_limits<f32>::infinity();
    i32  enter_axis = -1;
    bool enter_positive = false;

    for (i32 a = 0; a < 3; ++a)
    {
      f32 inv = 1.f / ld[a];
      f32 t0 = (-0.5f - lo[a]) * inv;
      f32 t1 = ( 0.5f - lo[a]) * inv;
      bool positive = false;

      if (t0 > t1)
      {
        std::swap(t0, t1);
        positive = true;
      }

      if (t0 > t_near)
      {
        t_near = t0;
        enter_axis = a;
        enter_positive = positive;
      }
      t_far = std::min(t_far, t1);
      if (t_near > t_far)
        return std::nullopt;
    }

    // 3) Range check
    if (t_near < t_min || t_near > t_max)
      return std::nullopt;

    // 4) Hit record
    HitRecord hit;
    hit.t = t_near;
    hit.p = ray(t_near);

    // normale locale: asse della faccia di ingresso, segno in base al lato
    Vector3f local_n{0.f, 0.f, 0.f};
    local_n[enter_axis] = enter_positive ? 1.f : -1.f;

    // normale in world con inversa-trasposta (Transform lo fa già via Normal3f)
    hit.n = normalize(object_to_world(Normal3f{local_n.x,local_n.y,local_n.z}).to_vector());
    if (dot(hit.n, ray.d) > 0.f) hit.n = -hit.n;

    hit.reflectance = reflectance;
    return hit;
  }

  Transform object_to_world;
  Transform world_to_object; // pre-calculated: inverse(object_to_world)
  Color3f   reflectance;
};
