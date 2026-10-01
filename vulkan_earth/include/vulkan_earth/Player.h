#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>

const std::int32_t player_max_weapons = 5;
const std::int32_t player_max_items = 5;

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "vulkan_earth/OpenGLColors.h"
#include "vulkan_earth/Vector.h"
#include "vulkan_earth/Vertex.h"

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
    Vertex getEnemyPosition();
    void setEnemyPosition();
    std::int32_t getAIState();
    /*	END OF ACCESS AI TO OUTSIDE WORLD	*/

    /*	TEST DRAW FUNCTIONS				*/
    void drawTestLinesandPlanes();
    bool getDrawDebugLinesandPlanes();
    void setDrawDebugLinesandPlanes(bool flag);
    /*	DONE WITH TEST DRAW FUNCTIONS	*/

    /*	AI	FUNCTIONS/DAT			*/
    /*	NOTE: PLAYER_HUMAN.CPP		*/
protected:
    GameState* game_state;
    Tank* target;
    Vertex enemy_position;
    Vector projectile_path;
    Vector enemy_path;
    Vector right_vector;
    Vector left_vector;
    Vector up_vector;
    Vector down_vector;
    Vector pitch_vector;
    Vector ortho_right;
    Vector ortho_left;
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
    Vertex previous_projectile_landing_spot;
    float distance_off_from_target;
    float previous_distance_off_from_target;
    float degrees_rotated;
    float first_acquired_power;
    float first_acquired_pitch;
    /*	END OF AI FUNCTIONS/DATA	*/
};

#endif /*	PLAYER_H	*/