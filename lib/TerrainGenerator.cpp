#include "vulkan_graphix/TerrainGenerator.h"

#include <array>
#include <cmath>
#include <cstdlib>

namespace vulkan_graphix {

TerrainGenerator::TerrainGenerator(int grid_size, int grid_scale)
        : m_grid_size(grid_size)
        , m_grid_scale(grid_scale)
        , m_heights(grid_size, std::vector<int>(grid_size, 0)) {}

int TerrainGenerator::gridSize() const { return m_grid_size; }
int TerrainGenerator::gridScale() const { return m_grid_scale; }

int TerrainGenerator::heightAt(int x, int z) const { return m_heights[z][x]; }

bool TerrainGenerator::inGrid(int x, int z) const {
    return x >= 0 && x < m_grid_size && z >= 0 && z < m_grid_size;
}

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
                if ((distance < radius) && ((x >= 0 && x < m_grid_size) &&
                                            (y >= 0 && y < m_grid_size))) {
                    m_heights[x][y] += increase;
                }
            }
        }
    }
}

void TerrainGenerator::terrainSmoothe(int box_width) {
    // In place, exactly as TerrainMaker::terrainSmoothe() does it: cells
    // later in the pass average over neighbors already smoothed earlier in
    // this same pass.
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
            m_heights[y][x] = height_sum / (box_width * box_width);
        }
    }

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

