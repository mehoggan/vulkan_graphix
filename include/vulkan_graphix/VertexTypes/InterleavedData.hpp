#ifndef VULKAN_GRAPHIX_VERTEXTYPES_INTERLEAVEDDATA_HPP
#define VULKAN_GRAPHIX_VERTEXTYPES_INTERLEAVEDDATA_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <span>
#include <utility>
#include <vector>

#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"
#include "vulkan_graphix/VertexTypes/InterleavedDatum.hpp"

namespace vulkan_graphix::VertexTypes {

// A vertex buffer's worth of interleaved attribute records, built up like a
// std::vector of InterleavedDatum and packed into vertex-buffer bytes with
// packInto()/pack().
template <typename... Ts>
struct InterleavedData {
public:
  using datum_type = InterleavedDatum<Ts...>;
  using collection_type = std::vector<datum_type>;
  using traits = AttributeTraits<Ts...>;
  using iterator = typename collection_type::iterator;
  using const_iterator = typename collection_type::const_iterator;

  InterleavedData() = default;

  explicit InterleavedData(collection_type data) :
      m_data(std::move(data)) {}

  InterleavedData(std::initializer_list<datum_type> data) :
      m_data(data) {}

  // count default (zero) records, to be assigned in place.
  explicit InterleavedData(std::size_t count) :
      m_data(count) {}

  const collection_type& getData() const { return m_data; }

  std::size_t getAttributeCount() const { return m_data.size(); }

  std::size_t getByteCount() const { return traits::stride * m_data.size(); }

  std::size_t size() const { return m_data.size(); }
  bool empty() const { return m_data.empty(); }
  void reserve(std::size_t count) { m_data.reserve(count); }
  void clear() { m_data.clear(); }
  void add(const datum_type& datum) { m_data.push_back(datum); }
  void append(const InterleavedData& other) {
    m_data.insert(m_data.end(), other.m_data.begin(), other.m_data.end());
  }
  void append(std::initializer_list<datum_type> data) {
    m_data.insert(m_data.end(), data);
  }

  datum_type& operator[](std::size_t index) { return m_data[index]; }
  const datum_type& operator[](std::size_t index) const {
    return m_data[index];
  }

  iterator begin() { return m_data.begin(); }
  iterator end() { return m_data.end(); }
  const_iterator begin() const { return m_data.begin(); }
  const_iterator end() const { return m_data.end(); }

  // Writes every record into destination (at least getByteCount() bytes),
  // each attribute at its AttributeTraits byte offset - the layout a
  // vertex buffer is read with, which InterleavedDatum's std::tuple
  // storage doesn't itself promise.
  void packInto(std::span<std::byte> destination) const {
    assert(destination.size() >= getByteCount());
    constexpr std::array<std::size_t, sizeof...(Ts)> offsets =
        traits::byteOffsets();
    std::byte* record = destination.data();
    for (const datum_type& datum : m_data) {
      [&]<std::size_t... I>(std::index_sequence<I...>) {
        (std::memcpy(
             record + offsets[I],
             &datum.template get<I>(),
             sizeof(datum.template get<I>())),
         ...);
      }(std::index_sequence_for<Ts...>{});
      record += traits::stride;
    }
  }

  std::vector<std::byte> pack() const {
    std::vector<std::byte> bytes(getByteCount());
    packInto(bytes);
    return bytes;
  }

private:
  collection_type m_data;
};

}  // namespace vulkan_graphix::VertexTypes

#endif
