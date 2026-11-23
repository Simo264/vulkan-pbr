export module image;

import core;
import std;

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
