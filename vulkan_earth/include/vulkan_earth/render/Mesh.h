#ifndef VULKAN_EARTH_RENDER_MESH_H
#define VULKAN_EARTH_RENDER_MESH_H

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
#include <vector>

#include "vulkan_earth/render/RenderTypes.h"

namespace vulkan_earth::render {

class Texture;

// A VkBuffer and its memory; mapped is non-null for host-visible buffers.
struct GpuBuffer {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;
    void* mapped = nullptr;
};

// Geometry uploaded once into device-local memory: terrain, water, the
// skybox, .ogl models, spheres.
class StaticMesh {
public:
    StaticMesh(GpuBuffer buffer, std::uint32_t vertex_count);
    ~StaticMesh();

    StaticMesh(const StaticMesh&) = delete;
    StaticMesh& operator=(const StaticMesh&) = delete;

    VkBuffer buffer() const;
    std::uint32_t vertexCount() const;

private:
    GpuBuffer m_buffer;
    std::uint32_t m_vertex_count;
};

// Retained UI geometry a control owns: it fills the vertex lists, and the
// GPU copy is rebuilt only when they change (see markDirty()) - the
// replacement for redrawing every quad through glBegin()/glEnd() each
// frame. Triangles and lines share one texture (a 1x1 white texture when
// none is set, so untextured geometry is just its vertex color).
class UiMesh {
public:
    UiMesh();
    ~UiMesh();

    UiMesh(const UiMesh&) = delete;
    UiMesh& operator=(const UiMesh&) = delete;

    void clear();
    // GL_QUADS winding: corners in drawing order (split 0-1-2, 0-2-3).
    void addQuad(std::array<Vec3, 4> const& corners, Vec4 const& color);
    void addTexturedQuad(std::array<Vec3, 4> const& corners,
                         std::array<Vec2, 4> const& texcoords,
                         Vec4 const& color);
    void addTriangle(std::array<Vec3, 3> const& corners, Vec4 const& color);
    // Per-vertex colors (glColor changed between glVertex calls).
    void addTriangle(std::array<Vec3, 3> const& corners,
                     std::array<Vec4, 3> const& colors);
    void addLine(Vec3 const& from, Vec3 const& to, Vec4 const& color);

    void setTexture(Texture const* texture);
    Texture const* texture() const;
    // GL_REPLACE instead of GL_MODULATE: texels drawn without the vertex
    // color.
    void setReplaceTexEnv(bool replace);
    bool replaceTexEnv() const;
    void setLineWidth(float width);
    float lineWidth() const;

    std::vector<UiVertex> const& triangles() const;
    std::vector<UiVertex> const& lines() const;

    // Called by RenderContext::draw(): re-uploads after any change.
    GpuBuffer const& upload();

private:
    void releaseBuffer();

    std::vector<UiVertex> m_triangles;
    std::vector<UiVertex> m_lines;
    Texture const* m_texture = nullptr;
    bool m_replace = false;
    float m_line_width = 1.0f;
    bool m_dirty = true;
    GpuBuffer m_buffer;
};

}  // namespace vulkan_earth::render

#endif  // VULKAN_EARTH_RENDER_MESH_H
