#include "vulkan_earth/GameState.h"
#include <cstdint>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include "math.h"
#include "time.h"
#include "vulkan_earth/ChaseCam.h"
#include "vulkan_earth/Explosion.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/GlobalSettings.h"
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/Inventory.h"
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Player.h"
#include "vulkan_earth/PlayerFactory.h"
#include "vulkan_earth/PossibleGameStates.h"
#include "vulkan_earth/Projectile.h"
#include "vulkan_earth/SkyboxFactory.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/SpecialEffect.h"
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/Water.h"
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/WorldCam.h"
#include "vulkan_graphix/Ballistics.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Tools.h"
#include "vulkan_earth/MacroCrtdbg.h"

/* Later, when a round is finished, make sure all human and/or cpu players must
 * unload their weapons. Call player(i)->setLoadedWeapon(NULL)*/
/* For the same reason, initialize all the durations of status of tanks*/

extern void playSFX(std::int32_t sfx);
extern void playMusic(std::int32_t music);

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

namespace {
// The HUD sizes itself by GLUT_SCREEN_WIDTH and GLUT_SCREEN_HEIGHT - the
// glutGet() query enums themselves (0x00C8 and 0x00C9), never passed to
// glutGet() - so its layout was really built on a 200 x 201 "screen".
// Kept as-is so the HUD looks exactly as it did.
constexpr std::int32_t c_glut_screen_width = 0x00C8;
constexpr std::int32_t c_glut_screen_height = 0x00C9;
}  // namespace

GameState::GameState() = default;

GameState::GameState(std::int32_t new_width,
                     std::int32_t new_height,
                     PlayerFactory* new_player_factory,
                     GlobalSettings* new_global_settings,
                     std::int32_t* new_current_game_state) {
    scale_gravity = 30;
    balistic_scalar = 50;
    gravity = -9.8 * scale_gravity;

    timer = 0;

    current_game_state = new_current_game_state;
    need_help = false;
    start_music_played = false;
    prev_music_volume = 0;
    special_effect_timer = 0;
    radius_of_current_explosion = 0;
    special_effect_type = 0;
    special_effects = nullptr;
    draw_hit_box = false;

    for (std::int32_t x = 0; x < 256; x++) {
        key_monitor[x] = 0;
    }

    selected_weapon_img = nullptr;
    selected_weapon_remain = nullptr;
    weapon_slot = new ImageObject(0,
                                  0,
                                  2,
                                  new_width * 0.08,
                                  new_height * 0.11,
                                  0,
                                  1024,
                                  1024,
                                  "TestImage.raw");
    srand(time(nullptr));
    player_factory = new_player_factory;
    global_settings = new_global_settings;
    width = new_width;
    height = new_height;
    player_cam = false;
    chase_cam_active = false;
    projectile = nullptr;
    world_cam = new WorldCam(
            0,
            20000,
            global_settings->getCurrentTerrain()->getActualSize() / 2);

    game_sub_state = PASS_TIME;
    current_player_index = 0;
    current_player = new_player_factory->getPlayer(current_player_index);

    // PLACE TANKS
    for (std::int32_t i = 0; i < new_global_settings->getPlayerCount(); i++) {
        float x, y, z;
        std::int32_t size = static_cast<std::int32_t>(
                new_global_settings->getCurrentTerrain()->getActualSize());
        std::int32_t scale = static_cast<std::int32_t>(
                new_global_settings->getCurrentTerrain()->getScale());

        x = 5 + rand() % ((size / scale) - 10);
        z = 5 + rand() % ((size / scale) - 10);

        vulkan_graphix::Math::Vec3<float> n =
                new_global_settings->getCurrentTerrain()->getTriangleNormal(x,
                                                                            z);

        if (new_player_factory->getPlayer(i)->getCurrentTank()) {
            new_player_factory->getPlayer(i)->getCurrentTank()->orientTank(&n);
        } else {
            cout << "I lost a tank, HOW???" << endl;
        }

        // delete n;

        /*	FINALLY POSITION TANKS	*/
        y = new_global_settings->getCurrentTerrain()->getHeightAt(x * scale,
                                                                  z * scale);
        new_player_factory->getPlayer(i)->getCurrentTank()->setTankPos(
                x * scale, y, z * scale);
        number_of_players = new_global_settings->getPlayerCount();
    }

    //*
    if (global_settings) {
    } else if (!new_global_settings) {
        printf("Quiting because Global Settings Does Not Exist\n");
        exit(0);
    } else if (!(new_global_settings->getCurrentTerrain())) {
        printf("The terrain does not exist\n");
        exit(0);
    } else {
    }
    //*/

    /************************************************************************************/
    /*	NOTE TERRAIN IS 256 X 256 WITH SCALE OF 100 --> TERRAIN IS OF SIZE
     * 655360000	*/
    /*	MID POINT OF THE TERRAIN WOULD RESULT IN OPENGL COORD (12800,12800) AT
     * MIDDLE	*/
    /*	OF TERRAIN. THUS IF YOU START AT (-9000,-9000) THE RADIUS BEING
     * 30829.8556 		*/
    /*	FROM YOUR START LOCATION TO THE CENTER OF THE TERRAIN.
     */
    /************************************************************************************/

    offset = global_settings->getCurrentTerrain()->getActualSize() / 2.0;
    camera_x = 4000;
    camera_y = 15000;
    camera_z = 4000;
    //
    camera_radius = sqrt((pow(static_cast<double>(camera_x - offset), 2.0)) +
                         (pow(static_cast<double>(camera_y), 2.0)) +
                         (pow(static_cast<double>(camera_z - offset), 2.0)));
    plane_radius = sqrt((pow(static_cast<double>(camera_x - offset), 2.0)) +
                        (pow(static_cast<double>(camera_z - offset), 2.0)));
    current_tank_theta = 0.0;
    current_world_theta = 0.0;
    current_tank_phi =
            45.0;  // NOTE THIS NEEDS TO BE UPDATED TO FIRST PLAYERS ANGLE

    skybox_factory = new SkyboxFactory(2048);
    ocean = new Water(400, 512);
    sfx_random = 0;
    projectile_fired = false;

    inventory = new Inventory(new_width * 0.25,
                              new_height * 0.25,
                              new_width * 0.5,
                              new_height * 0.5);

    // Creating models for projectile.
    // IMPORTANT: Be careful about the order. It should match with the order in
    // that the weapons are created in the ShopMenu constructor
    for (std::int32_t i = 0; i < max_projectile_models; i++) {
        projectile_models[i] = new VBOShaderLibrary();
    }

    projectile_models[10]->loadClientData("Projectiles/projectileDefault.ogl");
    projectile_models[10]->loadTexture(
            "Projectiles/projectileDefault.raw", 512, 512);

    projectile_models[0]->loadClientData("Projectiles/projectileBFB.ogl");
    projectile_models[0]->loadTexture(
            "Projectiles/projectileMFB.raw", 512, 512);

    projectile_models[1]->loadClientData("Projectiles/projectileBFB.ogl");
    projectile_models[1]->loadTexture(
            "Projectiles/projectileBFB.raw", 512, 512);

    projectile_models[2]->loadClientData("Projectiles/projectileAcid.ogl");
    projectile_models[2]->loadTexture(
            "Projectiles/projectileAcid.raw", 512, 512);

    projectile_models[3]->loadClientData("Projectiles/projectileThor.ogl");
    projectile_models[3]->loadTexture(
            "Projectiles/projectileThor.raw", 512, 512);

    projectile_models[4]->loadClientData("Projectiles/projectileEMP.ogl");
    projectile_models[4]->loadTexture(
            "Projectiles/projectileEMP.raw", 512, 512);

    projectile_models[5]->loadClientData("Projectiles/projectileDefault.ogl");
    projectile_models[5]->loadTexture(
            "Projectiles/projectilePadlock.raw", 512, 512);

    projectile_models[6]->loadClientData("Projectiles/projectileDefault.ogl");
    projectile_models[6]->loadTexture(
            "Projectiles/projectileRevive.raw", 512, 512);

    projectile_models[7]->loadClientData("Projectiles/projectileDefault.ogl");
    projectile_models[7]->loadTexture(
            "Projectiles/projectileTeleport.raw", 512, 512);

    projectile_models[8]->loadClientData("Projectiles/projectileDefault.ogl");
    projectile_models[8]->loadTexture(
            "Projectiles/projectileAtom.raw", 512, 512);

    projectile_models[9]->loadClientData("Projectiles/projectileNuke.ogl");
    projectile_models[9]->loadTexture(
            "Projectiles/projectileNuke.raw", 512, 512);

    manual = new ImageObject(new_width * -0.175,
                             new_height * 0.25,
                             2,
                             new_width * 0.35,
                             new_height * 0.55,
                             new_width * 0.004,
                             1024,
                             1024,
                             "manual.raw");

    /* AI VARIABLES	*/
    tank_reachable = new bool*[player_factory->getNumberofPlayers()];
    tank_list = new Tank**[player_factory->getNumberofPlayers()];
    distance_to_target = new float*[player_factory->getNumberofPlayers()];
    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        tank_list[i] = new Tank*[player_factory->getNumberofPlayers()];
        distance_to_target[i] =
                new float[player_factory->getNumberofPlayers()];
        tank_reachable[i] = new bool[player_factory->getNumberofPlayers()];
        for (std::int32_t j = 0; j < player_factory->getNumberofPlayers();
             j++) {
            tank_list[i][j] = player_factory->getPlayer(j)->getCurrentTank();
            distance_to_target[i][j] = 1E+37;  // MAX FLOAT
            tank_reachable[i][j] = true;
        }
    }
    cout << "Done Building Table" << endl;
    /* END OF AI VARIABLES */

    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        player_factory->getPlayer(i)->setLoadedWeapon(nullptr);
        player_factory->getPlayer(i)->getCurrentTank()->initDuration();
        player_factory->getPlayer(i)->getCurrentTank()->updateHitBox();
    }

    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        player_factory->getPlayer(i)->setGameState(this);
    }
}

