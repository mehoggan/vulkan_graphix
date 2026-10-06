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
    m_scale_gravity = 30;
    m_balistic_scalar = 50;
    m_gravity = -9.8 * m_scale_gravity;

    m_timer = 0;

    m_current_game_state = new_current_game_state;
    m_need_help = false;
    m_start_music_played = false;
    m_prev_music_volume = 0;
    m_special_effect_timer = 0;
    m_radius_of_current_explosion = 0;
    m_special_effect_type = 0;
    m_special_effects = nullptr;
    m_draw_hit_box = false;

    for (std::int32_t x = 0; x < 256; x++) {
        m_key_monitor[x] = 0;
    }

    m_selected_weapon_img = nullptr;
    m_selected_weapon_remain = nullptr;
    m_weapon_slot = new ImageObject(0,
      0,
      2,
      new_width * 0.08,
      new_height * 0.11,
      0,
      1024,
      1024,
      "TestImage.raw");
    srand(time(nullptr));
    m_player_factory = new_player_factory;
    m_global_settings = new_global_settings;
    m_width = new_width;
    m_height = new_height;
    m_player_cam = false;
    m_chase_cam_active = false;
    m_projectile = nullptr;
    m_world_cam = new WorldCam(
      0, 20000, m_global_settings->getCurrentTerrain()->getActualSize() / 2);

    m_game_sub_state = PASS_TIME;
    m_current_player_index = 0;
    m_current_player = new_player_factory->getPlayer(m_current_player_index);

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
          new_global_settings->getCurrentTerrain()->getTriangleNormal(x, z);

        if (new_player_factory->getPlayer(i)->getCurrentTank()) {
            new_player_factory->getPlayer(i)->getCurrentTank()->orientTank(&n);
        } else {
            cout << "I lost a tank, HOW???" << endl;
        }

        // delete n;

        /*	FINALLY POSITION TANKS	*/
        y = new_global_settings->getCurrentTerrain()->getHeightAt(
          x * scale, z * scale);
        new_player_factory->getPlayer(i)->getCurrentTank()->setTankPos(
          x * scale, y, z * scale);
        m_number_of_players = new_global_settings->getPlayerCount();
    }

    //*
    if (m_global_settings) {
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

    m_offset = m_global_settings->getCurrentTerrain()->getActualSize() / 2.0;
    m_camera_x = 4000;
    m_camera_y = 15000;
    m_camera_z = 4000;
    //
    m_camera_radius =
      sqrt((pow(static_cast<double>(m_camera_x - m_offset), 2.0)) +
        (pow(static_cast<double>(m_camera_y), 2.0)) +
        (pow(static_cast<double>(m_camera_z - m_offset), 2.0)));
    m_plane_radius =
      sqrt((pow(static_cast<double>(m_camera_x - m_offset), 2.0)) +
        (pow(static_cast<double>(m_camera_z - m_offset), 2.0)));
    m_current_tank_theta = 0.0;
    m_current_world_theta = 0.0;
    m_current_tank_phi =
      45.0;  // NOTE THIS NEEDS TO BE UPDATED TO FIRST PLAYERS ANGLE

    m_skybox_factory = new SkyboxFactory(2048);
    m_ocean = new Water(400, 512);
    m_sfx_random = 0;
    m_projectile_fired = false;

    m_inventory = new Inventory(
      new_width * 0.25, new_height * 0.25, new_width * 0.5, new_height * 0.5);

    // Creating models for projectile.
    // IMPORTANT: Be careful about the order. It should match with the order in
    // that the weapons are created in the ShopMenu constructor
    for (std::int32_t i = 0; i < max_projectile_models; i++) {
        m_projectile_models[i] = new VBOShaderLibrary();
    }

    m_projectile_models[10]->loadClientData(
      "Projectiles/projectileDefault.ogl");
    m_projectile_models[10]->loadTexture(
      "Projectiles/projectileDefault.raw", 512, 512);

    m_projectile_models[0]->loadClientData("Projectiles/projectileBFB.ogl");
    m_projectile_models[0]->loadTexture(
      "Projectiles/projectileMFB.raw", 512, 512);

    m_projectile_models[1]->loadClientData("Projectiles/projectileBFB.ogl");
    m_projectile_models[1]->loadTexture(
      "Projectiles/projectileBFB.raw", 512, 512);

    m_projectile_models[2]->loadClientData("Projectiles/projectileAcid.ogl");
    m_projectile_models[2]->loadTexture(
      "Projectiles/projectileAcid.raw", 512, 512);

    m_projectile_models[3]->loadClientData("Projectiles/projectileThor.ogl");
    m_projectile_models[3]->loadTexture(
      "Projectiles/projectileThor.raw", 512, 512);

    m_projectile_models[4]->loadClientData("Projectiles/projectileEMP.ogl");
    m_projectile_models[4]->loadTexture(
      "Projectiles/projectileEMP.raw", 512, 512);

    m_projectile_models[5]->loadClientData(
      "Projectiles/projectileDefault.ogl");
    m_projectile_models[5]->loadTexture(
      "Projectiles/projectilePadlock.raw", 512, 512);

    m_projectile_models[6]->loadClientData(
      "Projectiles/projectileDefault.ogl");
    m_projectile_models[6]->loadTexture(
      "Projectiles/projectileRevive.raw", 512, 512);

    m_projectile_models[7]->loadClientData(
      "Projectiles/projectileDefault.ogl");
    m_projectile_models[7]->loadTexture(
      "Projectiles/projectileTeleport.raw", 512, 512);

    m_projectile_models[8]->loadClientData(
      "Projectiles/projectileDefault.ogl");
    m_projectile_models[8]->loadTexture(
      "Projectiles/projectileAtom.raw", 512, 512);

    m_projectile_models[9]->loadClientData("Projectiles/projectileNuke.ogl");
    m_projectile_models[9]->loadTexture(
      "Projectiles/projectileNuke.raw", 512, 512);

    m_manual = new ImageObject(new_width * -0.175,
      new_height * 0.25,
      2,
      new_width * 0.35,
      new_height * 0.55,
      new_width * 0.004,
      1024,
      1024,
      "manual.raw");

    /* AI VARIABLES	*/
    m_tank_reachable = new bool*[m_player_factory->getNumberofPlayers()];
    m_tank_list = new Tank**[m_player_factory->getNumberofPlayers()];
    m_distance_to_target = new float*[m_player_factory->getNumberofPlayers()];
    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        m_tank_list[i] = new Tank*[m_player_factory->getNumberofPlayers()];
        m_distance_to_target[i] =
          new float[m_player_factory->getNumberofPlayers()];
        m_tank_reachable[i] = new bool[m_player_factory->getNumberofPlayers()];
        for (std::int32_t j = 0; j < m_player_factory->getNumberofPlayers();
          j++) {
            m_tank_list[i][j] =
              m_player_factory->getPlayer(j)->getCurrentTank();
            m_distance_to_target[i][j] = 1E+37;  // MAX FLOAT
            m_tank_reachable[i][j] = true;
        }
    }
    cout << "Done Building Table" << endl;
    /* END OF AI VARIABLES */

    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        m_player_factory->getPlayer(i)->setLoadedWeapon(nullptr);
        m_player_factory->getPlayer(i)->getCurrentTank()->initDuration();
        m_player_factory->getPlayer(i)->getCurrentTank()->updateHitBox();
    }

    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        m_player_factory->getPlayer(i)->setGameState(this);
    }
}

GameState::~GameState() {
    Mix_HaltMusic();
    delete m_ocean;
    delete m_skybox_factory;
    delete m_world_cam;
    delete m_inventory;
    delete m_selected_weapon_img;
    delete m_selected_weapon_remain;
    delete m_weapon_slot;
    for (std::int32_t x = 0; x < max_projectile_models; x++) {
        delete m_projectile_models[x];
    }
    delete m_manual;
    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        delete[] m_tank_reachable[i];
    }
    delete[] m_tank_reachable;
    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        delete[] m_tank_list[i];
    }
    delete[] m_tank_list;
    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        delete[] m_distance_to_target[i];
    }
    delete[] m_distance_to_target;
}

float GameState::calcDistanceBetweenVertices(
  vulkan_graphix::Math::Vec3<float>* v0,
  vulkan_graphix::Math::Vec3<float>* v1) {
    return static_cast<float>(sqrt(
      pow((static_cast<double>(v0->x) - static_cast<double>(v1->x)), 2.0) +
      pow((static_cast<double>(v0->z) - static_cast<double>(v1->z)), 2.0)));
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
    if (m_game_sub_state == PLAYER_CONTROL) {
        // QUICK FIX FOR TURRET

        if (m_player_factory->getPlayer(m_current_player_index)
              ->getCurrentWait() > 0) {
            m_game_sub_state = PASS_TIME;
        }

        if (m_current_player->getPlayerType() == "CPU") {
            handlePlayerControlUpdates();
            if (m_current_player->getCurrentTank()->isAlive()) {
                controlAI();
            } else {
                cout << "Tank " << m_current_player_index << " is dead"
                     << endl;
            }
            updateWorldCam();
        } else if (m_current_player->getPlayerType() == "HUMAN") {
            handlePlayerControlUpdates();
            updateWorldCam();
        }
    }
    // else
    if (m_game_sub_state == PASS_TIME) {
        handlePassTime();
    }

    // projectile update/collision/special effect
    if (m_game_sub_state == PROJECTILE) {
        handleProjectileState();
        updateWorldCam();
    }
    if (m_game_sub_state == SPECIAL_EFFECT) {
        handleSpecialEffectState();
    }
}

