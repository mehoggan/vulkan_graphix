#ifndef VULKAN_GRAPHIX_RENDER_VERTEX_H
#define VULKAN_GRAPHIX_RENDER_VERTEX_H

// Vertex formats for the Render module: the Vulkan vertex-input layout of
// any vertex described by VertexTypes::AttributeTraits, and the two
// standard VertexTypes vertex types (and their InterleavedData containers)
// most drawing needs.

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"
#include "vulkan_graphix/VertexTypes/InterleavedData.hpp"

namespace vulkan_graphix::Render {

// The VkFormat a vertex attribute of type T is read as.
template <typename T>
struct AttributeFormat;

template <>
struct AttributeFormat<float> {
  static constexpr VkFormat value = VK_FORMAT_R32_SFLOAT;
};
template <>
struct AttributeFormat<glm::vec2> {
  static constexpr VkFormat value = VK_FORMAT_R32G32_SFLOAT;
};
template <>
struct AttributeFormat<glm::vec3> {
  static constexpr VkFormat value = VK_FORMAT_R32G32B32_SFLOAT;
};
template <>
struct AttributeFormat<glm::vec4> {
  static constexpr VkFormat value = VK_FORMAT_R32G32B32A32_SFLOAT;
};
template <>
struct AttributeFormat<glm::ivec2> {
  static constexpr VkFormat value = VK_FORMAT_R32G32_SINT;
};
template <>
struct AttributeFormat<glm::ivec3> {
  static constexpr VkFormat value = VK_FORMAT_R32G32B32_SINT;
};
template <>
struct AttributeFormat<glm::ivec4> {
  static constexpr VkFormat value = VK_FORMAT_R32G32B32A32_SINT;
};
template <>
struct AttributeFormat<glm::uvec2> {
  static constexpr VkFormat value = VK_FORMAT_R32G32_UINT;
};
template <>
struct AttributeFormat<glm::uvec3> {
  static constexpr VkFormat value = VK_FORMAT_R32G32B32_UINT;
};
template <>
struct AttributeFormat<glm::uvec4> {
  static constexpr VkFormat value = VK_FORMAT_R32G32B32A32_UINT;
};

// One interleaved vertex binding (binding 0): its stride and one attribute
// per type, at locations 0, 1, 2, ... in declaration order.
struct VertexLayout {
  std::uint32_t m_stride = 0;
  std::vector<VkVertexInputAttributeDescription> m_attributes;
};

template <typename... Ts>
VertexLayout vertexLayout(VertexTypes::AttributeTraits<Ts...> /*traits*/) {
  using Traits = VertexTypes::AttributeTraits<Ts...>;
  constexpr std::array<std::size_t, sizeof...(Ts)> offsets =
      Traits::byteOffsets();
  constexpr std::array<VkFormat, sizeof...(Ts)> formats = {
      AttributeFormat<Ts>::value...};
  VertexLayout layout;
  layout.m_stride = static_cast<std::uint32_t>(Traits::stride);
  for (std::size_t i = 0; i < sizeof...(Ts); ++i) {
    layout.m_attributes.push_back({static_cast<std::uint32_t>(i),
        0,
        formats[i],
        static_cast<std::uint32_t>(offsets[i])});
  }
  return layout;
}

// The layout of a vertex type V: any InterleavedDatum (V::traits names
// its attributes).
template <typename V>
VertexLayout vertexLayout() {
  return vertexLayout(typename V::traits{});
}

// Flat-colored or textured geometry: UI, text, lines, debug shapes.
// Attributes: position, color, texcoord (get<0>(), get<1>(), get<2>()).
using UiVertex = VertexTypes::
    InterleavedDatum<Math::Vec3<float>, Math::Vec4<float>, Math::Vec2<float>>;
using UiVertices = VertexTypes::
    InterleavedData<Math::Vec3<float>, Math::Vec4<float>, Math::Vec2<float>>;

// Lit, textured surfaces: models, terrain, water, spheres.
// Attributes: position, normal, texcoord (get<0>(), get<1>(), get<2>()).
using MeshVertex = VertexTypes::
    InterleavedDatum<Math::Vec3<float>, Math::Vec3<float>, Math::Vec2<float>>;
using MeshVertices = VertexTypes::
    InterleavedData<Math::Vec3<float>, Math::Vec3<float>, Math::Vec2<float>>;

}  // namespace vulkan_graphix::Render

#endif  // VULKAN_GRAPHIX_RENDER_VERTEX_H
