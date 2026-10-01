cmake_minimum_required(VERSION 4.0)

# enable module scanning
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(vulkan-pbr VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Lib for types module ---
add_library(types_modules)
target_sources(types_modules PUBLIC FILE_SET CXX_MODULES FILES
  src/types.cppm
)

# --- Lib for math modules ---
add_library(math_modules)
target_sources(math_modules PUBLIC FILE_SET CXX_MODULES FILES
  src/math/math.cppm
  src/math/vector.cppm
  src/math/point.cppm
  src/math/normal.cppm
)
# math modules need types
target_link_libraries(math_modules PUBLIC types_modules)

add_executable(${PROJECT_NAME}
  src/main.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE math_modules)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