GameState::~GameState() {
    Mix_HaltMusic();
    delete ocean;
    delete skybox_factory;
    delete world_cam;
    delete inventory;
    delete selected_weapon_img;
    delete selected_weapon_remain;
    delete weapon_slot;
    for (std::int32_t x = 0; x < max_projectile_models; x++) {
        delete projectile_models[x];
    }
    delete manual;
    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        delete[] tank_reachable[i];
    }
    delete[] tank_reachable;
    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        delete[] tank_list[i];
    }
    delete[] tank_list;
    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        delete[] distance_to_target[i];
    }
    delete[] distance_to_target;
}

float GameState::calcDistanceBetweenVertices(
        vulkan_graphix::Math::Vec3<float>* v0,
        vulkan_graphix::Math::Vec3<float>* v1) {
    return static_cast<float>(
            sqrt(pow((static_cast<double>(v0->x) - static_cast<double>(v1->x)),
                     2.0) +
                 pow((static_cast<double>(v0->z) - static_cast<double>(v1->z)),
                     2.0)));
}

void GameState::timerEvent(float new_timer) {}

void GameState::calcNormalVector(vulkan_graphix::Math::Vec3<float>* v0,
                                 vulkan_graphix::Math::Vec3<float>* v1,
                                 vulkan_graphix::Math::Vec3<float>* v2,
                                 vulkan_graphix::Math::Vec3<float>* n) {
    float u[3] = {
            (v1->x - v0->x), (v1->y / 100 - v0->y / 100), (v1->z - v0->z)};
    float v[3] = {
            (v2->x - v0->x), (v2->y / 100 - v0->y / 100), (v2->z - v0->z)};

    n->x = u[1] * v[2] - v[1] * u[2];
    n->y = u[2] * v[0] - u[0] * v[2];
    n->z = u[0] * v[1] - v[0] * u[1];

    float mag = static_cast<float>(sqrt(pow(static_cast<double>(n->x), 2.0) +
                                        pow(static_cast<double>(n->y), 2.0) +
                                        pow(static_cast<double>(n->z), 2.0)));
    if (mag != 0) {
        n->x /= mag;
        n->y /= mag;
        n->z /= mag;
    }
}

void GameState::update() {
    // Game logic
    if (game_sub_state == PLAYER_CONTROL) {
        // QUICK FIX FOR TURRET

        if (player_factory->getPlayer(current_player_index)->getCurrentWait() >
            0) {
            game_sub_state = PASS_TIME;
        }

        if (current_player->getPlayerType() == "CPU") {
            handlePlayerControlUpdates();
            if (current_player->getCurrentTank()->isAlive()) {
                controlAI();
            } else {
                cout << "Tank " << current_player_index << " is dead" << endl;
            }
            updateWorldCam();
        } else if (current_player->getPlayerType() == "HUMAN") {
            handlePlayerControlUpdates();
            updateWorldCam();
        }
    }
    // else
    if (game_sub_state == PASS_TIME) {
        handlePassTime();
    }

    // projectile update/collision/special effect
    if (game_sub_state == PROJECTILE) {
        handleProjectileState();
        updateWorldCam();
    }
    if (game_sub_state == SPECIAL_EFFECT) {
        handleSpecialEffectState();
    }
}

void GameState::draw(render::RenderContext& context) {
    playBackgroundSounds();
    update();

    // This frame's camera: the gameplay projection the caller set, and
    // whichever view gluLookAt() used to multiply into the modelview.
    math::Mat4<float> view = context.view();
    if (game_sub_state == PROJECTILE || game_sub_state == SPECIAL_EFFECT) {
        if (chase_cam_active) view = view * projectile->chaseView();
    }
    if (!chase_cam_active) {
        if (!player_cam) {
            view = view * world_cam->view();
        } else {
            const float* turret_matrix =
                    current_player->getCurrentTank()->getTurretMatrix();
            const float* head_matrix =
                    current_player->getCurrentTank()->getHeadMatrix();
            view = view *
                   glm::lookAt(
                           math::Vec3<float>(
                                   head_matrix[12] + head_matrix[8] * 4000,
                                   head_matrix[13] + 2000,
                                   head_matrix[14] + head_matrix[10] * 4000),
                           math::Vec3<float>(
                                   head_matrix[12] - head_matrix[8] * 3000,
                                   head_matrix[13] + head_matrix[9] * 0,
                                   head_matrix[14] - head_matrix[10] * 3000),
                           math::Vec3<float>(0, 1, 0));
        }
    }
    context.setCamera(context.projection(), view);

    global_settings->getCurrentTerrain()->draw(context);
    skybox_factory->draw(context);

    if (current_player->getPlayerType() == "CPU") {
        if (current_player->getDrawDebugLinesandPlanes()) {
            current_player->drawTestLinesandPlanes(context);
        }
    }

    if (game_sub_state != SPECIAL_EFFECT) {
        if (projectile) {
            projectile->draw(context);
        }
    }

    // The ocean alone is translated (glPushMatrix()/glPopMatrix()).
    context.setCamera(context.projection(),
                      glm::translate(view,
                                     math::Vec3<float>(-256.00f * 400,
                                                       -5000.00f,
                                                       -256.00f * 400)));
    ocean->draw(context);
    context.setCamera(context.projection(), view);

    // DRAW TANKS (the original set the current player's color here first,
    // but the tank shader never read it)
    for (std::int32_t i = 0; i < global_settings->getPlayerCount(); i++) {
        if (player_factory->getPlayer(i)
                    ->getCurrentTank()
                    ->getDurationCloak() == 0) {
            player_factory->getPlayer(i)->getCurrentTank()->draw(context);
            if (draw_hit_box) {
                player_factory->getPlayer(i)->getCurrentTank()->drawTankHitBox(
                        context);
            }
        }
    }

    // DRAW EXPLOSION/SPECIAL EFFECT
    // specialEffectsCount =
    // (int)(SPECIAL_EFFECT_TIME_LIMIT/TIME_DIVISORS);
    if (game_sub_state == SPECIAL_EFFECT) {
        if (special_effect_type == EXPLOSION) {
            for (std::int32_t x = 0; x < special_effects_count; x++) {
                if (special_effect_timer > 0) {
                    special_effects[0]->draw(context);
                }
                if (special_effect_timer > 10) {
                    special_effects[1]->draw(context);
                }
                if (special_effect_timer > 20) {
                    special_effects[2]->draw(context);
                }
            }
        }
    }

    if (game_sub_state == PLAYER_CONTROL) {
        if (current_player->getPlayerType() == "CPU") {
            const float* turret_matrix =
                    current_player->getCurrentTank()->getTurretMatrix();
            const float* body_matrix =
                    current_player->getCurrentTank()->getBodyMatrix();
            // No color of its own: the original drew this in whatever GL's
            // current color was, which each tank mesh's draw call had just
            // set to red.
            const math::Vec4<float> color(1.0f, 0.0f, 0.0f, 1.0f);
            const std::vector<render::UiVertex> line = {
                    {math::Vec3<float>(turret_matrix[12],
                                       turret_matrix[13],
                                       turret_matrix[14]),
                     color,
                     math::Vec2<float>(0.0f)},
                    {math::Vec3<float>(body_matrix[12],
                                       body_matrix[13],
                                       body_matrix[14]),
                     color,
                     math::Vec2<float>(0.0f)}};
            context.drawTransient(line,
                                  vulkan_earth::pipelines().ui_lines,
                                  nullptr,
                                  math::Mat4<float>(1.0f),
                                  math::Vec4<float>(0.0f),
                                  1000);
        }
    }

    if (current_player->getCurrentTank()->getDurationEMP() > 0) {
        // the current player is in effect of EMP, so do not draw HUD and
        // minimap. He doesn't deserve them.
    } else {
        drawHUD(context);
        drawMinimap(context);
    }
    if (game_sub_state == INVENTORY) {
        inventory->draw(context);
    } else if (game_sub_state == HELP) {
        drawHelp(context);
    }
}