// TerrainMaker::calcNormal() indexed its grid th[x][z] here, transposed
// from the th[z][x] its own vertices were built from - so every vertex was
// lit with the normal of the terrain mirrored across its diagonal. This
// reads m_heights[z][x], the same orientation as heightAt().
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
            v1[1] = static_cast<float>(m_heights[z - 1][x - 1] -
                                       m_heights[z][x]);
            v1[2] = -static_cast<float>(m_grid_scale);

            v2[0] = -static_cast<float>(m_grid_scale);
            v2[1] = static_cast<float>(m_heights[z][x - 1] - m_heights[z][x]);
            v2[2] = 0.0f;
        } else {
            can_calculate = false;
        }
    } else {
        if ((((x - 1) >= 0) && (x < m_grid_size)) && ((z + 1) < m_grid_size) &&
            (z > 0)) {
            v1[0] = -static_cast<float>(m_grid_scale);
            v1[1] = static_cast<float>(m_heights[z][x - 1] - m_heights[z][x]);
            v1[2] = 0.0f;

            v2[0] = 0.0f;
            v2[1] = static_cast<float>(m_heights[z + 1][x] - m_heights[z][x]);
            v2[2] = static_cast<float>(m_grid_scale);
        } else {
            can_calculate = false;
        }
    }

    if (can_calculate) {
        normal->x = v1[1] * v2[2] - v1[2] * v2[1];
        normal->y = v1[2] * v2[0] - v1[0] * v2[2];
        normal->z = v1[0] * v2[1] - v1[1] * v2[0];
        float const mag =
                std::sqrt((normal->x * normal->x) + (normal->y * normal->y) +
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

// TerrainMaker::getHeightAt() returned th[v_x][v_z] - transposed - and
// every one of its callers compensated by passing (z, x). This returns the
// untransposed m_heights[v_z][v_x], so callers pass (x, z).
float TerrainGenerator::heightAtWorld(float world_x, float world_z) const {
    float const scale = static_cast<float>(m_grid_scale);
    if ((world_x >= 0 && world_x < (m_grid_size - 1) * scale) &&
        (world_z >= 0 && world_z < (m_grid_size - 1) * scale)) {
        int v_x = static_cast<int>(world_x / scale);
        int v_z = static_cast<int>(world_z / scale);
        float const d_x = world_x / scale - static_cast<float>(v_x);
        float const d_z = world_z / scale - static_cast<float>(v_z);

        if (d_x < 0) {
            if (std::fabs(d_x) < 0.5f) {
                v_x++;
            }
        } else {
            if (std::fabs(d_x) > 0.5f) {
                v_x++;
            }
        }
        if (d_z < 0) {
            if (std::fabs(d_z) < 0.5f) {
                v_z++;
            }
        } else {
            if (std::fabs(d_z) > 0.5f) {
                v_z++;
            }
        }
        return static_cast<float>(m_heights[v_z][v_x]);
    }
    return 0.0f;
}

Math::Vec3<float> TerrainGenerator::normalAtWorld(float world_x,
                                                  float world_z) const {
    float const scale = static_cast<float>(m_grid_scale);
    int n_x = static_cast<int>(world_x / scale);
    int n_z = static_cast<int>(world_z / scale);
    float const d_x = world_x / scale - static_cast<float>(n_x);
    float const d_z = world_z / scale - static_cast<float>(n_z);

    if (d_x > 0) {
        if (std::fabs(d_x) > 0.5f) {
            n_x++;
        }
    } else {
        if (std::fabs(d_x) < 0.5f) {
            n_x++;
        }
    }
    if (d_z > 0) {
        if (std::fabs(d_z) > 0.5f) {
            n_z++;
        }
    } else {
        if (std::fabs(d_z) < 0.5f) {
            n_z++;
        }
    }

    // TerrainMaker::getNormalAt() read th[n_z][n_x + 1]/th[n_z + 1][n_x]
    // unchecked, past the grid's edge for an out-of-range position.
    if (!inGrid(n_x, n_z) || !inGrid(n_x + 1, n_z + 1)) {
        return Math::Vec3<float>(0.0f, 1.0f, 0.0f);
    }

    std::array<float, 3> const v = {
            0.0f,
            static_cast<float>(m_heights[n_z][n_x + 1] - m_heights[n_z][n_x]),
            scale};
    std::array<float, 3> const u = {
            scale,
            static_cast<float>(m_heights[n_z + 1][n_x] - m_heights[n_z][n_x]),
            0.0f};

    Math::Vec3<float> normal((v[1] * u[2] - u[1] * v[2]),
                             (u[0] * v[2] - v[0] * u[2]),
                             (v[0] * u[1] - u[0] * v[1]));
    float const mag = std::sqrt(normal.x * normal.x + normal.y * normal.y +
                                normal.z * normal.z);
    normal.x /= mag;
    normal.y /= mag;
    normal.z /= mag;
    return normal;
}

// TerrainMaker::getTriangleNormal()/collectVerticesForTriangleNormal()
// printed "Tank out of bounds" and then normalized a zero vector (NaN)
// outside the grid, and its bounds check let (x + 1, z + 1) read one past
// the grid's edge; both now return a flat (0, 1, 0) instead.
Math::Vec3<float> TerrainGenerator::triangleNormalAt(int x, int z) const {
    if (!inGrid(x, z) || !inGrid(x + 1, z + 1)) {
        return Math::Vec3<float>(0.0f, 1.0f, 0.0f);
    }

    Math::Vec3<float> const v0(static_cast<float>(x),
                               static_cast<float>(m_heights[z][x]),
                               static_cast<float>(z));
    Math::Vec3<float> const v1(static_cast<float>(x + 1),
                               static_cast<float>(m_heights[z][x + 1]),
                               static_cast<float>(z));
    Math::Vec3<float> const v2(static_cast<float>(x),
                               static_cast<float>(m_heights[z + 1][x]),
                               static_cast<float>(z + 1));

    std::array<float, 3> const u = {
            (v1.x - v0.x), (v1.y / 100 - v0.y / 100), (v1.z - v0.z)};
    std::array<float, 3> const v = {
            (v2.x - v0.x), (v2.y / 100 - v0.y / 100), (v2.z - v0.z)};

    Math::Vec3<float> normal((u[1] * v[2] - v[1] * u[2]),
                             (u[2] * v[0] - u[0] * v[2]),
                             (u[0] * v[1] - v[0] * u[1]));

    float const mag = static_cast<float>(
            std::sqrt(std::pow(static_cast<double>(normal.x), 2.0) +
                      std::pow(static_cast<double>(normal.y), 2.0) +
                      std::pow(static_cast<double>(normal.z), 2.0)));
    normal.x /= mag;
    normal.y /= mag;
    normal.z /= mag;
    if (normal.y < 0) {
        normal.x *= -1;
        normal.y *= -1;
        normal.z *= -1;
    }
    return normal;
}

std::vector<TerrainGridCell> TerrainGenerator::makeCrater(float impact_x,
                                                          float impact_z,
                                                          float blast_size) {
    std::vector<TerrainGridCell> affected;
    float const scale = static_cast<float>(m_grid_scale);
    int const x = static_cast<int>(impact_x / scale);
    int const z = static_cast<int>(impact_z / scale);
    int const crater_size = static_cast<int>(blast_size * 1.5);

    if (!inGrid(x, z)) {
        return affected;
    }

    float const impact_y = static_cast<float>(m_heights[z][x]);
    for (int i = x - crater_size; i < x + crater_size; i++) {
        for (int j = z - crater_size; j < z + crater_size; j++) {
            float const distance = std::sqrt(
                    static_cast<float>((x - i) * (x - i) + (z - j) * (z - j)));
            if (!inGrid(i, j) || distance > blast_size) {
                continue;
            }
            float const x_dis = static_cast<float>(std::abs(i - x));
            float const z_dis = static_cast<float>(std::abs(j - z));
            float const dist_radius = std::sqrt(x_dis * x_dis + z_dis * z_dis);

            float const damage_depth =
                    -((std::sqrt(dist_radius * dist_radius + x_dis * x_dis +
                                 z_dis * z_dis) -
                       blast_size * 2) *
                      scale / 2);
            float adjust_height = static_cast<float>(m_heights[j][i]);
            if (adjust_height > (impact_y + damage_depth)) {
                adjust_height -= damage_depth;
            } else if (adjust_height > (impact_y - damage_depth)) {
                adjust_height = impact_y - damage_depth;
            }
            m_heights[j][i] = static_cast<int>(adjust_height);
            if (i == x && j == z && j > 1 && i > 1) {
                adjust_height = static_cast<float>((m_heights[j - 1][i] +
                                                    m_heights[j][i - 1] +
                                                    m_heights[j][i]) /
                                                   3);
            }
            m_heights[j][i] = static_cast<int>(adjust_height);
            affected.push_back({i, j});
        }
    }
    return affected;
}

}  // namespace vulkan_graphix
