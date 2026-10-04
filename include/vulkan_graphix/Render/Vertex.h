#ifndef VULKAN_GRAPHIX_RENDER_VERTEX_H
#define VULKAN_GRAPHIX_RENDER_VERTEX_H

// Vertex formats for the Render module: the Vulkan vertex-input layout of
// any vertex described by VertexTypes::AttributeTraits, the two standard
// vertex structs most drawing needs, and packing VertexTypes::
// InterleavedData into vertex-buffer bytes.

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>
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
        layout.m_attributes.push_back(
                {static_cast<std::uint32_t>(i),
                 0,
                 formats[i],
                 static_cast<std::uint32_t>(offsets[i])});
    }
    return layout;
}

// The layout of a vertex struct V that names its attributes as
// `using Traits = VertexTypes::AttributeTraits<...>` - which must match
// its members exactly, checked here.
template <typename V>
VertexLayout vertexLayout() {
    static_assert(sizeof(V) == V::Traits::stride,
                  "vertex struct and its AttributeTraits disagree");
    return vertexLayout(typename V::Traits{});
}

// Flat-colored or textured geometry: UI, text, lines, debug shapes.
struct UiVertex {
    using Traits = VertexTypes::AttributeTraits<Math::Vec3<float>,
                                                Math::Vec4<float>,
                                                Math::Vec2<float>>;
    Math::Vec3<float> m_position;
    Math::Vec4<float> m_color;
    Math::Vec2<float> m_texcoord;
};

// Lit, textured surfaces: models, terrain, water, spheres.
struct MeshVertex {
    using Traits = VertexTypes::AttributeTraits<Math::Vec3<float>,
                                                Math::Vec3<float>,
                                                Math::Vec2<float>>;
    Math::Vec3<float> m_position;
    Math::Vec3<float> m_normal;
    Math::Vec2<float> m_texcoord;
};

// InterleavedData's records as vertex-buffer bytes: each attribute copied
// to its AttributeTraits byte offset (std::tuple, which InterleavedDatum
// stores them in, doesn't promise that layout itself).
template <typename... Ts>
std::vector<std::byte> packInterleaved(
        const VertexTypes::InterleavedData<Ts...>& data) {
    using Traits = VertexTypes::AttributeTraits<Ts...>;
    constexpr std::array<std::size_t, sizeof...(Ts)> offsets =
            Traits::byteOffsets();
    std::vector<std::byte> bytes(data.getByteCount());
    std::size_t record = 0;
    for (const auto& datum : data.getData()) {
        std::byte* const base = bytes.data() + record * Traits::stride;
        [&]<std::size_t... I>(std::index_sequence<I...>) {
            (std::memcpy(base + offsets[I],
                         &datum.template get<I>(),
                         sizeof(datum.template get<I>())),
             ...);
        }(std::index_sequence_for<Ts...>{});
        ++record;
    }
    return bytes;
}

}  // namespace vulkan_graphix::Render

#endif  // VULKAN_GRAPHIX_RENDER_VERTEX_H
