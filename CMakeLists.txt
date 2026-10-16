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

# -------------------------------
#       Core.Types module
# -------------------------------
add_library(core_types_module)
target_sources(core_types_module PUBLIC FILE_SET CXX_MODULES FILES
  src/core/types.cppm
)

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
)
target_link_libraries(core_math_module PUBLIC
  core_types_module
)

# -------------------------------
#       Core.Color module
# -------------------------------
add_library(core_color_module)
target_sources(core_color_module PUBLIC FILE_SET CXX_MODULES FILES
  src/core/color.cppm
)
target_link_libraries(core_color_module PUBLIC
  core_types_module
)

# -------------------------------
#       Core module
# -------------------------------
add_library(core_module)
target_sources(core_module PUBLIC FILE_SET CXX_MODULES FILES
  src/core/core.cppm
)
target_link_libraries(core_module PUBLIC
  core_types_module
  core_math_module
  core_color_module
)

# -------------------------------
#       Camera module
# -------------------------------
add_library(camera_module)
target_sources(camera_module PUBLIC FILE_SET CXX_MODULES FILES
  src/camera/ray.cppm
  src/camera/camera.cppm
)
target_link_libraries(camera_module PUBLIC
  core_module
)

# -------------------------------
#       Scene module
# -------------------------------
add_library(scene_module)
target_sources(scene_module PUBLIC FILE_SET CXX_MODULES FILES
  src/scene/scene.cppm
  src/scene/hit_record.cppm
  src/scene/sphere.cppm
  src/scene/disk.cppm
  src/scene/triangle_mesh.cppm
)
target_link_libraries(scene_module PUBLIC
  core_module
  camera_module
)

# -------------------------------
#       glTF_loader module
# -------------------------------
add_library(glTFLoader_module)
target_sources(glTFLoader_module PUBLIC FILE_SET CXX_MODULES FILES
  src/gltf_loader/gltf_loader.cppm
)
target_link_libraries(glTFLoader_module PUBLIC
  core_module
  scene_module
)

# -------------------------------
#       executable file
# -------------------------------
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
  core_module
  camera_module
  scene_module
  glTFLoader_module
)
