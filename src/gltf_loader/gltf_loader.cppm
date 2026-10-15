module;
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

export module gltf_loader;

import types;
import math;
import color;
import scene;
import std;

template <typename T>
std::vector<T> read_accessor(const tinygltf::Model& model, i32 accessor_index)
{
  if (accessor_index < 0) return {};

  const auto& accessor = model.accessors[accessor_index];
  const auto& buffer_view = model.bufferViews[accessor.bufferView];
  const auto& buffer = model.buffers[buffer_view.buffer];

  const byte* data_ptr = reinterpret_cast<const byte*>(buffer.data.data() + buffer_view.byteOffset + accessor.byteOffset);
  usize element_size = sizeof(T);
  usize stride = buffer_view.byteStride > 0 ? buffer_view.byteStride : element_size;

  std::vector<T> result(accessor.count);
  for (usize i = 0; i < accessor.count; ++i)
    std::memcpy(&result[i], data_ptr + i * stride, element_size);

  return result;
}

std::vector<Vector3i> read_indices(const tinygltf::Model& model, i32 accessor_index, usize vertex_count)
{
  std::vector<Vector3i> indices;

  if (accessor_index < 0)
  {
    indices.reserve(vertex_count / 3);
    for (usize i = 0; i < vertex_count; i += 3)
      indices.emplace_back(static_cast<i32>(i), static_cast<i32>(i + 1), static_cast<i32>(i + 2));
    return indices;
  }

  const auto& accessor = model.accessors[accessor_index];
  const auto& buffer_view = model.bufferViews[accessor.bufferView];
  const auto& buffer = model.buffers[buffer_view.buffer];
  const byte* data_ptr = reinterpret_cast<const byte*>(buffer.data.data() + buffer_view.byteOffset + accessor.byteOffset);

  indices.reserve(accessor.count / 3);
  for (usize i = 0; i < accessor.count; i += 3)
  {
    Vector3i tri;
    switch(accessor.componentType)
    {
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        tri.x = static_cast<i32>(data_ptr[i]);
        tri.y = static_cast<i32>(data_ptr[i+1]);
        tri.z = static_cast<i32>(data_ptr[i+2]);
        break;

      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        {
          const u16* ptr = reinterpret_cast<const u16*>(data_ptr);
          tri.x = static_cast<i32>(ptr[i]);
          tri.y = static_cast<i32>(ptr[i+1]);
          tri.z = static_cast<i32>(ptr[i+2]);
        }
        break;

      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        {
          const u32* ptr = reinterpret_cast<const u32*>(data_ptr);
          tri.x = static_cast<i32>(ptr[i]);
          tri.y = static_cast<i32>(ptr[i+1]);
          tri.z = static_cast<i32>(ptr[i+2]);
        }
        break;

      default:
        break;
    }
    indices.push_back(tri);
  }
  return indices;
}

export TriangleMesh load_gltf_model(const std::filesystem::path& inputfile)
{
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err, warn;

  bool ret = false;
  std::string ext = inputfile.extension().string();

  if (ext == ".glb")
    ret = loader.LoadBinaryFromFile(&model, &err, &warn, inputfile.string());
  else
    ret = loader.LoadASCIIFromFile(&model, &err, &warn, inputfile.string());

  if (!ret)
    throw std::runtime_error(std::format("Error on loading glTF model: {}", err));

  if (model.meshes.empty())
    return TriangleMesh{};

  TriangleMesh mesh;
  u32 vertex_offset = 0;

  // Iterate on all meshes (support files with more objects)
  for (const auto& gltf_mesh : model.meshes)
  {
    for (const auto& primitive : gltf_mesh.primitives)
    {
      // Positions
      auto pos_it = primitive.attributes.find("POSITION");
      if (pos_it == primitive.attributes.end()) continue;

      auto positions = read_accessor<Point3f>(model, pos_it->second);
      u32 current_vertex_count = static_cast<u32>(mesh.positions.size());

      // Normals
      auto norm_it = primitive.attributes.find("NORMAL");
      std::vector<Vector3f> normals;
      if (norm_it != primitive.attributes.end())
        normals = read_accessor<Vector3f>(model, norm_it->second);
      else
        normals.resize(positions.size(), Vector3f(0.0f, 0.0f, 0.0f));

      // UVs
      auto uv_it = primitive.attributes.find("TEXCOORD_0");
      std::vector<Point2f> uvs;
      if (uv_it != primitive.attributes.end())
        uvs = read_accessor<Point2f>(model, uv_it->second);
      else
        uvs.resize(positions.size(), Point2f(0.0f, 0.0f));

      // indices
      auto prim_indices = read_indices(model, primitive.indices, positions.size());
      mesh.positions.insert(mesh.positions.end(), positions.begin(), positions.end());
      mesh.normals.insert(mesh.normals.end(), normals.begin(), normals.end());
      mesh.uvs.insert(mesh.uvs.end(), uvs.begin(), uvs.end());
      for (const auto& tri : prim_indices)
      {
        mesh.indices.emplace_back(tri.x + current_vertex_count,
                                  tri.y + current_vertex_count,
                                  tri.z + current_vertex_count);
      }

      // Materials
      if (primitive.material >= 0 && mesh.reflectance.r == 0 && mesh.reflectance.g == 0 && mesh.reflectance.b == 0)
      {
        const auto& mat = model.materials[primitive.material];
        if (mat.pbrMetallicRoughness.baseColorFactor.size() >= 3)
        {
            mesh.reflectance = Color3f(static_cast<f32>(mat.pbrMetallicRoughness.baseColorFactor[0]),
                                       static_cast<f32>(mat.pbrMetallicRoughness.baseColorFactor[1]),
                                       static_cast<f32>(mat.pbrMetallicRoughness.baseColorFactor[2]));
        }
      }

      vertex_offset += static_cast<u32>(positions.size());
    }
  }

  return mesh;
}
