#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdio.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include "vulkan_earth/Normal.h"
#include "vulkan_earth/Vector.h"
#include "vulkan_earth/Vertex.h"
#include "vulkan_earth/render/RenderTypes.h"

namespace vulkan_earth::render {
class RenderContext;
}

using namespace std;

class Player;
class GlobalSettings;
class PlayerFactory;
class SkyboxFactory;
class Water;
class TextObject;
class Projectile;
class WorldCam;
class Inventory;
class ImageObject;
class VBOShaderLibrary;
class SpecialEffect;
class Explosion;
class Tank;

const std::uint64_t raw1 = 0x7fc00000;  // NAN
const float f1 = *(float*)&raw1;
const std::uint64_t raw2 = 0xffc00000;  //-NAN
const float f2 = *(float*)&raw2;
const std::uint64_t raw3 = 0x7f800000;  // INF
const float f3 = *(float*)&raw3;
const std::uint64_t raw4 = 0xff800000;  //-INF
const float f4 = *(float*)&raw4;

const double pi = 3.141592653589793238462643383279502884;
const std::int32_t max_projectile_models = 11;
const std::int32_t special_effect_time_limit = 150;
const std::int32_t time_divisors = 50;

enum PossibleGameSubStates {
    PLAYER_CONTROL,
    PASS_TIME,
    PROJECTILE,
    INVENTORY,
    SPECIAL_EFFECT,
    HELP
};
enum PossibleSpecialEffects { EXPLOSION };

class GameState {
public:
    GameState();
    GameState(std::int32_t new_width,
              std::int32_t new_height,
              PlayerFactory* new_player_factory,
              GlobalSettings* new_global_settings,
              std::int32_t* new_current_game_state);
    ~GameState();
    void update();
    void draw(vulkan_earth::render::RenderContext& context);
    void drawHUD(vulkan_earth::render::RenderContext& context);
    void drawHUDText(vulkan_earth::render::RenderContext& context,
                     vulkan_earth::render::Vec4 const& color,
                     const std::string& input,
                     float x,
                     float y);
    void updateMouse(std::int32_t x, std::int32_t y);
    void useTurn();
    std::int32_t getWinner();
    void toggleCamera();
    void handleKeyboardInput(std::int32_t key, bool key_status);
    void debugMode(vulkan_earth::render::RenderContext& context);
    void drawMinimap(vulkan_earth::render::RenderContext& context);
    void currentPlayerFire();
    void timerEvent(float new_timer);
    bool getProjectileFired();
    void destroyProjectile();
    float calcAngleBetweenVectors(Vector one, Vector two);
    void calcNormalVector(Vertex* v0, Vertex* v1, Vertex* v2, Normal* n);
    float calcDistanceBetweenVertices(Vertex* v0, Vertex* v1);
    void playBackgroundSounds();
    void drawHelp(vulkan_earth::render::RenderContext& context);
    GlobalSettings* getGlobalSettings();
    PlayerFactory* getPlayerFactory();
    float getGravity();
    float getBalisticScalar();
    // Looks into table for given player
    void nearestEnemy();
    Vertex getPositionOfLastProjectile();
    void setPositionOfLastProjectile(float x, float y, float z);

private:
    void createSpecialEffect();
    void constructProjectile();
    void handlePlayerControlUpdates();
    void updateWorldCam();
    void handleProjectileState();
    void handleSpecialEffectState();
    void handleInventory(std::int32_t inven_index);
    void handlePassTime();
    void handleNonInventoryKeyboard(std::int32_t key, bool key_status);
    void handleInventoryKeyboard(std::int32_t key, bool key_status);

    PlayerFactory* player_factory;
    GlobalSettings* global_settings;
    Player* current_player;
    WorldCam* world_cam;
    PossibleGameSubStates game_sub_state;
    std::int32_t current_player_index;
    std::int32_t width;
    std::int32_t height;
    float camera_x;
    float camera_y;
    float camera_z;
    std::int32_t old_mouse_x;
    std::int32_t old_mouse_y;
    float camera_radius;
    float plane_radius;
    float current_world_theta;
    float current_tank_phi;
    float current_tank_theta;
    float offset;
    SkyboxFactory* skybox_factory;
    Water* ocean;
    std::int32_t sfx_random;
    bool player_cam;
    bool chase_cam_active;
    std::int32_t key_monitor[256];
    std::int32_t number_of_players;

    float gravity;
    Projectile* projectile;
    float scale_gravity;
    float balistic_scalar;
    bool projectile_fired;
    Inventory* inventory;
    ImageObject* weapon_slot;
    ImageObject* selected_weapon_img;
    TextObject* selected_weapon_remain;
    VBOShaderLibrary* projectile_models[max_projectile_models];

    SpecialEffect** special_effects;
    float radius_increase1;
    std::int32_t special_effect_timer;
    float special_effect_x;
    float special_effect_y;
    float special_effect_z;
    float radius_of_current_explosion;
    std::int32_t special_effect_type;
    std::int32_t special_effects_count;
    bool start_music_played;
    std::int32_t prev_music_volume;
    bool need_help;
    ImageObject* manual;
    std::int32_t* current_game_state;

    bool draw_hit_box;

    /*	AI VARIABLES AND MEMBER FUNCTIONS	*/
    float timer;  // Should match frames per second
    void controlAI();
    void resetTables(std::int32_t index);  // Tell it which player to reset or
                                           // -1 for all
    void printTables();
    bool** tank_reachable;
    float** distance_to_target;
    Tank*** tank_list;
    Vertex position_of_last_projectile;
    ofstream myfile;
    /*	END OF AI */
};

#endif
