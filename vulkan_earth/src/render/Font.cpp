#include "vulkan_earth/render/Font.h"

#include <array>
#include <cstdint>
#include <utility>

#include "vulkan_earth/render/Texture.h"

namespace vulkan_earth::render {

namespace {
// glutBitmapWidth() for ASCII 32-127, measured from freeglut itself.
constexpr std::array<std::int32_t, 96> c_times_roman_24_advances = {
        6,  8,  10, 13, 12, 19, 18, 8,  8,  8,  12, 14, 7,  14, 6,  7,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 6,  7,  13, 14, 13, 11,
        22, 17, 16, 16, 17, 15, 14, 18, 19, 8,  11, 17, 14, 22, 18, 18,
        15, 18, 16, 13, 16, 18, 17, 23, 18, 16, 15, 8,  7,  8,  11, 13,
        7,  11, 12, 11, 12, 11, 7,  12, 13, 6,  6,  12, 6,  20, 13, 12,
        12, 12, 8,  10, 7,  13, 11, 17, 13, 11, 10, 10, 6,  10, 13, 6,
};
constexpr std::array<std::int32_t, 96> c_fixed_9_by_15_advances = {
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
};
// glutBitmapHeight().
constexpr std::int32_t c_times_roman_24_height = 29;
constexpr std::int32_t c_fixed_9_by_15_height = 16;
}  // namespace

std::int32_t glutBitmapWidth(FontId id, char character) {
    std::int32_t const index =
            static_cast<std::int32_t>(static_cast<std::uint8_t>(character)) -
            GlutFont::c_first_char;
    if (index < 0 || index >= GlutFont::c_columns * GlutFont::c_rows) {
        return 0;
    }
    auto const& table = id == FontId::TimesRoman24 ? c_times_roman_24_advances
                                                   : c_fixed_9_by_15_advances;
    return table[static_cast<std::size_t>(index)];
}

GlutFont::GlutFont(FontId id, std::shared_ptr<Texture> atlas)
        : m_id(id), m_atlas(std::move(atlas)) {}

FontId GlutFont::id() const { return m_id; }

Texture const& GlutFont::atlas() const { return *m_atlas; }

std::int32_t GlutFont::advance(char character) const {
    return glutBitmapWidth(m_id, character);
}

std::int32_t GlutFont::stringWidth(std::string_view text) const {
    std::int32_t width = 0;
    for (char character : text) {
        width += advance(character);
    }
    return width;
}

std::int32_t GlutFont::height() const {
    return m_id == FontId::TimesRoman24 ? c_times_roman_24_height
                                        : c_fixed_9_by_15_height;
}

}  // namespace vulkan_earth::render
