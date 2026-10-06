#ifndef VULKAN_EARTH_GAMESTATE_H
#define VULKAN_EARTH_GAMESTATE_H

#include <cstdint>
#include <fstream>
#include <string>
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_graphix::Render {
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
    void draw(vulkan_graphix::Render::RenderContext& context);
    void drawHUD(vulkan_graphix::Render::RenderContext& context);
    void drawHUDText(vulkan_graphix::Render::RenderContext& context,
      const vulkan_graphix::Math::Vec4<float>& color,
      const std::string& input,
      float x,
      float y);
    void updateMouse(std::int32_t x, std::int32_t y);
    void useTurn();
    std::int32_t getWinner();
    void toggleCamera();
    void handleKeyboardInput(std::int32_t key, bool key_status);
    void debugMode(vulkan_graphix::Render::RenderContext& context);
    void drawMinimap(vulkan_graphix::Render::RenderContext& context);
    void currentPlayerFire();
    void timerEvent(float new_timer);
    bool getProjectileFired();
    void destroyProjectile();
    void calcNormalVector(vulkan_graphix::Math::Vec3<float>* v0,
      vulkan_graphix::Math::Vec3<float>* v1,
      vulkan_graphix::Math::Vec3<float>* v2,
      vulkan_graphix::Math::Vec3<float>* n);
    float calcDistanceBetweenVertices(vulkan_graphix::Math::Vec3<float>* v0,
      vulkan_graphix::Math::Vec3<float>* v1);
    void playBackgroundSounds();
    void drawHelp(vulkan_graphix::Render::RenderContext& context);
    GlobalSettings* getGlobalSettings();
    PlayerFactory* getPlayerFactory();
    float getGravity();
    float getBalisticScalar();
    // Looks into table for given player
    void nearestEnemy();
    vulkan_graphix::Math::Vec3<float> getPositionOfLastProjectile();
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

    PlayerFactory* m_player_factory;
    GlobalSettings* m_global_settings;
    Player* m_current_player;
    WorldCam* m_world_cam;
    PossibleGameSubStates m_game_sub_state;
    std::int32_t m_current_player_index;
    std::int32_t m_width;
    std::int32_t m_height;
    float m_camera_x;
    float m_camera_y;
    float m_camera_z;
    std::int32_t m_old_mouse_x;
    std::int32_t m_old_mouse_y;
    float m_camera_radius;
    float m_plane_radius;
    float m_current_world_theta;
    float m_current_tank_phi;
    float m_current_tank_theta;
    float m_offset;
    SkyboxFactory* m_skybox_factory;
    Water* m_ocean;
    std::int32_t m_sfx_random;
    bool m_player_cam;
    bool m_chase_cam_active;
    std::int32_t m_key_monitor[256];
    std::int32_t m_number_of_players;

    float m_gravity;
    Projectile* m_projectile;
    float m_scale_gravity;
    float m_balistic_scalar;
    bool m_projectile_fired;
    Inventory* m_inventory;
    ImageObject* m_weapon_slot;
    ImageObject* m_selected_weapon_img;
    TextObject* m_selected_weapon_remain;
    VBOShaderLibrary* m_projectile_models[max_projectile_models];

    SpecialEffect** m_special_effects;
    float m_radius_increase1;
    std::int32_t m_special_effect_timer;
    float m_special_effect_x;
    float m_special_effect_y;
    float m_special_effect_z;
    float m_radius_of_current_explosion;
    std::int32_t m_special_effect_type;
    std::int32_t m_special_effects_count;
    bool m_start_music_played;
    std::int32_t m_prev_music_volume;
    bool m_need_help;
    ImageObject* m_manual;
    std::int32_t* m_current_game_state;

    bool m_draw_hit_box;

    /*	AI VARIABLES AND MEMBER FUNCTIONS	*/
    float m_timer;  // Should match frames per second
    void controlAI();
    void resetTables(std::int32_t index);  // Tell it which player to reset or
                                           // -1 for all
    void printTables();
    bool** m_tank_reachable;
    float** m_distance_to_target;
    Tank*** m_tank_list;
    vulkan_graphix::Math::Vec3<float> m_position_of_last_projectile;
    ofstream m_myfile;
    /*	END OF AI */
};

#endif
