// Exercises the gameplay math libvulkan_graphix shares between the Vulkan
// tutorials and vulkan_earth - TerrainGenerator's world queries and
// craters, Ballistics, and TankOrientation - directly. Pure math, no
// Vulkan device or X11 window, so this runs unconditionally (no DISPLAY
// check).

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <set>
#include <utility>

#include <gtest/gtest.h>

#include "vulkan_graphix/Ballistics.h"
#include "vulkan_graphix/TankOrientation.h"
#include "vulkan_graphix/TerrainGenerator.h"

namespace {

namespace vg = vulkan_graphix;

constexpr float c_epsilon = 1e-4f;
constexpr std::int32_t c_grid_size = 32;
constexpr std::int32_t c_grid_scale = 100;

vg::TerrainGenerator makeGeneratedTerrain() {
    vg::TerrainGenerator terrain(c_grid_size, c_grid_scale);
    srand(4321);
    terrain.generate(1500, 5, 6.0f, 10, 2);
    return terrain;
}

}  // namespace

TEST(GameLogicTest, NewTerrainIsFlatUntilGenerated) {
    vg::TerrainGenerator const terrain(c_grid_size, c_grid_scale);
    for (std::int32_t z = 0; z < c_grid_size; ++z) {
        for (std::int32_t x = 0; x < c_grid_size; ++x) {
            EXPECT_EQ(terrain.heightAt(x, z), 0);
        }
    }
    vg::Math::Vec3<float> const normal = terrain.triangleNormalAt(3, 3);
    EXPECT_NEAR(normal.y, 1.0f, c_epsilon);
}

TEST(GameLogicTest, GenerationIsDeterministicForASeed) {
    vg::TerrainGenerator const first = makeGeneratedTerrain();
    vg::TerrainGenerator const second = makeGeneratedTerrain();
    std::int32_t nonzero = 0;
    for (std::int32_t z = 0; z < c_grid_size; ++z) {
        for (std::int32_t x = 0; x < c_grid_size; ++x) {
            EXPECT_EQ(first.heightAt(x, z), second.heightAt(x, z));
            nonzero += first.heightAt(x, z) != 0 ? 1 : 0;
        }
    }
    EXPECT_GT(nonzero, 0);
    // Smoothing always zeroes the grid's edges.
    for (std::int32_t i = 0; i < c_grid_size; ++i) {
        EXPECT_EQ(first.heightAt(i, 0), 0);
        EXPECT_EQ(first.heightAt(0, i), 0);
        EXPECT_EQ(first.heightAt(i, c_grid_size - 1), 0);
        EXPECT_EQ(first.heightAt(c_grid_size - 1, i), 0);
    }
}

TEST(GameLogicTest, HeightAtWorldRoundsToTheNearestVertexInXThenZOrder) {
    vg::TerrainGenerator const terrain = makeGeneratedTerrain();
    // Just past a vertex rounds down; more than halfway rounds up.
    EXPECT_EQ(terrain.heightAtWorld(5 * c_grid_scale + 10.0f,
                                    9 * c_grid_scale + 10.0f),
              static_cast<float>(terrain.heightAt(5, 9)));
    EXPECT_EQ(terrain.heightAtWorld(5 * c_grid_scale + 60.0f,
                                    9 * c_grid_scale + 60.0f),
              static_cast<float>(terrain.heightAt(6, 10)));
    // Outside the grid reads as height 0.
    EXPECT_EQ(terrain.heightAtWorld(-1.0f, 50.0f), 0.0f);
    EXPECT_EQ(terrain.heightAtWorld(50.0f, c_grid_size * c_grid_scale), 0.0f);
}

TEST(GameLogicTest, NormalsAreUnitLengthAndFlatOutsideTheGrid) {
    vg::TerrainGenerator const terrain = makeGeneratedTerrain();
    for (std::int32_t z = 0; z < c_grid_size - 1; ++z) {
        for (std::int32_t x = 0; x < c_grid_size - 1; ++x) {
            vg::Math::Vec3<float> const triangle =
                    terrain.triangleNormalAt(x, z);
            EXPECT_NEAR(glm::length(triangle), 1.0f, c_epsilon);
            EXPECT_GE(triangle.y, 0.0f);  // always flipped to face up
        }
    }
    vg::Math::Vec3<float> const outside =
            terrain.triangleNormalAt(c_grid_size - 1, 0);
    EXPECT_EQ(outside, vg::Math::Vec3<float>(0.0f, 1.0f, 0.0f));
    vg::Math::Vec3<float> const world_outside =
            terrain.normalAtWorld(c_grid_size * c_grid_scale * 2.0f, 0.0f);
    EXPECT_EQ(world_outside, vg::Math::Vec3<float>(0.0f, 1.0f, 0.0f));
}

