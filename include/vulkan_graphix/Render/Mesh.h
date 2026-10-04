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

    void setTexture(const Texture* texture);
    const Texture* texture() const;
    // The draw's params push constant (each pipeline's own meaning).
    void setParams(const Math::Vec4<float>& params);
    const Math::Vec4<float>& params() const;
    void setLineWidth(float width);
    float lineWidth() const;

protected:
    // Re-uploads after any change; returns the buffer (triangles first,
    // then lines).
    const HostBuffer& upload(const void* triangles,
                             std::size_t triangle_bytes,
                             const void* lines,
                             std::size_t line_bytes);
    void markDirty();

private:
    void releaseBuffer();

    const Texture* m_texture = nullptr;
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
    void addTriangle(const V& v0, const V& v1, const V& v2) {
        m_triangles.insert(m_triangles.end(), {v0, v1, v2});
        markDirty();
    }
    // Corners in drawing order, split 0-1-2, 0-2-3 (a GL_QUADS quad).
    void addQuad(const V& v0, const V& v1, const V& v2, const V& v3) {
        m_triangles.insert(m_triangles.end(), {v0, v1, v2, v0, v2, v3});
        markDirty();
    }
    void addLine(const V& start, const V& end) {
        m_lines.insert(m_lines.end(), {start, end});
        markDirty();
    }

    const std::vector<V>& triangles() const { return m_triangles; }
    const std::vector<V>& lines() const { return m_lines; }

    const HostBuffer& upload() {
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

    void addQuad(const std::array<Math::Vec3<float>, 4>& corners,
                 const Math::Vec4<float>& color);
    void addTexturedQuad(const std::array<Math::Vec3<float>, 4>& corners,
                         const std::array<Math::Vec2<float>, 4>& texcoords,
                         const Math::Vec4<float>& color);
    void addTriangle(const std::array<Math::Vec3<float>, 3>& corners,
                     const Math::Vec4<float>& color);
    // Per-vertex colors.
    void addTriangle(const std::array<Math::Vec3<float>, 3>& corners,
                     const std::array<Math::Vec4<float>, 3>& colors);
    void addLine(const Math::Vec3<float>& start,
                 const Math::Vec3<float>& end,
                 const Math::Vec4<float>& color);
};

}  // namespace vulkan_graphix::Render

#endif  // VULKAN_GRAPHIX_RENDER_MESH_H
