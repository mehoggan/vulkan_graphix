#ifndef VULKAN_GRAPHIX_RENDER_FONT_H
#define VULKAN_GRAPHIX_RENDER_FONT_H

#include <memory>
#include <string>

#include "vulkan_graphix/BitmapFont.h"

namespace vulkan_graphix::Render {

class Texture;

// A BitmapFont and its glyph atlas uploaded as a Texture - what
// RenderContext::drawText() draws with. Created through
// Renderer::loadFont().
class Font {
public:
  Font(BitmapFont bitmap, std::shared_ptr<Texture> atlas);

  const BitmapFont& bitmap() const;
  const Texture& atlas() const;
  // BitmapFont::textWidth().
  float textWidth(const std::string& text) const;

private:
  BitmapFont m_bitmap;
  std::shared_ptr<Texture> m_atlas;
};

}  // namespace vulkan_graphix::Render

#endif  // VULKAN_GRAPHIX_RENDER_FONT_H
