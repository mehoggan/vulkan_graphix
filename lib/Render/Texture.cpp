#include "vulkan_graphix/Render/Texture.h"

#include <utility>

#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_graphix::Render {

Texture::Texture(ImageParameters image,
                 VkDescriptorSet descriptor_set,
                 std::uint32_t width,
                 std::uint32_t height)
        : m_image(std::move(image))
        , m_descriptor_set(descriptor_set)
        , m_width(width)
        , m_height(height) {}

Texture::~Texture() {
    if (Renderer::hasInstance()) {
        Renderer::instance().deferRelease(m_image, m_descriptor_set);
    }
}

VkDescriptorSet Texture::descriptorSet() const { return m_descriptor_set; }

std::uint32_t Texture::width() const { return m_width; }

std::uint32_t Texture::height() const { return m_height; }

}  // namespace vulkan_graphix::Render
