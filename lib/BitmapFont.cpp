#include "vulkan_graphix/BitmapFont.h"

#include <cstring>

#define STB_TRUETYPE_IMPLEMENTATION
// NO SECURITY GUARANTEE, per this vendored header's own comment: it does
// no bounds-checking on untrusted font files. Not a practical risk here -
// BitmapFont::load() only ever reads a fixed, trusted system font path,
// never attacker-supplied data.
#include "vulkan_graphix/STBTrueType.h"

#include "vulkan_graphix/Tools.h"

namespace vulkan_graphix {

namespace {
constexpr std::uint32_t c_atlas_width = 512;
constexpr std::uint32_t c_atlas_height = 512;
// Reserved fully-opaque-white block for BitmapFont::solidTexelUv() - an
// 8x8 corner the "crappy packing" glyph baker never reaches as long as
// it doesn't use the whole atlas (checked in load() below), sampled 4px
// in from every edge so a linear-filtering sampler can't blend in
// neighboring (possibly non-white) atlas data.
constexpr std::uint32_t c_solid_block_size = 8;
}  // namespace

bool BitmapFont::load(const std::string& font_path, float pixel_height) {
    std::vector<char> font_data = Tools::getBinaryFileContents(font_path);
    if (font_data.empty()) {
        return false;
    }

    std::vector<unsigned char> bitmap(c_atlas_width * c_atlas_height, 0);
    std::array<stbtt_bakedchar, kGlyphCount> baked_chars{};

    int const bake_result = stbtt_BakeFontBitmap(
            reinterpret_cast<unsigned char const*>(font_data.data()),
            0,
            pixel_height,
            bitmap.data(),
            static_cast<int>(c_atlas_width),
            static_cast<int>(c_atlas_height),
            kFirstChar,
            kGlyphCount,
            baked_chars.data());
    // A positive result is the first unused bitmap row - everything at
    // or past it is guaranteed untouched, so the reserved solid block
    // (the last c_solid_block_size rows) is safe only if baking left at
    // least that many rows free.
    if (bake_result <= 0 ||
        bake_result > static_cast<int>(c_atlas_height - c_solid_block_size)) {
        return false;
    }

    m_atlas_width = c_atlas_width;
    m_atlas_height = c_atlas_height;
    m_atlas_pixels.assign(
            static_cast<std::size_t>(m_atlas_width) * m_atlas_height * 4, 0);
    for (std::size_t i = 0; i < bitmap.size(); ++i) {
        m_atlas_pixels[i * 4 + 0] = static_cast<char>(0xFF);
        m_atlas_pixels[i * 4 + 1] = static_cast<char>(0xFF);
        m_atlas_pixels[i * 4 + 2] = static_cast<char>(0xFF);
        m_atlas_pixels[i * 4 + 3] = static_cast<char>(bitmap[i]);
    }

    std::uint32_t const block_x = m_atlas_width - c_solid_block_size;
    std::uint32_t const block_y = m_atlas_height - c_solid_block_size;
    for (std::uint32_t y = block_y; y < m_atlas_height; ++y) {
        for (std::uint32_t x = block_x; x < m_atlas_width; ++x) {
            std::size_t const pixel_index = (y * m_atlas_width + x) * 4;
            m_atlas_pixels[pixel_index + 0] = static_cast<char>(0xFF);
            m_atlas_pixels[pixel_index + 1] = static_cast<char>(0xFF);
            m_atlas_pixels[pixel_index + 2] = static_cast<char>(0xFF);
            m_atlas_pixels[pixel_index + 3] = static_cast<char>(0xFF);
        }
    }
    m_solid_texel_uv = Math::Vec2<float>(
            static_cast<float>(block_x + c_solid_block_size / 2) /
                    static_cast<float>(m_atlas_width),
            static_cast<float>(block_y + c_solid_block_size / 2) /
                    static_cast<float>(m_atlas_height));

    for (int i = 0; i < kGlyphCount; ++i) {
        float pen_x = 0.0f;
        float pen_y = 0.0f;
        stbtt_aligned_quad quad{};
        stbtt_GetBakedQuad(baked_chars.data(),
                          static_cast<int>(m_atlas_width),
                          static_cast<int>(m_atlas_height),
                          i,
                          &pen_x,
                          &pen_y,
                          &quad,
                          1);

        Glyph& glyph = m_glyphs[i];
        glyph.offset = Math::Vec2<float>(quad.x0, quad.y0);
        glyph.size = Math::Vec2<float>(quad.x1 - quad.x0, quad.y1 - quad.y0);
        glyph.uv_min = Math::Vec2<float>(quad.s0, quad.t0);
        glyph.uv_max = Math::Vec2<float>(quad.s1, quad.t1);
        glyph.advance = baked_chars[i].xadvance;
    }

    m_line_height = pixel_height * 1.25f;
    return true;
}

const std::vector<char>& BitmapFont::atlasPixels() const {
    return m_atlas_pixels;
}
std::uint32_t BitmapFont::atlasWidth() const { return m_atlas_width; }
std::uint32_t BitmapFont::atlasHeight() const { return m_atlas_height; }
Math::Vec2<float> BitmapFont::solidTexelUv() const { return m_solid_texel_uv; }
float BitmapFont::lineHeight() const { return m_line_height; }

std::vector<BitmapFontGlyphQuad> BitmapFont::layoutText(
        const std::string& text, Math::Vec2<float> origin) const {
    std::vector<BitmapFontGlyphQuad> quads;
    quads.reserve(text.size());

    float pen_x = origin.x;
    for (unsigned char character : text) {
        if (character < kFirstChar || character >= kFirstChar + kGlyphCount) {
            continue;
        }
        Glyph const& glyph = m_glyphs[character - kFirstChar];
        if (glyph.size.x > 0.0f && glyph.size.y > 0.0f) {
            BitmapFontGlyphQuad quad;
            quad.top_left = Math::Vec2<float>(pen_x + glyph.offset.x,
                                              origin.y + glyph.offset.y);
            quad.bottom_right = quad.top_left + glyph.size;
            quad.uv_top_left = glyph.uv_min;
            quad.uv_bottom_right = glyph.uv_max;
            quads.push_back(quad);
        }
        pen_x += glyph.advance;
    }
    return quads;
}

float BitmapFont::textWidth(const std::string& text) const {
    float width = 0.0f;
    for (unsigned char character : text) {
        if (character < kFirstChar || character >= kFirstChar + kGlyphCount) {
            continue;
        }
        width += m_glyphs[character - kFirstChar].advance;
    }
    return width;
}

}  // namespace vulkan_graphix
