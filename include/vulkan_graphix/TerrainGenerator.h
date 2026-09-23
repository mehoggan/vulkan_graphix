#ifndef VULKAN_GRAPHIX_TERRAIN_GENERATOR_H
#define VULKAN_GRAPHIX_TERRAIN_GENERATOR_H

// Shared height-field generator, ported from vulkan_earth's own
// TerrainMaker::terrainGen()/terrainSmoothe()/calcNormal()/
// smoothShadeNormal() (see vulkan_earth/src/TerrainMaker.cpp) - a random-
// walk, diamond-square-style height accumulation followed by a box-blur
// smoothing pass, none of which ever made a GL call in the original either.
// Lives here (not as a private helper on Tutorial12) so both the tutorial
// and any future Vulkan port of vulkan_earth's own terrain rendering can
// call the same generation code instead of keeping separate copies.

#include <array>
#include <cstdint>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix {

class TerrainGenerator {
public:
    // grid_size x grid_size height cells, grid_scale world units apart.
    TerrainGenerator(int grid_size, int grid_scale);

    // Resets the height field, then runs new_steps random-walk/jump steps
    // (each raising every cell within new_radius of the current point by
    // new_increase), then applies smoothing_passes+1 box-blur smoothing
    // passes (matching TerrainMaker::prepareData()'s own `for (int i = -1;
    // i < smoothness; i++) terrainSmoothe(10);` loop).
    void generate(int steps,
                  int increase,
                  float radius,
                  int random_jump,
                  int smoothing_passes);

    int gridSize() const;
    int gridScale() const;
    int heightAt(int x, int z) const;

    // Averages the six adjacent triangle normals around grid vertex (x, z) -
    // ported from TerrainMaker::smoothShadeNormal(). Returns (0, 1, 0) at
    // points where every adjacent triangle falls outside the grid.
    Math::Vec3<float> normalAt(int x, int z) const;

private:
    void terrainGen(int steps, int increase, float radius, int random_jump);
    void terrainSmoothe(int box_width);
    void calcNormal(int x, int z, int flag, Math::Vec3<float>* normal) const;

    int m_grid_size;
    int m_grid_scale;
    std::vector<std::vector<int>> m_heights;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TERRAIN_GENERATOR_H
