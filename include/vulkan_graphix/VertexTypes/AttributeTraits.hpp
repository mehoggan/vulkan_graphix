#ifndef VULKAN_GRAPHIX_VERTEXTYPES_ATTRIBUTETRAITS_HPP
#define VULKAN_GRAPHIX_VERTEXTYPES_ATTRIBUTETRAITS_HPP

#include <array>
#include <cstddef>

#include <glm/glm.hpp>

namespace vulkan_graphix::VertexTypes {

// Primary template covers user-defined attribute PODs that expose a
// `static constexpr std::size_t dimension`. glm types don't, so they get
// explicit specializations below.
template <typename T>
struct AttributeDimension {
  static constexpr std::size_t value = T::dimension;
};

template <>
struct AttributeDimension<glm::vec2> {
  static constexpr std::size_t value = 2;
};

template <>
struct AttributeDimension<glm::vec3> {
  static constexpr std::size_t value = 3;
};

template <>
struct AttributeDimension<glm::vec4> {
  static constexpr std::size_t value = 4;
};

template <>
struct AttributeDimension<glm::ivec2> {
  static constexpr std::size_t value = 2;
};

template <>
struct AttributeDimension<glm::ivec3> {
  static constexpr std::size_t value = 3;
};

template <>
struct AttributeDimension<glm::ivec4> {
  static constexpr std::size_t value = 4;
};

template <>
struct AttributeDimension<glm::uvec2> {
  static constexpr std::size_t value = 2;
};

template <>
struct AttributeDimension<glm::uvec3> {
  static constexpr std::size_t value = 3;
};

template <>
struct AttributeDimension<glm::uvec4> {
  static constexpr std::size_t value = 4;
};

// Stride, per-attribute value count, and byte offsets for a set of
// interleaved attribute types, in declaration order.
template <typename... Ts>
struct AttributeTraits {
  static constexpr std::size_t attribute_count = sizeof...(Ts);
  static constexpr std::size_t stride = (sizeof(Ts) + ...);
  static constexpr std::size_t attribute_value_count =
      (AttributeDimension<Ts>::value + ...);

  static constexpr std::array<std::size_t, sizeof...(Ts)> byteOffsets() {
    std::array<std::size_t, sizeof...(Ts)> sizes{sizeof(Ts)...};
    std::array<std::size_t, sizeof...(Ts)> offsets{};
    std::size_t running_offset = 0;
    for (std::size_t index = 0; index < sizes.size(); ++index) {
      offsets[index] = running_offset;
      running_offset += sizes[index];
    }
    return offsets;
  }
};

}  // namespace vulkan_graphix::VertexTypes

#endif
