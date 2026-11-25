module;
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

export module stb;

import core;
import std;

import image;

export namespace stb
{
  [[nodiscard]] bool save_image_rgb8(std::string_view filename, const ImageLDR& image)
  {
    if (!image.is_valid() || filename.empty())
      return false;

    std::string path{filename};
    bool write = stbi_write_png(
        path.c_str(),
        image.width,
        image.height,
        image.channels,
        image.pixels.data(),
        image.width * image.channels
    );
    return write != 0;
  }
  [[nodiscard]] bool save_image_rgb32(std::string_view filename, const ImageHDR& image)
  {
    if (!image.is_valid() || filename.empty())
      return false;

    std::string path{filename};
    bool write = stbi_write_hdr(
        path.c_str(),
        image.width,
        image.height,
        image.channels,
        image.pixels.data()
    );
    return write != 0;
  }

  [[nodiscard]] std::optional<ImageLDR> load_image_rgb8(std::string_view filename) {
    if (filename.empty())
      return std::nullopt;

    std::string path{filename};
    i32 width{0}, height{0}, channels{0};

    u8* data = static_cast<u8*>(stbi_load(path.c_str(), &width, &height, &channels, 3));
    if (!data)
      return std::nullopt;

    usize pixel_count = static_cast<usize>(width) * static_cast<usize>(height) * 3;
    std::vector<u8> pixels(data, data + pixel_count);
    stbi_image_free(data);

    ImageLDR image{
      .pixels = std::move(pixels),
      .width = width,
      .height = height,
      .channels = 3
    };
    return image;
  }

  [[nodiscard]] std::optional<ImageHDR> load_image_rgb32(std::string_view filename)
  {
    if (filename.empty())
      return std::nullopt;

    // Set gamma to 1.0 for linear color space
    // stbi_ldr_to_hdr_gamma(1.0f);

    // Load image as float array (values will be linear)
    std::string path{filename};
    i32 width{0}, height{0}, channels{0};
    f32* data = stbi_loadf(path.c_str(), &width, &height, &channels, 3); // 3 -> force for RGB

    if (!data)
      return std::nullopt;

    usize pixel_count = static_cast<usize>(width) * static_cast<usize>(height) * 3;
    std::vector<f32> pixels(data, data + pixel_count);
    stbi_image_free(data);

    ImageHDR image{
      .pixels = std::move(pixels),
      .width = width,
      .height = height,
      .channels = 3
    };
    return image;
  }
}