void GameState::draw(render::RenderContext& context) {
    playBackgroundSounds();
    update();

    // This frame's camera: the gameplay projection the caller set, and
    // whichever view gluLookAt() used to multiply into the modelview.
    math::Mat4<float> view = context.view();
    if (m_game_sub_state == PROJECTILE || m_game_sub_state == SPECIAL_EFFECT) {
        if (m_chase_cam_active) view = view * m_projectile->chaseView();
    }
    if (!m_chase_cam_active) {
        if (!m_player_cam) {
            view = view * m_world_cam->view();
        } else {
            const float* turret_matrix =
              m_current_player->getCurrentTank()->getTurretMatrix();
            const float* head_matrix =
              m_current_player->getCurrentTank()->getHeadMatrix();
            view = view *
              glm::lookAt(
                math::Vec3<float>(head_matrix[12] + head_matrix[8] * 4000,
                  head_matrix[13] + 2000,
                  head_matrix[14] + head_matrix[10] * 4000),
                math::Vec3<float>(head_matrix[12] - head_matrix[8] * 3000,
                  head_matrix[13] + head_matrix[9] * 0,
                  head_matrix[14] - head_matrix[10] * 3000),
                math::Vec3<float>(0, 1, 0));
        }
    }
    context.setCamera(context.projection(), view);

    m_global_settings->getCurrentTerrain()->draw(context);
    m_skybox_factory->draw(context);

    if (m_current_player->getPlayerType() == "CPU") {
        if (m_current_player->getDrawDebugLinesandPlanes()) {
            m_current_player->drawTestLinesandPlanes(context);
        }
    }

    if (m_game_sub_state != SPECIAL_EFFECT) {
        if (m_projectile) {
            m_projectile->draw(context);
        }
    }

    // The ocean alone is translated (glPushMatrix()/glPopMatrix()).
    context.setCamera(context.projection(),
      glm::translate(
        view, math::Vec3<float>(-256.00f * 400, -5000.00f, -256.00f * 400)));
    m_ocean->draw(context);
    context.setCamera(context.projection(), view);

    // DRAW TANKS (the original set the current player's color here first,
    // but the tank shader never read it)
    for (std::int32_t i = 0; i < m_global_settings->getPlayerCount(); i++) {
        if (m_player_factory->getPlayer(i)
              ->getCurrentTank()
              ->getDurationCloak() == 0) {
            m_player_factory->getPlayer(i)->getCurrentTank()->draw(context);
            if (m_draw_hit_box) {
                m_player_factory->getPlayer(i)
                  ->getCurrentTank()
                  ->drawTankHitBox(context);
            }
        }
    }

    // DRAW EXPLOSION/SPECIAL EFFECT
    // specialEffectsCount =
    // (int)(SPECIAL_EFFECT_TIME_LIMIT/TIME_DIVISORS);
    if (m_game_sub_state == SPECIAL_EFFECT) {
        if (m_special_effect_type == EXPLOSION) {
            for (std::int32_t x = 0; x < m_special_effects_count; x++) {
                if (m_special_effect_timer > 0) {
                    m_special_effects[0]->draw(context);
                }
                if (m_special_effect_timer > 10) {
                    m_special_effects[1]->draw(context);
                }
                if (m_special_effect_timer > 20) {
                    m_special_effects[2]->draw(context);
                }
            }
        }
    }

    if (m_game_sub_state == PLAYER_CONTROL) {
        if (m_current_player->getPlayerType() == "CPU") {
            const float* turret_matrix =
              m_current_player->getCurrentTank()->getTurretMatrix();
            const float* body_matrix =
              m_current_player->getCurrentTank()->getBodyMatrix();
            // No color of its own: the original drew this in whatever GL's
            // current color was, which each tank mesh's draw call had just
            // set to red.
            const math::Vec4<float> color(1.0f, 0.0f, 0.0f, 1.0f);
            const std::vector<render::UiVertex> line = {
              {math::Vec3<float>(
                 turret_matrix[12], turret_matrix[13], turret_matrix[14]),
                color,
                math::Vec2<float>(0.0f)},
              {math::Vec3<float>(
                 body_matrix[12], body_matrix[13], body_matrix[14]),
                color,
                math::Vec2<float>(0.0f)}};
            context.drawTransient(line,
              vulkan_earth::pipelines().m_ui_lines,
              nullptr,
              math::Mat4<float>(1.0f),
              math::Vec4<float>(0.0f),
              1000);
        }
    }

    if (m_current_player->getCurrentTank()->getDurationEMP() > 0) {
        // the current player is in effect of EMP, so do not draw HUD and
        // minimap. He doesn't deserve them.
    } else {
        drawHUD(context);
        drawMinimap(context);
    }
    if (m_game_sub_state == INVENTORY) {
        m_inventory->draw(context);
    } else if (m_game_sub_state == HELP) {
        drawHelp(context);
    }
}

void GameState::drawHUD(render::RenderContext& context) {
    if (m_player_cam || m_chase_cam_active) {
        m_world_cam->setShakeCam(0);
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
    if (m_game_sub_state == PLAYER_CONTROL) {
        color = math::Vec4<float>(
          m_player_factory->getPlayer(m_current_player_index)->getRed(),
          m_player_factory->getPlayer(m_current_player_index)->getGreen(),
          m_player_factory->getPlayer(m_current_player_index)->getBlue(),
          1);
        drawHUDText(context,
          color,
          m_player_factory->getPlayer(m_current_player_index)->getPlayerName(),
          0,
          0.75 * new_y);
    }

    // List of Players and health/delay/team
    color = math::Vec4<float>(1, 1, 1, 1);
    drawHUDText(context, color, "Wait", -0.99 * new_x, 0.75 * new_y);
    drawHUDText(context, color, "HP", -0.87 * new_x, 0.75 * new_y);
    drawHUDText(context, color, "Player Name", -0.76 * new_x, 0.75 * new_y);
    drawHUDText(context, color, "Team", -0.525 * new_x, 0.75 * new_y);
    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        color = math::Vec4<float>(m_player_factory->getPlayer(i)->getRed(),
          m_player_factory->getPlayer(i)->getGreen(),
          m_player_factory->getPlayer(i)->getBlue(),
          1);
        drawHUDText(context,
          color,
          m_player_factory->getPlayer(i)->getPlayerName(),
          -0.76 * new_x,
          0.70 * new_y - 0.05 * new_y * i);
        std::string team(1, m_player_factory->getPlayer(i)->getTeamLabel());
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
               m_player_factory->getPlayer(i)->getCurrentWait())),
            buffer),
          -0.95 * new_x,
          0.70 * new_y - 0.05 * new_y * i);
        if (!m_player_factory->getPlayer(i)->getCurrentTank()->isAlive()) {
            color = math::Vec4<float>(1, 0, 0, 1);
        }
        if (m_player_factory->getPlayer(i)
              ->getCurrentTank()
              ->getDurationShield() > 0) {
            color = math::Vec4<float>(0, 0, 1, 1);
        }
        if (m_player_factory->getPlayer(i)
              ->getCurrentTank()
              ->getDurationAcid() > 0) {
            color = math::Vec4<float>(0, 1, 0, 1);
        }
        drawHUDText(context,
          color,
          (sprintf(buffer,
             "%d",
             m_player_factory->getPlayer(i)->getCurrentTank()->getHP()),
            buffer),
          -0.87 * new_x,
          0.70 * new_y - 0.05 * new_y * i);
    }

    // Power Output
    if (m_game_sub_state == PLAYER_CONTROL) {
        // Text
        float power_ratio = m_player_factory->getPlayer(m_current_player_index)
                              ->getCurrentTank()
                              ->getCurrentPower() /
          10.0;
        char buffer[128];
        memset(buffer, 0, 128);
        color = math::Vec4<float>(1, 1, 1, 1);
        drawHUDText(context, color, "Power:", 0.7 * new_x, -0.7 * new_y);
        drawHUDText(context,
          color,
          (sprintf(
             buffer, "%d", static_cast<std::int32_t>(power_ratio * 1000)),
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
             m_player_factory->getPlayer(m_current_player_index)
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
               m_player_factory->getPlayer(m_current_player_index)
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
             m_player_factory->getPlayer(m_current_player_index)
               ->getCurrentTank()
               ->getPreviousAngle()),
            buffer3),
          0.95 * new_x,
          -0.75 * new_y);

        // Show Weapon Slot
        m_weapon_slot->setXpos(-new_x * 0.7);
        m_weapon_slot->setYpos(-0.6 * new_y);
        m_weapon_slot->setZpos(2);
        m_weapon_slot->draw(context);
        if (m_current_player->getLoadedWeapon() != nullptr) {
            m_selected_weapon_img->setXpos(m_weapon_slot->getXpos() * 0.965);
            m_selected_weapon_img->setYpos(m_weapon_slot->getYpos() * 0.985);
            m_selected_weapon_img->setZpos(20);
            m_selected_weapon_remain->setXpos(
              m_weapon_slot->getXpos() * 0.965);
            m_selected_weapon_remain->setYpos(m_weapon_slot->getYpos() * 1.3);
            m_selected_weapon_remain->setZpos(22);
            m_selected_weapon_img->draw(context);
            m_selected_weapon_remain->draw(context);
        }

        // Display Need Help?
        if (m_need_help) {
            color = math::Vec4<float>(1, 1, 0, 1);
            drawHUDText(
              context, color, "Press F1 for help", -1.3 * new_x, -0.5 * new_y);
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
            loop.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
                                   (0.1 * c_glut_screen_width),
                                 0.05 * c_glut_screen_height,
                                 2),
                color,
                math::Vec2<float>(0.0f)});
            loop.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
                                   (0.1 * c_glut_screen_width),
                                 0.08 * c_glut_screen_height,
                                 2),
                color,
                math::Vec2<float>(0.0f)});
            loop.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) +
                                   (0.1 * c_glut_screen_width),
                                 0.08 * c_glut_screen_height,
                                 2),
                color,
                math::Vec2<float>(0.0f)});
            loop.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) +
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
              out_lines, vulkan_earth::pipelines().m_ui_lines, nullptr);
            color = math::Vec4<float>(power_ratio, 1 - power_ratio, 0, 1.0f);
            quad.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
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
            quad.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
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
            quad.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
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
            quad.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
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
              out_tris, vulkan_earth::pipelines().m_ui_triangles, nullptr);
        }

        // Graphic meter: Health
        float health_ratio = static_cast<float>(m_player_factory
                                 ->getPlayer(m_current_player_index)
                                 ->getCurrentTank()
                                 ->getHP()) /
          static_cast<float>(
            m_player_factory->getPlayer(m_current_player_index)
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
            loop.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
                                   (0.1 * c_glut_screen_width),
                                 0.90 * c_glut_screen_height,
                                 2),
                color,
                math::Vec2<float>(0.0f)});
            loop.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
                                   (0.1 * c_glut_screen_width),
                                 0.93 * c_glut_screen_height,
                                 2),
                color,
                math::Vec2<float>(0.0f)});
            loop.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) +
                                   (0.1 * c_glut_screen_width),
                                 0.93 * c_glut_screen_height,
                                 2),
                color,
                math::Vec2<float>(0.0f)});
            loop.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) +
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
              out_lines, vulkan_earth::pipelines().m_ui_lines, nullptr);
            color = math::Vec4<float>(1 - health_ratio, health_ratio, 0, 1.0f);
            quad.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
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
            quad.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
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
            quad.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
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
            quad.push_back(
              render::UiVertex{math::Vec3<float>((c_glut_screen_width / 2) -
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
              out_tris, vulkan_earth::pipelines().m_ui_triangles, nullptr);
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
        std::int32_t step =
          vulkan_earth::textAdvance(vulkan_earth::FontId::TimesRoman24, ch);
        context.drawText(font,
          math::Vec3<float>(x_pos, y, 0),
          std::string_view(&ch, 1),
          color);
        x_pos += step;
    }
}

