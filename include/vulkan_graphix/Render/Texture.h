#ifndef VULKAN_GRAPHIX_RENDER_TEXTURE_H
#define VULKAN_GRAPHIX_RENDER_TEXTURE_H

#include <vulkan/vulkan.h>

#include <cstdint>

#include "vulkan_graphix/Tutorial/TutorialBase.h"

namespace vulkan_graphix::Render {

// A sampled RGBA8 texture plus the descriptor set that binds it (set 0,
// binding 0 of every Render pipeline). Created through Renderer
// (createTexture()/loadRawTexture()/loadImageTexture()); destroying it
// hands its Vulkan objects to the Renderer to free once no in-flight frame
// can still use them.
class Texture {
public:
  Texture(ImageParameters image,
      VkDescriptorSet descriptor_set,
      std::uint32_t width,
      std::uint32_t height);
  ~Texture();

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  VkDescriptorSet descriptorSet() const;
  std::uint32_t width() const;
  std::uint32_t height() const;

private:
  ImageParameters m_image;
  VkDescriptorSet m_descriptor_set;
  std::uint32_t m_width;
  std::uint32_t m_height;
};

}  // namespace vulkan_graphix::Render

#endif  // VULKAN_GRAPHIX_RENDER_TEXTURE_H
