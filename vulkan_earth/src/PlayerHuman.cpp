#include "PlayerHuman.h"
#include <cstring>
#include "PlayerFactory.h"
#include "Tank.h"
#include "TankA.h"
#include "TankB.h"
#include "TankC.h"
#include "TankD.h"
#include "TankE.h"
#include "TankF.h"
#include "TankG.h"
#include "TankH.h"
#include "macro_crtdbg.h"

PlayerHuman::PlayerHuman() = default;

PlayerHuman::PlayerHuman(GLfloat red, GLfloat green, GLfloat blue) {
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;

    currentTank = new TankA(0, 0, 0);
    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) {
        currentItems[i] = nullptr;
    }
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        currentWeapons[i] = nullptr;
    }

    strcpy(this->tankType, "Rhinoxx");
    strcpy(this->ai_type, "Shooter");
    strcpy(this->player_type, "CPU");
    strcpy(this->name, "");

    teamLabel = '-';
    loadedWeapon = nullptr;
}

PlayerHuman::PlayerHuman(GLfloat red,
                         GLfloat green,
                         GLfloat blue,
                         char* tankType,
                         char* ai_type,
                         char* name,
                         char teamLabel,
                         char* player_type,
                         int startingCash) {
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;

    this->currentCash = startingCash;
    this->currentWait = 0;
    this->teamLabel = teamLabel;

    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) {
        currentItems[i] = nullptr;
    }
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        currentWeapons[i] = nullptr;
    }
    loadedWeapon = nullptr;

    strcpy(this->ai_type, ai_type);
    strcpy(this->player_type, player_type);
    strcpy(this->name, name);
    strcpy(this->tankType, tankType);
    if (strcmp(this->tankType, "Rhinoxx") == 0)
        currentTank = new TankA(0, 0, 0);
    else if (strcmp(this->tankType, "Hellfire") == 0)
        currentTank = new TankB(0, 0, 0);
    else if (strcmp(this->tankType, "HeavyD") == 0)
        currentTank = new TankC(0, 0, 0);
    else if (strcmp(this->tankType, "Panzer") == 0)
        currentTank = new TankD(0, 0, 0);
    else if (strcmp(this->tankType, "Eggroid") == 0)
        currentTank = new TankE(0, 0, 0);
    else if (strcmp(this->tankType, "Behemoth") == 0)
        currentTank = new TankF(0, 0, 0);
    else if (strcmp(this->tankType, "Cubix") == 0)
        currentTank = new TankG(0, 0, 0);
    else if (strcmp(this->tankType, "Predator") == 0)
        currentTank = new TankH(0, 0, 0);
    else {
        printf("ERROR: Wrong Tank Type!!\n");
        currentTank = nullptr;
    }
}

PlayerHuman::~PlayerHuman() {
    delete currentTank;

    // before we delete inventory, where should we store the inventory? figure
    // out this later
    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) {
        if (currentItems[i] != nullptr) {
            delete currentItems[i];
            currentItems[i] = nullptr;
        }
    }
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        if (currentWeapons[i] != nullptr) {
            delete currentWeapons[i];
            currentItems[i] = nullptr;
        }
    }
}
void drawHUD() {}
void PlayerHuman::updateTank(
        /* Pass in all paramaters that are associated with a tank */) {}
Tank* PlayerHuman::getCurrentTank() { return this->currentTank; }
char* PlayerHuman::getTankType() { return this->tankType; }
Item** PlayerHuman::getCurrentItems() { return this->currentItems; }
Weapon** PlayerHuman::getCurrentWeapons() { return this->currentWeapons; }
char* PlayerHuman::getAI_Type() { return this->ai_type; }
void PlayerHuman::setAI_Type(char* ai_type) { strcpy(this->ai_type, ai_type); }
char* PlayerHuman::getPlayer_Type() { return this->player_type; }
void PlayerHuman::setPlayer_Type(char* player_type) {
    strcpy(this->player_type, player_type);
}
char* PlayerHuman::getPlayerName() { return this->name; }
void PlayerHuman::setPlayerName(char* name) { strcpy(this->name, name); }
int PlayerHuman::getCash() { return this->currentCash; }
void PlayerHuman::setCash(int cash) { this->currentCash = cash; }
GLfloat PlayerHuman::getRed() { return this->color[0]; }
GLfloat PlayerHuman::getGreen() { return this->color[1]; }
GLfloat PlayerHuman::getBlue() { return this->color[2]; }
float PlayerHuman::getCurrentWait() { return this->currentWait; }
void PlayerHuman::setCurrentWait(float time) { this->currentWait = time; }
char PlayerHuman::getTeamLabel() { return this->teamLabel; }
void PlayerHuman::setTeamLabel(char t) { this->teamLabel = t; }
Weapon* PlayerHuman::getLoadedWeapon() { return loadedWeapon; }
void PlayerHuman::setLoadedWeapon(Weapon* wpn) { loadedWeapon = wpn; }
GLfloat* PlayerHuman::getBalisticMatrix() { return this->balisticMatrix; }
void PlayerHuman::setItems(Item** itemSet) {
    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) {
        this->currentItems[i] = itemSet[i];
    }
}
void PlayerHuman::setTankType(char* tankType) {
    strcpy(this->tankType, tankType);
    delete currentTank;
    if (strcmp(this->tankType, "Rhinoxx") == 0)
        currentTank = new TankA(0, 0, 0);
    else if (strcmp(this->tankType, "Hellfire") == 0)
        currentTank = new TankB(0, 0, 0);
    else if (strcmp(this->tankType, "HeavyD") == 0)
        currentTank = new TankC(0, 0, 0);
    else if (strcmp(this->tankType, "Panzer") == 0)
        currentTank = new TankD(0, 0, 0);
    else if (strcmp(this->tankType, "Eggroid") == 0)
        currentTank = new TankE(0, 0, 0);
    else if (strcmp(this->tankType, "Behemoth") == 0)
        currentTank = new TankF(0, 0, 0);
    else if (strcmp(this->tankType, "Cubix") == 0)
        currentTank = new TankG(0, 0, 0);
    else if (strcmp(this->tankType, "Predator") == 0)
        currentTank = new TankH(0, 0, 0);
    else {
        printf("ERROR <PlayerHuman::setTankType(char*)>: Wrong Tank Type!!\n");
        currentTank = nullptr;
    }
}
void PlayerHuman::setWeapons(Weapon** weaponSet) {
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        this->currentWeapons[i] = weaponSet[i];
    }
}