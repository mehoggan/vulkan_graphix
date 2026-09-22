#ifndef VULKAN_GRAPHIX_TOOLS_H
#define VULKAN_GRAPHIX_TOOLS_H

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