void GameState::drawHUD(render::RenderContext& context) {
    if (player_cam || chase_cam_active) {
        world_cam->setShakeCam(0);
    }
    // Drawn over the scene with blending and depth testing off (every color
    // here is opaque, so only the latter matters), modelview reset to a
    // translation 800 units into the screen.
    const math::Mat4<float> saved_view = context.view();
    math::Mat4<float> projection = context.projection();
    const math::Mat4<float> hud_view = glm::translate(
            math::Mat4<float>(1.0f), math::Vec3<float>(0.0f, 0.0f, -800.0f));
    context.setDepthTest(false);
    context.setCamera(projection, hud_view);
    math::Vec4<float> color(1.0f);

    float new_x = c_glut_screen_width / 2 +
                  800 * tan(30 * pi / 180);  // this is wrong, change later
    float new_y = c_glut_screen_height / 2 + 800 * tan(30 * pi / 180);

    // Current Player Name
    if (game_sub_state == PLAYER_CONTROL) {
        color = math::Vec4<float>(
                player_factory->getPlayer(current_player_index)->getRed(),
                player_factory->getPlayer(current_player_index)->getGreen(),
                player_factory->getPlayer(current_player_index)->getBlue(),
                1);
        drawHUDText(context,
                    color,
                    player_factory->getPlayer(current_player_index)
                            ->getPlayerName(),
                    0,
                    0.75 * new_y);
    }

    // List of Players and health/delay/team
    color = math::Vec4<float>(1, 1, 1, 1);
    drawHUDText(context, color, "Wait", -0.99 * new_x, 0.75 * new_y);
    drawHUDText(context, color, "HP", -0.87 * new_x, 0.75 * new_y);
    drawHUDText(context, color, "Player Name", -0.76 * new_x, 0.75 * new_y);
    drawHUDText(context, color, "Team", -0.525 * new_x, 0.75 * new_y);
    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        color = math::Vec4<float>(player_factory->getPlayer(i)->getRed(),
                                  player_factory->getPlayer(i)->getGreen(),
                                  player_factory->getPlayer(i)->getBlue(),
                                  1);
        drawHUDText(context,
                    color,
                    player_factory->getPlayer(i)->getPlayerName(),
                    -0.76 * new_x,
                    0.70 * new_y - 0.05 * new_y * i);
        std::string team(1, player_factory->getPlayer(i)->getTeamLabel());
        color = math::Vec4<float>(1, 1, 1, 1);
        drawHUDText(context,
                    color,
                    team,
                    -0.475 * new_x,
                    0.70 * new_y - 0.05 * new_y * i);
        char buffer[128];
        memset(buffer, 0, 128);
        drawHUDText(context,
                    color,
                    (sprintf(buffer,
                             "%d",
                             static_cast<std::int32_t>(
                                     player_factory->getPlayer(i)
                                             ->getCurrentWait())),
                     buffer),
                    -0.95 * new_x,
                    0.70 * new_y - 0.05 * new_y * i);
        if (!player_factory->getPlayer(i)->getCurrentTank()->isAlive()) {
            color = math::Vec4<float>(1, 0, 0, 1);
        }
        if (player_factory->getPlayer(i)
                    ->getCurrentTank()
                    ->getDurationShield() > 0) {
            color = math::Vec4<float>(0, 0, 1, 1);
        }
        if (player_factory->getPlayer(i)->getCurrentTank()->getDurationAcid() >
            0) {
            color = math::Vec4<float>(0, 1, 0, 1);
        }
        drawHUDText(context,
                    color,
                    (sprintf(buffer,
                             "%d",
                             player_factory->getPlayer(i)
                                     ->getCurrentTank()
                                     ->getHP()),
                     buffer),
                    -0.87 * new_x,
                    0.70 * new_y - 0.05 * new_y * i);
    }

    // Power Output
    if (game_sub_state == PLAYER_CONTROL) {
        // Text
        float power_ratio = player_factory->getPlayer(current_player_index)
                                    ->getCurrentTank()
                                    ->getCurrentPower() /
                            10.0;
        char buffer[128];
        memset(buffer, 0, 128);
        color = math::Vec4<float>(1, 1, 1, 1);
        drawHUDText(context, color, "Power:", 0.7 * new_x, -0.7 * new_y);
        drawHUDText(context,
                    color,
                    (sprintf(buffer,
                             "%d",
                             static_cast<std::int32_t>(power_ratio * 1000)),
                     buffer),
                    0.95 * new_x,
                    -0.7 * new_y);
        // show previous power
        char buffer1[128];
        memset(buffer1, 0, 128);
        color = math::Vec4<float>(0.55, 0.55, 0.55, 1);
        drawHUDText(context, color, "Power:", 0.7 * new_x, -0.65 * new_y);
        drawHUDText(context,
                    color,
                    (sprintf(buffer1,
                             "%d",
                             player_factory->getPlayer(current_player_index)
                                     ->getCurrentTank()
                                     ->getPreviousPower()),
                     buffer1),
                    0.95 * new_x,
                    -0.65 * new_y);

        // Angle
        char buffer2[128];
        memset(buffer2, 0, 128);
        color = math::Vec4<float>(1, 1, 1, 1);
        drawHUDText(context, color, "Angle:", 0.7 * new_x, -0.8 * new_y);
        drawHUDText(context,
                    color,
                    (sprintf(buffer2,
                             "%d",
                             static_cast<std::int32_t>(
                                     player_factory
                                             ->getPlayer(current_player_index)
                                             ->getCurrentTank()
                                             ->getTurretDegrees() +
                                     1)),
                     buffer2),
                    0.95 * new_x,
                    -0.8 * new_y);
        // show previous angle
        char buffer3[128];
        memset(buffer3, 0, 128);
        color = math::Vec4<float>(0.55, 0.55, 0.55, 1);
        drawHUDText(context, color, "Angle:", 0.7 * new_x, -0.75 * new_y);
        drawHUDText(context,
                    color,
                    (sprintf(buffer3,
                             "%d",
                             player_factory->getPlayer(current_player_index)
                                     ->getCurrentTank()
                                     ->getPreviousAngle()),
                     buffer3),
                    0.95 * new_x,
                    -0.75 * new_y);

        // Show Weapon Slot
        weapon_slot->setXpos(-new_x * 0.7);
        weapon_slot->setYpos(-0.6 * new_y);
        weapon_slot->setZpos(2);
        weapon_slot->draw(context);
        if (current_player->getLoadedWeapon() != nullptr) {
            selected_weapon_img->setXpos(weapon_slot->getXpos() * 0.965);
            selected_weapon_img->setYpos(weapon_slot->getYpos() * 0.985);
            selected_weapon_img->setZpos(20);
            selected_weapon_remain->setXpos(weapon_slot->getXpos() * 0.965);
            selected_weapon_remain->setYpos(weapon_slot->getYpos() * 1.3);
            selected_weapon_remain->setZpos(22);
            selected_weapon_img->draw(context);
            selected_weapon_remain->draw(context);
        }

        // Display Need Help?
        if (need_help) {
            color = math::Vec4<float>(1, 1, 0, 1);
            drawHUDText(context,
                        color,
                        "Press F1 for help",
                        -1.3 * new_x,
                        -0.5 * new_y);
        }

        // Graphic meter: Power
        // glOrtho() over c_glut_screen_width x c_glut_screen_height; as in
        // the original, it stays in effect for the rest of the HUD.
        projection = vulkan_graphix::Tools::getOrthographicProjectionMatrix(
                0, c_glut_screen_width, c_glut_screen_height, 0, 1, 2000000);
        context.setCamera(projection, hud_view);
        {
            std::vector<render::UiVertex> out_tris;
            std::vector<render::UiVertex> out_lines;
            std::vector<render::UiVertex> quad;
            std::vector<render::UiVertex> loop;
            // glPolygonMode(GL_LINE): the quad's outline
            color = math::Vec4<float>(1, 0, 0, 1.0f);
            loop.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) -
                                              (0.1 * c_glut_screen_width),
                                      0.05 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            loop.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) -
                                              (0.1 * c_glut_screen_width),
                                      0.08 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            loop.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) +
                                              (0.1 * c_glut_screen_width),
                                      0.08 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            loop.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) +
                                              (0.1 * c_glut_screen_width),
                                      0.05 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            for (std::size_t corner = 0; corner < loop.size(); ++corner) {
                out_lines.push_back(loop[corner]);
                out_lines.push_back(loop[(corner + 1) % loop.size()]);
            }
            loop.clear();
            context.drawTransient(
                    out_lines, vulkan_earth::pipelines().ui_lines, nullptr);
            color = math::Vec4<float>(power_ratio, 1 - power_ratio, 0, 1.0f);
            quad.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) -
                                              (0.1 * c_glut_screen_width),
                                      0.05 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            if (quad.size() == 4) {
                for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                    out_tris.push_back(quad[corner]);
                quad.clear();
            }
            quad.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) -
                                              (0.1 * c_glut_screen_width),
                                      0.08 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            if (quad.size() == 4) {
                for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                    out_tris.push_back(quad[corner]);
                quad.clear();
            }
            quad.push_back(render::UiVertex{
                    math::Vec3<float>(
                            (c_glut_screen_width / 2) -
                                    (0.1 * c_glut_screen_width) +
                                    (power_ratio * 0.2) * c_glut_screen_width,
                            0.08 * c_glut_screen_height,
                            2),
                    color,
                    math::Vec2<float>(0.0f)});
            if (quad.size() == 4) {
                for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                    out_tris.push_back(quad[corner]);
                quad.clear();
            }
            quad.push_back(render::UiVertex{
                    math::Vec3<float>(
                            (c_glut_screen_width / 2) -
                                    (0.1 * c_glut_screen_width) +
                                    (power_ratio * 0.2) * c_glut_screen_width,
                            0.05 * c_glut_screen_height,
                            2),
                    color,
                    math::Vec2<float>(0.0f)});
            if (quad.size() == 4) {
                for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                    out_tris.push_back(quad[corner]);
                quad.clear();
            }
            context.drawTransient(
                    out_tris, vulkan_earth::pipelines().ui_triangles, nullptr);
        }

        // Graphic meter: Health
        float health_ratio =
                static_cast<float>(
                        player_factory->getPlayer(current_player_index)
                                ->getCurrentTank()
                                ->getHP()) /
                static_cast<float>(
                        player_factory->getPlayer(current_player_index)
                                ->getCurrentTank()
                                ->getArmor() *
                        100);
        // glOrtho() over c_glut_screen_width x c_glut_screen_height; as in
        // the original, it stays in effect for the rest of the HUD.
        projection = vulkan_graphix::Tools::getOrthographicProjectionMatrix(
                0, c_glut_screen_width, c_glut_screen_height, 0, 1, 2000000);
        context.setCamera(projection, hud_view);
        {
            std::vector<render::UiVertex> out_tris;
            std::vector<render::UiVertex> out_lines;
            std::vector<render::UiVertex> quad;
            std::vector<render::UiVertex> loop;
            // glPolygonMode(GL_LINE): the quad's outline
            color = math::Vec4<float>(1, 0, 0, 1.0f);
            loop.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) -
                                              (0.1 * c_glut_screen_width),
                                      0.90 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            loop.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) -
                                              (0.1 * c_glut_screen_width),
                                      0.93 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            loop.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) +
                                              (0.1 * c_glut_screen_width),
                                      0.93 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            loop.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) +
                                              (0.1 * c_glut_screen_width),
                                      0.90 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            for (std::size_t corner = 0; corner < loop.size(); ++corner) {
                out_lines.push_back(loop[corner]);
                out_lines.push_back(loop[(corner + 1) % loop.size()]);
            }
            loop.clear();
            context.drawTransient(
                    out_lines, vulkan_earth::pipelines().ui_lines, nullptr);
            color = math::Vec4<float>(1 - health_ratio, health_ratio, 0, 1.0f);
            quad.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) -
                                              (0.1 * c_glut_screen_width),
                                      0.90 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            if (quad.size() == 4) {
                for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                    out_tris.push_back(quad[corner]);
                quad.clear();
            }
            quad.push_back(render::UiVertex{
                    math::Vec3<float>((c_glut_screen_width / 2) -
                                              (0.1 * c_glut_screen_width),
                                      0.93 * c_glut_screen_height,
                                      2),
                    color,
                    math::Vec2<float>(0.0f)});
            if (quad.size() == 4) {
                for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                    out_tris.push_back(quad[corner]);
                quad.clear();
            }
            quad.push_back(render::UiVertex{
                    math::Vec3<float>(
                            (c_glut_screen_width / 2) -
                                    (0.1 * c_glut_screen_width) +
                                    (health_ratio * 0.2) * c_glut_screen_width,
                            0.93 * c_glut_screen_height,
                            2),
                    color,
                    math::Vec2<float>(0.0f)});
            if (quad.size() == 4) {
                for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                    out_tris.push_back(quad[corner]);
                quad.clear();
            }
            quad.push_back(render::UiVertex{
                    math::Vec3<float>(
                            (c_glut_screen_width / 2) -
                                    (0.1 * c_glut_screen_width) +
                                    (health_ratio * 0.2) * c_glut_screen_width,
                            0.90 * c_glut_screen_height,
                            2),
                    color,
                    math::Vec2<float>(0.0f)});
            if (quad.size() == 4) {
                for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                    out_tris.push_back(quad[corner]);
                quad.clear();
            }
            context.drawTransient(
                    out_tris, vulkan_earth::pipelines().ui_triangles, nullptr);
        }
    }

    context.setDepthTest(true);
    context.setCamera(projection, saved_view);
}

