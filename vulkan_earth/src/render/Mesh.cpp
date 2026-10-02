#include "vulkan_earth/render/Mesh.h"

#include <cstring>

#include "vulkan_earth/render/Renderer.h"

namespace vulkan_earth::render {

StaticMesh::StaticMesh(GpuBuffer buffer, std::uint32_t vertex_count)
        : m_buffer(buffer), m_vertex_count(vertex_count) {}

StaticMesh::~StaticMesh() {
    if (Renderer::hasInstance()) {
        Renderer::instance().deferRelease(m_buffer);
    }
}

VkBuffer StaticMesh::buffer() const { return m_buffer.buffer; }

std::uint32_t StaticMesh::vertexCount() const { return m_vertex_count; }

UiMesh::UiMesh() = default;

UiMesh::~UiMesh() { releaseBuffer(); }

void UiMesh::clear() {
    m_triangles.clear();
    m_lines.clear();
    m_dirty = true;
}

void UiMesh::addQuad(std::array<Vec3, 4> const& corners, Vec4 const& color) {
    std::array<Vec2, 4> const no_texcoords = {
            Vec2(0.0f), Vec2(0.0f), Vec2(0.0f), Vec2(0.0f)};
    addTexturedQuad(corners, no_texcoords, color);
}

void UiMesh::addTexturedQuad(std::array<Vec3, 4> const& corners,
                             std::array<Vec2, 4> const& texcoords,
                             Vec4 const& color) {
    for (std::size_t index : {0U, 1U, 2U, 0U, 2U, 3U}) {
        m_triangles.push_back({corners[index], color, texcoords[index]});
    }
    m_dirty = true;
}

void UiMesh::addTriangle(std::array<Vec3, 3> const& corners,
                         Vec4 const& color) {
    for (Vec3 const& corner : corners) {
        m_triangles.push_back({corner, color, Vec2(0.0f)});
    }
    m_dirty = true;
}

void UiMesh::addTriangle(std::array<Vec3, 3> const& corners,
                         std::array<Vec4, 3> const& colors) {
    for (std::size_t i = 0; i < corners.size(); ++i) {
        m_triangles.push_back({corners[i], colors[i], Vec2(0.0f)});
    }
    m_dirty = true;
}

void UiMesh::addLine(Vec3 const& from, Vec3 const& to, Vec4 const& color) {
    m_lines.push_back({from, color, Vec2(0.0f)});
    m_lines.push_back({to, color, Vec2(0.0f)});
    m_dirty = true;
}

void UiMesh::setTexture(Texture const* texture) { m_texture = texture; }

Texture const* UiMesh::texture() const { return m_texture; }

void UiMesh::setReplaceTexEnv(bool replace) { m_replace = replace; }

bool UiMesh::replaceTexEnv() const { return m_replace; }

void UiMesh::setLineWidth(float width) { m_line_width = width; }

float UiMesh::lineWidth() const { return m_line_width; }

std::vector<UiVertex> const& UiMesh::triangles() const { return m_triangles; }

std::vector<UiVertex> const& UiMesh::lines() const { return m_lines; }

GpuBuffer const& UiMesh::upload() {
    if (!m_dirty) {
        return m_buffer;
    }
    // A buffer a still-in-flight frame may be reading is never rewritten
    // in place: the old one is released (deferred) and a new one created.
    releaseBuffer();
    std::size_t const vertex_count = m_triangles.size() + m_lines.size();
    if (vertex_count != 0) {
        VkDeviceSize const size = vertex_count * sizeof(UiVertex);
        m_buffer = Renderer::instance().createHostBuffer(
                size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        if (m_buffer.mapped != nullptr) {
            auto* destination = static_cast<char*>(m_buffer.mapped);
            std::memcpy(destination,
                        m_triangles.data(),
                        m_triangles.size() * sizeof(UiVertex));
            std::memcpy(destination + m_triangles.size() * sizeof(UiVertex),
                        m_lines.data(),
                        m_lines.size() * sizeof(UiVertex));
        }
    }
    m_dirty = false;
    return m_buffer;
}

void UiMesh::releaseBuffer() {
    if (m_buffer.buffer != VK_NULL_HANDLE && Renderer::hasInstance()) {
        Renderer::instance().deferRelease(m_buffer);
    }
    m_buffer = GpuBuffer{};
}

}  // namespace vulkan_earth::render
