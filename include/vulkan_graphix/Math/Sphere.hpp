#ifndef VULKAN_GRAPHIX_MATH_SPHERE_HPP
#define VULKAN_GRAPHIX_MATH_SPHERE_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <glm/geometric.hpp>

// Vec3<T> keys in DuplicateCheck_t below need std::hash, which glm only
// provides behind this flag (see glm/gtx/hash.hpp).
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/hash.hpp>

#include "vulkan_graphix/Math/Geometry.hpp"
#include "vulkan_graphix/Math/Icosahedron.hpp"
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A triangulated sphere mesh, built either by walking a latitude/longitude
// grid (a "UV sphere") or by recursively subdividing an Icosahedron (an
// "icosphere").
template <typename T = float, typename I = std::uint32_t>
class Sphere {
public:
    static constexpr T c_min_theta_degrees = T(0);
    static constexpr T c_max_theta_degrees = T(360);
    static constexpr T c_min_elevation_degrees = T(-90);
    static constexpr T c_max_elevation_degrees = T(90);

    // Builds a UV sphere: theta_steps divisions around the equator,
    // phi_steps divisions from pole to pole.
    Sphere(T radius, std::uint16_t theta_steps, std::uint16_t phi_steps) :
            m_radius(radius) {
        generateVertices(theta_steps, phi_steps);
        generateIndices(theta_steps, phi_steps);
    }

    // Builds an icosphere: an Icosahedron<T, I> recursively subdivided
    // level_of_detail times, each subdivision replacing every triangle with
    // three triangles fanned around its (radius-projected) centroid.
    Sphere(T radius, std::uint8_t level_of_detail) :
            m_radius(radius) {
        const Icosahedron<T, I> icosahedron(m_radius);

        IndexedVertices_t data = subdivide(icosahedron.points(),
                                           icosahedron.indices(),
                                           0,
                                           level_of_detail);

        m_points = std::move(data.first);
        m_indices = std::move(data.second);
        generateNormals();
    }

    const std::vector<Vec3<T>>& points() const { return m_points; }
    const std::vector<Vec3<T>>& normals() const { return m_normals; }
    const std::vector<I>& indices() const { return m_indices; }

    T radius() const { return m_radius; }

private:
    T m_radius;
    std::vector<Vec3<T>> m_points;
    std::vector<Vec3<T>> m_normals;
    std::vector<I> m_indices;

    Vec3<T> pointAt(T theta_degrees, T elevation_degrees) const {
        SphericalCoordinates<T, AngleMode::Degrees> coords(
                theta_degrees, elevation_degrees, m_radius);
        return sphericalToCartesian(coords);
    }

    void generateNormals() {
        m_normals.clear();
        m_normals.reserve(m_points.size());
        for (const Vec3<T>& point : m_points) {
            m_normals.push_back(glm::normalize(point));
        }
    }

    // --- UV sphere ---

    void generateVertices(std::uint16_t theta_steps, std::uint16_t phi_steps) {
        const T phi_step =
                (c_max_elevation_degrees - c_min_elevation_degrees) /
                static_cast<T>(phi_steps);
        T elevation_degrees = c_max_elevation_degrees;
        while (elevation_degrees >= c_min_elevation_degrees) {
            if (elevation_degrees - phi_step < c_min_elevation_degrees) {
                iterateThetaAtElevation(c_min_elevation_degrees, theta_steps);
                break;
            }
            iterateThetaAtElevation(elevation_degrees, theta_steps);
            elevation_degrees -= phi_step;
        }

        generateNormals();
    }

    void iterateThetaAtElevation(T elevation_degrees,
                                 std::uint16_t theta_steps) {
        if (elevation_degrees == c_max_elevation_degrees ||
            elevation_degrees == c_min_elevation_degrees) {
            m_points.push_back(
                    pointAt(c_min_theta_degrees, elevation_degrees));
            return;
        }

        const T theta_step = (c_max_theta_degrees - c_min_theta_degrees) /
                             static_cast<T>(theta_steps);
        T theta_degrees = c_min_theta_degrees;
        while (theta_degrees < c_max_theta_degrees) {
            m_points.push_back(pointAt(theta_degrees, elevation_degrees));
            theta_degrees += theta_step;
        }
    }