void GameState::drawHUDText(render::RenderContext& context,
                            const math::Vec4<float>& color,
                            const std::string& input,
                            float x,
                            float y) {
    // One glRasterPos2f() per character, advanced by its pixel width in
    // world units (so, as in the original, the spacing shrinks with the
    // HUD's distance from the camera).
    const render::Font& font =
            vulkan_earth::font(vulkan_earth::FontId::TimesRoman24);
    float x_pos = x;
    for (char ch : input) {
        std::int32_t step = vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
        context.drawText(font,
                         math::Vec3<float>(x_pos, y, 0),
                         std::string_view(&ch, 1),
                         color);
        x_pos += step;
    }
}

void GameState::updateMouse(std::int32_t x, std::int32_t y) {
    if (player_cam) {
        float t_height = global_settings->getCurrentTerrain()->getHeightAt(
                current_player->getCurrentTank()->getHeadMatrix()[12] -
                        50 * current_player->getCurrentTank()
                                        ->getHeadMatrix()[8],
                current_player->getCurrentTank()->getHeadMatrix()[14] -
                        50 * current_player->getCurrentTank()
                                        ->getHeadMatrix()[10]);
        float tank_radius = 400.0f;

        if (y < old_mouse_y) {
            camera_y -= 10;
        } else if (y > old_mouse_y) {
            camera_y += 10;
        }

    } else {
        if (current_world_theta >= 360.0) {
            current_world_theta -= 360.0;
        } else if (current_world_theta < 0.0) {
            current_world_theta += 360.0;
        }
        if (x < old_mouse_x) {
            current_world_theta -= 1.0;
            camera_x =
                    (plane_radius) * (cos(current_world_theta * (pi / 180))) +
                    offset;
            camera_z =
                    (plane_radius) * (sin(current_world_theta * (pi / 180))) +
                    offset;
        } else if (x > old_mouse_x) {
            current_world_theta += 1.0;
            camera_x =
                    (plane_radius) * (cos(current_world_theta * (pi / 180))) +
                    offset;
            camera_z =
                    (plane_radius) * (sin(current_world_theta * (pi / 180))) +
                    offset;
        }
    }
    old_mouse_x = x;
    old_mouse_y = y;
}

void GameState::useTurn() {
    game_sub_state = PLAYER_CONTROL;
    if (current_player->getCurrentTank()->getDurationDoubleAction() == 0) {
        current_player->getCurrentTank()->setDurationAllPassTurn();
        current_player->setCurrentWait(
                150 - current_player->getCurrentTank()->getBaseSpeed());
    } else {
        current_player->getCurrentTank()->setDurationDoubleAction(
                current_player->getCurrentTank()->getDurationDoubleAction() -
                1);
    }

    if (player_cam) {
        toggleCamera();
    }
}

// call this function every time someone is dead (or gets damaged).
// NOTE: call this once when game starts to check the case in which everyone is
// on the same team. Return values: -1==Draw game, 0==Nobody's won, n==Solo
// player num, n+100==Team num
std::int32_t GameState::getWinner() {
    std::int32_t winner = 0;

    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        if (player_factory->getPlayer(i)->getCurrentTank()->getHP() > 0) {
            // CHECK FOR SOLO WINNER
            if (player_factory->getPlayer(i)->getTeamLabel() == '-' &&
                winner == 0) {
                winner = i + 1;
            }
            // CHECK FOR TEAM WINNER
            else if ((player_factory->getPlayer(i)->getTeamLabel() - 48) +
                                     100 ==
                             winner ||
                     winner == 0) {
                winner = (player_factory->getPlayer(i)->getTeamLabel() - 48) +
                         100;
            }
            // NOBODY HAS WON YET
            else {
                return 0;
            }
        } else {
            if (winner != 0)
                return winner;
            else
                return -1;  // Everyone is DEAD?! Draw Game!
        }
    }
    return 0;
}

void GameState::toggleCamera() {
    if (game_sub_state == PROJECTILE) {
        if (chase_cam_active) {
            chase_cam_active = false;
            Mix_HaltChannel(1);
            player_cam = false;
        } else {
            chase_cam_active = true;
            Mix_HaltChannel(1);
            playSFX(BOMB_FLY);
            player_cam = false;
        }
    } else if (player_cam) {
        current_tank_theta = 0.0f;
        player_cam = false;
    } else {
        player_cam = true;
        camera_x = current_player->getCurrentTank()->getHeadMatrix()[12] +
                   current_player->getCurrentTank()->getHeadMatrix()[8] * 1000;
        camera_y = current_player->getCurrentTank()->getHeadMatrix()[13] + 350;
        camera_z =
                current_player->getCurrentTank()->getHeadMatrix()[14] +
                current_player->getCurrentTank()->getHeadMatrix()[10] * 1000;
    }
}

void GameState::debugMode(render::RenderContext& context) {
    std::vector<render::UiVertex> out_lines;
    math::Vec4<float> color(1.0f);
    /*THIS IS DEBUG TEXT REMOVE LATER THIS JUST HELPS TO SEE IF TANKS ARE
     * ORIENTED CORRECTLY*/
    //*
    vulkan_graphix::Math::Vec3<float> n =
            current_player->getCurrentTank()->getAlignmentVector();
    color = math::Vec4<float>(0.00, 0.50, 0.50, 1.0f);
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12],
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            100,
                    current_player->getCurrentTank()->getHeadMatrix()[14]),
            color,
            math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12] +
                            800 * n.x,
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            100 + 800 * n.y,
                    current_player->getCurrentTank()->getHeadMatrix()[14] +
                            800 * n.z),
            color,
            math::Vec2<float>(0.0f)});
    //*/
    //*/
    color = math::Vec4<float>(0.00, 0.00, 1.00, 1.0f);
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12],
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            100,
                    current_player->getCurrentTank()->getHeadMatrix()[14]),
            color,
            math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12] +
                            1000 * (current_player->getCurrentTank()
                                            ->getHeadMatrix()[4]),
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            100 +
                            1000 * (current_player->getCurrentTank()
                                            ->getHeadMatrix()[5]),
                    current_player->getCurrentTank()->getHeadMatrix()[14] +
                            1000 * (current_player->getCurrentTank()
                                            ->getHeadMatrix()[6])),
            color,
            math::Vec2<float>(0.0f)});
    //*/
    //*/
    vulkan_graphix::Math::Vec3<float> m =
            current_player->getCurrentTank()->getRotateAbout();
    color = math::Vec4<float>(0.75, 0.50, 0.50, 1.0f);
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12],
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            100,
                    current_player->getCurrentTank()->getHeadMatrix()[14]),
            color,
            math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12] +
                            1000 * m.x,
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            100 + 1000 * m.y,
                    current_player->getCurrentTank()->getHeadMatrix()[14] +
                            1000 * m.z),
            color,
            math::Vec2<float>(0.0f)});
    //*/
    /*END OF TANK ORIENTATION DEBUGGING*/
    context.drawTransient(
            out_lines, vulkan_earth::pipelines().ui_lines, nullptr);
}

