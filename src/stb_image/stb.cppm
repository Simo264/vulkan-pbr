module;
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

export module stb;

import core;
import std;

export namespace stb
{
  [[nodiscard]] bool save_image(std::string_view filename, std::span<const u8> pixels, i32 image_width, i32 image_height)
  {
    if (image_width <= 0 || image_height <= 0 || filename.empty())
      return false;

    constexpr usize channels = 3;

    auto width = static_cast<usize>(image_width);
    auto height = static_cast<usize>(image_height);

    // Check size overflow.
    if (width > std::numeric_limits<usize>::max() / height / channels)
        return false;

    usize expected_size = width * height * channels;

    if (pixels.size() != expected_size)
      return false;

    // stb_image_write expects an int stride.
    if (width > static_cast<usize>(std::numeric_limits<i32>::max()) / channels)
      return false;

    // string_view::data() is not necessarily null-terminated.
    std::string path{filename};

    return stbi_write_png(path.c_str(),
                          static_cast<i32>(image_width),
                          static_cast<i32>(image_height),
                          static_cast<i32>(channels),
                          pixels.data(),
                          static_cast<i32>(width * channels)) != 0;
  }
}
