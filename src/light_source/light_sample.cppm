export module light_source:light_sample;

import core;

export struct LightSample
{
  Vector3f wi{};    // the direction (world space) along which light arrives at the point
  Color3f  L{};     // the amount of radiance leaving the light toward the receiving point
  f32      pdf{};   // the PDF value for the light sample
  f32      dist{};  // distance from light
};
