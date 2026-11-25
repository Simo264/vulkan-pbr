export module image;

import core;
import std;

// 8-bit images (i.e. 24 bits per pixel for a color image) are considered Low Dynamic Range.
export struct ImageLDR
{
  std::vector<u8> pixels{}; // RGB8 format
  i32 width{0};
  i32 height{0};
  i32 channels{0};

  bool is_valid() const
  {
    return width > 0 && height > 0 && channels > 0 && pixels.size() == static_cast<usize>(width) * height * channels;
  }
};

// An High Dynamic Range image stores pixel values that span the whole tonal range of real-world scenes.
// Therefore, is encoded in a format that allows the largest range of values, e.g. floating-point values 
// stored with 32 bits per color channel.
// Another characteristic of an HDR image is that it stores linear values. 
// This means that the value of a pixel from an HDR image is proportional to the amount of light measured by the camera.
export struct ImageHDR
{
  std::vector<f32> pixels{};
  i32 width{0};
  i32 height{0};
  i32 channels{0};

  bool is_valid() const
  {
    return width > 0 && height > 0 && channels > 0 && pixels.size() == static_cast<usize>(width) * height * channels;
  }
};