void GameState::updateMouse(std::int32_t x, std::int32_t y) {
    if (m_player_cam) {
        float t_height = m_global_settings->getCurrentTerrain()->getHeightAt(
          m_current_player->getCurrentTank()->getHeadMatrix()[12] -
            50 * m_current_player->getCurrentTank()->getHeadMatrix()[8],
          m_current_player->getCurrentTank()->getHeadMatrix()[14] -
            50 * m_current_player->getCurrentTank()->getHeadMatrix()[10]);
        float tank_radius = 400.0f;

        if (y < m_old_mouse_y) {
            m_camera_y -= 10;
        } else if (y > m_old_mouse_y) {
            m_camera_y += 10;
        }

    } else {
        if (m_current_world_theta >= 360.0) {
            m_current_world_theta -= 360.0;
        } else if (m_current_world_theta < 0.0) {
            m_current_world_theta += 360.0;
        }
        if (x < m_old_mouse_x) {
            m_current_world_theta -= 1.0;
            m_camera_x =
              (m_plane_radius) * (cos(m_current_world_theta * (pi / 180))) +
              m_offset;
            m_camera_z =
              (m_plane_radius) * (sin(m_current_world_theta * (pi / 180))) +
              m_offset;
        } else if (x > m_old_mouse_x) {
            m_current_world_theta += 1.0;
            m_camera_x =
              (m_plane_radius) * (cos(m_current_world_theta * (pi / 180))) +
              m_offset;
            m_camera_z =
              (m_plane_radius) * (sin(m_current_world_theta * (pi / 180))) +
              m_offset;
        }
    }
    m_old_mouse_x = x;
    m_old_mouse_y = y;
}

void GameState::useTurn() {
    m_game_sub_state = PLAYER_CONTROL;
    if (m_current_player->getCurrentTank()->getDurationDoubleAction() == 0) {
        m_current_player->getCurrentTank()->setDurationAllPassTurn();
        m_current_player->setCurrentWait(
          150 - m_current_player->getCurrentTank()->getBaseSpeed());
    } else {
        m_current_player->getCurrentTank()->setDurationDoubleAction(
          m_current_player->getCurrentTank()->getDurationDoubleAction() - 1);
    }

    if (m_player_cam) {
        toggleCamera();
    }
}

// call this function every time someone is dead (or gets damaged).
// NOTE: call this once when game starts to check the case in which everyone is
// on the same team. Return values: -1==Draw game, 0==Nobody's won, n==Solo
// player num, n+100==Team num
std::int32_t GameState::getWinner() {
    std::int32_t winner = 0;

    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        if (m_player_factory->getPlayer(i)->getCurrentTank()->getHP() > 0) {
            // CHECK FOR SOLO WINNER
            if (m_player_factory->getPlayer(i)->getTeamLabel() == '-' &&
              winner == 0) {
                winner = i + 1;
            }
            // CHECK FOR TEAM WINNER
            else if ((m_player_factory->getPlayer(i)->getTeamLabel() - 48) +
                  100 ==
                winner ||
              winner == 0) {
                winner =
                  (m_player_factory->getPlayer(i)->getTeamLabel() - 48) + 100;
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
    if (m_game_sub_state == PROJECTILE) {
        if (m_chase_cam_active) {
            m_chase_cam_active = false;
            Mix_HaltChannel(1);
            m_player_cam = false;
        } else {
            m_chase_cam_active = true;
            Mix_HaltChannel(1);
            playSFX(BOMB_FLY);
            m_player_cam = false;
        }
    } else if (m_player_cam) {
        m_current_tank_theta = 0.0f;
        m_player_cam = false;
    } else {
        m_player_cam = true;
        m_camera_x = m_current_player->getCurrentTank()->getHeadMatrix()[12] +
          m_current_player->getCurrentTank()->getHeadMatrix()[8] * 1000;
        m_camera_y =
          m_current_player->getCurrentTank()->getHeadMatrix()[13] + 350;
        m_camera_z = m_current_player->getCurrentTank()->getHeadMatrix()[14] +
          m_current_player->getCurrentTank()->getHeadMatrix()[10] * 1000;
    }
}

void GameState::debugMode(render::RenderContext& context) {
    std::vector<render::UiVertex> out_lines;
    math::Vec4<float> color(1.0f);
    /*THIS IS DEBUG TEXT REMOVE LATER THIS JUST HELPS TO SEE IF TANKS ARE
     * ORIENTED CORRECTLY*/
    //*
    vulkan_graphix::Math::Vec3<float> n =
      m_current_player->getCurrentTank()->getAlignmentVector();
    color = math::Vec4<float>(0.00, 0.50, 0.50, 1.0f);
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12],
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 100,
        m_current_player->getCurrentTank()->getHeadMatrix()[14]),
      color,
      math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12] + 800 * n.x,
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 100 +
          800 * n.y,
        m_current_player->getCurrentTank()->getHeadMatrix()[14] + 800 * n.z),
      color,
      math::Vec2<float>(0.0f)});
    //*/
    //*/
    color = math::Vec4<float>(0.00, 0.00, 1.00, 1.0f);
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12],
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 100,
        m_current_player->getCurrentTank()->getHeadMatrix()[14]),
      color,
      math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12] +
          1000 * (m_current_player->getCurrentTank()->getHeadMatrix()[4]),
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 100 +
          1000 * (m_current_player->getCurrentTank()->getHeadMatrix()[5]),
        m_current_player->getCurrentTank()->getHeadMatrix()[14] +
          1000 * (m_current_player->getCurrentTank()->getHeadMatrix()[6])),
      color,
      math::Vec2<float>(0.0f)});
    //*/
    //*/
    vulkan_graphix::Math::Vec3<float> m =
      m_current_player->getCurrentTank()->getRotateAbout();
    color = math::Vec4<float>(0.75, 0.50, 0.50, 1.0f);
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12],
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 100,
        m_current_player->getCurrentTank()->getHeadMatrix()[14]),
      color,
      math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12] + 1000 * m.x,
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 100 +
          1000 * m.y,
        m_current_player->getCurrentTank()->getHeadMatrix()[14] + 1000 * m.z),
      color,
      math::Vec2<float>(0.0f)});
    //*/
    /*END OF TANK ORIENTATION DEBUGGING*/
    context.drawTransient(
      out_lines, vulkan_earth::pipelines().m_ui_lines, nullptr);
}

