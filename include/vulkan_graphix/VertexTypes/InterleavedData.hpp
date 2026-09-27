#ifndef VULKAN_GRAPHIX_INTERLEAVEDDATA_HPP
#define VULKAN_GRAPHIX_INTERLEAVEDDATA_HPP

#include <cstddef>
#include <utility>
#include <vector>

#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"
#include "vulkan_graphix/VertexTypes/InterleavedDatum.hpp"

namespace vulkan_graphix::VertexTypes {

// A vertex buffer's worth of interleaved attribute records.
template <typename... Ts>
struct InterleavedData {
public:
    using datum_type = InterleavedDatum<Ts...>;
    using collection_type = std::vector<datum_type>;
    using traits = AttributeTraits<Ts...>;

    InterleavedData() = default;

    explicit InterleavedData(collection_type data) : m_data(std::move(data)) {}

    collection_type const& getData() const { return m_data; }

    std::size_t getAttributeCount() const { return m_data.size(); }

    std::size_t getByteCount() const { return traits::stride * m_data.size(); }

private:
    collection_type m_data;
};

}  // namespace vulkan_graphix::VertexTypes

#endif
