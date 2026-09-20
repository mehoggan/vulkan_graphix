#ifndef PLAYER_H
#define PLAYER_H

const int PLAYER_MAX_WEAPONS = 5;
const int PLAYER_MAX_ITEMS = 5;

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include "OpenGLColors.h"
#include "Vector.h"
#include "Vertex.h"

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
    virtual char* getTankType() = 0;
    virtual void setTankType(char* tankType) = 0;
    virtual Tank* getCurrentTank() = 0;
    virtual Item** getCurrentItems() = 0;
    virtual void setItems(Item** itemSet) = 0;
    virtual Weapon** getCurrentWeapons() = 0;
    virtual void setWeapons(Weapon** weaponSet) = 0;
    virtual char* getAI_Type() = 0;
    virtual void setAI_Type(char* ai_type) = 0;
    virtual char* getPlayer_Type() = 0;
    virtual void setPlayer_Type(char* player_type) = 0;
    virtual char* getPlayerName() = 0;
    virtual void setPlayerName(char* name) = 0;
    virtual int getCash() = 0;
    virtual void setCash(int cash) = 0;
    virtual GLfloat getRed() = 0;
    virtual GLfloat getGreen() = 0;
    virtual GLfloat getBlue() = 0;
    virtual void setCurrentWait(float time) = 0;
    virtual float getCurrentWait() = 0;
    virtual char getTeamLabel() = 0;
    virtual void setTeamLabel(char t) = 0;
    virtual Weapon* getLoadedWeapon() = 0;
    virtual void setLoadedWeapon(Weapon* wpn) = 0;

    /*	ACCESS TO AI TO OUTSIDE WORLD		*/
    void aiMainLogisticFunction();
    Tank* getTarget();
    void setTarget(Tank* target);
    void setGameState(GameState* game_state);
    void setUpYawVectors();
    void setUpPitchVectors();
    void updateBalsticMatrix();
    bool calculateProjectilePhysics(GLfloat Xerr, GLfloat Yerr, GLfloat Zerr);
    void displayProjectilePhysiscs();
    GLfloat* getBalisticMatrix();
    Vertex getEnemyPosition();
    void setEnemyPosition();
    int getAIState();
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
    Vertex enemyPosition;
    Vector projectile_path;
    Vector enemy_path;
    Vector right_vector;
    Vector left_vector;
    Vector up_vector;
    Vector down_vector;
    Vector pitch_vector;
    Vector ortho_right;
    Vector ortho_left;
    GLfloat yaw_angle;
    GLfloat _rangle;
    GLfloat _langle;
    GLfloat pitch_angle;
    GLfloat _uangle;
    GLfloat _dangle;
    GLfloat balisticMatrix[16];
    GLfloat maxPitchAngle;
    int state_of_AI;
    int prev_state_of_AI;
    int sub_state_of_AI;
    /*	AI ACTIONS	*/
    void restoreTurretTo0Degrees();
    void yawLeft(GLfloat degrees);
    void yawRight(GLfloat degrees);
    void pitchUp(GLfloat degrees);
    void pitchDown(GLfloat degrees);
    bool drawDebugLinesandPlanes;
    char minimumYawAngle(GLfloat degrees1, GLfloat degrees2);
    Vertex previous_projectile_landing_spot;
    GLfloat distance_off_from_target;
    GLfloat previous_distance_off_from_target;
    GLfloat degrees_rotated;
    GLfloat first_acquired_power;
    GLfloat first_acquired_pitch;
    /*	END OF AI FUNCTIONS/DATA	*/
};

#endif /*	PLAYER_H	*/