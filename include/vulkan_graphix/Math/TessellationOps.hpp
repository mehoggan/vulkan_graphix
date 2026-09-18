#ifndef VULKAN_GRAPHIX_TESSELLATIONOPS_HPP
#define VULKAN_GRAPHIX_TESSELLATIONOPS_HPP

#include <cstddef>
#include <unordered_map>
#include <vector>

// Vec3<T> keys below need std::hash, which glm only provides behind this
// flag (see glm/gtx/hash.hpp). Must match Math/Sphere.hpp's use of the
// exact same GLM_ENABLE_EXPERIMENTAL + glm/gtx/hash.hpp combination - a
// second, hand-rolled std::hash<Vec3<T>> specialization here previously
// competed with GLM's own for the same type. Since template
// specializations get weak/vague linkage, whichever definition a given
// translation unit saw was inlined into that TU's compiled code, but the
// linker then picks just one definition to keep across the whole binary -
// so a TU built against one specialization could still be calling into
// std::unordered_map<Vec3<T>, ...> machinery compiled against the other,
// which changes the hashtable node's internal layout (a cached vs.
// uncached hash code) and corrupts the heap on new/delete. Caught via
// AddressSanitizer's new-delete-type-mismatch report during Tutorial09's
// terrain tessellation.
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/hash.hpp>

#include "vulkan_graphix/Math/Geometry.hpp"
#include "vulkan_graphix/Math/Line.hpp"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Math/TessellationTypes.hpp"
#include "vulkan_graphix/Math/Triangle.hpp"

