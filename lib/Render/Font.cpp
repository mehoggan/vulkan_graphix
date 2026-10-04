#include "vulkan_graphix/Render/Font.h"

#include <utility>

#include "vulkan_graphix/Render/Texture.h"

namespace vulkan_graphix::Render {

Font::Font(BitmapFont bitmap, std::shared_ptr<Texture> atlas)
        : m_bitmap(std::move(bitmap)), m_atlas(std::move(atlas)) {}

BitmapFont const& Font::bitmap() const { return m_bitmap; }

Texture const& Font::atlas() const { return *m_atlas; }

float Font::textWidth(std::string const& text) const {
    return m_bitmap.textWidth(text);
}

}  // namespace vulkan_graphix::Render
