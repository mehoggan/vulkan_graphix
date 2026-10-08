#ifndef VULKAN_GRAPHIX_IMAGEATLAS_H
#define VULKAN_GRAPHIX_IMAGEATLAS_H

// A grid of equally sized RGBA tiles stitched into one image (icons for an
// inventory grid, say), plus each tile's texture coordinates - so a whole
// set of small images can be one texture and one descriptor set. CPU-side
// pixels only; upload them however the caller creates textures.

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix {

class ImageAtlas {
public:
  struct UvRect {
    Math::Vec2<float> m_min;
    Math::Vec2<float> m_max;
  };

  // columns x rows tiles of tile_size x tile_size pixels, all transparent
  // black. Tiles are numbered row-major from the top left.
  ImageAtlas(
      std::uint32_t tile_size, std::uint32_t columns, std::uint32_t rows);

  // Copies tile_size x tile_size RGBA pixels into tile index; false (and
  // nothing written) when the index or pixel count is wrong.
  bool setTile(std::size_t index, const std::vector<char>& rgba_pixels);
  // setTile() with a raw RGBA file (Tools::getRawImageData()); false when
  // it can't be read.
  bool setTileFromRawFile(std::size_t index, const std::string& filename);

  std::uint32_t width() const;
  std::uint32_t height() const;
  // width() x height() RGBA, row-major from the top left.
  const std::vector<char>& pixels() const;

  UvRect tileUv(std::size_t index) const;
  // The same, for any columns x rows grid, without the pixels.
  static UvRect tileUv(
      std::size_t index, std::uint32_t columns, std::uint32_t rows);

private:
  std::uint32_t m_tile_size;
  std::uint32_t m_columns;
  std::uint32_t m_rows;
  std::vector<char> m_pixels;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_IMAGEATLAS_H