void GameState::drawMinimap(render::RenderContext& context) {
    // Its own viewport (glViewport()'s float -> int truncation kept),
    // cleared to the sea color.
    context.setViewport(vulkan_earth::glRect(
            static_cast<std::int32_t>(width - width / 5.8),
            static_cast<std::int32_t>(height - height / 4.8),
            static_cast<std::int32_t>(width / 6.0),
            static_cast<std::int32_t>(height / 5.0)));
    context.clearColorAndDepth(math::Vec4<float>(.22, .65, .60, 1));

    std::int32_t size = static_cast<std::int32_t>(
            global_settings->getCurrentTerrain()->getActualSize());

    context.setCamera(
            vulkan_graphix::Tools::getOrthographicProjectionMatrix(
                    -18000, 18000, 15000, -15000, 1, 20000000),
            glm::lookAt(math::Vec3<float>(
                                size / 2, size + (size / width), size / 2),
                        math::Vec3<float>(size / 2, 0, size / 2),
                        math::Vec3<float>(1.0f, 0.0f, 0.0f)));

    global_settings->getCurrentTerrain()->draw(context);

    std::vector<render::UiVertex> out_tris;
    std::vector<render::UiVertex> out_lines;
    std::vector<render::UiVertex> quad;
    std::vector<render::UiVertex> loop;
    math::Vec4<float> color(1.0f);
    // Draw Tank Marks
    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        if (player_factory->getPlayer(i)
                    ->getCurrentTank()
                    ->getDurationCloak() == 0) {
            color = math::Vec4<float>(player_factory->collectPlayerColor(i)[0],
                                      player_factory->collectPlayerColor(i)[1],
                                      player_factory->collectPlayerColor(i)[2],
                                      1.0f);
            out_tris.push_back(render::UiVertex{
                    math::Vec3<float>(
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[12] -
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[8] *
                                            900,
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[13] +
                                    12000,
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[14] -
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[10] *
                                            900),
                    color,
                    math::Vec2<float>(0.0f)});
            out_tris.push_back(render::UiVertex{
                    math::Vec3<float>(
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[12] +
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[0] *
                                            700 +
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[8] *
                                            900,
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[13] +
                                    12000,
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[14] +
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[2] *
                                            700 +
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[10] *
                                            900),
                    color,
                    math::Vec2<float>(0.0f)});
            out_tris.push_back(render::UiVertex{
                    math::Vec3<float>(
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[12] -
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[0] *
                                            700 +
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[8] *
                                            900,
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[13] +
                                    12000,
                            player_factory->getPlayer(i)
                                            ->getCurrentTank()
                                            ->getHeadMatrix()[14] -
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[2] *
                                            700 +
                                    player_factory->getPlayer(i)
                                                    ->getCurrentTank()
                                                    ->getHeadMatrix()[10] *
                                            900),
                    color,
                    math::Vec2<float>(0.0f)});
        }
    }
    // Draw Current Player Tank's Aiming Line
    color = math::Vec4<float>(
            player_factory->collectPlayerColor(current_player_index)[0],
            player_factory->collectPlayerColor(current_player_index)[1],
            player_factory->collectPlayerColor(current_player_index)[2],
            0.50);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12] +
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[0] *
                                    60,
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            10000,
                    current_player->getCurrentTank()->getHeadMatrix()[14] +
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[2] *
                                    60),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    quad.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12] +
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[0] *
                                    60 -
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[8] *
                                    24000,
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            10000,
                    current_player->getCurrentTank()->getHeadMatrix()[14] +
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[2] *
                                    60 -
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[10] *
                                    24000),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    quad.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12] -
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[0] *
                                    60 -
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[8] *
                                    24000,
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            10000,
                    current_player->getCurrentTank()->getHeadMatrix()[14] -
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[2] *
                                    60 -
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[10] *
                                    24000),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    quad.push_back(render::UiVertex{
            math::Vec3<float>(
                    current_player->getCurrentTank()->getHeadMatrix()[12] -
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[0] *
                                    60,
                    current_player->getCurrentTank()->getHeadMatrix()[13] +
                            10000,
                    current_player->getCurrentTank()->getHeadMatrix()[14] -
                            current_player->getCurrentTank()
                                            ->getHeadMatrix()[2] *
                                    60),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }

    // Draw Lines of Sight of World Camera
    if (!player_cam && !chase_cam_active) {
        color = math::Vec4<float>(1, 1, 1, 0.8);
        loop.push_back(render::UiVertex{
                math::Vec3<float>(world_cam->getMatrix()[12] -
                                          world_cam->getMatrix()[13] * 0.38,
                                  10000,
                                  world_cam->getMatrix()[14] +
                                          world_cam->getMatrix()[13] -
                                          15000 / width),
                color,
                math::Vec2<float>(0.0f)});
        loop.push_back(render::UiVertex{
                math::Vec3<float>(world_cam->getMatrix()[12] -
                                          world_cam->getMatrix()[13] * 0.38,
                                  10000,
                                  world_cam->getMatrix()[14] -
                                          world_cam->getMatrix()[13] +
                                          15000 / width),
                color,
                math::Vec2<float>(0.0f)});
        loop.push_back(render::UiVertex{
                math::Vec3<float>(world_cam->getMatrix()[12] +
                                          world_cam->getMatrix()[13] * 0.82,
                                  10000,
                                  world_cam->getMatrix()[14] -
                                          world_cam->getMatrix()[13] +
                                          15000 / width - 1350),
                color,
                math::Vec2<float>(0.0f)});
        loop.push_back(render::UiVertex{
                math::Vec3<float>(world_cam->getMatrix()[12] +
                                          world_cam->getMatrix()[13] * 0.82,
                                  10000,
                                  world_cam->getMatrix()[14] +
                                          world_cam->getMatrix()[13] -
                                          15000 / width + 1350),
                color,
                math::Vec2<float>(0.0f)});
        for (std::size_t corner = 0; corner < loop.size(); ++corner) {
            out_lines.push_back(loop[corner]);
            out_lines.push_back(loop[(corner + 1) % loop.size()]);
        }
        loop.clear();
    }

    // Flushed here to keep the original's draw order: the projectile's
    // quad below shares the line of sight's height, so whichever is drawn
    // first wins the depth test.
    context.drawTransient(
            out_tris, vulkan_earth::pipelines().ui_triangles, nullptr);
    context.drawTransient(
            out_lines, vulkan_earth::pipelines().ui_lines, nullptr);
    out_tris.clear();
    out_lines.clear();

    // Draw Projectile
    if (projectile) {
        color = math::Vec4<float>(1, 1, 1, 1.0f);
        quad.push_back(render::UiVertex{
                math::Vec3<float>(projectile->getPos()[0] - 200,
                                  10000,
                                  projectile->getPos()[2] - 200),
                color,
                math::Vec2<float>(0.0f)});
        if (quad.size() == 4) {
            for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                out_tris.push_back(quad[corner]);
            quad.clear();
        }
        quad.push_back(render::UiVertex{
                math::Vec3<float>(projectile->getPos()[0] - 200,
                                  10000,
                                  projectile->getPos()[2] + 200),
                color,
                math::Vec2<float>(0.0f)});
        if (quad.size() == 4) {
            for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                out_tris.push_back(quad[corner]);
            quad.clear();
        }
        quad.push_back(render::UiVertex{
                math::Vec3<float>(projectile->getPos()[0] + 200,
                                  10000,
                                  projectile->getPos()[2] + 200),
                color,
                math::Vec2<float>(0.0f)});
        if (quad.size() == 4) {
            for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                out_tris.push_back(quad[corner]);
            quad.clear();
        }
        quad.push_back(render::UiVertex{
                math::Vec3<float>(projectile->getPos()[0] + 200,
                                  10000,
                                  projectile->getPos()[2] - 200),
                color,
                math::Vec2<float>(0.0f)});
        if (quad.size() == 4) {
            for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                out_tris.push_back(quad[corner]);
            quad.clear();
        }
    }

    // Draw Projectile Land Mark
    color = math::Vec4<float>(
            player_factory->collectPlayerColor(current_player_index)[0],
            player_factory->collectPlayerColor(current_player_index)[1],
            player_factory->collectPlayerColor(current_player_index)[2],
            1.0f);
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(current_player->getCurrentTank()
                                              ->getProjectileLandPos()[0] -
                                      900,
                              10000,
                              current_player->getCurrentTank()
                                              ->getProjectileLandPos()[1] -
                                      900),
            color,
            math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(current_player->getCurrentTank()
                                              ->getProjectileLandPos()[0] +
                                      900,
                              10000,
                              current_player->getCurrentTank()
                                              ->getProjectileLandPos()[1] +
                                      900),
            color,
            math::Vec2<float>(0.0f)});
    color = math::Vec4<float>(
            player_factory->collectPlayerColor(current_player_index)[0],
            player_factory->collectPlayerColor(current_player_index)[1],
            player_factory->collectPlayerColor(current_player_index)[2],
            1.0f);
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(current_player->getCurrentTank()
                                              ->getProjectileLandPos()[0] +
                                      900,
                              10000,
                              current_player->getCurrentTank()
                                              ->getProjectileLandPos()[1] -
                                      900),
            color,
            math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
            math::Vec3<float>(current_player->getCurrentTank()
                                              ->getProjectileLandPos()[0] -
                                      900,
                              10000,
                              current_player->getCurrentTank()
                                              ->getProjectileLandPos()[1] +
                                      900),
            color,
            math::Vec2<float>(0.0f)});

    context.drawTransient(
            out_tris, vulkan_earth::pipelines().ui_triangles, nullptr);
    context.drawTransient(
            out_lines, vulkan_earth::pipelines().ui_lines, nullptr);

    vulkan_earth::resetToFullWindow(context);
}

