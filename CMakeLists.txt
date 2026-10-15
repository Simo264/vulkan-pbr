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
#       Types module
# -------------------------------
add_library(types_module)
target_sources(types_module PUBLIC FILE_SET CXX_MODULES FILES src/types.cppm)

# -------------------------------
#       Math module
# -------------------------------
add_library(math_module)
target_sources(math_module PUBLIC FILE_SET CXX_MODULES FILES
  src/math/math.cppm
  src/math/vector.cppm
  src/math/point.cppm
  src/math/matrix.cppm
  src/math/transform.cppm
  src/math/utils.cppm
)
target_link_libraries(math_module PUBLIC types_module)

# -------------------------------
#       Color module
# -------------------------------
add_library(color_module)
target_sources(color_module PUBLIC FILE_SET CXX_MODULES FILES src/color.cppm)
target_link_libraries(color_module PUBLIC types_module)

# -------------------------------
#       Camera module
# -------------------------------
add_library(camera_module)
target_sources(camera_module PUBLIC FILE_SET CXX_MODULES FILES
  src/camera/ray.cppm
  src/camera/camera.cppm
)
target_link_libraries(camera_module PUBLIC math_module color_module)

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
target_link_libraries(scene_module PUBLIC types_module math_module camera_module)

# -------------------------------
#       glTF_loader module
# -------------------------------
add_library(glTFLoader_module)
target_sources(glTFLoader_module PUBLIC FILE_SET CXX_MODULES FILES
  src/gltf_loader/gltf_loader.cppm
)
target_link_libraries(glTFLoader_module PUBLIC types_module math_module color_module scene_module)


add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
  types_module
  math_module
  camera_module
  scene_module
  glTFLoader_module
)