    void generateIndices(std::uint16_t theta_steps, std::uint16_t phi_steps) {
        std::uint16_t phi_step = 0;
        while (phi_step < phi_steps) {
            const std::size_t bottom_row_vertex_count = theta_steps + 1;
            const std::size_t top_row_vertex_count = bottom_row_vertex_count;
            const std::size_t vertex_per_row_count = theta_steps;
            const bool at_bottom = phi_step == 0;
            const bool at_top = phi_step == phi_steps - 1;

            if (at_bottom) {
                for (I index = 1; index < bottom_row_vertex_count; ++index) {
                    m_indices.push_back(0);
                    if (index != static_cast<I>(bottom_row_vertex_count - 1)) {
                        m_indices.push_back(index + 1);
                        m_indices.push_back(index);
                    } else {
                        m_indices.push_back(1);
                        m_indices.push_back(index);
                    }
                }
            } else if (at_top) {
                const I curr_row = phi_step - 1;
                const I start_index_for_row = curr_row * theta_steps + 1;
                const I last_start_index =
                        start_index_for_row +
                        static_cast<I>(top_row_vertex_count) - 2;
                I distance = static_cast<I>(vertex_per_row_count);
                for (I start_index = start_index_for_row;
                     start_index <= last_start_index;
                     ++start_index) {
                    m_indices.push_back(start_index);
                    if (start_index != last_start_index) {
                        m_indices.push_back(start_index + 1);
                    } else {
                        m_indices.push_back(start_index_for_row);
                    }
                    m_indices.push_back(start_index + distance);
                    --distance;
                }
            } else {
                const I curr_row = phi_step - 1;
                const I start_index_for_row = curr_row * theta_steps + 1;
                const I last_start_index =
                        start_index_for_row +
                        static_cast<I>(vertex_per_row_count) - 1;
                for (I start_index = start_index_for_row;
                     start_index <= last_start_index;
                     ++start_index) {
                    const I next_row_index =
                            start_index + static_cast<I>(vertex_per_row_count);
                    if (start_index != last_start_index) {
                        m_indices.push_back(next_row_index + 1);
                        m_indices.push_back(next_row_index);
                        m_indices.push_back(start_index);

                        m_indices.push_back(next_row_index + 1);
                        m_indices.push_back(start_index);
                        m_indices.push_back(start_index + 1);
                    } else {
                        const I initial_index = start_index_for_row;
                        m_indices.push_back(
                                initial_index +
                                static_cast<I>(vertex_per_row_count));
                        m_indices.push_back(next_row_index);
                        m_indices.push_back(start_index);

                        m_indices.push_back(
                                initial_index +
                                static_cast<I>(vertex_per_row_count));
                        m_indices.push_back(start_index);
                        m_indices.push_back(initial_index);
                    }
                }
            }

            ++phi_step;
        }
    }

    // --- Icosphere ---

    using IndexedVertices_t = std::pair<std::vector<Vec3<T>>, std::vector<I>>;
    using DuplicateCheck_t = std::unordered_map<Vec3<T>, I>;

    IndexedVertices_t subdivide(const std::vector<Vec3<T>>& vertices,
                                const std::vector<I>& indices,
                                std::uint32_t curr_level,
                                std::uint32_t desired_level) {
        if (curr_level == desired_level) {
            return std::make_pair(vertices, indices);
        }

        std::vector<Vec3<T>> next_vertices;
        std::vector<I> next_indices;
        next_vertices.reserve(vertices.size() * 2);
        next_indices.reserve(indices.size() * 3);
        I curr_index = 0;

        // Deduplicates vertices within this subdivision level only: it maps
        // a position to its index in next_vertices, which is meaningless
        // once next_vertices becomes the input to the next level, so it
        // must not be reused across recursive calls.
        DuplicateCheck_t duplicate_check;

        auto emit = [&](const Vec3<T>& vertex) -> I {
            auto find_it = duplicate_check.find(vertex);
            if (find_it != duplicate_check.end()) {
                return find_it->second;
            }
            next_vertices.push_back(vertex);
            const I index = curr_index++;
            duplicate_check.emplace(vertex, index);
            return index;
        };

        for (std::size_t index = 0; index < indices.size(); index += 3) {
            const I index_0 = indices[index + 0];
            const I index_1 = indices[index + 1];
            const I index_2 = indices[index + 2];

            const Vec3<T>& point_0 = vertices[index_0];
            const Vec3<T>& point_1 = vertices[index_1];
            const Vec3<T>& point_2 = vertices[index_2];

            // Edge midpoints, projected back onto the sphere. Unlike a
            // per-triangle centroid, these lie exactly on the shared edge
            // between two adjacent triangles, so emit() dedupes them to the
            // same vertex from both sides - the original 12 icosahedron
            // vertices/30 edges get refined along with everything else,
            // instead of staying fixed as sharp corners forever.
            const Vec3<T> mid_01 =
                    glm::normalize((point_0 + point_1) * T(0.5)) * m_radius;
            const Vec3<T> mid_12 =
                    glm::normalize((point_1 + point_2) * T(0.5)) * m_radius;
            const Vec3<T> mid_20 =
                    glm::normalize((point_2 + point_0) * T(0.5)) * m_radius;

            const I new_0 = emit(point_0);
            const I new_1 = emit(point_1);
            const I new_2 = emit(point_2);
            const I new_01 = emit(mid_01);
            const I new_12 = emit(mid_12);
            const I new_20 = emit(mid_20);

            next_indices.push_back(new_0);
            next_indices.push_back(new_01);
            next_indices.push_back(new_20);

            next_indices.push_back(new_1);
            next_indices.push_back(new_12);
            next_indices.push_back(new_01);

            next_indices.push_back(new_2);
            next_indices.push_back(new_20);
            next_indices.push_back(new_12);

            next_indices.push_back(new_01);
            next_indices.push_back(new_12);
            next_indices.push_back(new_20);
        }

        return subdivide(
                next_vertices, next_indices, curr_level + 1, desired_level);
    }
};

}  // namespace vulkan_graphix::Math

#endif
