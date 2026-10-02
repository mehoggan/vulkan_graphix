#ifndef VULKAN_EARTH_RENDER_FONT_H
#define VULKAN_EARTH_RENDER_FONT_H

// The two GLUT bitmap fonts vulkan_earth drew all of its text with
// (GLUT_BITMAP_TIMES_ROMAN_24 and GLUT_BITMAP_9_BY_15), reproduced exactly:
// fonts/glut_*.raw are freeglut's own glyph bitmaps baked into atlases
// (16 x 6 cells of 48 x 48 pixels covering ASCII 32-127, each glyph drawn
// with its raster position at (8, 36) from the cell's top-left), and the
// advance tables below are glutBitmapWidth()'s own values - so text lays
// out and looks the same as it did under GLUT.

#include <cstdint>
#include <memory>
#include <string_view>

namespace vulkan_earth::render {

class Texture;

enum class FontId : std::uint8_t {
    TimesRoman24,
    Fixed9By15,
};

// glutBitmapWidth(font, character), without needing a live renderer - for
// layout code that measures text before anything is drawn.
std::int32_t glutBitmapWidth(FontId id, char character);

class GlutFont {
public:
    static constexpr std::int32_t c_cell_size = 48;
    static constexpr std::int32_t c_columns = 16;
    static constexpr std::int32_t c_rows = 6;
    static constexpr std::int32_t c_origin_x = 8;
    static constexpr std::int32_t c_origin_y = 36;
    static constexpr std::int32_t c_first_char = 32;

    GlutFont(FontId id, std::shared_ptr<Texture> atlas);

    FontId id() const;
    Texture const& atlas() const;
    // glutBitmapWidth(font, character).
    std::int32_t advance(char character) const;
    // Sum of advance() over text.
    std::int32_t stringWidth(std::string_view text) const;
    // glutBitmapHeight(font).
    std::int32_t height() const;

private:
    FontId m_id;
    std::shared_ptr<Texture> m_atlas;
};

}  // namespace vulkan_earth::render

#endif  // VULKAN_EARTH_RENDER_FONT_H
