#include "PlayerHuman.h"
#include <string>
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

    this->tankType = "Rhinoxx";
    this->ai_type = "Shooter";
    this->player_type = "CPU";
    this->name = "";

    teamLabel = '-';
    loadedWeapon = nullptr;
}

PlayerHuman::PlayerHuman(GLfloat red,
                         GLfloat green,
                         GLfloat blue,
                         const std::string& tankType,
                         const std::string& ai_type,
                         const std::string& name,
                         char teamLabel,
                         const std::string& player_type,
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

    this->ai_type = ai_type;
    this->player_type = player_type;
    this->name = name;
    this->tankType = tankType;
    if (this->tankType == "Rhinoxx")
        currentTank = new TankA(0, 0, 0);
    else if (this->tankType == "Hellfire")
        currentTank = new TankB(0, 0, 0);
    else if (this->tankType == "HeavyD")
        currentTank = new TankC(0, 0, 0);
    else if (this->tankType == "Panzer")
        currentTank = new TankD(0, 0, 0);
    else if (this->tankType == "Eggroid")
        currentTank = new TankE(0, 0, 0);
    else if (this->tankType == "Behemoth")
        currentTank = new TankF(0, 0, 0);
    else if (this->tankType == "Cubix")
        currentTank = new TankG(0, 0, 0);
    else if (this->tankType == "Predator")
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
std::string PlayerHuman::getTankType() { return this->tankType; }
Item** PlayerHuman::getCurrentItems() { return this->currentItems; }
Weapon** PlayerHuman::getCurrentWeapons() { return this->currentWeapons; }
std::string PlayerHuman::getAI_Type() { return this->ai_type; }
void PlayerHuman::setAI_Type(const std::string& ai_type) {
    this->ai_type = ai_type;
}
std::string PlayerHuman::getPlayer_Type() { return this->player_type; }
void PlayerHuman::setPlayer_Type(const std::string& player_type) {
    this->player_type = player_type;
}
std::string PlayerHuman::getPlayerName() { return this->name; }
void PlayerHuman::setPlayerName(const std::string& name) { this->name = name; }
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
void PlayerHuman::setTankType(const std::string& tankType) {
    this->tankType = tankType;
    delete currentTank;
    if (this->tankType == "Rhinoxx")
        currentTank = new TankA(0, 0, 0);
    else if (this->tankType == "Hellfire")
        currentTank = new TankB(0, 0, 0);
    else if (this->tankType == "HeavyD")
        currentTank = new TankC(0, 0, 0);
    else if (this->tankType == "Panzer")
        currentTank = new TankD(0, 0, 0);
    else if (this->tankType == "Eggroid")
        currentTank = new TankE(0, 0, 0);
    else if (this->tankType == "Behemoth")
        currentTank = new TankF(0, 0, 0);
    else if (this->tankType == "Cubix")
        currentTank = new TankG(0, 0, 0);
    else if (this->tankType == "Predator")
        currentTank = new TankH(0, 0, 0);
    else {
        printf("ERROR <PlayerHuman::setTankType(const std::string&)>: Wrong "
               "Tank Type!!\n");
        currentTank = nullptr;
    }
}
void PlayerHuman::setWeapons(Weapon** weaponSet) {
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        this->currentWeapons[i] = weaponSet[i];
    }
}