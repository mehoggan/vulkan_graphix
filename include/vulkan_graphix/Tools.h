#ifndef VULKAN_GRAPHIX_TOOLS_H
#define VULKAN_GRAPHIX_TOOLS_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Math/MathTypes.hpp"

// TODO (mehoggan@gmail.com): This file needs to be doccumented.

namespace vulkan_graphix::Tools {

template <class T, class F>
class AutoDeleter {
public:
  AutoDeleter() :
      m_object(VK_NULL_HANDLE),
      m_deleter(nullptr),
      m_device(VK_NULL_HANDLE) {}

  AutoDeleter(T new_object, F new_deleter, VkDevice new_device) :
      m_object(new_object),
      m_deleter(new_deleter),
      m_device(new_device) {}

  AutoDeleter(AutoDeleter&& other) noexcept { *this = std::move(other); }

  AutoDeleter(const AutoDeleter&) = delete;
  AutoDeleter& operator=(const AutoDeleter&) = delete;

  ~AutoDeleter() {
    if ((m_object != VK_NULL_HANDLE) && (m_deleter != nullptr) &&
        (m_device != VK_NULL_HANDLE)) {
      m_deleter(m_device, m_object, nullptr);
    }
  }

  AutoDeleter& operator=(AutoDeleter&& other) noexcept {
    if (this != &other) {
      m_object = other.m_object;
      m_deleter = other.m_deleter;
      m_device = other.m_device;
      other.m_object = VK_NULL_HANDLE;
    }
    return *this;
  }

  T get() { return m_object; }

  bool operator!() const { return m_object == VK_NULL_HANDLE; }

private:
  T m_object;
  F m_deleter;
  VkDevice m_device;
};

// The directory of the running executable (/proc/self/exe's), skipping a
// libtool .libs/ wrapper directory: where the build copies every asset,
// and what the file readers below resolve relative filenames against.
std::filesystem::path executableDir();

std::vector<char> getBinaryFileContents(const std::string& filename);

std::vector<char> getImageData(const std::string& filename,
    std::int32_t requested_components,
    std::int32_t* width,
    std::int32_t* height,
    std::int32_t* components,
    std::int32_t* data_size);

// Loads a headerless raw RGB (3 bytes/pixel) file of exactly width*height*3
// bytes - the format every .raw asset under vulkan_earth/src/ uses (no
// magic number, no dimensions stored in the file; the caller has to already
// know them) - and expands it to RGBA (inserting a 0xFF alpha byte after
// every pixel) so the result is drop-in compatible with the same
// VK_FORMAT_R8G8B8A8_UNORM upload path getImageData()'s callers use.
// Returns an empty vector on any read failure.
std::vector<char> getRawImageData(
    const std::string& filename, std::uint32_t width, std::uint32_t height);

// One vertex from a vulkan_earth ".ogl" mesh file - a plain-ASCII,
// whitespace-delimited, unindexed format with no header:
// [texcoord.s, texcoord.t, normal.x, normal.y, normal.z, vertex.x,
// vertex.y, vertex.z] repeated per vertex, three vertices per triangle
// (see VBOShaderLibrary::loadClientData(const std::string&) in
// vulkan_earth/src/VBOShaderLibrary.cpp for the format this mirrors).
struct OglVertexData {
  Math::Vec2<float> m_texcoord;
  Math::Vec3<float> m_normal;
  Math::Vec3<float> m_position;
};

// Parses an entire ".ogl" file into a flat, unindexed vertex list (three
// consecutive entries make one triangle). Returns an empty vector on any
// read failure or if the file's token count isn't a multiple of 8.
std::vector<OglVertexData> loadOglMeshData(const std::string& filename);

vulkan_graphix::Math::Mat4<float> getPerspectiveProjectionMatrix(
    const float aspect_ratio,
    const float field_of_view,
    const float near_clip,
    const float far_clip);

vulkan_graphix::Math::Mat4<float> getOrthographicProjectionMatrix(
    const float left_plane,
    const float right_plane,
    const float top_plane,
    const float bottom_plane,
    const float near_plane,
    const float far_plane);

}  // namespace vulkan_graphix::Tools

#endif
