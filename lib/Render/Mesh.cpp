#include "vulkan_graphix/Render/Mesh.h"

#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_graphix::Render {

namespace {
using Vec2 = Math::Vec2<float>;
using Vec3 = Math::Vec3<float>;
using Vec4 = Math::Vec4<float>;
}  // namespace

Mesh::Mesh(BufferParameters buffer, std::uint32_t vertex_count) :
    m_buffer(buffer),
    m_vertex_count(vertex_count) {}

Mesh::~Mesh() {
  if (Renderer::hasInstance()) {
    Renderer::instance().deferRelease(m_buffer);
  }
}

VkBuffer Mesh::buffer() const { return m_buffer.getVkBuffer(); }

std::uint32_t Mesh::vertexCount() const { return m_vertex_count; }

RetainedMeshBase::~RetainedMeshBase() { releaseBuffer(); }

void RetainedMeshBase::setTexture(const Texture* texture) {
  m_texture = texture;
}

const Texture* RetainedMeshBase::texture() const { return m_texture; }

void RetainedMeshBase::setParams(const Vec4& params) { m_params = params; }

const Vec4& RetainedMeshBase::params() const { return m_params; }

void RetainedMeshBase::setLineWidth(float width) { m_line_width = width; }

float RetainedMeshBase::lineWidth() const { return m_line_width; }

void RetainedMeshBase::markDirty() { m_dirty = true; }

bool RetainedMeshBase::dirty() const { return m_dirty; }

std::span<std::byte> RetainedMeshBase::reallocate(std::size_t byte_count) {
  releaseBuffer();
  if (byte_count != 0) {
    m_buffer = Renderer::instance().createHostBuffer(
        byte_count, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  }
  m_dirty = false;
  return m_buffer.m_mapped;
}

const HostBuffer& RetainedMeshBase::buffer() const { return m_buffer; }

void RetainedMeshBase::releaseBuffer() {
  if (m_buffer.m_buffer.getVkBuffer() != VK_NULL_HANDLE &&
      Renderer::hasInstance()) {
    Renderer::instance().deferRelease(m_buffer.m_buffer);
  }
  m_buffer = HostBuffer{};
}

void UiMesh::addQuad(const std::array<Vec3, 4>& corners, const Vec4& color) {
  addQuad(
      {corners[0], color, Vec2(0.0f)},
      {corners[1], color, Vec2(0.0f)},
      {corners[2], color, Vec2(0.0f)},
      {corners[3], color, Vec2(0.0f)});
}

void UiMesh::addTexturedQuad(
    const std::array<Vec3, 4>& corners,
    const std::array<Vec2, 4>& texcoords,
    const Vec4& color) {
  addQuad(
      {corners[0], color, texcoords[0]},
      {corners[1], color, texcoords[1]},
      {corners[2], color, texcoords[2]},
      {corners[3], color, texcoords[3]});
}

void UiMesh::addTriangle(
    const std::array<Vec3, 3>& corners, const Vec4& color) {
  addTriangle(
      {corners[0], color, Vec2(0.0f)},
      {corners[1], color, Vec2(0.0f)},
      {corners[2], color, Vec2(0.0f)});
}

void UiMesh::addTriangle(
    const std::array<Vec3, 3>& corners, const std::array<Vec4, 3>& colors) {
  addTriangle(
      {corners[0], colors[0], Vec2(0.0f)},
      {corners[1], colors[1], Vec2(0.0f)},
      {corners[2], colors[2], Vec2(0.0f)});
}

void UiMesh::addLine(const Vec3& start, const Vec3& end, const Vec4& color) {
  addLine({start, color, Vec2(0.0f)}, {end, color, Vec2(0.0f)});
}

}  // namespace vulkan_graphix::Render
