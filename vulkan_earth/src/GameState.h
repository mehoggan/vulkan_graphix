#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include "Normal.h"
#include "Vector.h"
#include "Vertex.h"

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

const unsigned long long raw1 = 0x7fc00000;  // NAN
const float f1 = *(float*)&raw1;
const unsigned long long raw2 = 0xffc00000;  //-NAN
const float f2 = *(float*)&raw2;
const unsigned long long raw3 = 0x7f800000;  // INF
const float f3 = *(float*)&raw3;
const unsigned long long raw4 = 0xff800000;  //-INF
const float f4 = *(float*)&raw4;

const double pi = 3.141592653589793238462643383279502884;
const int max_projectile_models = 11;
const int special_effect_time_limit = 150;
const int time_divisors = 50;

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
    GameState(int new_width,
              int new_height,
              PlayerFactory* new_player_factory,
              GlobalSettings* new_global_settings,
              int* new_current_game_state);
    ~GameState();
    void update();
    void draw();
    void drawHUD();
    void drawHUDText(const std::string& input, float x, float y);
    void updateMouse(int x, int y);
    void useTurn();
    int getWinner();
    void toggleCamera();
    void handleKeyboardInput(int key, bool key_status);
    void debugMode();
    void drawMinimap();
    void currentPlayerFire();
    void timerEvent(GLfloat new_timer);
    bool getProjectileFired();
    void destroyProjectile();
    GLfloat calcAngleBetweenVectors(Vector one, Vector two);
    void calcNormalVector(Vertex* v0, Vertex* v1, Vertex* v2, Normal* n);
    GLfloat calcDistanceBetweenVertices(Vertex* v0, Vertex* v1);
    void normalizeVector(Vector* v);
    void playBackgroundSounds();
    void drawHelp();
    GlobalSettings* getGlobalSettings();
    PlayerFactory* getPlayerFactory();
    GLfloat getGravity();
    GLfloat getBalisticScalar();
    // Looks into table for given player
    void nearestEnemy();
    Vertex getPositionOfLastProjectile();
    void setPositionOfLastProjectile(GLfloat x, GLfloat y, GLfloat z);

private:
    void createSpecialEffect();
    void constructProjectile();
    void handlePlayerControlUpdates();
    void updateWorldCam();
    void handleProjectileState();
    void handleSpecialEffectState();
    void handleInventory(int inven_index);
    void handlePassTime();
    void handleNonInventoryKeyboard(int key, bool key_status);
    void handleInventoryKeyboard(int key, bool key_status);

    PlayerFactory* player_factory;
    GlobalSettings* global_settings;
    Player* current_player;
    WorldCam* world_cam;
    PossibleGameSubStates game_sub_state;
    int current_player_index;
    int width;
    int height;
    float camera_x;
    float camera_y;
    float camera_z;
    int old_mouse_x;
    int old_mouse_y;
    float camera_radius;
    float plane_radius;
    float current_world_theta;
    float current_tank_phi;
    float current_tank_theta;
    float offset;
    SkyboxFactory* skybox_factory;
    Water* ocean;
    int sfx_random;
    bool player_cam;
    bool chase_cam_active;
    int key_monitor[256];
    int number_of_players;

    GLfloat gravity;
    Projectile* projectile;
    GLfloat scale_gravity;
    GLfloat balistic_scalar;
    bool projectile_fired;
    Inventory* inventory;
    ImageObject* weapon_slot;
    ImageObject* selected_weapon_img;
    TextObject* selected_weapon_remain;
    VBOShaderLibrary* projectile_models[max_projectile_models];

    SpecialEffect** special_effects;
    GLfloat radius_increase1;
    int special_effect_timer;
    GLfloat special_effect_x;
    GLfloat special_effect_y;
    GLfloat special_effect_z;
    GLfloat radius_of_current_explosion;
    int special_effect_type;
    int special_effects_count;
    bool start_music_played;
    int prev_music_volume;
    bool need_help;
    ImageObject* manual;
    int* current_game_state;

    bool draw_hit_box;

    /*	AI VARIABLES AND MEMBER FUNCTIONS	*/
    GLfloat timer;  // Should match frames per second
    void controlAI();
    void resetTables(int index);  // Tell it which player to reset or -1 for
                                  // all
    void printTables();
    bool** tank_reachable;
    GLfloat** distance_to_target;
    Tank*** tank_list;
    Vertex position_of_last_projectile;
    ofstream myfile;
    /*	END OF AI */
};

#endif
