cmake_minimum_required(VERSION 4.0)

# enable module scanning
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(vulkan-pbr VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Lib for types module ---
add_library(types_module)
target_sources(types_module PUBLIC FILE_SET CXX_MODULES FILES src/types.cppm)

# --- Lib for color modules ---
add_library(color_module)
target_sources(color_module PUBLIC FILE_SET CXX_MODULES FILES src/color.cppm)
target_link_libraries(color_module PUBLIC types_module)

# --- Lib for math modules ---
add_library(math_module)
target_sources(math_module PUBLIC FILE_SET CXX_MODULES FILES
  src/math/math.cppm
  src/math/vector.cppm
  src/math/point.cppm
  src/math/matrix.cppm
  src/math/transform.cppm
  src/math/utils.cppm
)
# math modules need types
target_link_libraries(math_module PUBLIC types_module)

# --- Lib for camera modules ---
add_library(camera_module)
target_sources(camera_module PUBLIC FILE_SET CXX_MODULES FILES
  src/camera/ray.cppm
  src/camera/camera.cppm
)
# camera module needs types and math modules
target_link_libraries(camera_module PUBLIC math_module color_module)

# --- Lib for geometry module ---
add_library(geometry_module)
target_sources(geometry_module PUBLIC FILE_SET CXX_MODULES FILES
  src/geometry/geometry.cppm
  src/geometry/sphere.cppm
)
target_link_libraries(geometry_module PUBLIC types_module math_module camera_module)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
  types_module
  math_module
  camera_module
  geometry_module
)
