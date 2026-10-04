// Exercises the gameplay math libvulkan_graphix shares between the Vulkan
// tutorials and vulkan_earth - TerrainGenerator's world queries and
// craters, Ballistics, TankOrientation, the GameCatalog item/weapon data,
// and the EffectSimulation particle/explosion effects - directly. Pure
// math, no
// Vulkan device or X11 window, so this runs unconditionally (no DISPLAY
// check).

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <set>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "vulkan_graphix/Ballistics.h"
#include "vulkan_graphix/EffectSimulation.h"
#include "vulkan_graphix/GameCatalog.h"
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
    const vg::TerrainGenerator terrain(c_grid_size, c_grid_scale);
    for (std::int32_t z = 0; z < c_grid_size; ++z) {
        for (std::int32_t x = 0; x < c_grid_size; ++x) {
            EXPECT_EQ(terrain.heightAt(x, z), 0);
        }
    }
    const vg::Math::Vec3<float> normal = terrain.triangleNormalAt(3, 3);
    EXPECT_NEAR(normal.y, 1.0f, c_epsilon);
}

TEST(GameLogicTest, GenerationIsDeterministicForASeed) {
    const vg::TerrainGenerator first = makeGeneratedTerrain();
    const vg::TerrainGenerator second = makeGeneratedTerrain();
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
    const vg::TerrainGenerator terrain = makeGeneratedTerrain();
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
    const vg::TerrainGenerator terrain = makeGeneratedTerrain();
    for (std::int32_t z = 0; z < c_grid_size - 1; ++z) {
        for (std::int32_t x = 0; x < c_grid_size - 1; ++x) {
            const vg::Math::Vec3<float> triangle =
                    terrain.triangleNormalAt(x, z);
            EXPECT_NEAR(glm::length(triangle), 1.0f, c_epsilon);
            EXPECT_GE(triangle.y, 0.0f);  // always flipped to face up
        }
    }
    const vg::Math::Vec3<float> outside =
            terrain.triangleNormalAt(c_grid_size - 1, 0);
    EXPECT_EQ(outside, vg::Math::Vec3<float>(0.0f, 1.0f, 0.0f));
    const vg::Math::Vec3<float> world_outside =
            terrain.normalAtWorld(c_grid_size * c_grid_scale * 2.0f, 0.0f);
    EXPECT_EQ(world_outside, vg::Math::Vec3<float>(0.0f, 1.0f, 0.0f));
}