namespace vulkan_graphix::Math {

namespace detail {

template <typename T, typename I>
void updateTessellatedTriangleData(
        Vec3<T> const& point,
        I& current_index,
        std::unordered_map<Vec3<T>, I>& point_to_index_map,
        TessellatedTriangleData<T, I>& out) {
    auto point_it = point_to_index_map.find(point);
    if (point_it == point_to_index_map.end()) {
        point_to_index_map[point] = current_index;
        out.points().push_back(point);
        out.indices().push_back(current_index);
        ++current_index;
    } else {
        out.indices().push_back(point_it->second);
    }
}

template <typename T, typename I>
void updateTessellatedWireframeTriangleData(
        Line<T> const& line,
        I& current_index,
        std::unordered_map<Vec3<T>, I>& point_to_index_map,
        TessellatedTriangleData<T, I>& out) {
    updateTessellatedTriangleData(
            line.p0(), current_index, point_to_index_map, out);
    updateTessellatedTriangleData(
            line.p1(), current_index, point_to_index_map, out);
}

template <typename T, typename I>
void handleBaseCase(Triangle<T> const& tri,
                    I& current_index,
                    std::unordered_map<Vec3<T>, I>& point_to_index_map,
                    TessellatedTriangleData<T, I>& out) {
    if (out.mode() == GeneratorMode::Fill) {
        updateTessellatedTriangleData(
                tri.p0(), current_index, point_to_index_map, out);
        updateTessellatedTriangleData(
                tri.p1(), current_index, point_to_index_map, out);
        updateTessellatedTriangleData(
                tri.p2(), current_index, point_to_index_map, out);
    } else {
        Line<T> side0(tri.p0(), tri.p1());
        Line<T> side1(tri.p1(), tri.p2());
        Line<T> side2(tri.p2(), tri.p0());
        updateTessellatedWireframeTriangleData(
                side0, current_index, point_to_index_map, out);
        updateTessellatedWireframeTriangleData(
                side1, current_index, point_to_index_map, out);
        updateTessellatedWireframeTriangleData(
                side2, current_index, point_to_index_map, out);
    }
}

template <typename T, typename I>
void tessellateTriangleBySubdivision(
        Triangle<T> const& tri,
        std::size_t subdivision_count,
        I& current_index,
        std::unordered_map<Vec3<T>, I>& point_to_index_map,
        TessellatedTriangleData<T, I>& out) {
    if (pointsOfTriangleAreCollinear(tri)) {
        return;
    }
    if (subdivision_count == 0) {
        handleBaseCase(tri, current_index, point_to_index_map, out);
        return;
    }

    Vec3<T> centroid = centroidOfTriangle(tri);
    Triangle<T> tri0(tri.p0(), centroid, tri.p2());
    Triangle<T> tri1(tri.p0(), tri.p1(), centroid);
    Triangle<T> tri2(tri.p1(), tri.p2(), centroid);

    tessellateTriangleBySubdivision(tri0,
                                    subdivision_count - 1,
                                    current_index,
                                    point_to_index_map,
                                    out);
    tessellateTriangleBySubdivision(tri1,
                                    subdivision_count - 1,
                                    current_index,
                                    point_to_index_map,
                                    out);
    tessellateTriangleBySubdivision(tri2,
                                    subdivision_count - 1,
                                    current_index,
                                    point_to_index_map,
                                    out);
}

template <typename T, typename I>
void tessellateTriangleByMidpointSubdivision(
        Triangle<T> const& tri,
        std::size_t subdivision_count,
        I& current_index,
        std::unordered_map<Vec3<T>, I>& point_to_index_map,
        TessellatedTriangleData<T, I>& out) {
    if (pointsOfTriangleAreCollinear(tri)) {
        return;
    }
    if (subdivision_count == 0) {
        handleBaseCase(tri, current_index, point_to_index_map, out);
        return;
    }

    Line<T> side0(tri.p0(), tri.p1());
    Line<T> side1(tri.p1(), tri.p2());
    Line<T> side2(tri.p2(), tri.p0());

    Vec3<T> midpoint0 = midpointOfLine(side0);
    Vec3<T> midpoint1 = midpointOfLine(side1);
    Vec3<T> midpoint2 = midpointOfLine(side2);

    Triangle<T> tri0(tri.p0(), midpoint0, midpoint2);
    Triangle<T> tri1(midpoint0, tri.p1(), midpoint1);
    Triangle<T> tri2(midpoint0, midpoint1, midpoint2);
    Triangle<T> tri3(midpoint2, midpoint1, tri.p2());

    tessellateTriangleByMidpointSubdivision(tri0,
                                            subdivision_count - 1,
                                            current_index,
                                            point_to_index_map,
                                            out);
    tessellateTriangleByMidpointSubdivision(tri1,
                                            subdivision_count - 1,
                                            current_index,
                                            point_to_index_map,
                                            out);
    tessellateTriangleByMidpointSubdivision(tri2,
                                            subdivision_count - 1,
                                            current_index,
                                            point_to_index_map,
                                            out);
    tessellateTriangleByMidpointSubdivision(tri3,
                                            subdivision_count - 1,
                                            current_index,
                                            point_to_index_map,
                                            out);
}

}  // namespace detail

template <typename T, typename I>
void tessellateTriangleBySubdivision(Triangle<T> const& tri,
                                     std::size_t subdivision_count,
                                     I& current_index,
                                     TessellatedTriangleData<T, I>& out) {
    std::unordered_map<Vec3<T>, I> point_to_index_map;
    detail::tessellateTriangleBySubdivision(
            tri, subdivision_count, current_index, point_to_index_map, out);
}

template <typename T, typename I>
void tessellateTrianglesBySubdivision(std::vector<Triangle<T>> const& tris,
                                      std::size_t subdivision_count,
                                      I& current_index,
                                      TessellatedTriangleData<T, I>& out) {
    std::unordered_map<Vec3<T>, I> point_to_index_map;
    for (auto const& input_tri : tris) {
        detail::tessellateTriangleBySubdivision(input_tri,
                                                subdivision_count,
                                                current_index,
                                                point_to_index_map,
                                                out);
    }
}

template <typename T, typename I>
void tessellateTriangleByMidpointSubdivision(
        Triangle<T> const& tri,
        std::size_t subdivision_count,
        I& current_index,
        TessellatedTriangleData<T, I>& out) {
    std::unordered_map<Vec3<T>, I> point_to_index_map;
    detail::tessellateTriangleByMidpointSubdivision(
            tri, subdivision_count, current_index, point_to_index_map, out);
}

template <typename T, typename I>
void tessellateTrianglesByMidpointSubdivision(
        std::vector<Triangle<T>> const& tris,
        std::size_t subdivision_count,
        I& current_index,
        TessellatedTriangleData<T, I>& out) {
    std::unordered_map<Vec3<T>, I> point_to_index_map;
    for (auto const& input_tri : tris) {
        detail::tessellateTriangleByMidpointSubdivision(input_tri,
                                                        subdivision_count,
                                                        current_index,
                                                        point_to_index_map,
                                                        out);
    }
}

}  // namespace vulkan_graphix::Math

#endif
