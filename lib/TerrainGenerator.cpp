#include "vulkan_graphix/TerrainGenerator.h"

#include <cmath>
#include <cstdlib>

namespace vulkan_graphix {

TerrainGenerator::TerrainGenerator(int grid_size, int grid_scale)
        : m_grid_size(grid_size), m_grid_scale(grid_scale) {}

int TerrainGenerator::gridSize() const { return m_grid_size; }
int TerrainGenerator::gridScale() const { return m_grid_scale; }

int TerrainGenerator::heightAt(int x, int z) const { return m_heights[x][z]; }

void TerrainGenerator::generate(int steps,
                                int increase,
                                float radius,
                                int random_jump,
                                int smoothing_passes) {
    m_heights.assign(m_grid_size, std::vector<int>(m_grid_size, 0));
    terrainGen(steps, increase, radius, random_jump);
    for (int i = -1; i < smoothing_passes; ++i) {
        terrainSmoothe(10);
    }
}

void TerrainGenerator::terrainGen(int steps,
                                  int increase,
                                  float radius,
                                  int random_jump) {
    float current_x = static_cast<float>(m_grid_size / 2);
    float current_y = static_cast<float>(m_grid_size / 2);

    for (int current_step = 1; current_step < steps; ++current_step) {
        int const random_value = rand() % 100;

        if (random_value > random_jump) {
            switch (rand() % 4) {
                case 0:
                    current_x -= 1.0f;
                    break;
                case 1:
                    current_x += 1.0f;
                    break;
                case 2:
                    current_y -= 1.0f;
                    break;
                case 3:
                    current_y += 1.0f;
                    break;
                default:
                    break;
            }
            if (((current_x >= m_grid_size) || (current_x < 0)) ||
                ((current_y >= m_grid_size) || (current_y < 0))) {
                current_x = static_cast<float>(rand() % m_grid_size);
                current_y = static_cast<float>(rand() % m_grid_size);
            }
        } else {
            current_x = static_cast<float>(rand() % m_grid_size);
            current_y = static_cast<float>(rand() % m_grid_size);
        }

        int const x_min = static_cast<int>(current_x - radius);
        int const x_max = static_cast<int>(current_x + radius);
        int const y_min = static_cast<int>(current_y - radius);
        int const y_max = static_cast<int>(current_y + radius);
        for (int x = x_min; x < x_max; ++x) {
            for (int y = y_min; y < y_max; ++y) {
                float const distance = std::sqrt(
                        std::pow(static_cast<double>(current_x - x), 2) +
                        std::pow(static_cast<double>(current_y) - y, 2));
                if ((distance < radius) &&
                    ((x >= 0 && x < m_grid_size) &&
                     (y >= 0 && y < m_grid_size))) {
                    m_heights[x][y] += increase;
                }
            }
        }
    }
}

void TerrainGenerator::terrainSmoothe(int box_width) {
    std::vector<std::vector<int>> smoothed = m_heights;
    for (int y = 0; y < m_grid_size; ++y) {
        for (int x = 0; x < m_grid_size; ++x) {
            int height_sum = 0;
            for (int i = y - (box_width / 2); i < y + (box_width / 2); ++i) {
                for (int j = x - (box_width / 2); j < x + (box_width / 2);
                     ++j) {
                    if ((i >= 0 && i < m_grid_size) &&
                        (j >= 0 && j < m_grid_size)) {
                        height_sum += m_heights[i][j];
                    }
                }
            }
            smoothed[y][x] = height_sum / (box_width * box_width);
        }
    }
    m_heights = smoothed;

    // Zero the four grid edges - collapses TerrainMaker::terrainSmoothe()'s
    // four separate (but overlapping) edge-clearing conditions into the one
    // condition they're together equivalent to.
    for (int y = 0; y < m_grid_size; ++y) {
        for (int x = 0; x < m_grid_size; ++x) {
            if (x == 0 || y == 0 || x == m_grid_size - 1 ||
                y == m_grid_size - 1) {
                m_heights[y][x] = 0;
            }
        }
    }
}

void TerrainGenerator::calcNormal(int x,
                                  int z,
                                  int flag,
                                  Math::Vec3<float>* normal) const {
    std::array<float, 3> v1 = {};
    std::array<float, 3> v2 = {};
    bool can_calculate = true;
    if (flag == 1) {
        if (((x - 1) >= 0) && ((z - 1) >= 0) && (x < m_grid_size) &&
            (z < m_grid_size)) {
            v1[0] = -static_cast<float>(m_grid_scale);
            v1[1] = static_cast<float>(m_heights[x - 1][z - 1] -
                                       m_heights[x][z]);
            v1[2] = -static_cast<float>(m_grid_scale);

            v2[0] = -static_cast<float>(m_grid_scale);
            v2[1] =
                    static_cast<float>(m_heights[x - 1][z] - m_heights[x][z]);
            v2[2] = 0.0f;
        } else {
            can_calculate = false;
        }
    } else {
        if ((((x - 1) >= 0) && (x < m_grid_size)) && ((z + 1) < m_grid_size) &&
            (z > 0)) {
            v1[0] = -static_cast<float>(m_grid_scale);
            v1[1] =
                    static_cast<float>(m_heights[x - 1][z] - m_heights[x][z]);
            v1[2] = 0.0f;

            v2[0] = 0.0f;
            v2[1] =
                    static_cast<float>(m_heights[x][z + 1] - m_heights[x][z]);
            v2[2] = static_cast<float>(m_grid_scale);
        } else {
            can_calculate = false;
        }
    }

    if (can_calculate) {
        normal->x = v1[1] * v2[2] - v1[2] * v2[1];
        normal->y = v1[2] * v2[0] - v1[0] * v2[2];
        normal->z = v1[0] * v2[1] - v1[1] * v2[0];
        float const mag = std::sqrt((normal->x * normal->x) +
                                    (normal->y * normal->y) +
                                    (normal->z * normal->z));
        normal->x /= mag;
        normal->y /= mag;
        normal->z /= mag;
    } else {
        normal->x = 0.0f;
        normal->y = 1.0f;
        normal->z = 0.0f;
    }
}

Math::Vec3<float> TerrainGenerator::normalAt(int x, int z) const {
    Math::Vec3<float> n0(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n1(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n2(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n3(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n4(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n5(0.0f, 0.0f, 0.0f);
    calcNormal(x, z - 1, 0, &n0);
    calcNormal(x, z, 0, &n1);
    calcNormal(x, z, 1, &n2);
    calcNormal(x + 1, z + 1, 1, &n3);
    calcNormal(x + 1, z, 1, &n4);
    calcNormal(x + 1, z, 0, &n5);

    Math::Vec3<float> normal(0.0f, 0.0f, 0.0f);
    normal.x = (n0.x + n1.x + n2.x + n3.x + n4.x + n5.x) / 6.0f;
    normal.y = (n0.y + n1.y + n2.y + n3.y + n4.y + n5.y) / 6.0f;
    normal.z = (n0.z + n1.z + n2.z + n3.z + n4.z + n5.z) / 6.0f;
    return normal;
}

}  // namespace vulkan_graphix
