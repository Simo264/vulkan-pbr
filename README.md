# vulkan-pbr

A physically-based renderer implemented in C++23, following the
[Physically Based Rendering: From Theory to Implementation](https://pbr-book.org/) book.

The project starts as a CPU-only ray tracer and will progressively evolve
to leverage GPU acceleration via Vulkan.

## Requirements

- CMake 4.0 or newer
- Ninja build system
- A compiler with C++23 module support:
  - GCC 16+ or Clang 22+
- Vulkan SDK

## Building with GCC

```bash
cmake --preset gcc-debug
cmake --build --preset gcc-debug
```

## Building with Clang

```bash
cmake --preset clang-debug
cmake --build --preset clang-debug
```

## Manual configuration (without presets)

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Running 

```bash
./build/gcc-debug/vulkan-pbr
```