void GameState::drawMinimap(render::RenderContext& context) {
    // Its own viewport (glViewport()'s float -> int truncation kept),
    // cleared to the sea color.
    context.setViewport(
      vulkan_earth::glRect(static_cast<std::int32_t>(m_width - m_width / 5.8),
        static_cast<std::int32_t>(m_height - m_height / 4.8),
        static_cast<std::int32_t>(m_width / 6.0),
        static_cast<std::int32_t>(m_height / 5.0)));
    context.clearColorAndDepth(math::Vec4<float>(.22, .65, .60, 1));

    std::int32_t size = static_cast<std::int32_t>(
      m_global_settings->getCurrentTerrain()->getActualSize());

    context.setCamera(vulkan_graphix::Tools::getOrthographicProjectionMatrix(
                        -18000, 18000, 15000, -15000, 1, 20000000),
      glm::lookAt(
        math::Vec3<float>(size / 2, size + (size / m_width), size / 2),
        math::Vec3<float>(size / 2, 0, size / 2),
        math::Vec3<float>(1.0f, 0.0f, 0.0f)));

    m_global_settings->getCurrentTerrain()->draw(context);

    std::vector<render::UiVertex> out_tris;
    std::vector<render::UiVertex> out_lines;
    std::vector<render::UiVertex> quad;
    std::vector<render::UiVertex> loop;
    math::Vec4<float> color(1.0f);
    // Draw Tank Marks
    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        if (m_player_factory->getPlayer(i)
              ->getCurrentTank()
              ->getDurationCloak() == 0) {
            color =
              math::Vec4<float>(m_player_factory->collectPlayerColor(i)[0],
                m_player_factory->collectPlayerColor(i)[1],
                m_player_factory->collectPlayerColor(i)[2],
                1.0f);
            out_tris.push_back(
              render::UiVertex{math::Vec3<float>(m_player_factory->getPlayer(i)
                                                   ->getCurrentTank()
                                                   ->getHeadMatrix()[12] -
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[8] *
                                     900,
                                 m_player_factory->getPlayer(i)
                                     ->getCurrentTank()
                                     ->getHeadMatrix()[13] +
                                   12000,
                                 m_player_factory->getPlayer(i)
                                     ->getCurrentTank()
                                     ->getHeadMatrix()[14] -
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[10] *
                                     900),
                color,
                math::Vec2<float>(0.0f)});
            out_tris.push_back(
              render::UiVertex{math::Vec3<float>(m_player_factory->getPlayer(i)
                                                   ->getCurrentTank()
                                                   ->getHeadMatrix()[12] +
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[0] *
                                     700 +
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[8] *
                                     900,
                                 m_player_factory->getPlayer(i)
                                     ->getCurrentTank()
                                     ->getHeadMatrix()[13] +
                                   12000,
                                 m_player_factory->getPlayer(i)
                                     ->getCurrentTank()
                                     ->getHeadMatrix()[14] +
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[2] *
                                     700 +
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[10] *
                                     900),
                color,
                math::Vec2<float>(0.0f)});
            out_tris.push_back(
              render::UiVertex{math::Vec3<float>(m_player_factory->getPlayer(i)
                                                   ->getCurrentTank()
                                                   ->getHeadMatrix()[12] -
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[0] *
                                     700 +
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[8] *
                                     900,
                                 m_player_factory->getPlayer(i)
                                     ->getCurrentTank()
                                     ->getHeadMatrix()[13] +
                                   12000,
                                 m_player_factory->getPlayer(i)
                                     ->getCurrentTank()
                                     ->getHeadMatrix()[14] -
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[2] *
                                     700 +
                                   m_player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[10] *
                                     900),
                color,
                math::Vec2<float>(0.0f)});
        }
    }
    // Draw Current Player Tank's Aiming Line
    color = math::Vec4<float>(
      m_player_factory->collectPlayerColor(m_current_player_index)[0],
      m_player_factory->collectPlayerColor(m_current_player_index)[1],
      m_player_factory->collectPlayerColor(m_current_player_index)[2],
      0.50);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12] +
          m_current_player->getCurrentTank()->getHeadMatrix()[0] * 60,
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 10000,
        m_current_player->getCurrentTank()->getHeadMatrix()[14] +
          m_current_player->getCurrentTank()->getHeadMatrix()[2] * 60),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    quad.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12] +
          m_current_player->getCurrentTank()->getHeadMatrix()[0] * 60 -
          m_current_player->getCurrentTank()->getHeadMatrix()[8] * 24000,
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 10000,
        m_current_player->getCurrentTank()->getHeadMatrix()[14] +
          m_current_player->getCurrentTank()->getHeadMatrix()[2] * 60 -
          m_current_player->getCurrentTank()->getHeadMatrix()[10] * 24000),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    quad.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12] -
          m_current_player->getCurrentTank()->getHeadMatrix()[0] * 60 -
          m_current_player->getCurrentTank()->getHeadMatrix()[8] * 24000,
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 10000,
        m_current_player->getCurrentTank()->getHeadMatrix()[14] -
          m_current_player->getCurrentTank()->getHeadMatrix()[2] * 60 -
          m_current_player->getCurrentTank()->getHeadMatrix()[10] * 24000),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    quad.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getHeadMatrix()[12] -
          m_current_player->getCurrentTank()->getHeadMatrix()[0] * 60,
        m_current_player->getCurrentTank()->getHeadMatrix()[13] + 10000,
        m_current_player->getCurrentTank()->getHeadMatrix()[14] -
          m_current_player->getCurrentTank()->getHeadMatrix()[2] * 60),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }

    // Draw Lines of Sight of World Camera
    if (!m_player_cam && !m_chase_cam_active) {
        color = math::Vec4<float>(1, 1, 1, 0.8);
        loop.push_back(
          render::UiVertex{math::Vec3<float>(m_world_cam->getMatrix()[12] -
                               m_world_cam->getMatrix()[13] * 0.38,
                             10000,
                             m_world_cam->getMatrix()[14] +
                               m_world_cam->getMatrix()[13] - 15000 / m_width),
            color,
            math::Vec2<float>(0.0f)});
        loop.push_back(
          render::UiVertex{math::Vec3<float>(m_world_cam->getMatrix()[12] -
                               m_world_cam->getMatrix()[13] * 0.38,
                             10000,
                             m_world_cam->getMatrix()[14] -
                               m_world_cam->getMatrix()[13] + 15000 / m_width),
            color,
            math::Vec2<float>(0.0f)});
        loop.push_back(render::UiVertex{
          math::Vec3<float>(
            m_world_cam->getMatrix()[12] + m_world_cam->getMatrix()[13] * 0.82,
            10000,
            m_world_cam->getMatrix()[14] - m_world_cam->getMatrix()[13] +
              15000 / m_width - 1350),
          color,
          math::Vec2<float>(0.0f)});
        loop.push_back(render::UiVertex{
          math::Vec3<float>(
            m_world_cam->getMatrix()[12] + m_world_cam->getMatrix()[13] * 0.82,
            10000,
            m_world_cam->getMatrix()[14] + m_world_cam->getMatrix()[13] -
              15000 / m_width + 1350),
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
      out_tris, vulkan_earth::pipelines().m_ui_triangles, nullptr);
    context.drawTransient(
      out_lines, vulkan_earth::pipelines().m_ui_lines, nullptr);
    out_tris.clear();
    out_lines.clear();

    // Draw Projectile
    if (m_projectile) {
        color = math::Vec4<float>(1, 1, 1, 1.0f);
        quad.push_back(
          render::UiVertex{math::Vec3<float>(m_projectile->getPos()[0] - 200,
                             10000,
                             m_projectile->getPos()[2] - 200),
            color,
            math::Vec2<float>(0.0f)});
        if (quad.size() == 4) {
            for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                out_tris.push_back(quad[corner]);
            quad.clear();
        }
        quad.push_back(
          render::UiVertex{math::Vec3<float>(m_projectile->getPos()[0] - 200,
                             10000,
                             m_projectile->getPos()[2] + 200),
            color,
            math::Vec2<float>(0.0f)});
        if (quad.size() == 4) {
            for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                out_tris.push_back(quad[corner]);
            quad.clear();
        }
        quad.push_back(
          render::UiVertex{math::Vec3<float>(m_projectile->getPos()[0] + 200,
                             10000,
                             m_projectile->getPos()[2] + 200),
            color,
            math::Vec2<float>(0.0f)});
        if (quad.size() == 4) {
            for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
                out_tris.push_back(quad[corner]);
            quad.clear();
        }
        quad.push_back(
          render::UiVertex{math::Vec3<float>(m_projectile->getPos()[0] + 200,
                             10000,
                             m_projectile->getPos()[2] - 200),
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
      m_player_factory->collectPlayerColor(m_current_player_index)[0],
      m_player_factory->collectPlayerColor(m_current_player_index)[1],
      m_player_factory->collectPlayerColor(m_current_player_index)[2],
      1.0f);
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getProjectileLandPos()[0] - 900,
        10000,
        m_current_player->getCurrentTank()->getProjectileLandPos()[1] - 900),
      color,
      math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getProjectileLandPos()[0] + 900,
        10000,
        m_current_player->getCurrentTank()->getProjectileLandPos()[1] + 900),
      color,
      math::Vec2<float>(0.0f)});
    color = math::Vec4<float>(
      m_player_factory->collectPlayerColor(m_current_player_index)[0],
      m_player_factory->collectPlayerColor(m_current_player_index)[1],
      m_player_factory->collectPlayerColor(m_current_player_index)[2],
      1.0f);
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getProjectileLandPos()[0] + 900,
        10000,
        m_current_player->getCurrentTank()->getProjectileLandPos()[1] - 900),
      color,
      math::Vec2<float>(0.0f)});
    out_lines.push_back(render::UiVertex{
      math::Vec3<float>(
        m_current_player->getCurrentTank()->getProjectileLandPos()[0] - 900,
        10000,
        m_current_player->getCurrentTank()->getProjectileLandPos()[1] + 900),
      color,
      math::Vec2<float>(0.0f)});

    context.drawTransient(
      out_tris, vulkan_earth::pipelines().m_ui_triangles, nullptr);
    context.drawTransient(
      out_lines, vulkan_earth::pipelines().m_ui_lines, nullptr);

    vulkan_earth::resetToFullWindow(context);
}

