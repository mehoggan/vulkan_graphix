// Exercises the library's CPU-side geometry and image building blocks -
// ImageAtlas, SkyboxGeometry, and UiGeometry's GridLayout and HUD bar
// colors - directly. No Vulkan device or X11 window, so this runs
// unconditionally (no DISPLAY check).

#include <array>
#include <cstddef>
#include <cstdint>
#include <set>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "vulkan_graphix/ImageAtlas.h"
#include "vulkan_graphix/SkyboxGeometry.h"
#include "vulkan_graphix/UiGeometry.h"

namespace {

namespace vg = vulkan_graphix;
namespace math = vulkan_graphix::Math;

std::vector<char> solidTile(std::uint32_t tile_size, char value) {
  return std::vector<char>(
      static_cast<std::size_t>(tile_size) * tile_size * 4, value);
}

}  // namespace

TEST(ImageAtlasTest, PlacesTilesRowMajorFromTheTopLeft) {
  vg::ImageAtlas atlas(2, 3, 2);
  ASSERT_EQ(6u, atlas.width());
  ASSERT_EQ(4u, atlas.height());
  ASSERT_EQ(6u * 4u * 4u, atlas.pixels().size());
  EXPECT_TRUE(atlas.setTile(4, solidTile(2, 7)));  // column 1, row 1

  for (std::uint32_t y = 0; y < atlas.height(); ++y) {
    for (std::uint32_t x = 0; x < atlas.width(); ++x) {
      const bool inside = x >= 2 && x < 4 && y >= 2 && y < 4;
      for (std::uint32_t channel = 0; channel < 4; ++channel) {
        EXPECT_EQ(
            inside ? 7 : 0,
            atlas.pixels()
                [(static_cast<std::size_t>(y) * atlas.width() + x) * 4 +
                 channel])
            << "pixel " << x << "," << y;
      }
    }
  }
}

TEST(ImageAtlasTest, RejectsBadTilesAndMissingFiles) {
  vg::ImageAtlas atlas(2, 2, 2);
  EXPECT_FALSE(atlas.setTile(4, solidTile(2, 1)));  // out of range
  EXPECT_FALSE(atlas.setTile(0, solidTile(3, 1)));  // wrong size
  EXPECT_FALSE(atlas.setTileFromRawFile(0, "no/such/file.raw"));
  for (const char value : atlas.pixels()) {
    ASSERT_EQ(0, value);
  }
}

TEST(ImageAtlasTest, TileUvsCoverEachTile) {
  const vg::ImageAtlas atlas(256, 4, 2);
  const vg::ImageAtlas::UvRect tile_uv = atlas.tileUv(6);  // column 2, row 1
  EXPECT_FLOAT_EQ(0.5f, tile_uv.m_min.x);
  EXPECT_FLOAT_EQ(0.5f, tile_uv.m_min.y);
  EXPECT_FLOAT_EQ(0.75f, tile_uv.m_max.x);
  EXPECT_FLOAT_EQ(1.0f, tile_uv.m_max.y);
  const vg::ImageAtlas::UvRect same = vg::ImageAtlas::tileUv(6, 4, 2);
  EXPECT_EQ(tile_uv.m_min, same.m_min);
  EXPECT_EQ(tile_uv.m_max, same.m_max);
}

TEST(GridLayoutTest, SplitsTheGridIntoEqualCellsWithGaps) {
  const vg::UiGeometry::GridLayout grid{
      math::Vec2<float>(10.0f, 20.0f),
      math::Vec2<float>(320.0f, 110.0f),
      4,
      2,
      10.0f};
  EXPECT_EQ(math::Vec2<float>(72.5f, 50.0f), grid.cellSize());
  EXPECT_EQ(math::Vec2<float>(10.0f, 20.0f), grid.cellTopLeft(0));
  EXPECT_EQ(math::Vec2<float>(10.0f + 3 * 82.5f, 20.0f), grid.cellTopLeft(3));
  EXPECT_EQ(math::Vec2<float>(10.0f + 82.5f, 80.0f), grid.cellTopLeft(5));
}

TEST(HudBarColorTest, HealthAndPowerRampInOppositeDirections) {
  EXPECT_EQ(
      math::Vec4<float>(0.0f, 1.0f, 0.0f, 1.0f),
      vg::UiGeometry::healthBarColor(1.0f));
  EXPECT_EQ(
      math::Vec4<float>(1.0f, 0.0f, 0.0f, 1.0f),
      vg::UiGeometry::healthBarColor(0.0f));
  EXPECT_EQ(
      math::Vec4<float>(0.25f, 0.75f, 0.0f, 1.0f),
      vg::UiGeometry::powerBarColor(0.25f));
}

TEST(SkyboxGeometryTest, SixFacesEachOnTheirOwnSideOfTheBox) {
  const math::Vec3<float> min_corner(-4.0f, -2.0f, -6.0f);
  const math::Vec3<float> max_corner(4.0f, 1.0f, 6.0f);
  const std::array<vg::SkyboxGeometry::Face, 6> faces =
      vg::SkyboxGeometry::buildFaces(min_corner, max_corner);

  // front, right, back, left, top, bottom: the axis each face is flat on
  // and the value it's flat at.
  const std::array<std::pair<int, float>, 6> planes = {
      {{2, -6.0f}, {0, 4.0f}, {2, 6.0f}, {0, -4.0f}, {1, 1.0f}, {1, -2.0f}}};
  std::set<std::array<float, 3>> corners_seen;
  for (std::size_t face = 0; face < faces.size(); ++face) {
    for (std::size_t corner = 0; corner < 4; ++corner) {
      const math::Vec3<float>& point = faces[face].m_corners[corner];
      EXPECT_FLOAT_EQ(planes[face].second, point[planes[face].first])
          << "face " << face;
      corners_seen.insert({point.x, point.y, point.z});
    }
    EXPECT_EQ(math::Vec2<float>(0.0f, 1.0f), faces[face].m_texcoords[0]);
    EXPECT_EQ(math::Vec2<float>(1.0f, 1.0f), faces[face].m_texcoords[3]);
  }
  EXPECT_EQ(8u, corners_seen.size());  // every corner of the box, only
}
