#include "vulkan_graphix/ImageAtlas.h"

#include <cstring>

#include "vulkan_graphix/Tools.h"

namespace vulkan_graphix {

ImageAtlas::ImageAtlas(
    std::uint32_t tile_size, std::uint32_t columns, std::uint32_t rows) :
    m_tile_size(tile_size),
    m_columns(columns),
    m_rows(rows),
    m_pixels(
        static_cast<std::size_t>(tile_size) * columns * tile_size * rows * 4,
        0) {}

bool ImageAtlas::setTile(
    std::size_t index, const std::vector<char>& rgba_pixels) {
  const std::size_t row_bytes = static_cast<std::size_t>(m_tile_size) * 4;
  if (index >= static_cast<std::size_t>(m_columns) * m_rows ||
      rgba_pixels.size() != row_bytes * m_tile_size) {
    return false;
  }
  const std::size_t dest_x = (index % m_columns) * m_tile_size;
  const std::size_t dest_y = (index / m_columns) * m_tile_size;
  for (std::size_t y = 0; y < m_tile_size; ++y) {
    std::memcpy(m_pixels.data() + ((dest_y + y) * width() + dest_x) * 4,
        rgba_pixels.data() + y * row_bytes,
        row_bytes);
  }
  return true;
}

bool ImageAtlas::setTileFromRawFile(
    std::size_t index, const std::string& filename) {
  const std::vector<char> pixels =
      Tools::getRawImageData(filename, m_tile_size, m_tile_size);
  return !pixels.empty() && setTile(index, pixels);
}

std::uint32_t ImageAtlas::width() const { return m_tile_size * m_columns; }

std::uint32_t ImageAtlas::height() const { return m_tile_size * m_rows; }

const std::vector<char>& ImageAtlas::pixels() const { return m_pixels; }

ImageAtlas::UvRect ImageAtlas::tileUv(std::size_t index) const {
  return tileUv(index, m_columns, m_rows);
}

ImageAtlas::UvRect ImageAtlas::tileUv(
    std::size_t index, std::uint32_t columns, std::uint32_t rows) {
  const std::uint32_t column = static_cast<std::uint32_t>(index) % columns;
  const std::uint32_t grid_row = static_cast<std::uint32_t>(index) / columns;
  return {Math::Vec2<float>(
              static_cast<float>(column) / static_cast<float>(columns),
              static_cast<float>(grid_row) / static_cast<float>(rows)),
      Math::Vec2<float>(
          static_cast<float>(column + 1) / static_cast<float>(columns),
          static_cast<float>(grid_row + 1) / static_cast<float>(rows))};
}

}  // namespace vulkan_graphix
