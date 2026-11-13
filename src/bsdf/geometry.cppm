export module bsdf:geometry;

import core;
import std;

export class ShadingFrame
{
public:
  ShadingFrame(Vector3f n)
  {
    this->n = n;
    f32 sign = std::copysign(1.f, n.z);
    f32 a = -1.f / (sign + n.z);
    f32 b = n.x * n.y * a;

    s = Vector3f{ 1.f + sign * n.x * n.x * a, sign * b, -sign * n.x };
    t = Vector3f{ b, sign + n.y * n.y * a, -n.y };
  }

  // world to local
  Vector3f to_local(Vector3f x) const { return { dot(x, s), dot(x, t), dot(x, n) }; }
  // local to world
  Vector3f to_world(Vector3f x) const { return x.x * s + x.y * t + x.z * n; }

  Vector3f s, t, n;
};

export bool same_hemisphere(Vector3f w, Vector3f wp)
{
  return w.z * wp.z > 0;
}

// mirror reflection of wo about the surface normal wm.
export Vector3f reflect(Vector3f wo, Vector3f wm)
{
  // Perfect Specular Reflection:
  //   wi = -wo + 2 * dot(wo, wm) * wm
  return -wo + 2.f * dot(wo, wm) * wm;
}

// Returns the refracted direction (unit vector, pointing away from the surface
// on the other side), or std::nullopt on Total Internal Reflection (TIR).
// Inputs:
//   wo      : outgoing direction
//   wm      : surface normal
//   eta_rel : eta_transmitted / eta_incident.
export std::optional<Vector3f> refract(Vector3f wo, Vector3f wm, f32 eta_rel)
{
  f32 cos_i  = dot(wo, wm);
  f32 sin2_i = std::max(0.f, 1.f - cos_i * cos_i);
  f32 sin2_t = sin2_i / (eta_rel * eta_rel);

  if (sin2_t >= 1.f)
    return std::nullopt; // Total Internal Reflection

  f32 cos_t  = std::sqrt(1.f - sin2_t);
  return -wo / eta_rel + (cos_i / eta_rel - std::copysign(cos_t, cos_i)) * wm;
}