TEST(GameLogicTest, CraterLowersTheImpactAndReportsTheBlastCells) {
    vg::TerrainGenerator terrain(c_grid_size, c_grid_scale);
    const float blast_size = 3.0f;
    const std::int32_t impact_x = 16;
    const std::int32_t impact_z = 12;

    const std::vector<vg::TerrainGridCell> cells = terrain.makeCrater(
            impact_x * c_grid_scale, impact_z * c_grid_scale, blast_size);

    std::set<std::pair<std::int32_t, std::int32_t>> reported;
    for (const vg::TerrainGridCell& cell : cells) {
        reported.insert({cell.x, cell.z});
        const float distance = std::sqrt(
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

    const vg::Math::Vec3<float> probe =
            vg::Ballistics::pointAlongBarrel(turret, 5.0f);
    EXPECT_EQ(probe, vg::Math::Vec3<float>(10.0f, 20.0f, 25.0f));

    const vg::Ballistics::Launch launch =
            vg::Ballistics::launchFromBarrel(turret, 4.0f, 5.0f);
    EXPECT_EQ(launch.origin, probe);
    EXPECT_EQ(launch.velocity, vg::Math::Vec3<float>(0.0f, 0.0f, -4.0f));

    const vg::Math::Vec3<float> at_two =
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

    const vg::Math::Vec3<float> ground =
            glm::normalize(vg::Math::Vec3<float>(0.3f, 1.0f, -0.2f));
    const std::optional<vg::TankOrientation::Alignment> alignment =
            vg::TankOrientation::alignToGround(body, ground);
    ASSERT_TRUE(alignment.has_value());
    const vg::Math::Vec3<float> new_up(alignment->matrix[1]);
    EXPECT_NEAR(new_up.x, ground.x, 1e-3f);
    EXPECT_NEAR(new_up.y, ground.y, 1e-3f);
    EXPECT_NEAR(new_up.z, ground.z, 1e-3f);
    // Translation is untouched.
    EXPECT_EQ(vg::Math::Vec3<float>(alignment->matrix[3]),
              vg::Math::Vec3<float>(5, 6, 7));
}

TEST(GameLogicTest, CatalogHasEveryShopItemAndWeaponInShopOrder) {
    namespace catalog = vg::GameCatalog;
    EXPECT_EQ(
            std::string(
                    catalog::item(catalog::ItemKind::SmallRepair).image_file),
            "ItemSmallRepair.raw");
    EXPECT_EQ(catalog::item(catalog::ItemKind::BigRepair).special_num, 700);
    EXPECT_EQ(std::string(catalog::item(catalog::ItemKind::Float).image_file),
              "ItemFloat.raw");
    EXPECT_EQ(catalog::weapon(catalog::WeaponKind::MFB).damage, 300);
    EXPECT_EQ(catalog::weapon(catalog::WeaponKind::Nuke).price, 500);
    EXPECT_EQ(
            std::string(
                    catalog::weapon(catalog::WeaponKind::Default).image_file),
            "TestImage.raw");
    // Every entry is filled in.
    for (const catalog::ItemSpec& item : catalog::items()) {
        EXPECT_NE(item.image_file, nullptr);
        EXPECT_GT(item.price, 0);
        EXPECT_GE(item.max_stack, item.remaining);
    }
    for (const catalog::WeaponSpec& weapon : catalog::weapons()) {
        EXPECT_NE(weapon.description, nullptr);
        EXPECT_GT(weapon.scale, 0.0f);
        EXPECT_GE(weapon.max_stack, weapon.remaining);
    }
}

TEST(GameLogicTest, CatalogKeepsThorsTwoComponentMediumSlateBlue) {
    namespace catalog = vg::GameCatalog;
    // OpenGLColors.h's MediumSlateBlue has no blue component.
    const auto& color =
            catalog::weapon(catalog::WeaponKind::Thor).explosion_colors[1];
    EXPECT_DOUBLE_EQ(color[0], 0.498039);
    EXPECT_DOUBLE_EQ(color[1], 1.0);
    EXPECT_DOUBLE_EQ(color[2], 0.0);
}

TEST(GameLogicTest, SmokeParticleRisesAndFadesThroughItsColorBands) {
    namespace effects = vg::EffectSimulation;
    effects::Particle particle = effects::makeParticle(
            effects::ParticleKind::Smoke, 0, 0, 0, 1, 0, 0, 1, 100);
    EXPECT_EQ(particle.size, 2.0f);
    float previous_y = particle.y;
    for (std::int32_t frame = 0; frame < 31; ++frame) {
        ASSERT_TRUE(effects::updateParticle(particle));
    }
    EXPECT_GT(particle.y, previous_y);
    EXPECT_NEAR(particle.x, 31.0f, c_epsilon);
    EXPECT_NEAR(particle.blue, 0.0f, c_epsilon);
    EXPECT_EQ(particle.green, 1.0f);
    for (std::int32_t frame = 31; frame < 61; ++frame) {
        ASSERT_TRUE(effects::updateParticle(particle));
    }
    EXPECT_NEAR(particle.green, 0.0f, c_epsilon);
    ASSERT_TRUE(effects::updateParticle(particle));
    // Frame 61: (61 - 30) / 40 - the game's own formula.
    EXPECT_NEAR(particle.red, 1.0f - 31.0f / 40.0f, c_epsilon);
    previous_y = particle.y;
    for (std::int32_t frame = 62; frame < 99; ++frame) {
        ASSERT_TRUE(effects::updateParticle(particle));
    }
    EXPECT_FALSE(effects::updateParticle(particle));
    EXPECT_GT(particle.y, previous_y);
}

TEST(GameLogicTest, FloatParticleDampsItsVerticalMotion) {
    namespace effects = vg::EffectSimulation;
    effects::Particle particle = effects::makeParticle(
            effects::ParticleKind::Float, 0, 0, 0, 0, 1, 0, 2, 10);
    EXPECT_TRUE(effects::updateParticle(particle));
    EXPECT_NEAR(particle.y, 0.2f, c_epsilon);
    EXPECT_EQ(particle.size, 4.0f);
}

TEST(GameLogicTest, EmitterSpawnsEachUpdateAndRecyclesFinishedParticles) {
    namespace effects = vg::EffectSimulation;
    effects::ParticleEmitter emitter(10, 5, 2, 3, effects::ParticleKind::Acid);
    auto live = [&emitter] {
        std::size_t count = 0;
        for (const auto& slot : emitter.slots()) {
            count += slot.has_value() ? 1 : 0;
        }
        return count;
    };
    srand(99);
    emitter.update(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(live(), 10U);
    for (const auto& slot : emitter.slots()) {
        if (slot) {
            EXPECT_EQ(slot->x, 1.0f);
            const float length = std::sqrt(slot->dir[0] * slot->dir[0] +
                                           slot->dir[1] * slot->dir[1] +
                                           slot->dir[2] * slot->dir[2]);
            EXPECT_NEAR(length, 1.0f, c_epsilon);
        }
    }
    emitter.update(0, 0, 0);
    emitter.update(0, 0, 0);
    EXPECT_EQ(live(), 30U);
    // The first ten have lived their three frames; their slots refill.
    emitter.update(0, 0, 0);
    EXPECT_EQ(live(), 30U);
    emitter.clear();
    EXPECT_EQ(live(), 0U);
}

TEST(GameLogicTest, ExplosionStepsThroughItsColorsAndLeavesTheGap) {
    namespace effects = vg::EffectSimulation;
    effects::Explosion explosion;
    // Starts at timer 50: the third color band.
    effects::ExplosionFrame frame = effects::advanceExplosion(explosion);
    EXPECT_EQ(frame.color_index, 2);
    EXPECT_NEAR(frame.alpha, 1.0f - 50.5f / 150.0f, c_epsilon);
    while (explosion.timer < 75.0f) {
        frame = effects::advanceExplosion(explosion);
    }
    EXPECT_EQ(frame.color_index, -1);
    while (explosion.timer < 100.0f) {
        frame = effects::advanceExplosion(explosion);
    }
    EXPECT_EQ(frame.color_index, 3);
    EXPECT_NEAR(effects::explosionSphereRadius(explosion, 30),
                explosion.radius * (30 * 5.56f + 22.22f),
                1e-2f);
}
