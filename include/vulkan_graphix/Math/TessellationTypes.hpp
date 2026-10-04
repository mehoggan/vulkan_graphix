#ifndef VULKAN_GRAPHIX_MATH_TESSELLATIONTYPES_HPP
#define VULKAN_GRAPHIX_MATH_TESSELLATIONTYPES_HPP

#include <ostream>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

enum class GeneratorMode { Fill, Wireframe };

// The point cloud and index buffer produced by a tessellation operation.
template <typename T, typename I>
class TessellatedTriangleData {
public:
    explicit TessellatedTriangleData(
            GeneratorMode mode = GeneratorMode::Fill) :
            m_mode(mode) {}

    std::vector<Vec3<T>>& points() { return m_points; }
    std::vector<I>& indices() { return m_indices; }

    const std::vector<Vec3<T>>& points() const { return m_points; }
    const std::vector<I>& indices() const { return m_indices; }

    GeneratorMode mode() const { return m_mode; }

private:
    std::vector<Vec3<T>> m_points;
    std::vector<I> m_indices;
    GeneratorMode m_mode;
};

template <typename T, typename I>
inline std::ostream& operator<<(std::ostream& out,
                                const TessellatedTriangleData<T, I>& data) {
    out << "points = ";
    for (const auto& point : data.points()) {
        out << "(" << point.x << ", " << point.y << ", " << point.z << ") ";
    }
    out << "\nindices = ";
    for (const auto& index : data.indices()) {
        out << index << " ";
    }
    return out;
}

}  // namespace vulkan_graphix::Math

#endif