void GameState::playBackgroundSounds() {
    if (m_global_settings->getHillGirth() == "Rock") {
        m_sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (m_sfx_random == 0)
            playSFX(WAVE1);
        else if (m_sfx_random == 1)
            playSFX(WAVE2);
        else if (m_sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        m_sfx_random = (rand() * 9) % 5000;
        if (m_sfx_random == 0)
            playSFX(SEAGULLS1);
        else if (m_sfx_random == 1)
            playSFX(SEAGULLS2);
        else if (m_sfx_random == 2)
            playSFX(SEAGULLS3);
        else if (m_sfx_random == 3)
            playSFX(SEAGULLS4);
        else if (m_sfx_random == 4)
            playSFX(SEAGULLS5);

        playMusic(gamestate_rock);
    } else if (m_global_settings->getHillGirth() == "Snow") {
        m_sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (m_sfx_random == 0)
            playSFX(WAVE1);
        else if (m_sfx_random == 1)
            playSFX(WAVE2);
        else if (m_sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        m_sfx_random = (rand() * 9) % 5000;
        if (m_sfx_random == 0)
            playSFX(SEAGULLS1);
        else if (m_sfx_random == 1)
            playSFX(SEAGULLS2);
        else if (m_sfx_random == 2)
            playSFX(SEAGULLS3);
        else if (m_sfx_random == 3)
            playSFX(SEAGULLS4);
        else if (m_sfx_random == 4)
            playSFX(SEAGULLS5);

        playMusic(gamestate_snow);
    } else if (m_global_settings->getHillGirth() == "Ice") {
        m_sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (m_sfx_random == 0)
            playSFX(WAVE1);
        else if (m_sfx_random == 1)
            playSFX(WAVE2);
        else if (m_sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);

        playMusic(gamestate_ice);
    } else if (m_global_settings->getHillGirth() == "Desert") {
        m_sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (m_sfx_random == 0)
            playSFX(WAVE1);
        else if (m_sfx_random == 1)
            playSFX(WAVE2);
        else if (m_sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        m_sfx_random = (rand() * 9) % 5000;
        if (m_sfx_random == 0)
            playSFX(SEAGULLS1);
        else if (m_sfx_random == 1)
            playSFX(SEAGULLS2);
        else if (m_sfx_random == 2)
            playSFX(SEAGULLS3);
        else if (m_sfx_random == 3)
            playSFX(SEAGULLS4);
        else if (m_sfx_random == 4)
            playSFX(SEAGULLS5);

        if (atoi(m_global_settings->getHillHeight().c_str()) < 2 &&
          atoi(m_global_settings->getHillyness().c_str()) > 4) {
            if (!m_start_music_played) {
                playMusic(gamestate_beach_start);
                m_start_music_played = true;
            } else {
                playMusic(gamestate_beach_loop);
            }
        } else {
            playMusic(gamestate_desert);
        }
    } else if (m_global_settings->getHillGirth() == "Mars") {
        m_sfx_random = rand() % 3000;  // set the frequency of the wave sound
        if (m_sfx_random == 0)
            playSFX(WAVE1);
        else if (m_sfx_random == 1)
            playSFX(WAVE2);
        else if (m_sfx_random == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);

        playMusic(gamestate_mars);
    } else if (m_global_settings->getHillGirth() == "Lava") {
        playMusic(gamestate_lava);
    }
}

void GameState::drawHelp(render::RenderContext& context) {
    // glViewport()'s float -> int truncation kept.
    vulkan_earth::beginOverlayPanel(context,
      vulkan_earth::glRect(static_cast<std::int32_t>(m_width * 0.25),
        static_cast<std::int32_t>(m_height * 0.1),
        static_cast<std::int32_t>(m_width * 0.5),
        static_cast<std::int32_t>(m_height * 0.8)),
      m_width,
      m_height);

    m_manual->draw(context);

    vulkan_earth::resetToFullWindow(context);
}

void GameState::handlePlayerControlUpdates() {
    if (m_key_monitor[1]) {
        playSFX(TANK_CONTROL2);
        if (m_current_player->getPlayerType() == "CPU") {
            m_current_player->updateBalsticMatrix();
            m_current_player->setUpYawVectors();
        }
        m_player_factory->getPlayer(m_current_player_index)
          ->getCurrentTank()
          ->rotateHead(-0.1 - 0.4 * m_key_monitor[1] / 50);
        if (m_current_player->getCurrentTank()->getName() == "Rhinoxx" ||
          m_current_player->getCurrentTank()->getName() == "HeavyD") {
            m_current_player->getCurrentTank()->rotateWheel(-.1);
        }
        m_camera_x = m_current_player->getCurrentTank()->getHeadMatrix()[12] +
          m_current_player->getCurrentTank()->getHeadMatrix()[8] * 1000;
        m_camera_z = m_current_player->getCurrentTank()->getHeadMatrix()[14] +
          m_current_player->getCurrentTank()->getHeadMatrix()[10] * 1000;
    } else if (m_key_monitor[3]) {
        playSFX(TANK_CONTROL2);
        if (m_current_player->getPlayerType() == "CPU") {
            m_current_player->updateBalsticMatrix();
            m_current_player->setUpYawVectors();
        }
        m_player_factory->getPlayer(m_current_player_index)
          ->getCurrentTank()
          ->rotateHead(0.1 + 0.4 * m_key_monitor[3] / 50);
        if (m_current_player->getCurrentTank()->getName() == "Rhinoxx" ||
          m_current_player->getCurrentTank()->getName() == "HeavyD") {
            m_current_player->getCurrentTank()->rotateWheel(.1);
        }
        m_camera_x = m_current_player->getCurrentTank()->getHeadMatrix()[12] +
          m_current_player->getCurrentTank()->getHeadMatrix()[8] * 1000;
        m_camera_z = m_current_player->getCurrentTank()->getHeadMatrix()[14] +
          m_current_player->getCurrentTank()->getHeadMatrix()[10] * 1000;
    } else {
        if (m_current_player->getPlayerType() != "CPU") {
            Mix_HaltChannel(3);
        }
    }

    if (m_key_monitor[2]) {
        if ((m_player_factory->getPlayer(m_current_player_index)
                ->getCurrentTank()
                ->getTurretDegrees() +
              0.1 + 0.4 * m_key_monitor[2] / 50) < 90) {
            playSFX(TANK_CONTROL1);
            m_player_factory->getPlayer(m_current_player_index)
              ->getCurrentTank()
              ->rotateTurret(0.1 + 0.4 * m_key_monitor[2] / 50);
        } else {
            Mix_HaltChannel(2);
            if (Mix_Playing(4) == 0) playSFX(TANK_STUCK);
        }
    } else if (m_key_monitor[4]) {
        if ((m_player_factory->getPlayer(m_current_player_index)
                ->getCurrentTank()
                ->getTurretDegrees() -
              0.1 - 0.4 * m_key_monitor[4] / 50) >= 0) {
            playSFX(TANK_CONTROL1);
            m_player_factory->getPlayer(m_current_player_index)
              ->getCurrentTank()
              ->rotateTurret(-0.1 - 0.4 * m_key_monitor[4] / 50);
        } else {
            Mix_HaltChannel(2);
            if (Mix_Playing(4) == 0) playSFX(TANK_STUCK);
        }
    } else {
        if (m_current_player->getPlayerType() != "CPU") {
            Mix_HaltChannel(2);
        }
    }

    if (m_key_monitor['-'])
        m_player_factory->getPlayer(m_current_player_index)
          ->getCurrentTank()
          ->adjustPower(-0.01 - 0.01 * m_key_monitor['-']);
    if (m_key_monitor['='])
        m_player_factory->getPlayer(m_current_player_index)
          ->getCurrentTank()
          ->adjustPower(0.01 + 0.01 * m_key_monitor['=']);
}

void GameState::destroyProjectile() {
    m_projectile_fired = false;
    m_chase_cam_active = false;
    delete m_projectile;
    m_projectile = nullptr;
}

bool GameState::getProjectileFired() { return m_projectile_fired; }

void GameState::updateWorldCam() {
    if (m_key_monitor['w'])
        m_world_cam->moveCam(20 + 20 * m_key_monitor['w'], 0, 0);
    if (m_key_monitor['a'])
        m_world_cam->moveCam(0, 0, -20 - 20 * m_key_monitor['a']);
    if (m_key_monitor['s'])
        m_world_cam->moveCam(-20 - 20 * m_key_monitor['s'], 0, 0);
    if (m_key_monitor['d'])
        m_world_cam->moveCam(0, 0, 20 + 20 * m_key_monitor['d']);
    if (m_key_monitor['r'])
        m_world_cam->moveCam(0, -20 - 20 * m_key_monitor['r'], 0);
    if (m_key_monitor['f'])
        m_world_cam->moveCam(0, 20 + 20 * m_key_monitor['f'], 0);
}

void GameState::createSpecialEffect() {
    float power_ratio = m_player_factory->getPlayer(m_current_player_index)
                          ->getCurrentTank()
                          ->getCurrentPower() /
      10.0;
    m_player_factory->getPlayer(m_current_player_index)
      ->getCurrentTank()
      ->setPreviousPower(static_cast<std::int32_t>(power_ratio * 1000));
    m_player_factory->getPlayer(m_current_player_index)
      ->getCurrentTank()
      ->setPreviousAngle(static_cast<std::int32_t>(
                           m_player_factory->getPlayer(m_current_player_index)
                             ->getCurrentTank()
                             ->getTurretDegrees()) +
        1);
    m_player_factory->getPlayer(m_current_player_index)
      ->getCurrentTank()
      ->setProjectileLandPos(
        m_projectile->getPos()[0], m_projectile->getPos()[2]);
    m_special_effect_x = m_projectile->getPos()[0];
    m_special_effect_y = m_projectile->getPos()[1];
    m_special_effect_z = m_projectile->getPos()[2];
    // Teleport weapon effect handling
    if (m_projectile->getWeapon()->getImageFileName() ==
      "WeaponTeleport.raw") {
        std::int32_t size =
          m_global_settings->getCurrentTerrain()->getActualSize();
        // Make sure the projectile has not landed on the water
        if (0 < m_projectile->getPos()[0] &&
          m_projectile->getPos()[0] < size && 0 < m_projectile->getPos()[2] &&
          m_projectile->getPos()[2] < size) {
            std::int32_t new_x =
              static_cast<std::int32_t>(m_projectile->getPos()[0] / 100.0) *
              100;
            std::int32_t new_y =
              static_cast<std::int32_t>(m_projectile->getPos()[1] / 100.0) *
              100;
            std::int32_t new_z =
              static_cast<std::int32_t>(m_projectile->getPos()[2] / 100.0) *
              100;
            m_current_player->getCurrentTank()->setTankPos(
              new_x, new_y, new_z);
            const float* body_matrix =
              m_current_player->getCurrentTank()->getBodyMatrix();
            float new_height =
              m_global_settings->getCurrentTerrain()->getHeightAt(
                body_matrix[12], body_matrix[14]);
            std::int32_t scale = static_cast<std::int32_t>(
              m_global_settings->getCurrentTerrain()->getScale());
            vulkan_graphix::Math::Vec3<float> n =
              m_global_settings->getCurrentTerrain()->getTriangleNormal(
                body_matrix[12] / scale, body_matrix[14] / scale);
            m_current_player->getCurrentTank()->orientTank(&n);
            m_current_player->getCurrentTank()->setTankPos(
              body_matrix[12], new_height, body_matrix[14]);
        }
    }
    m_projectile->getWeapon()->playExplosionSFX();
    m_radius_of_current_explosion = m_projectile->getWeapon()->getRadius();
    m_special_effect_type = EXPLOSION;

    if (m_special_effect_type == EXPLOSION) {
        m_special_effects_count = (special_effect_time_limit / time_divisors);
        m_special_effects = new SpecialEffect*[m_special_effects_count];
        for (std::int32_t x = 0; x < m_special_effects_count; x++) {
            m_special_effects[x] = new Explosion(m_special_effect_x,
              m_special_effect_y,
              m_special_effect_z,
              static_cast<std::int32_t>(m_radius_of_current_explosion));
            if (m_projectile->getWeapon() != nullptr) {
                m_special_effects[x]->setColors1(
                  m_projectile->getWeapon()->getExplosionColor1());
                m_special_effects[x]->setColors2(
                  m_projectile->getWeapon()->getExplosionColor2());
                m_special_effects[x]->setColors3(
                  m_projectile->getWeapon()->getExplosionColor3());
                m_special_effects[x]->setColors4(
                  m_projectile->getWeapon()->getExplosionColor4());
            }
        }
    }
    m_game_sub_state = SPECIAL_EFFECT;
}

void GameState::handleProjectileState() {
    bool collision_occured = false;
    if (m_projectile) {
        if (m_projectile->getPos()[1] <=
          m_global_settings->getCurrentTerrain()->getHeightAt(
            m_projectile->getPos()[0], m_projectile->getPos()[2])) {
            collision_occured = true;
        } else {
            for (std::int32_t i = 0; i < m_number_of_players; i++) {
                if (m_player_factory->getPlayer(i)
                      ->getCurrentTank()
                      ->checkCollision(m_projectile->getPos()[0],
                        m_projectile->getPos()[1],
                        m_projectile->getPos()[2])) {
                    collision_occured = true;
                }
            }
        }
        if (collision_occured) {
            /* RESET AI VARIABLES	*/
            m_timer = 0;
            /* DONE RESETING AI VARIABLES */
            setPositionOfLastProjectile(m_projectile->getPos()[0],
              m_projectile->getPos()[1],
              m_projectile->getPos()[2]);
            // Make sure chase cam does not shake till impact
            m_projectile->getChaseCam()->setShakeCam(0);
            createSpecialEffect();
        } else {
            m_timer = m_timer + .02f;
            // Note: gravity is negative
            const vulkan_graphix::Math::Vec3<float> position =
              vulkan_graphix::Ballistics::positionAt(
                m_projectile->getLaunch(), m_gravity, m_timer);
            m_projectile->update(position.x, position.y, position.z);
        }
    } else {
        float tank_attribute_power =
          m_player_factory->getPlayer(m_current_player_index)
            ->getCurrentTank()
            ->getPower();
        float power_bar = m_player_factory->getPlayer(m_current_player_index)
                            ->getCurrentTank()
                            ->getCurrentPower();
        const float* turret_matrix =
          m_player_factory->getPlayer(m_current_player_index)
            ->getCurrentTank()
            ->getTurretMatrix();
        float matrix[16];
        for (std::int32_t i = 0; i < 16; i++) {
            matrix[i] = turret_matrix[i];
        }
        matrix[12] = turret_matrix[12] + 1000 * turret_matrix[4];
        matrix[13] = turret_matrix[13] + 1000 * turret_matrix[5];
        matrix[13] = turret_matrix[14] + 1000 * turret_matrix[6];
        m_projectile = new Projectile(this,
          matrix,
          tank_attribute_power * power_bar * m_balistic_scalar,
          m_projectile_models);
        if (m_current_player->getLoadedWeapon() != nullptr) {
            m_projectile->setWeapon(m_current_player->getLoadedWeapon());
        } else {
            m_projectile->setWeapon(m_projectile->getDefaultWeapon());
        }
        m_special_effect_type = EXPLOSION;
        createSpecialEffect();
    }
}

void GameState::currentPlayerFire() {
    if (m_current_player->getPlayerType() == "HUMAN") {
        // just in case
        if (m_projectile) {
            destroyProjectile();
        }
        m_game_sub_state = PROJECTILE;
        constructProjectile();
        if (m_projectile) {
            if (m_current_player->getLoadedWeapon() != nullptr) {
                m_projectile->setWeapon(m_current_player->getLoadedWeapon());
            } else {
                m_projectile->setWeapon(m_projectile->getDefaultWeapon());
            }
        }
    } else if (m_current_player->getPlayerType() == "CPU") {
        if (m_projectile) {
            destroyProjectile();
        }
        m_game_sub_state = PROJECTILE;
        constructProjectile();
        if (m_current_player->getLoadedWeapon() != nullptr) {
            if (m_projectile) {
                m_projectile->setWeapon(m_current_player->getLoadedWeapon());
                m_chase_cam_active = true;
            }
        } else {
            if (m_projectile) {
                m_projectile->setWeapon(m_projectile->getDefaultWeapon());
                m_chase_cam_active = true;
            }
        }
    }
    if (m_projectile) {
        m_projectile->getWeapon()->playFireSFX();
    }
}

void GameState::constructProjectile() {
    float tank_attribute_power =
      m_player_factory->getPlayer(m_current_player_index)
        ->getCurrentTank()
        ->getPower();
    float power_bar = m_player_factory->getPlayer(m_current_player_index)
                        ->getCurrentTank()
                        ->getCurrentPower();

    const float* turret_matrix =
      m_current_player->getCurrentTank()->getTurretMatrix();
    // Refuses to fire if a point just past the muzzle is already below the
    // terrain (e.g. the barrel is buried in a hillside).
    const vulkan_graphix::Math::Vec3<float> barrel_probe =
      vulkan_graphix::Ballistics::pointAlongBarrel(
        glm::make_mat4(turret_matrix), 700.0f);
    float land_pos[3] = {barrel_probe.x, barrel_probe.y, barrel_probe.z};

    if (land_pos[1] < m_global_settings->getCurrentTerrain()->getHeightAt(
                        land_pos[0], land_pos[2])) {
        m_projectile = nullptr;
    } else {
        m_projectile = new Projectile(this,
          m_player_factory->getPlayer(m_current_player_index)
            ->getCurrentTank()
            ->getTurretMatrix(),
          tank_attribute_power * power_bar * m_balistic_scalar,
          m_projectile_models);
    }
}

void GameState::handleSpecialEffectState() {
    m_special_effect_timer++;
    if (m_special_effect_timer < special_effect_time_limit) {
        if (m_special_effect_timer == 1) {
            if (m_projectile->getWeapon()->getImageFileName() !=
              "WeaponRevive.raw") {
                m_global_settings->getCurrentTerrain()->makeCrater(
                  m_projectile->getPos()[0],
                  m_projectile->getPos()[2],
                  m_radius_of_current_explosion);
            }
            std::int32_t total_players = m_global_settings->getPlayerCount();
            for (std::int32_t p = 0; p < total_players; p++) {
                if (m_player_factory->getPlayer(p)->getCurrentTank() !=
                  nullptr) {
                    const float* body_matrix = m_player_factory->getPlayer(p)
                                                 ->getCurrentTank()
                                                 ->getBodyMatrix();
                    vulkan_graphix::Math::Vec3<float> v0(
                      body_matrix[12], body_matrix[13], body_matrix[14]);
                    vulkan_graphix::Math::Vec3<float> v1(
                      m_projectile->getPos()[0],
                      m_projectile->getPos()[1],
                      m_projectile->getPos()[2]);
                    float distance = calcDistanceBetweenVertices(&v0, &v1);
                    float scale =
                      m_global_settings->getCurrentTerrain()->getScale();
                    if (distance < m_radius_of_current_explosion * scale) {
                        // PLACEHOLDER BELOW, pass the tank into the weapon and
                        // call dealDamage (or whatever) from there
                        m_projectile->getWeapon()->causeEffectToTank(distance,
                          m_player_factory->getPlayer(p)->getCurrentTank());
                        if (m_player_factory->getPlayer(p)
                              ->getCurrentTank()
                              ->getDurationFloat() > 0) {
                            vulkan_graphix::Math::Vec3<float>* n =
                              new vulkan_graphix::Math::Vec3<float>(0, 1, 0);

                            m_player_factory->getPlayer(p)
                              ->getCurrentTank()
                              ->orientTank(n);
                            m_player_factory->getPlayer(p)
                              ->getCurrentTank()
                              ->setTankPos(m_player_factory->getPlayer(p)
                                             ->getCurrentTank()
                                             ->getBodyMatrix()[12],
                                m_player_factory->getPlayer(p)
                                  ->getCurrentTank()
                                  ->getBodyMatrix()[13],
                                m_player_factory->getPlayer(p)
                                  ->getCurrentTank()
                                  ->getBodyMatrix()[14]);
                            delete n;
                        } else {
                            std::int32_t size = static_cast<std::int32_t>(
                              m_global_settings->getCurrentTerrain()
                                ->getActualSize());
                            std::int32_t scale_int = static_cast<std::int32_t>(
                              m_global_settings->getCurrentTerrain()
                                ->getScale());
                            float new_height =
                              m_global_settings->getCurrentTerrain()
                                ->getHeightAt(
                                  body_matrix[12], body_matrix[14]);

                            vulkan_graphix::Math::Vec3<float> n =
                              m_global_settings->getCurrentTerrain()
                                ->getTriangleNormal(
                                  body_matrix[12] / scale_int,
                                  body_matrix[14] / scale_int);

                            m_player_factory->getPlayer(p)
                              ->getCurrentTank()
                              ->orientTank(&n);
                            m_player_factory->getPlayer(p)
                              ->getCurrentTank()
                              ->setTankPos(
                                body_matrix[12], new_height, body_matrix[14]);
                            m_player_factory->getPlayer(p)
                              ->getCurrentTank()
                              ->checkFallingDamage();
                        }
                    }
                }
            }
        }
        m_projectile->getChaseCam()->updateFactor();
        std::int32_t shake_it_baby = rand() % 30;
        m_projectile->getChaseCam()->setShakeCam(20 + shake_it_baby);
    } else {
        if (m_special_effect_type == EXPLOSION) {
            for (std::int32_t x = 0; x < m_special_effects_count; x++) {
                if (m_special_effects[x]) {
                    delete m_special_effects[x];
                }
            }

            delete m_special_effects;
        }
        m_projectile->getChaseCam()->setShakeCam(0);
        m_projectile->getChaseCam()->resetFactor();
        destroyProjectile();
        m_special_effect_x = 0;
        m_special_effect_y = 0;
        m_special_effect_z = 0;
        m_special_effect_timer = 0;
        m_special_effects_count = 0;
        m_radius_of_current_explosion = 0;
        useTurn();
        m_game_sub_state = PASS_TIME;
        std::int32_t inven_index = 0;
        m_inventory->handleInventory(m_current_player, inven_index);
    }
}

void GameState::handleKeyboardInput(std::int32_t key, bool key_status) {
    if (m_current_player->getPlayerType() == "CPU") {
        handleNonInventoryKeyboard(key, key_status);
    }
    if (m_current_player->getPlayerType() ==
      "HUMAN") {  // TEMP TEST FOR CPU PLAYERS REMOVE CPU'S DON'T USE
                  // KEYBOARDS
        if (m_game_sub_state == INVENTORY) {
            handleInventoryKeyboard(key, key_status);
        } else if (m_game_sub_state != INVENTORY) {
            handleNonInventoryKeyboard(key, key_status);
        }
    }
}

void GameState::handleInventoryKeyboard(std::int32_t key, bool key_status) {
    if (key_status) {
        if (key == 27 || key == 'i') {
            Mix_VolumeMusic(m_prev_music_volume * 3);
            m_game_sub_state = PLAYER_CONTROL;
        } else if (key == 13) {
            Mix_VolumeMusic(m_prev_music_volume * 3);
            std::int32_t index = m_inventory->getSelectedIndex();
            // UN/LOAD A WEAPON
            if (index < player_max_weapons &&
              m_current_player->getCurrentWeapons()[index] != nullptr) {
                // UNLOADING
                if (m_current_player->getLoadedWeapon() != nullptr &&
                  m_current_player->getLoadedWeapon()->getUNIQUEIDENTIFIER() ==
                    m_current_player->getCurrentWeapons()[index]
                      ->getUNIQUEIDENTIFIER()) {
                    playSFX(WEAPON_UNLOAD);
                    delete m_selected_weapon_img;
                    delete m_selected_weapon_remain;
                    m_current_player->setLoadedWeapon(nullptr);
                    m_selected_weapon_img = nullptr;
                    m_selected_weapon_remain = nullptr;
                }
                // LOADING
                else {
                    playSFX(WEAPON_LOAD);
                    if (m_selected_weapon_img != nullptr) {
                        delete m_selected_weapon_img;
                        delete m_selected_weapon_remain;
                    }
                    m_current_player->setLoadedWeapon(
                      m_current_player->getCurrentWeapons()[index]);
                    m_selected_weapon_img = new ImageObject(
                      m_weapon_slot->getXpos() * 1.01,
                      m_weapon_slot->getYpos() * 1.01,
                      2,
                      m_weapon_slot->getWidth() * 0.9,
                      m_weapon_slot->getHeight() * 0.9,
                      0,
                      256,
                      256,
                      m_current_player->getLoadedWeapon()->getImageFileName());
                    std::string remain = "x " +
                      std::to_string(
                        m_current_player->getLoadedWeapon()->getRemaining());
                    m_selected_weapon_remain = new TextObject(remain,
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
              m_current_player
                  ->getCurrentItems()[index - player_max_weapons] != nullptr) {
                // If causeEffectToTank(...) returns true, that means player
                // has used an item which costs 1 turn
                if (m_current_player
                      ->getCurrentItems()[index - player_max_weapons]
                      ->causeEffectToTank(
                        m_current_player->getCurrentTank())) {
                    useTurn();
                }
                m_current_player->getCurrentItems()[index - player_max_weapons]
                  ->playUseSFX();
                m_inventory->handleInventory(m_current_player, index);
                while (Mix_Playing(0));
                m_inventory->setupInventory(m_current_player);
                Mix_HaltChannel(0);
            }
            m_game_sub_state = PLAYER_CONTROL;
        } else {
            m_inventory->keyHandler(key);
        }
    }
}

void GameState::handleNonInventoryKeyboard(std::int32_t key, bool key_status) {
    if ((key == 'c') && (key_status))
        toggleCamera();
    else if ((key == ' ') && (key_status) &&
      (m_game_sub_state == PLAYER_CONTROL)) {
        currentPlayerFire();
    } else if ((key == 5) && (key_status) &&
      (m_game_sub_state == PLAYER_CONTROL)) {
        m_prev_music_volume = Mix_VolumeMusic(-1);
        m_prev_music_volume = Mix_VolumeMusic(m_prev_music_volume / 3);
        playSFX(MANUAL);
        m_game_sub_state = HELP;
        m_need_help = false;
    } else if ((key == 5) && (key_status) && (m_game_sub_state == HELP)) {
        Mix_VolumeMusic(m_prev_music_volume * 3);
        m_game_sub_state = PLAYER_CONTROL;
        m_need_help = false;
    } else if ((key == 6) && (key_status)) {
        Mix_HaltMusic();
        Mix_HaltChannel(-1);
        *m_current_game_state = MAIN_MENU;
    } else if ((key == 'i') && (key_status) &&
      (m_game_sub_state == PLAYER_CONTROL)) {
        if (m_current_player->getPlayerType() == "HUMAN" &&
          m_current_player->getCurrentTank()->getDurationPadlock() == 0) {
            Mix_HaltChannel(2);
            Mix_HaltChannel(3);
            m_key_monitor[1] = 0;
            m_key_monitor[2] = 0;
            m_key_monitor[3] = 0;
            m_key_monitor[4] = 0;
            m_prev_music_volume =
              Mix_VolumeMusic(-1);  // -1 returns the current volume. Other
                                    // numbers will change the volume, and it
                                    // returns the volume before changed
            m_prev_music_volume = Mix_VolumeMusic(m_prev_music_volume / 3);
            playSFX(INVENTORY_ACCESS);
            m_game_sub_state = INVENTORY;
        } else {
            // playSFX(INVENTORY_INVALID);
        }
    } else if (key == 'b' && !key_status) {
        m_draw_hit_box = !m_draw_hit_box;
    } else if (key == 'p' && !key_status) {
        if (m_current_player->getPlayerType() == "CPU") {
            if (m_current_player->getDrawDebugLinesandPlanes())
                m_current_player->setDrawDebugLinesandPlanes(false);
            else
                m_current_player->setDrawDebugLinesandPlanes(true);
        }
    } else {
        if (key_status) {
            m_need_help = key > 6 && key != 'w' && key != 'a' && key != 's' &&
              key != 'd' && key != 'r' && key != 'f' && key != 'c' &&
              key != '-' && key != '=';
            m_key_monitor[key] += 1;
        } else {
            m_key_monitor[key] = 0;
        }
    }
}

void GameState::handlePassTime() {
    if ((m_player_factory->getPlayer(m_current_player_index)
            ->getCurrentWait() <= 0) &&
      (m_player_factory->getPlayer(m_current_player_index)
          ->getCurrentTank()
          ->isAlive())) {
        m_game_sub_state = PLAYER_CONTROL;
        m_current_player = m_player_factory->getPlayer(m_current_player_index);
    } else {
        do {
            m_player_factory->getPlayer(m_current_player_index)
              ->setCurrentWait(
                m_player_factory->getPlayer(m_current_player_index)
                  ->getCurrentWait() -
                1);
            if (!m_player_factory->getPlayer(m_current_player_index)
                  ->getCurrentTank()
                  ->isAlive()) {
                m_player_factory->getPlayer(m_current_player_index)
                  ->setCurrentWait(-1);
            }
            m_current_player_index++;
            if (m_current_player_index ==
              m_global_settings->getPlayerCount()) {
                m_current_player_index = 0;
            }
            m_current_player =
              m_player_factory->getPlayer(m_current_player_index);
        } while (m_player_factory->getPlayer(m_current_player_index)
                   ->getCurrentWait() != 0);
    }

    // STATUS EFFECT (ACID)
    if (m_current_player->getCurrentTank()->getDurationAcid() > 0) {
        playSFX(EFFECT_ACID);
        m_current_player->getCurrentTank()->dealDamage(
          m_current_player->getCurrentTank()->getBaseArmor() * 100 * 10 / 100);
        if (m_current_player->getCurrentTank()->getHP() <= 0) {
            useTurn();  // skip turn if acid killed him at the beginning of the
                        // turn
        }
        // playSFX(ACID_EFFECT);
    }
    // STATUS EFFECT (PARALYZED)
    if (m_current_player->getCurrentTank()->getDurationParalyze() > 0) {
        useTurn();
    } else {
        if (m_current_player->getPlayerType() == "HUMAN") {
            m_inventory->setupInventory(m_current_player);
            Mix_HaltChannel(0);
            if (m_selected_weapon_img != nullptr) {
                delete m_selected_weapon_img;
                delete m_selected_weapon_remain;
            }
            if (m_current_player->getLoadedWeapon() != nullptr) {
                m_selected_weapon_img =
                  new ImageObject(m_weapon_slot->getXpos() * 1.01,
                    m_weapon_slot->getYpos() * 1.01,
                    2,
                    m_weapon_slot->getWidth() * 0.9,
                    m_weapon_slot->getHeight() * 0.9,
                    0,
                    256,
                    256,
                    m_current_player->getLoadedWeapon()->getImageFileName());
                std::string remain = "x " +
                  std::to_string(
                    m_current_player->getLoadedWeapon()->getRemaining());
                m_selected_weapon_remain = new TextObject(remain,
                  m_weapon_slot->getXpos() * 1.1,
                  m_weapon_slot->getYpos() * 1.3,
                  3,
                  vulkan_earth::FontId::TimesRoman24,
                  0.6,
                  0.2,
                  0.4);
            } else {
                m_selected_weapon_img = nullptr;
                m_selected_weapon_remain = nullptr;
            }
        }
    }
}

void GameState::resetTables(std::int32_t index) {
    if (index == -1) {
        for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers();
          i++) {
            for (std::int32_t j = 0;
              j < m_player_factory->getNumberofPlayers();
              j++) {
                m_tank_reachable[i][j] = true;
                m_distance_to_target[i][j] = 1E+37;  // MAX FLOAT
            }
        }
    } else if (index < m_player_factory->getNumberofPlayers() && index >= 0) {
        for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers();
          i++) {
            m_tank_reachable[index][i] = true;
            m_distance_to_target[index][i] = 1E+37;  // MAX FLOAT
        }
    }
}

void GameState::printTables() {
    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        for (std::int32_t j = 0; j < m_player_factory->getNumberofPlayers();
          j++) {
            cout << " || " << m_tank_list[i][j] << " | "
                 << m_tank_reachable[i][j] << " | "
                 << m_distance_to_target[i][j] << " || ";
        }
        cout << endl;
    }
}

void GameState::controlAI() {
    // printTables();
    m_current_player->aiMainLogisticFunction();
}

void GameState::nearestEnemy() {
    if (m_current_player != nullptr &&
      m_current_player->getPlayerType() == "CPU") {
        /*	RECALULATE ALL DISTANCE	*/
        const float* tank_matrix =
          m_current_player->getCurrentTank()->getBodyMatrix();
        for (std::int32_t i = 0; i < m_number_of_players; i++) {
            if (m_tank_list[m_current_player_index][i] !=
                m_current_player->getCurrentTank() &&
              m_tank_list[i] != nullptr) {
                const float* target_matrix =
                  m_tank_list[m_current_player_index][i]->getBodyMatrix();
                float distance = sqrt((tank_matrix[12] - target_matrix[12]) *
                    (tank_matrix[12] - target_matrix[12]) +
                  (tank_matrix[13] - target_matrix[13]) *
                    (tank_matrix[13] - target_matrix[13]) +
                  (tank_matrix[14] - target_matrix[14]) *
                    (tank_matrix[14] - target_matrix[14]));
                m_distance_to_target[m_current_player_index][i] = distance;
            }
        }

        /*	FIND THE MINIMUM DISTANCE/ REACHABLE TARGET	*/
        std::int32_t minimum_reachable_tank_index = -1;
        float minimum_distance = 1E+37;
        for (std::int32_t i = 0; i < m_number_of_players; i++) {
            if (m_tank_list[m_current_player_index][i] !=
                m_current_player->getCurrentTank() &&
              m_tank_list[m_current_player_index][i] != nullptr) {
                if (m_distance_to_target[m_current_player_index][i] <
                    minimum_distance &&
                  m_tank_reachable[m_current_player_index][i] &&
                  m_tank_list[m_current_player_index][i]->isAlive() &&
                  m_tank_list[m_current_player_index][i]->getDurationCloak() ==
                    0 &&
                  (m_player_factory->getPlayer(m_current_player_index)
                        ->getTeamLabel() !=
                      m_player_factory->getPlayer(i)->getTeamLabel() ||
                    m_player_factory->getPlayer(m_current_player_index)
                        ->getTeamLabel() == '-')) {
                    minimum_reachable_tank_index = i;
                    minimum_distance =
                      m_distance_to_target[m_current_player_index][i];
                }
            }
        }
        /*	CHECK IF TANK FOUND	*/
        if (minimum_reachable_tank_index == -1) {
            m_current_player->setTarget(nullptr);
        } else {
            m_current_player->setTarget(
              m_tank_list[m_current_player_index]
                         [minimum_reachable_tank_index]);
            m_tank_reachable[m_current_player_index]
                            [minimum_reachable_tank_index] = false;
        }
    }
}

GlobalSettings* GameState::getGlobalSettings() { return m_global_settings; }
float GameState::getGravity() { return m_gravity; }
PlayerFactory* GameState::getPlayerFactory() { return m_player_factory; }
float GameState::getBalisticScalar() { return m_balistic_scalar; }
vulkan_graphix::Math::Vec3<float> GameState::getPositionOfLastProjectile() {
    return m_position_of_last_projectile;
}
void GameState::setPositionOfLastProjectile(float x, float y, float z) {
    m_position_of_last_projectile.x = x;
    m_position_of_last_projectile.y = y;
    m_position_of_last_projectile.z = z;
}