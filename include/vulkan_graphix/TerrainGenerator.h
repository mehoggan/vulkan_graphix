#ifndef VULKAN_GRAPHIX_TERRAINGENERATOR_H
#define VULKAN_GRAPHIX_TERRAINGENERATOR_H

// Shared height field, ported from vulkan_earth's own TerrainMaker (see
// vulkan_earth/src/TerrainMaker.cpp): generation (terrainGen()/
// terrainSmoothe()), per-vertex normals (calcNormal()/smoothShadeNormal()),
// the world-position queries gameplay uses (getHeightAt()/getNormalAt()/
// getTriangleNormal()), and crater deformation (makeCrater()) - none of
// which ever made a GL call in the original either. This is the one copy:
// TerrainMaker itself keeps only its GL buffer/draw code and calls into
// this class, and so do the Vulkan tutorials (Tutorial12/21).
//
// Grid layout matches TerrainMaker's own th[z][x]: grid vertex (x, z) sits
// at world position (x * gridScale(), heightAt(x, z), z * gridScale()),
// and every accessor here takes (x, z) in that order. World-position
// queries take coordinates in that same space (grid vertex (0, 0) at the
// world origin) - a caller that centers or offsets its mesh converts first.

#include <cstdint>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix {

// One grid vertex, by (x, z) grid index.
struct TerrainGridCell {
    std::int32_t m_x;
    std::int32_t m_z;
};

class TerrainGenerator {
public:
    // grid_size x grid_size height cells, grid_scale world units apart, all
    // starting at height 0 (as TerrainMaker::prepTerrain() zeroed them).
    TerrainGenerator(std::int32_t grid_size, std::int32_t grid_scale);

    // Resets the height field, then runs steps random-walk/jump steps
    // (each raising every cell within radius of the current point by
    // increase), then applies smoothing_passes+1 box-blur smoothing
    // passes (matching TerrainMaker::prepareData()'s own `for (int i = -1;
    // i < smoothness; i++) terrainSmoothe(10);` loop). Uses the C library's
    // rand(), exactly as TerrainMaker did - seed it with srand() first.
    void generate(std::int32_t steps,
                  std::int32_t increase,
                  float radius,
                  std::int32_t random_jump,
                  std::int32_t smoothing_passes);

    std::int32_t gridSize() const;
    std::int32_t gridScale() const;
    std::int32_t heightAt(std::int32_t x, std::int32_t z) const;

    // Averages the six adjacent triangle normals around grid vertex (x, z) -
    // ported from TerrainMaker::smoothShadeNormal(). Returns (0, 1, 0) at
    // points where every adjacent triangle falls outside the grid.
    Math::Vec3<float> normalAt(std::int32_t x, std::int32_t z) const;

    // Height of the grid vertex nearest world position (world_x, world_z),
    // or 0 outside the grid - ported from TerrainMaker::getHeightAt().
    float heightAtWorld(float world_x, float world_z) const;

    // Normal of the grid cell at world position (world_x, world_z), built
    // from that cell's two edge vectors - ported from
    // TerrainMaker::getNormalAt(), including its own rounding (an exact
    // grid-line coordinate rounds up to the next vertex). Returns (0, 1, 0)
    // where the cell's far edge would fall outside the grid.
    Math::Vec3<float> normalAtWorld(float world_x, float world_z) const;

    // Upward-facing normal of the triangle (x, z) -> (x + 1, z) ->
    // (x, z + 1), in grid units with heights divided by 100 - ported from
    // TerrainMaker::getTriangleNormal(), which vulkan_earth uses to tilt a
    // tank to the ground under it. Returns (0, 1, 0) where the triangle
    // falls outside the grid.
    Math::Vec3<float> triangleNormalAt(std::int32_t x, std::int32_t z) const;

    // Blasts a crater of radius blast_size (in grid cells) centered on
    // world position (impact_x, impact_z) - ported from
    // TerrainMaker::makeCrater()'s own height math. Returns every grid
    // vertex inside the blast radius (whether or not its height actually
    // dropped), in the order they were visited, so a renderer knows which
    // vertices' heights and normals to re-upload; empty if the impact is
    // outside the grid.
    std::vector<TerrainGridCell> makeCrater(float impact_x,
                                            float impact_z,
                                            float blast_size);

private:
    void terrainGen(std::int32_t steps,
                    std::int32_t increase,
                    float radius,
                    std::int32_t random_jump);
    void terrainSmoothe(std::int32_t box_width);
    void calcNormal(std::int32_t x,
                    std::int32_t z,
                    std::int32_t flag,
                    Math::Vec3<float>* normal) const;
    bool inGrid(std::int32_t x, std::int32_t z) const;

    std::int32_t m_grid_size;
    std::int32_t m_grid_scale;
    // m_heights[z][x], exactly like TerrainMaker's th[z][x].
    std::vector<std::vector<std::int32_t>> m_heights;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TERRAINGENERATOR_H
