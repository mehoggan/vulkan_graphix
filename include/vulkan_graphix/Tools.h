#ifndef VULKAN_GRAPHIX_TOOLS_H
#define VULKAN_GRAPHIX_TOOLS_H

#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Math/MathTypes.hpp"

// TODO (mehoggan@gmail.com): This file needs to be doccumented.

namespace vulkan_graphix::Tools {

template <class T, class F> class AutoDeleter {
public:
    AutoDeleter()
            : object(VK_NULL_HANDLE)
            , deleter(nullptr)
            , device(VK_NULL_HANDLE) {}

    AutoDeleter(T new_object, F new_deleter, VkDevice new_device)
            : object(new_object), deleter(new_deleter), device(new_device) {}

    AutoDeleter(AutoDeleter&& other) noexcept { *this = std::move(other); }

    AutoDeleter(const AutoDeleter&) = delete;
    AutoDeleter& operator=(const AutoDeleter&) = delete;

    ~AutoDeleter() {
        if ((object != VK_NULL_HANDLE) && (deleter != nullptr) &&
            (device != VK_NULL_HANDLE)) {
            deleter(device, object, nullptr);
        }
    }

    AutoDeleter& operator=(AutoDeleter&& other) noexcept {
        if (this != &other) {
            object = other.object;
            deleter = other.deleter;
            device = other.device;
            other.object = VK_NULL_HANDLE;
        }
        return *this;
    }

    T get() { return object; }

    bool operator!() const { return object == VK_NULL_HANDLE; }

private:
    T object;
    F deleter;
    VkDevice device;
};

std::vector<char> getBinaryFileContents(std::string const& filename);

std::vector<char> getImageData(std::string const& filename,
                               int requested_components,
                               int* width,
                               int* height,
                               int* components,
                               int* data_size);

// Loads a headerless raw RGB (3 bytes/pixel) file of exactly width*height*3
// bytes - the format every .raw asset under vulkan_earth/src/ uses (no
// magic number, no dimensions stored in the file; the caller has to already
// know them) - and expands it to RGBA (inserting a 0xFF alpha byte after
// every pixel) so the result is drop-in compatible with the same
// VK_FORMAT_R8G8B8A8_UNORM upload path getImageData()'s callers use.
// Returns an empty vector on any read failure.
std::vector<char> getRawImageData(std::string const& filename,
                                  std::uint32_t width,
                                  std::uint32_t height);

// One vertex from a vulkan_earth ".ogl" mesh file - a plain-ASCII,
// whitespace-delimited, unindexed format with no header:
// [texcoord.s, texcoord.t, normal.x, normal.y, normal.z, vertex.x,
// vertex.y, vertex.z] repeated per vertex, three vertices per triangle
// (see VBOShaderLibrary::loadClientData(const std::string&) in
// vulkan_earth/src/VBOShaderLibrary.cpp for the format this mirrors).
struct OglVertexData {
    Math::Vec2<float> texcoord;
    Math::Vec3<float> normal;
    Math::Vec3<float> position;
};

// Parses an entire ".ogl" file into a flat, unindexed vertex list (three
// consecutive entries make one triangle). Returns an empty vector on any
// read failure or if the file's token count isn't a multiple of 8.
std::vector<OglVertexData> loadOglMeshData(std::string const& filename);

vulkan_graphix::Math::Mat4<float> getPerspectiveProjectionMatrix(
        float const aspect_ratio,
        float const field_of_view,
        float const near_clip,
        float const far_clip);

vulkan_graphix::Math::Mat4<float> getOrthographicProjectionMatrix(
        float const left_plane,
        float const right_plane,
        float const top_plane,
        float const bottom_plane,
        float const near_plane,
        float const far_plane);

}  // namespace vulkan_graphix::Tools

#endif
