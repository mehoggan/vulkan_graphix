#ifndef VULKAN_GRAPHIX_INTERLEAVEDDATUM_HPP
#define VULKAN_GRAPHIX_INTERLEAVEDDATUM_HPP

#include <cstddef>
#include <tuple>
#include <utility>

namespace vulkan_graphix::VertexTypes {

// A single interleaved vertex record: one value per attribute type, in
// declaration order, packed contiguously as it would appear in a VBO.
template <typename... Ts>
struct InterleavedDatum {
public:
    static constexpr std::size_t attribute_count = sizeof...(Ts);

    InterleavedDatum() = default;

    explicit InterleavedDatum(Ts const&... values) : m_values(values...) {}

    template <std::size_t Index>
    auto& get() {
        return std::get<Index>(m_values);
    }

    template <std::size_t Index>
    auto const& get() const {
        return std::get<Index>(m_values);
    }

private:
    std::tuple<Ts...> m_values;

    friend bool operator==(InterleavedDatum const& lhs,
                           InterleavedDatum const& rhs) {
        return lhs.m_values == rhs.m_values;
    }

    friend bool operator!=(InterleavedDatum const& lhs,
                           InterleavedDatum const& rhs) {
        return !(lhs == rhs);
    }

    friend void swap(InterleavedDatum& lhs, InterleavedDatum& rhs) {
        std::swap(lhs.m_values, rhs.m_values);
    }
};

}  // namespace vulkan_graphix::VertexTypes

#endif
