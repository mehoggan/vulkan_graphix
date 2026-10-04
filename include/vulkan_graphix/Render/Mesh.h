#ifndef VULKAN_GRAPHIX_RENDER_MESH_H
#define VULKAN_GRAPHIX_RENDER_MESH_H

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Vertex.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

namespace vulkan_graphix::Render {

class Texture;

// A host-visible, coherent buffer and where it is persistently mapped.
struct HostBuffer {
    BufferParameters buffer;
    void* mapped = nullptr;
};

// Geometry uploaded once into device-local memory (models, terrain,
// spheres). Created through Renderer::createMesh(); destroying it hands its
// buffer to the Renderer to free once no in-flight frame can still use it.
class Mesh {
public:
    Mesh(BufferParameters buffer, std::uint32_t vertex_count);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    VkBuffer buffer() const;
    std::uint32_t vertexCount() const;

private:
    BufferParameters m_buffer;
    std::uint32_t m_vertex_count;
};

// Retained geometry an object owns and rebuilds only when it changes (a UI
// control, a panel): triangles and lines of vertex type V, re-uploaded to a
// new host-visible buffer the first time it's drawn after a change. Drawn
// with one texture (none: the Renderer's 1x1 white one) and one params
// vector, see RenderContext::draw().
class RetainedMeshBase {
public:
    RetainedMeshBase() = default;
    ~RetainedMeshBase();

    RetainedMeshBase(const RetainedMeshBase&) = delete;
    RetainedMeshBase& operator=(const RetainedMeshBase&) = delete;

    void setTexture(Texture const* texture);
    Texture const* texture() const;
    // The draw's params push constant (each pipeline's own meaning).
    void setParams(Math::Vec4<float> const& params);
    Math::Vec4<float> const& params() const;
    void setLineWidth(float width);
    float lineWidth() const;

protected:
    // Re-uploads after any change; returns the buffer (triangles first,
    // then lines).
    HostBuffer const& upload(void const* triangles,
                             std::size_t triangle_bytes,
                             void const* lines,
                             std::size_t line_bytes);
    void markDirty();

private:
    void releaseBuffer();

    Texture const* m_texture = nullptr;
    Math::Vec4<float> m_params = Math::Vec4<float>(0.0f);
    float m_line_width = 1.0f;
    bool m_dirty = true;
    HostBuffer m_buffer;
};

template <typename V>
class RetainedMesh : public RetainedMeshBase {
public:
    using vertex_type = V;

    void clear() {
        m_triangles.clear();
        m_lines.clear();
        markDirty();
    }
    void addTriangle(V const& v0, V const& v1, V const& v2) {
        m_triangles.insert(m_triangles.end(), {v0, v1, v2});
        markDirty();
    }
    // Corners in drawing order, split 0-1-2, 0-2-3 (a GL_QUADS quad).
    void addQuad(V const& v0, V const& v1, V const& v2, V const& v3) {
        m_triangles.insert(m_triangles.end(), {v0, v1, v2, v0, v2, v3});
        markDirty();
    }
    void addLine(V const& start, V const& end) {
        m_lines.insert(m_lines.end(), {start, end});
        markDirty();
    }

    std::vector<V> const& triangles() const { return m_triangles; }
    std::vector<V> const& lines() const { return m_lines; }

    HostBuffer const& upload() {
        return RetainedMeshBase::upload(m_triangles.data(),
                                        m_triangles.size() * sizeof(V),
                                        m_lines.data(),
                                        m_lines.size() * sizeof(V));
    }

private:
    std::vector<V> m_triangles;
    std::vector<V> m_lines;
};

// RetainedMesh of UiVertex, with shorthands for flat-colored and textured
// shapes; RenderContext::draw() draws it with the Renderer's UI pipelines.
class UiMesh : public RetainedMesh<UiVertex> {
public:
    using RetainedMesh<UiVertex>::addQuad;
    using RetainedMesh<UiVertex>::addTriangle;
    using RetainedMesh<UiVertex>::addLine;

    void addQuad(std::array<Math::Vec3<float>, 4> const& corners,
                 Math::Vec4<float> const& color);
    void addTexturedQuad(std::array<Math::Vec3<float>, 4> const& corners,
                         std::array<Math::Vec2<float>, 4> const& texcoords,
                         Math::Vec4<float> const& color);
    void addTriangle(std::array<Math::Vec3<float>, 3> const& corners,
                     Math::Vec4<float> const& color);
    // Per-vertex colors.
    void addTriangle(std::array<Math::Vec3<float>, 3> const& corners,
                     std::array<Math::Vec4<float>, 3> const& colors);
    void addLine(Math::Vec3<float> const& start,
                 Math::Vec3<float> const& end,
                 Math::Vec4<float> const& color);
};

}  // namespace vulkan_graphix::Render

#endif  // VULKAN_GRAPHIX_RENDER_MESH_H