void GameState::playBackgroundSounds() {
    if (global_settings->getHillGirth() == "Rock") {
        sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (sfx_random == 0)
            playSFX(WAVE1);
        else if (sfx_random == 1)
            playSFX(WAVE2);
        else if (sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        sfx_random = (rand() * 9) % 5000;
        if (sfx_random == 0)
            playSFX(SEAGULLS1);
        else if (sfx_random == 1)
            playSFX(SEAGULLS2);
        else if (sfx_random == 2)
            playSFX(SEAGULLS3);
        else if (sfx_random == 3)
            playSFX(SEAGULLS4);
        else if (sfx_random == 4)
            playSFX(SEAGULLS5);

        playMusic(gamestate_rock);
    } else if (global_settings->getHillGirth() == "Snow") {
        sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (sfx_random == 0)
            playSFX(WAVE1);
        else if (sfx_random == 1)
            playSFX(WAVE2);
        else if (sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        sfx_random = (rand() * 9) % 5000;
        if (sfx_random == 0)
            playSFX(SEAGULLS1);
        else if (sfx_random == 1)
            playSFX(SEAGULLS2);
        else if (sfx_random == 2)
            playSFX(SEAGULLS3);
        else if (sfx_random == 3)
            playSFX(SEAGULLS4);
        else if (sfx_random == 4)
            playSFX(SEAGULLS5);

        playMusic(gamestate_snow);
    } else if (global_settings->getHillGirth() == "Ice") {
        sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (sfx_random == 0)
            playSFX(WAVE1);
        else if (sfx_random == 1)
            playSFX(WAVE2);
        else if (sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);

        playMusic(gamestate_ice);
    } else if (global_settings->getHillGirth() == "Desert") {
        sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (sfx_random == 0)
            playSFX(WAVE1);
        else if (sfx_random == 1)
            playSFX(WAVE2);
        else if (sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        sfx_random = (rand() * 9) % 5000;
        if (sfx_random == 0)
            playSFX(SEAGULLS1);
        else if (sfx_random == 1)
            playSFX(SEAGULLS2);
        else if (sfx_random == 2)
            playSFX(SEAGULLS3);
        else if (sfx_random == 3)
            playSFX(SEAGULLS4);
        else if (sfx_random == 4)
            playSFX(SEAGULLS5);

        if (atoi(global_settings->getHillHeight().c_str()) < 2 &&
            atoi(global_settings->getHillyness().c_str()) > 4) {
            if (!start_music_played) {
                playMusic(gamestate_beach_start);
                start_music_played = true;
            } else {
                playMusic(gamestate_beach_loop);
            }
        } else {
            playMusic(gamestate_desert);
        }
    } else if (global_settings->getHillGirth() == "Mars") {
        sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (sfx_random == 0)
            playSFX(WAVE1);
        else if (sfx_random == 1)
            playSFX(WAVE2);
        else if (sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);

        playMusic(gamestate_mars);
    } else if (global_settings->getHillGirth() == "Lava") {
        playMusic(gamestate_lava);
    }
}

void GameState::drawHelp(render::RenderContext& context) {
    // glViewport()'s float -> int truncation kept.
    vulkan_earth::beginOverlayPanel(
            context,
            vulkan_earth::glRect(static_cast<std::int32_t>(width * 0.25),
                                 static_cast<std::int32_t>(height * 0.1),
                                 static_cast<std::int32_t>(width * 0.5),
                                 static_cast<std::int32_t>(height * 0.8)),
            width,
            height);

    manual->draw(context);

    vulkan_earth::resetToFullWindow(context);
}

void GameState::handlePlayerControlUpdates() {
    if (key_monitor[1]) {
        playSFX(TANK_CONTROL2);
        if (current_player->getPlayerType() == "CPU") {
            current_player->updateBalsticMatrix();
            current_player->setUpYawVectors();
        }
        player_factory->getPlayer(current_player_index)
                ->getCurrentTank()
                ->rotateHead(-0.1 - 0.4 * key_monitor[1] / 50);
        if (current_player->getCurrentTank()->getName() == "Rhinoxx" ||
            current_player->getCurrentTank()->getName() == "HeavyD") {
            current_player->getCurrentTank()->rotateWheel(-.1);
        }
        camera_x = current_player->getCurrentTank()->getHeadMatrix()[12] +
                   current_player->getCurrentTank()->getHeadMatrix()[8] * 1000;
        camera_z =
                current_player->getCurrentTank()->getHeadMatrix()[14] +
                current_player->getCurrentTank()->getHeadMatrix()[10] * 1000;
    } else if (key_monitor[3]) {
        playSFX(TANK_CONTROL2);
        if (current_player->getPlayerType() == "CPU") {
            current_player->updateBalsticMatrix();
            current_player->setUpYawVectors();
        }
        player_factory->getPlayer(current_player_index)
                ->getCurrentTank()
                ->rotateHead(0.1 + 0.4 * key_monitor[3] / 50);
        if (current_player->getCurrentTank()->getName() == "Rhinoxx" ||
            current_player->getCurrentTank()->getName() == "HeavyD") {
            current_player->getCurrentTank()->rotateWheel(.1);
        }
        camera_x = current_player->getCurrentTank()->getHeadMatrix()[12] +
                   current_player->getCurrentTank()->getHeadMatrix()[8] * 1000;
        camera_z =
                current_player->getCurrentTank()->getHeadMatrix()[14] +
                current_player->getCurrentTank()->getHeadMatrix()[10] * 1000;
    } else {
        if (current_player->getPlayerType() != "CPU") {
            Mix_HaltChannel(3);
        }
    }

    if (key_monitor[2]) {
        if ((player_factory->getPlayer(current_player_index)
                     ->getCurrentTank()
                     ->getTurretDegrees() +
             0.1 + 0.4 * key_monitor[2] / 50) < 90) {
            playSFX(TANK_CONTROL1);
            player_factory->getPlayer(current_player_index)
                    ->getCurrentTank()
                    ->rotateTurret(0.1 + 0.4 * key_monitor[2] / 50);
        } else {
            Mix_HaltChannel(2);
            if (Mix_Playing(4) == 0) playSFX(TANK_STUCK);
        }
    } else if (key_monitor[4]) {
        if ((player_factory->getPlayer(current_player_index)
                     ->getCurrentTank()
                     ->getTurretDegrees() -
             0.1 - 0.4 * key_monitor[4] / 50) >= 0) {
            playSFX(TANK_CONTROL1);
            player_factory->getPlayer(current_player_index)
                    ->getCurrentTank()
                    ->rotateTurret(-0.1 - 0.4 * key_monitor[4] / 50);
        } else {
            Mix_HaltChannel(2);
            if (Mix_Playing(4) == 0) playSFX(TANK_STUCK);
        }
    } else {
        if (current_player->getPlayerType() != "CPU") {
            Mix_HaltChannel(2);
        }
    }

    if (key_monitor['-'])
        player_factory->getPlayer(current_player_index)
                ->getCurrentTank()
                ->adjustPower(-0.01 - 0.01 * key_monitor['-']);
    if (key_monitor['='])
        player_factory->getPlayer(current_player_index)
                ->getCurrentTank()
                ->adjustPower(0.01 + 0.01 * key_monitor['=']);
}

void GameState::destroyProjectile() {
    projectile_fired = false;
    chase_cam_active = false;
    delete projectile;
    projectile = nullptr;
}

bool GameState::getProjectileFired() { return projectile_fired; }

void GameState::updateWorldCam() {
    if (key_monitor['w']) world_cam->moveCam(20 + 20 * key_monitor['w'], 0, 0);
    if (key_monitor['a'])
        world_cam->moveCam(0, 0, -20 - 20 * key_monitor['a']);
    if (key_monitor['s'])
        world_cam->moveCam(-20 - 20 * key_monitor['s'], 0, 0);
    if (key_monitor['d']) world_cam->moveCam(0, 0, 20 + 20 * key_monitor['d']);
    if (key_monitor['r'])
        world_cam->moveCam(0, -20 - 20 * key_monitor['r'], 0);
    if (key_monitor['f']) world_cam->moveCam(0, 20 + 20 * key_monitor['f'], 0);
}

void GameState::createSpecialEffect() {
    float power_ratio = player_factory->getPlayer(current_player_index)
                                ->getCurrentTank()
                                ->getCurrentPower() /
                        10.0;
    player_factory->getPlayer(current_player_index)
            ->getCurrentTank()
            ->setPreviousPower(static_cast<std::int32_t>(power_ratio * 1000));
    player_factory->getPlayer(current_player_index)
            ->getCurrentTank()
            ->setPreviousAngle(
                    static_cast<std::int32_t>(
                            player_factory->getPlayer(current_player_index)
                                    ->getCurrentTank()
                                    ->getTurretDegrees()) +
                    1);
    player_factory->getPlayer(current_player_index)
            ->getCurrentTank()
            ->setProjectileLandPos(projectile->getPos()[0],
                                   projectile->getPos()[2]);
    special_effect_x = projectile->getPos()[0];
    special_effect_y = projectile->getPos()[1];
    special_effect_z = projectile->getPos()[2];
    // Teleport weapon effect handling
    if (projectile->getWeapon()->getImageFileName() == "WeaponTeleport.raw") {
        std::int32_t size =
                global_settings->getCurrentTerrain()->getActualSize();
        // Make sure the projectile has not landed on the water
        if (0 < projectile->getPos()[0] && projectile->getPos()[0] < size &&
            0 < projectile->getPos()[2] && projectile->getPos()[2] < size) {
            std::int32_t new_x = static_cast<std::int32_t>(
                                         projectile->getPos()[0] / 100.0) *
                                 100;
            std::int32_t new_y = static_cast<std::int32_t>(
                                         projectile->getPos()[1] / 100.0) *
                                 100;
            std::int32_t new_z = static_cast<std::int32_t>(
                                         projectile->getPos()[2] / 100.0) *
                                 100;
            current_player->getCurrentTank()->setTankPos(new_x, new_y, new_z);
            const float* body_matrix =
                    current_player->getCurrentTank()->getBodyMatrix();
            float new_height =
                    global_settings->getCurrentTerrain()->getHeightAt(
                            body_matrix[12], body_matrix[14]);
            std::int32_t scale = static_cast<std::int32_t>(
                    global_settings->getCurrentTerrain()->getScale());
            vulkan_graphix::Math::Vec3<float> n =
                    global_settings->getCurrentTerrain()->getTriangleNormal(
                            body_matrix[12] / scale, body_matrix[14] / scale);
            current_player->getCurrentTank()->orientTank(&n);
            current_player->getCurrentTank()->setTankPos(
                    body_matrix[12], new_height, body_matrix[14]);
        }
    }
    projectile->getWeapon()->playExplosionSFX();
    radius_of_current_explosion = projectile->getWeapon()->getRadius();
    special_effect_type = EXPLOSION;

    if (special_effect_type == EXPLOSION) {
        special_effects_count = (special_effect_time_limit / time_divisors);
        special_effects = new SpecialEffect*[special_effects_count];
        for (std::int32_t x = 0; x < special_effects_count; x++) {
            special_effects[x] = new Explosion(
                    special_effect_x,
                    special_effect_y,
                    special_effect_z,
                    static_cast<std::int32_t>(radius_of_current_explosion));
            if (projectile->getWeapon() != nullptr) {
                special_effects[x]->setColors1(
                        projectile->getWeapon()->getExplosionColor1());
                special_effects[x]->setColors2(
                        projectile->getWeapon()->getExplosionColor2());
                special_effects[x]->setColors3(
                        projectile->getWeapon()->getExplosionColor3());
                special_effects[x]->setColors4(
                        projectile->getWeapon()->getExplosionColor4());
            }
        }
    }
    game_sub_state = SPECIAL_EFFECT;
}

void GameState::handleProjectileState() {
    bool collision_occured = false;
    if (projectile) {
        if (projectile->getPos()[1] <=
            global_settings->getCurrentTerrain()->getHeightAt(
                    projectile->getPos()[0], projectile->getPos()[2])) {
            collision_occured = true;
        } else {
            for (std::int32_t i = 0; i < number_of_players; i++) {
                if (player_factory->getPlayer(i)
                            ->getCurrentTank()
                            ->checkCollision(projectile->getPos()[0],
                                             projectile->getPos()[1],
                                             projectile->getPos()[2])) {
                    collision_occured = true;
                }
            }
        }
        if (collision_occured) {
            /* RESET AI VARIABLES	*/
            timer = 0;
            /* DONE RESETING AI VARIABLES */
            setPositionOfLastProjectile(projectile->getPos()[0],
                                        projectile->getPos()[1],
                                        projectile->getPos()[2]);
            // Make sure chase cam does not shake till impact
            projectile->getChaseCam()->setShakeCam(0);
            createSpecialEffect();
        } else {
            timer = timer + .02f;
            // Note: gravity is negative
            const vulkan_graphix::Math::Vec3<float> position =
                    vulkan_graphix::Ballistics::positionAt(
                            projectile->getLaunch(), gravity, timer);
            projectile->update(position.x, position.y, position.z);
        }
    } else {
        float tank_attribute_power =
                player_factory->getPlayer(current_player_index)
                        ->getCurrentTank()
                        ->getPower();
        float power_bar = player_factory->getPlayer(current_player_index)
                                  ->getCurrentTank()
                                  ->getCurrentPower();
        const float* turret_matrix =
                player_factory->getPlayer(current_player_index)
                        ->getCurrentTank()
                        ->getTurretMatrix();
        float matrix[16];
        for (std::int32_t i = 0; i < 16; i++) {
            matrix[i] = turret_matrix[i];
        }
        matrix[12] = turret_matrix[12] + 1000 * turret_matrix[4];
        matrix[13] = turret_matrix[13] + 1000 * turret_matrix[5];
        matrix[13] = turret_matrix[14] + 1000 * turret_matrix[6];
        projectile = new Projectile(
                this,
                matrix,
                tank_attribute_power * power_bar * balistic_scalar,
                projectile_models);
        if (current_player->getLoadedWeapon() != nullptr) {
            projectile->setWeapon(current_player->getLoadedWeapon());
        } else {
            projectile->setWeapon(projectile->getDefaultWeapon());
        }
        special_effect_type = EXPLOSION;
        createSpecialEffect();
    }
}

void GameState::currentPlayerFire() {
    if (current_player->getPlayerType() == "HUMAN") {
        // just in case
        if (projectile) {
            destroyProjectile();
        }
        game_sub_state = PROJECTILE;
        constructProjectile();
        if (projectile) {
            if (current_player->getLoadedWeapon() != nullptr) {
                projectile->setWeapon(current_player->getLoadedWeapon());
            } else {
                projectile->setWeapon(projectile->getDefaultWeapon());
            }
        }
    } else if (current_player->getPlayerType() == "CPU") {
        if (projectile) {
            destroyProjectile();
        }
        game_sub_state = PROJECTILE;
        constructProjectile();
        if (current_player->getLoadedWeapon() != nullptr) {
            if (projectile) {
                projectile->setWeapon(current_player->getLoadedWeapon());
                chase_cam_active = true;
            }
        } else {
            if (projectile) {
                projectile->setWeapon(projectile->getDefaultWeapon());
                chase_cam_active = true;
            }
        }
    }
    if (projectile) {
        projectile->getWeapon()->playFireSFX();
    }
}

void GameState::constructProjectile() {
    float tank_attribute_power =
            player_factory->getPlayer(current_player_index)
                    ->getCurrentTank()
                    ->getPower();
    float power_bar = player_factory->getPlayer(current_player_index)
                              ->getCurrentTank()
                              ->getCurrentPower();

    const float* turret_matrix =
            current_player->getCurrentTank()->getTurretMatrix();
    // Refuses to fire if a point just past the muzzle is already below the
    // terrain (e.g. the barrel is buried in a hillside).
    const vulkan_graphix::Math::Vec3<float> barrel_probe =
            vulkan_graphix::Ballistics::pointAlongBarrel(
                    glm::make_mat4(turret_matrix), 700.0f);
    float land_pos[3] = {barrel_probe.x, barrel_probe.y, barrel_probe.z};

    if (land_pos[1] < global_settings->getCurrentTerrain()->getHeightAt(
                              land_pos[0], land_pos[2])) {
        projectile = nullptr;
    } else {
        projectile = new Projectile(
                this,
                player_factory->getPlayer(current_player_index)
                        ->getCurrentTank()
                        ->getTurretMatrix(),
                tank_attribute_power * power_bar * balistic_scalar,
                projectile_models);
    }
}

void GameState::handleSpecialEffectState() {
    special_effect_timer++;
    if (special_effect_timer < special_effect_time_limit) {
        if (special_effect_timer == 1) {
            if (projectile->getWeapon()->getImageFileName() !=
                "WeaponRevive.raw") {
                global_settings->getCurrentTerrain()->makeCrater(
                        projectile->getPos()[0],
                        projectile->getPos()[2],
                        radius_of_current_explosion);
            }
            std::int32_t total_players = global_settings->getPlayerCount();
            for (std::int32_t p = 0; p < total_players; p++) {
                if (player_factory->getPlayer(p)->getCurrentTank() !=
                    nullptr) {
                    const float* body_matrix = player_factory->getPlayer(p)
                                                       ->getCurrentTank()
                                                       ->getBodyMatrix();
                    vulkan_graphix::Math::Vec3<float> v0(
                            body_matrix[12], body_matrix[13], body_matrix[14]);
                    vulkan_graphix::Math::Vec3<float> v1(
                            projectile->getPos()[0],
                            projectile->getPos()[1],
                            projectile->getPos()[2]);
                    float distance = calcDistanceBetweenVertices(&v0, &v1);
                    float scale =
                            global_settings->getCurrentTerrain()->getScale();
                    if (distance < radius_of_current_explosion * scale) {
                        // PLACEHOLDER BELOW, pass the tank into the weapon and
                        // call dealDamage (or whatever) from there
                        projectile->getWeapon()->causeEffectToTank(
                                distance,
                                player_factory->getPlayer(p)
                                        ->getCurrentTank());
                        if (player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->getDurationFloat() > 0) {
                            vulkan_graphix::Math::Vec3<float>* n =
                                    new vulkan_graphix::Math::Vec3<float>(
                                            0, 1, 0);

                            player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->orientTank(n);
                            player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->setTankPos(
                                            player_factory->getPlayer(p)
                                                    ->getCurrentTank()
                                                    ->getBodyMatrix()[12],
                                            player_factory->getPlayer(p)
                                                    ->getCurrentTank()
                                                    ->getBodyMatrix()[13],
                                            player_factory->getPlayer(p)
                                                    ->getCurrentTank()
                                                    ->getBodyMatrix()[14]);
                            delete n;
                        } else {
                            std::int32_t size = static_cast<std::int32_t>(
                                    global_settings->getCurrentTerrain()
                                            ->getActualSize());
                            std::int32_t scale_int = static_cast<std::int32_t>(
                                    global_settings->getCurrentTerrain()
                                            ->getScale());
                            float new_height =
                                    global_settings->getCurrentTerrain()
                                            ->getHeightAt(body_matrix[12],
                                                          body_matrix[14]);

                            vulkan_graphix::Math::Vec3<float> n =
                                    global_settings->getCurrentTerrain()
                                            ->getTriangleNormal(
                                                    body_matrix[12] /
                                                            scale_int,
                                                    body_matrix[14] /
                                                            scale_int);

                            player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->orientTank(&n);
                            player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->setTankPos(body_matrix[12],
                                                 new_height,
                                                 body_matrix[14]);
                            player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->checkFallingDamage();
                        }
                    }
                }
            }
        }
        projectile->getChaseCam()->updateFactor();
        std::int32_t shake_it_baby = rand() % 30;
        projectile->getChaseCam()->setShakeCam(20 + shake_it_baby);
    } else {
        if (special_effect_type == EXPLOSION) {
            for (std::int32_t x = 0; x < special_effects_count; x++) {
                if (special_effects[x]) {
                    delete special_effects[x];
                }
            }

            delete special_effects;
        }
        projectile->getChaseCam()->setShakeCam(0);
        projectile->getChaseCam()->resetFactor();
        destroyProjectile();
        special_effect_x = 0;
        special_effect_y = 0;
        special_effect_z = 0;
        special_effect_timer = 0;
        special_effects_count = 0;
        radius_of_current_explosion = 0;
        useTurn();
        game_sub_state = PASS_TIME;
        std::int32_t inven_index = 0;
        inventory->handleInventory(current_player, inven_index);
    }
}

void GameState::handleKeyboardInput(std::int32_t key, bool key_status) {
    if (current_player->getPlayerType() == "CPU") {
        handleNonInventoryKeyboard(key, key_status);
    }
    if (current_player->getPlayerType() ==
        "HUMAN") {  // TEMP TEST FOR CPU PLAYERS REMOVE CPU'S DON'T USE
                    // KEYBOARDS
        if (game_sub_state == INVENTORY) {
            handleInventoryKeyboard(key, key_status);
        } else if (game_sub_state != INVENTORY) {
            handleNonInventoryKeyboard(key, key_status);
        }
    }
}

void GameState::handleInventoryKeyboard(std::int32_t key, bool key_status) {
    if (key_status) {
        if (key == 27 || key == 'i') {
            Mix_VolumeMusic(prev_music_volume * 3);
            game_sub_state = PLAYER_CONTROL;
        } else if (key == 13) {
            Mix_VolumeMusic(prev_music_volume * 3);
            std::int32_t index = inventory->getSelectedIndex();
            // UN/LOAD A WEAPON
            if (index < player_max_weapons &&
                current_player->getCurrentWeapons()[index] != nullptr) {
                // UNLOADING
                if (current_player->getLoadedWeapon() != nullptr &&
                    current_player->getLoadedWeapon()->getUNIQUEIDENTIFIER() ==
                            current_player->getCurrentWeapons()[index]
                                    ->getUNIQUEIDENTIFIER()) {
                    playSFX(WEAPON_UNLOAD);
                    delete selected_weapon_img;
                    delete selected_weapon_remain;
                    current_player->setLoadedWeapon(nullptr);
                    selected_weapon_img = nullptr;
                    selected_weapon_remain = nullptr;
                }
                // LOADING
                else {
                    playSFX(WEAPON_LOAD);
                    if (selected_weapon_img != nullptr) {
                        delete selected_weapon_img;
                        delete selected_weapon_remain;
                    }
                    current_player->setLoadedWeapon(
                            current_player->getCurrentWeapons()[index]);
                    selected_weapon_img =
                            new ImageObject(weapon_slot->getXpos() * 1.01,
                                            weapon_slot->getYpos() * 1.01,
                                            2,
                                            weapon_slot->getWidth() * 0.9,
                                            weapon_slot->getHeight() * 0.9,
                                            0,
                                            256,
                                            256,
                                            current_player->getLoadedWeapon()
                                                    ->getImageFileName());
                    std::string remain =
                            "x " +
                            std::to_string(current_player->getLoadedWeapon()
                                                   ->getRemaining());
                    selected_weapon_remain =
                            new TextObject(remain,
                                           0,
                                           0,
                                           3,
                                           vulkan_earth::FontId::TimesRoman24,
                                           0.6,
                                           0.2,
                                           0.4);
                }
            }
            // USE AN ITEM
            else if (player_max_weapons <= index &&
                     index < player_max_weapons + player_max_items &&
                     current_player->getCurrentItems()[index -
                                                       player_max_weapons] !=
                             nullptr) {
                // If causeEffectToTank(...) returns true, that means player
                // has used an item which costs 1 turn
                if (current_player
                            ->getCurrentItems()[index - player_max_weapons]
                            ->causeEffectToTank(
                                    current_player->getCurrentTank())) {
                    useTurn();
                }
                current_player->getCurrentItems()[index - player_max_weapons]
                        ->playUseSFX();
                inventory->handleInventory(current_player, index);
                while (Mix_Playing(0));
                inventory->setupInventory(current_player);
                Mix_HaltChannel(0);
            }
            game_sub_state = PLAYER_CONTROL;
        } else {
            inventory->keyHandler(key);
        }
    }
}

void GameState::handleNonInventoryKeyboard(std::int32_t key, bool key_status) {
    if ((key == 'c') && (key_status))
        toggleCamera();
    else if ((key == ' ') && (key_status) &&
             (game_sub_state == PLAYER_CONTROL)) {
        currentPlayerFire();
    } else if ((key == 5) && (key_status) &&
               (game_sub_state == PLAYER_CONTROL)) {
        prev_music_volume = Mix_VolumeMusic(-1);
        prev_music_volume = Mix_VolumeMusic(prev_music_volume / 3);
        playSFX(MANUAL);
        game_sub_state = HELP;
        need_help = false;
    } else if ((key == 5) && (key_status) && (game_sub_state == HELP)) {
        Mix_VolumeMusic(prev_music_volume * 3);
        game_sub_state = PLAYER_CONTROL;
        need_help = false;
    } else if ((key == 6) && (key_status)) {
        Mix_HaltMusic();
        Mix_HaltChannel(-1);
        *current_game_state = MAIN_MENU;
    } else if ((key == 'i') && (key_status) &&
               (game_sub_state == PLAYER_CONTROL)) {
        if (current_player->getPlayerType() == "HUMAN" &&
            current_player->getCurrentTank()->getDurationPadlock() == 0) {
            Mix_HaltChannel(2);
            Mix_HaltChannel(3);
            key_monitor[1] = 0;
            key_monitor[2] = 0;
            key_monitor[3] = 0;
            key_monitor[4] = 0;
            prev_music_volume = Mix_VolumeMusic(
                    -1);  // -1 returns the current volume. Other numbers will
                          // change the volume, and it returns the volume
                          // before changed
            prev_music_volume = Mix_VolumeMusic(prev_music_volume / 3);
            playSFX(INVENTORY_ACCESS);
            game_sub_state = INVENTORY;
        } else {
            // playSFX(INVENTORY_INVALID);
        }
    } else if (key == 'b' && !key_status) {
        draw_hit_box = !draw_hit_box;
    } else if (key == 'p' && !key_status) {
        if (current_player->getPlayerType() == "CPU") {
            if (current_player->getDrawDebugLinesandPlanes())
                current_player->setDrawDebugLinesandPlanes(false);
            else
                current_player->setDrawDebugLinesandPlanes(true);
        }
    } else {
        if (key_status) {
            need_help = key > 6 && key != 'w' && key != 'a' && key != 's' &&
                        key != 'd' && key != 'r' && key != 'f' && key != 'c' &&
                        key != '-' && key != '=';
            key_monitor[key] += 1;
        } else {
            key_monitor[key] = 0;
        }
    }
}

void GameState::handlePassTime() {
    if ((player_factory->getPlayer(current_player_index)->getCurrentWait() <=
         0) &&
        (player_factory->getPlayer(current_player_index)
                 ->getCurrentTank()
                 ->isAlive())) {
        game_sub_state = PLAYER_CONTROL;
        current_player = player_factory->getPlayer(current_player_index);
    } else {
        do {
            player_factory->getPlayer(current_player_index)
                    ->setCurrentWait(
                            player_factory->getPlayer(current_player_index)
                                    ->getCurrentWait() -
                            1);
            if (!player_factory->getPlayer(current_player_index)
                         ->getCurrentTank()
                         ->isAlive()) {
                player_factory->getPlayer(current_player_index)
                        ->setCurrentWait(-1);
            }
            current_player_index++;
            if (current_player_index == global_settings->getPlayerCount()) {
                current_player_index = 0;
            }
            current_player = player_factory->getPlayer(current_player_index);
        } while (player_factory->getPlayer(current_player_index)
                         ->getCurrentWait() != 0);
    }

    // STATUS EFFECT (ACID)
    if (current_player->getCurrentTank()->getDurationAcid() > 0) {
        playSFX(EFFECT_ACID);
        current_player->getCurrentTank()->dealDamage(
                current_player->getCurrentTank()->getBaseArmor() * 100 * 10 /
                100);
        if (current_player->getCurrentTank()->getHP() <= 0) {
            useTurn();  // skip turn if acid killed him at the beginning of the
                        // turn
        }
        // playSFX(ACID_EFFECT);
    }
    // STATUS EFFECT (PARALYZED)
    if (current_player->getCurrentTank()->getDurationParalyze() > 0) {
        useTurn();
    } else {
        if (current_player->getPlayerType() == "HUMAN") {
            inventory->setupInventory(current_player);
            Mix_HaltChannel(0);
            if (selected_weapon_img != nullptr) {
                delete selected_weapon_img;
                delete selected_weapon_remain;
            }
            if (current_player->getLoadedWeapon() != nullptr) {
                selected_weapon_img = new ImageObject(
                        weapon_slot->getXpos() * 1.01,
                        weapon_slot->getYpos() * 1.01,
                        2,
                        weapon_slot->getWidth() * 0.9,
                        weapon_slot->getHeight() * 0.9,
                        0,
                        256,
                        256,
                        current_player->getLoadedWeapon()->getImageFileName());
                std::string remain =
                        "x " + std::to_string(current_player->getLoadedWeapon()
                                                      ->getRemaining());
                selected_weapon_remain =
                        new TextObject(remain,
                                       weapon_slot->getXpos() * 1.1,
                                       weapon_slot->getYpos() * 1.3,
                                       3,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0.6,
                                       0.2,
                                       0.4);
            } else {
                selected_weapon_img = nullptr;
                selected_weapon_remain = nullptr;
            }
        }
    }
}

void GameState::resetTables(std::int32_t index) {
    if (index == -1) {
        for (std::int32_t i = 0; i < player_factory->getNumberofPlayers();
             i++) {
            for (std::int32_t j = 0; j < player_factory->getNumberofPlayers();
                 j++) {
                tank_reachable[i][j] = true;
                distance_to_target[i][j] = 1E+37;  // MAX FLOAT
            }
        }
    } else if (index < player_factory->getNumberofPlayers() && index >= 0) {
        for (std::int32_t i = 0; i < player_factory->getNumberofPlayers();
             i++) {
            tank_reachable[index][i] = true;
            distance_to_target[index][i] = 1E+37;  // MAX FLOAT
        }
    }
}

void GameState::printTables() {
    for (std::int32_t i = 0; i < player_factory->getNumberofPlayers(); i++) {
        for (std::int32_t j = 0; j < player_factory->getNumberofPlayers();
             j++) {
            cout << " || " << tank_list[i][j] << " | " << tank_reachable[i][j]
                 << " | " << distance_to_target[i][j] << " || ";
        }
        cout << endl;
    }
}

void GameState::controlAI() {
    // printTables();
    current_player->aiMainLogisticFunction();
}

void GameState::nearestEnemy() {
    if (current_player != nullptr &&
        current_player->getPlayerType() == "CPU") {
        /*	RECALULATE ALL DISTANCE	*/
        const float* tank_matrix =
                current_player->getCurrentTank()->getBodyMatrix();
        for (std::int32_t i = 0; i < number_of_players; i++) {
            if (tank_list[current_player_index][i] !=
                        current_player->getCurrentTank() &&
                tank_list[i] != nullptr) {
                const float* target_matrix =
                        tank_list[current_player_index][i]->getBodyMatrix();
                float distance =
                        sqrt((tank_matrix[12] - target_matrix[12]) *
                                     (tank_matrix[12] - target_matrix[12]) +
                             (tank_matrix[13] - target_matrix[13]) *
                                     (tank_matrix[13] - target_matrix[13]) +
                             (tank_matrix[14] - target_matrix[14]) *
                                     (tank_matrix[14] - target_matrix[14]));
                distance_to_target[current_player_index][i] = distance;
            }
        }

        /*	FIND THE MINIMUM DISTANCE/ REACHABLE TARGET	*/
        std::int32_t minimum_reachable_tank_index = -1;
        float minimum_distance = 1E+37;
        for (std::int32_t i = 0; i < number_of_players; i++) {
            if (tank_list[current_player_index][i] !=
                        current_player->getCurrentTank() &&
                tank_list[current_player_index][i] != nullptr) {
                if (distance_to_target[current_player_index][i] <
                            minimum_distance &&
                    tank_reachable[current_player_index][i] &&
                    tank_list[current_player_index][i]->isAlive() &&
                    tank_list[current_player_index][i]->getDurationCloak() ==
                            0 &&
                    (player_factory->getPlayer(current_player_index)
                                     ->getTeamLabel() !=
                             player_factory->getPlayer(i)->getTeamLabel() ||
                     player_factory->getPlayer(current_player_index)
                                     ->getTeamLabel() == '-')) {
                    minimum_reachable_tank_index = i;
                    minimum_distance =
                            distance_to_target[current_player_index][i];
                }
            }
        }
        /*	CHECK IF TANK FOUND	*/
        if (minimum_reachable_tank_index == -1) {
            current_player->setTarget(nullptr);
        } else {
            current_player->setTarget(tank_list[current_player_index]
                                               [minimum_reachable_tank_index]);
            tank_reachable[current_player_index]
                          [minimum_reachable_tank_index] = false;
        }
    }
}

GlobalSettings* GameState::getGlobalSettings() { return global_settings; }
float GameState::getGravity() { return gravity; }
PlayerFactory* GameState::getPlayerFactory() { return player_factory; }
float GameState::getBalisticScalar() { return balistic_scalar; }
vulkan_graphix::Math::Vec3<float> GameState::getPositionOfLastProjectile() {
    return position_of_last_projectile;
}
void GameState::setPositionOfLastProjectile(float x, float y, float z) {
    position_of_last_projectile.x = x;
    position_of_last_projectile.y = y;
    position_of_last_projectile.z = z;
}