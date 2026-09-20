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

const double PI = 3.141592653589793238462643383279502884;
const int MAX_PROJECTILE_MODELS = 11;
const int SPECIAL_EFFECT_TIME_LIMIT = 150;
const int TIME_DIVISORS = 50;

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
    GameState(int width,
              int height,
              PlayerFactory* player_factory,
              GlobalSettings* global_settings,
              int* currentGameState);
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
    void timerEvent(GLfloat timer);
    bool getProjectileFired();
    void destroyProjectile();
    GLfloat calcAngleBetweenVectors(Vector one, Vector two);
    void calcNormalVector(Vertex* v0, Vertex* v1, Vertex* v2, Normal* n);
    GLfloat calcDistanceBetweenVertices(Vertex* v0, Vertex* v1);
    void NormalizeVector(Vector* v);
    void playBackgroundSounds();
    void drawHelp();
    GlobalSettings* getGlobalSettings();
    PlayerFactory* getPlayerFactory();
    GLfloat getGravity();
    GLfloat getBalisticScalar();
    // Looks into table for given player
    void nearestEnemy();
    Vertex getPositionOfLastProjectile();
    void setPositionOfLastProjectile(GLfloat X, GLfloat Y, GLfloat Z);

private:
    void createSpecialEffect();
    void constructProjectile();
    void handlePlayerControlUpdates();
    void updateWorldCam();
    void handleProjectileState();
    void handleSpecialEffectState();
    void handleInventory(int invenIndex);
    void handlePassTime();
    void handleNonInventoryKeyboard(int key, bool key_status);
    void handleInventoryKeyboard(int key, bool key_status);

    PlayerFactory* player_factory;
    GlobalSettings* global_settings;
    Player* currentPlayer;
    WorldCam* worldCam;
    PossibleGameSubStates gameSubState;
    int currentPlayerIndex;
    int width;
    int height;
    float cameraX;
    float cameraY;
    float cameraZ;
    int oldMouseX;
    int oldMouseY;
    float camera_radius;
    float plane_radius;
    float currentWorldTheta;
    float currentTankPhi;
    float currentTankTheta;
    float offset;
    SkyboxFactory* skybox_factory;
    Water* ocean;
    int sfxRandom;
    bool playerCam;
    bool chaseCamActive;
    int keyMonitor[256];
    int numberOfPlayers;

    GLfloat gravity;
    Projectile* projectile;
    GLfloat SCALE_GRAVITY;
    GLfloat BALISTIC_SCALAR;
    bool projectile_fired;
    Inventory* inventory;
    ImageObject* weaponSlot;
    ImageObject* selectedWeaponImg;
    TextObject* selectedWeaponRemain;
    VBOShaderLibrary* projectileModels[MAX_PROJECTILE_MODELS];

    SpecialEffect** special_effects;
    GLfloat radiusIncrease1;
    int specialEffectTimer;
    GLfloat specialEffectX;
    GLfloat specialEffectY;
    GLfloat specialEffectZ;
    GLfloat radiusOfCurrentExplosion;
    int specialEffectType;
    int specialEffectsCount;
    bool startMusicPlayed;
    int prevMusicVolume;
    bool needHelp;
    ImageObject* manual;
    int* currentGameState;

    bool drawHitBox;

    /*	AI VARIABLES AND MEMBER FUNCTIONS	*/
    GLfloat timer;  // Should match frames per second
    void controlAI();
    void resetTables(int index);  // Tell it which player to reset or -1 for
                                  // all
    void printTables();
    bool** tank_reachable;
    GLfloat** distance_to_target;
    Tank*** tankList;
    Vertex position_of_last_projectile;
    ofstream myfile;
    /*	END OF AI */
};

#endif
