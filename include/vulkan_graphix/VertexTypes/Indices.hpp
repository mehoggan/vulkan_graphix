#ifndef VULKAN_GRAPHIX_VERTEXTYPES_INDICES_HPP
#define VULKAN_GRAPHIX_VERTEXTYPES_INDICES_HPP

#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

namespace vulkan_graphix::VertexTypes {

template <typename T>
struct Indices {
public:
  static_assert(std::is_integral_v<T>, "Integer type required");

  using index_type = T;
  using collection_type = std::vector<index_type>;

  Indices() = default;

  explicit Indices(collection_type data) :
      m_data(std::move(data)) {}

  const collection_type& getData() const { return m_data; }

  std::size_t getByteCount() const { return sizeof(T) * m_data.size(); }

  std::size_t getIndicesCount() const { return m_data.size(); }

private:
  collection_type m_data;

  friend bool operator==(const Indices& lhs, const Indices& rhs) {
    return lhs.m_data == rhs.m_data;
  }

  friend bool operator!=(const Indices& lhs, const Indices& rhs) {
    return !(lhs == rhs);
  }
};

}  // namespace vulkan_graphix::VertexTypes

#endif