TEST(GameLogicTest, CraterLowersTheImpactAndReportsTheBlastCells) {
    vg::TerrainGenerator terrain(c_grid_size, c_grid_scale);
    float const blast_size = 3.0f;
    std::int32_t const impact_x = 16;
    std::int32_t const impact_z = 12;

    std::vector<vg::TerrainGridCell> const cells = terrain.makeCrater(
            impact_x * c_grid_scale, impact_z * c_grid_scale, blast_size);

    std::set<std::pair<std::int32_t, std::int32_t>> reported;
    for (vg::TerrainGridCell const& cell : cells) {
        reported.insert({cell.x, cell.z});
        float const distance = std::sqrt(
                static_cast<float>((cell.x - impact_x) * (cell.x - impact_x) +
                                   (cell.z - impact_z) * (cell.z - impact_z)));
        EXPECT_LE(distance, blast_size);
    }
    EXPECT_EQ(reported.size(), cells.size());  // no duplicates
    EXPECT_TRUE(reported.count({impact_x, impact_z}));
    EXPECT_LT(terrain.heightAt(impact_x, impact_z), 0);
    // Outside the blast radius is untouched.
    EXPECT_EQ(terrain.heightAt(impact_x + 5, impact_z), 0);
    // An impact off the grid does nothing.
    EXPECT_TRUE(terrain.makeCrater(-500.0f, 100.0f, blast_size).empty());
}

TEST(GameLogicTest, BallisticsLaunchesDownTheBarrelAndFallsUnderGravity) {
    // Barrel (column 2) along +Z, so shots travel toward -Z.
    vg::Math::Mat4<float> turret(1.0f);
    turret[3] = vg::Math::Vec4<float>(10.0f, 20.0f, 30.0f, 1.0f);

    vg::Math::Vec3<float> const probe =
            vg::Ballistics::pointAlongBarrel(turret, 5.0f);
    EXPECT_EQ(probe, vg::Math::Vec3<float>(10.0f, 20.0f, 25.0f));

    vg::Ballistics::Launch const launch =
            vg::Ballistics::launchFromBarrel(turret, 4.0f, 5.0f);
    EXPECT_EQ(launch.origin, probe);
    EXPECT_EQ(launch.velocity, vg::Math::Vec3<float>(0.0f, 0.0f, -4.0f));

    vg::Math::Vec3<float> const at_two =
            vg::Ballistics::positionAt(launch, -10.0f, 2.0f);
    EXPECT_NEAR(at_two.x, 10.0f, c_epsilon);
    EXPECT_NEAR(at_two.y, 20.0f - 20.0f, c_epsilon);  // 0.5 * -10 * 2^2
    EXPECT_NEAR(at_two.z, 25.0f - 8.0f, c_epsilon);
}

TEST(GameLogicTest, AngleBetweenDegrees) {
    using vg::Math::Vec3;
    EXPECT_NEAR(vg::TankOrientation::angleBetweenDegrees(Vec3<float>(1, 0, 0),
                                                         Vec3<float>(0, 1, 0)),
                90.0f,
                1e-3f);
    EXPECT_NEAR(vg::TankOrientation::angleBetweenDegrees(
                        Vec3<float>(2, 0, 0), Vec3<float>(-3, 0, 0)),
                180.0f,
                1e-3f);
    // A zero-length input stays unnormalized (dot = 0 -> 90 degrees), as
    // vulkan_earth's own normalizeVector() left it.
    EXPECT_NEAR(vg::TankOrientation::angleBetweenDegrees(Vec3<float>(0, 0, 0),
                                                         Vec3<float>(0, 1, 0)),
                90.0f,
                1e-3f);
}

TEST(GameLogicTest, AlignToGroundPointsTheTanksUpAlongTheNormal) {
    // vulkan_earth's tank basis: an x<->z axis swap.
    vg::Math::Mat4<float> body(vg::Math::Vec4<float>(0, 0, 1, 0),
                               vg::Math::Vec4<float>(0, 1, 0, 0),
                               vg::Math::Vec4<float>(1, 0, 0, 0),
                               vg::Math::Vec4<float>(5, 6, 7, 1));

    EXPECT_FALSE(vg::TankOrientation::alignToGround(
                         body, vg::Math::Vec3<float>(0, 1, 0))
                         .has_value());

    vg::Math::Vec3<float> const ground =
            glm::normalize(vg::Math::Vec3<float>(0.3f, 1.0f, -0.2f));
    std::optional<vg::TankOrientation::Alignment> const alignment =
            vg::TankOrientation::alignToGround(body, ground);
    ASSERT_TRUE(alignment.has_value());
    vg::Math::Vec3<float> const new_up(alignment->matrix[1]);
    EXPECT_NEAR(new_up.x, ground.x, 1e-3f);
    EXPECT_NEAR(new_up.y, ground.y, 1e-3f);
    EXPECT_NEAR(new_up.z, ground.z, 1e-3f);
    // Translation is untouched.
    EXPECT_EQ(vg::Math::Vec3<float>(alignment->matrix[3]),
              vg::Math::Vec3<float>(5, 6, 7));
}
