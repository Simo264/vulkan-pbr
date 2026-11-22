cmake_minimum_required(VERSION 4.0)

project(vulkan-pbr VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------------
#       Core.Types module
# -------------------------------
add_library(core_types_module)
target_sources(core_types_module PUBLIC FILE_SET CXX_MODULES FILES src/core/types.cppm)

# -------------------------------
#       Core.Math module
# -------------------------------
add_library(core_math_module)
target_sources(core_math_module PUBLIC FILE_SET CXX_MODULES FILES
  src/core/math/math.cppm
  src/core/math/vector.cppm
  src/core/math/point.cppm
  src/core/math/matrix.cppm
  src/core/math/transform.cppm
  src/core/math/utils.cppm
  src/core/math/random.cppm
  src/core/math/frame.cppm
)
target_link_libraries(core_math_module PUBLIC core_types_module)

# -------------------------------
#       Core.Color module
# -------------------------------
add_library(core_color_module)
target_sources(core_color_module PUBLIC FILE_SET CXX_MODULES FILES src/core/color.cppm)
target_link_libraries(core_color_module PUBLIC core_types_module)

# -------------------------------
#       Core module
# -------------------------------
add_library(core_module)
target_sources(core_module PUBLIC FILE_SET CXX_MODULES FILES src/core/core.cppm)
target_link_libraries(core_module PUBLIC core_types_module core_math_module core_color_module)

# -------------------------------
#       Camera module
# -------------------------------
add_library(camera_module)
target_sources(camera_module PUBLIC FILE_SET CXX_MODULES FILES
  src/camera/ray.cppm
  src/camera/camera.cppm
)
target_link_libraries(camera_module PUBLIC core_module)

# -------------------------------
#       Sampling module
# -------------------------------
add_library(sampling_module)
target_sources(sampling_module PUBLIC FILE_SET CXX_MODULES FILES src/sampling.cppm)
target_link_libraries(sampling_module PUBLIC core_module)

# -------------------------------
#       BSDF module
# -------------------------------
add_library(bsdf_module)
target_sources(bsdf_module PUBLIC FILE_SET CXX_MODULES FILES
  src/bsdf/bsdf.cppm
  src/bsdf/fresnel.cppm
  src/bsdf/trowbridge_reitz.cppm
)
target_link_libraries(bsdf_module PUBLIC core_module sampling_module)

# -------------------------------
#       Materials module
# -------------------------------
add_library(materials_module)
target_sources(materials_module PUBLIC FILE_SET CXX_MODULES FILES
  src/materials/materials.cppm
  src/materials/diffuse.cppm
  src/materials/conductor.cppm
  src/materials/dielectric.cppm
)
target_link_libraries(materials_module PUBLIC core_module bsdf_module)

# -------------------------------
#       Hit_record module
# -------------------------------
add_library(hitrecord_module)
target_sources(hitrecord_module PUBLIC FILE_SET CXX_MODULES FILES src/hit_record.cppm)
target_link_libraries(hitrecord_module PUBLIC core_module materials_module)

# -------------------------------
#       Primitives module
# -------------------------------
add_library(primitives_module)
target_sources(primitives_module PUBLIC FILE_SET CXX_MODULES FILES
  src/primitives/primitives.cppm
  src/primitives/primitive_sample.cppm
  src/primitives/surface.cppm
  src/primitives/plane.cppm
  src/primitives/disk.cppm
  src/primitives/sphere.cppm
  src/primitives/cube.cppm
)
target_link_libraries(primitives_module PUBLIC
  core_module
  camera_module
  hitrecord_module
  materials_module
  sampling_module
)

# -------------------------------
#       Lights module
# -------------------------------
add_library(lights_module)
target_sources(lights_module PUBLIC FILE_SET CXX_MODULES FILES
  src/lights/lights.cppm
  src/lights/light_sample.cppm
  src/lights/point_light.cppm
  src/lights/diffuse_area_light.cppm
  src/lights/uniform_infinite_light.cppm
)
target_link_libraries(lights_module PUBLIC
  core_module
  primitives_module
)

# -------------------------------
#       Scene module
# -------------------------------
add_library(scene_module)
target_sources(scene_module PUBLIC FILE_SET CXX_MODULES FILES src/scene.cppm)
target_link_libraries(scene_module PUBLIC
  core_module
  camera_module
  hitrecord_module
  primitives_module
  lights_module
)

# -------------------------------
#       stb module
# -------------------------------
add_library(stb_module)
target_sources(stb_module PUBLIC FILE_SET CXX_MODULES FILES src/stb_image/stb.cppm)
target_link_libraries(stb_module PUBLIC core_module core_module)

# -------------------------------
#       executable file
# -------------------------------
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
  core_module
  camera_module
  scene_module
  hitrecord_module
  primitives_module
  materials_module
  lights_module
  stb_module
)
