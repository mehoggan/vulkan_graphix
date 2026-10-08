#ifndef VULKAN_GRAPHIX_VERTEXTYPES_INTERLEAVEDDATUM_HPP
#define VULKAN_GRAPHIX_VERTEXTYPES_INTERLEAVEDDATUM_HPP

#include <cstddef>
#include <tuple>
#include <utility>

#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"

namespace vulkan_graphix::VertexTypes {

// A single interleaved vertex record: one value per attribute type, in
// declaration order. Its in-memory layout is std::tuple's, not a vertex
// buffer's - InterleavedData::packInto() lays records out for the GPU.
template <typename... Ts>
struct InterleavedDatum {
public:
  static constexpr std::size_t attribute_count = sizeof...(Ts);
  using traits = AttributeTraits<Ts...>;

  InterleavedDatum() = default;

  // Not explicit, so a braced list of attributes is a vertex:
  // vertices.push_back({position, color, texcoord}).
  InterleavedDatum(const Ts&... values) :
      m_values(values...) {}

  template <std::size_t Index>
  auto& get() {
    return std::get<Index>(m_values);
  }

  template <std::size_t Index>
  const auto& get() const {
    return std::get<Index>(m_values);
  }

private:
  std::tuple<Ts...> m_values;

  friend bool operator==(
      const InterleavedDatum& lhs, const InterleavedDatum& rhs) {
    return lhs.m_values == rhs.m_values;
  }

  friend bool operator!=(
      const InterleavedDatum& lhs, const InterleavedDatum& rhs) {
    return !(lhs == rhs);
  }

  friend void swap(InterleavedDatum& lhs, InterleavedDatum& rhs) {
    std::swap(lhs.m_values, rhs.m_values);
  }
};

}  // namespace vulkan_graphix::VertexTypes

#endif
