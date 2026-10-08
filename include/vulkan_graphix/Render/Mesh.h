#ifndef VULKAN_GRAPHIX_RENDER_MESH_H
#define VULKAN_GRAPHIX_RENDER_MESH_H

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Vertex.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

namespace vulkan_graphix::Render {

class Texture;

// A host-visible, coherent buffer and the bytes it is persistently mapped
// to (empty when creating or mapping it failed).
struct HostBuffer {
  BufferParameters m_buffer;
  std::span<std::byte> m_mapped;
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
// control, a panel): triangles and lines of one VertexTypes vertex type,
// re-uploaded to a new host-visible buffer the first time it's drawn after
// a change. Drawn with one texture (none: the Renderer's 1x1 white one) and
// one params vector, see RenderContext::draw().
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
  bool dirty() const;
  void markDirty();
  // Replaces the buffer with a new one of byte_count bytes (none when 0)
  // and marks the mesh clean; returns its mapped bytes to fill (empty on
  // failure). A buffer a still-in-flight frame may be reading is never
  // rewritten in place: the old one is released (deferred).
  std::span<std::byte> reallocate(std::size_t byte_count);
  const HostBuffer& buffer() const;

private:
  void releaseBuffer();

  const Texture* m_texture = nullptr;
  Math::Vec4<float> m_params = Math::Vec4<float>(0.0f);
  float m_line_width = 1.0f;
  bool m_dirty = true;
  HostBuffer m_buffer;
};

// Ts... are the vertex's attribute types, as in VertexTypes::
// InterleavedData<Ts...>.
template <typename... Ts>
class RetainedMesh : public RetainedMeshBase {
public:
  using vertex_type = VertexTypes::InterleavedDatum<Ts...>;
  using vertices_type = VertexTypes::InterleavedData<Ts...>;

  void clear() {
    m_triangles.clear();
    m_lines.clear();
    markDirty();
  }
  void addTriangle(
      const vertex_type& v0, const vertex_type& v1, const vertex_type& v2) {
    m_triangles.append({v0, v1, v2});
    markDirty();
  }
  // Corners in drawing order, split 0-1-2, 0-2-3 (a GL_QUADS quad).
  void addQuad(const vertex_type& v0,
      const vertex_type& v1,
      const vertex_type& v2,
      const vertex_type& v3) {
    m_triangles.append({v0, v1, v2, v0, v2, v3});
    markDirty();
  }
  void addLine(const vertex_type& start, const vertex_type& end) {
    m_lines.append({start, end});
    markDirty();
  }

  const vertices_type& triangles() const { return m_triangles; }
  const vertices_type& lines() const { return m_lines; }

  // Re-uploads after any change; returns the buffer (triangles first,
  // then lines).
  const HostBuffer& upload() {
    if (dirty()) {
      const std::size_t triangle_bytes = m_triangles.getByteCount();
      const std::span<std::byte> bytes =
          reallocate(triangle_bytes + m_lines.getByteCount());
      if (!bytes.empty()) {
        m_triangles.packInto(bytes.first(triangle_bytes));
        m_lines.packInto(bytes.subspan(triangle_bytes));
      }
    }
    return buffer();
  }

private:
  vertices_type m_triangles;
  vertices_type m_lines;
};

// RetainedMesh of UiVertex, with shorthands for flat-colored and textured
// shapes; RenderContext::draw() draws it with the Renderer's UI pipelines.
class UiMesh : public RetainedMesh<Math::Vec3<float>,
                   Math::Vec4<float>,
                   Math::Vec2<float>> {
public:
  using RetainedMesh::addLine;
  using RetainedMesh::addQuad;
  using RetainedMesh::addTriangle;

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
