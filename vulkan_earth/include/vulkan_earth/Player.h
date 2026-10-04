#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>

const std::int32_t player_max_weapons = 5;
const std::int32_t player_max_items = 5;

#include <string>
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Renderer.h"

using namespace std;

class Tank;
class Item;
class Weapon;
class PlayerFactory;
class GameState;

enum AI_STATUS {
    NEED_NEW_TARGET,
    FIND_TARGET,
    HOMING_IN_ON_TARGET,
    WALKING_IN,
    TARGET_NOT_REACHABLE,
    SHOT_LAST_ROUND
};
enum AI_SUB_STATUS {
    NOTHING,
    MAKE_MINOR_ADJUSTMENTS_IN,
    MAKE_MINOR_ADJUSTMENTS_OUT,
    ON_TARGET
};

namespace vulkan_graphix::Render {
class RenderContext;
}

class Player {
public:
    Player();
    virtual ~Player();
    virtual void updateTank(
            /* Pass in all paramaters that are associated with a tank */) = 0;
    virtual std::string getTankType() = 0;
    virtual void setTankType(const std::string& tank_type) = 0;
    virtual Tank* getCurrentTank() = 0;
    virtual Item** getCurrentItems() = 0;
    virtual void setItems(Item** item_set) = 0;
    virtual Weapon** getCurrentWeapons() = 0;
    virtual void setWeapons(Weapon** weapon_set) = 0;
    virtual std::string getAiType() = 0;
    virtual void setAiType(const std::string& ai_type) = 0;
    virtual std::string getPlayerType() = 0;
    virtual void setPlayerType(const std::string& player_type) = 0;
    virtual std::string getPlayerName() = 0;
    virtual void setPlayerName(const std::string& name) = 0;
    virtual std::int32_t getCash() = 0;
    virtual void setCash(std::int32_t cash) = 0;
    virtual float getRed() = 0;
    virtual float getGreen() = 0;
    virtual float getBlue() = 0;
    virtual void setCurrentWait(float time) = 0;
    virtual float getCurrentWait() = 0;
    virtual char getTeamLabel() = 0;
    virtual void setTeamLabel(char t) = 0;
    virtual Weapon* getLoadedWeapon() = 0;
    virtual void setLoadedWeapon(Weapon* wpn) = 0;

    /*	ACCESS TO AI TO OUTSIDE WORLD		*/
    void aiMainLogisticFunction();
    Tank* getTarget();
    void setTarget(Tank* new_target);
    void setGameState(GameState* new_game_state);
    void setUpYawVectors();
    void setUpPitchVectors();
    void updateBalsticMatrix();
    bool calculateProjectilePhysics(float xerr, float yerr, float zerr);
    void displayProjectilePhysiscs();
    float* getBalisticMatrix();
    vulkan_graphix::Math::Vec3<float> getEnemyPosition();
    void setEnemyPosition();
    std::int32_t getAIState();
    /*	END OF ACCESS AI TO OUTSIDE WORLD	*/

    /*	TEST DRAW FUNCTIONS				*/
    void drawTestLinesandPlanes(
            vulkan_graphix::Render::RenderContext& context);
    bool getDrawDebugLinesandPlanes();
    void setDrawDebugLinesandPlanes(bool flag);
    /*	DONE WITH TEST DRAW FUNCTIONS	*/

    /*	AI	FUNCTIONS/DAT			*/
    /*	NOTE: PLAYER_HUMAN.CPP		*/
protected:
    GameState* game_state;
    Tank* target;
    vulkan_graphix::Math::Vec3<float> enemy_position;
    vulkan_graphix::Math::Vec3<float> projectile_path =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    vulkan_graphix::Math::Vec3<float> enemy_path =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    vulkan_graphix::Math::Vec3<float> right_vector =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    vulkan_graphix::Math::Vec3<float> left_vector =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    vulkan_graphix::Math::Vec3<float> up_vector =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    vulkan_graphix::Math::Vec3<float> down_vector =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    vulkan_graphix::Math::Vec3<float> pitch_vector =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    vulkan_graphix::Math::Vec3<float> ortho_right =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    vulkan_graphix::Math::Vec3<float> ortho_left =
            vulkan_graphix::Math::Vec3<float>(0.0f);
    float yaw_angle;
    float rangle;
    float langle;
    float pitch_angle;
    float uangle;
    float dangle;
    float balistic_matrix[16];
    float max_pitch_angle;
    std::int32_t state_of_ai;
    std::int32_t prev_state_of_ai;
    std::int32_t sub_state_of_ai;
    /*	AI ACTIONS	*/
    void restoreTurretTo0Degrees();
    void yawLeft(float degrees);
    void yawRight(float degrees);
    void pitchUp(float degrees);
    void pitchDown(float degrees);
    bool draw_debug_linesand_planes;
    char minimumYawAngle(float right_degrees, float left_degrees);
    vulkan_graphix::Math::Vec3<float> previous_projectile_landing_spot;
    float distance_off_from_target;
    float previous_distance_off_from_target;
    float degrees_rotated;
    float first_acquired_power;
    float first_acquired_pitch;
    /*	END OF AI FUNCTIONS/DATA	*/
};

#endif /*	PLAYER_H	*/