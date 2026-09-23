#ifndef VULKAN_GRAPHIX_BITMAP_FONT_H
#define VULKAN_GRAPHIX_BITMAP_FONT_H

// Bakes a TrueType font's printable-ASCII glyphs into an RGBA texture
// atlas, using the vendored STBTrueType.h (see that file's header comment)
// - but keeps stb entirely out of this interface: every public member
// here is std::string/std::vector/Math:: only. No stbtt_* type or symbol
// is reachable outside BitmapFont.cpp, which is the only file that
// includes STBTrueType.h.
//
// Ported in spirit from vulkan_earth's TextObject (see
// vulkan_earth/src/TextObject.cpp), which drew text via freeglut's
// glutBitmapCharacter() - a font compiled into libglut itself with no
// accessible glyph pixels, and so no equivalent under Vulkan. This is a
// from-scratch replacement, not a port of TextObject's own code.

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix {

struct BitmapFontGlyphQuad {
    Math::Vec2<float> top_left;
    Math::Vec2<float> bottom_right;
    Math::Vec2<float> uv_top_left;
    Math::Vec2<float> uv_bottom_right;
};

class BitmapFont {
public:
    // Bakes the printable ASCII range (32-126) at pixel_height from the
    // TrueType file at font_path into a square RGBA8 atlas. Returns false
    // (font left unusable) if the file can't be read, isn't a valid TTF,
    // or its glyphs don't fit the atlas - same fail-cleanly convention as
    // Tools::getImageData()/getRawImageData().
    bool load(const std::string& font_path, float pixel_height);

    // RGBA8 atlas pixels, atlasWidth()*atlasHeight()*4 bytes, ready for a
    // VK_FORMAT_R8G8B8A8_UNORM upload - callers own the Vulkan side.
    const std::vector<char>& atlasPixels() const;
    std::uint32_t atlasWidth() const;
    std::uint32_t atlasHeight() const;

    // UV of a small reserved region that's fully opaque white and never
    // overlaps a glyph. Lets a caller draw flat-color quads (e.g. a
    // button's bevel, see UiGeometry.h) through the exact same textured
    // pipeline as glyph text, tinted via vertex color instead of
    // sampling an actual glyph.
    Math::Vec2<float> solidTexelUv() const;

    // Lays out `text` as a sequence of glyph quads with `origin` as the
    // left end of the text baseline (y increases downward, matching this
    // project's screen-pixel convention elsewhere) - pure CPU geometry,
    // no Vulkan calls, same "logic only" precedent as TerrainGenerator/
    // Tools::loadOglMeshData(). Characters outside the baked range are
    // skipped; zero-area glyphs (e.g. space) emit no quad.
    std::vector<BitmapFontGlyphQuad> layoutText(const std::string& text,
                                                Math::Vec2<float> origin) const;
    float textWidth(const std::string& text) const;
    float lineHeight() const;

private:
    static constexpr int kFirstChar = 32;
    static constexpr int kGlyphCount = 95;  // ASCII 32-126 inclusive

    struct Glyph {
        Math::Vec2<float> size;
        Math::Vec2<float> offset;
        Math::Vec2<float> uv_min;
        Math::Vec2<float> uv_max;
        float advance = 0.0f;
    };

    std::vector<char> m_atlas_pixels;
    std::uint32_t m_atlas_width = 0;
    std::uint32_t m_atlas_height = 0;
    Math::Vec2<float> m_solid_texel_uv;
    std::array<Glyph, kGlyphCount> m_glyphs;
    float m_line_height = 0.0f;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_BITMAP_FONT_H
