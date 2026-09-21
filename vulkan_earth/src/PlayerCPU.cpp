#include "PlayerCPU.h"
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

PlayerCPU::PlayerCPU() = default;

PlayerCPU::PlayerCPU(GLfloat red, GLfloat green, GLfloat blue) {
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;

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

    this->teamLabel = '-';
    loadedWeapon = nullptr;
}

PlayerCPU::PlayerCPU(GLfloat red,
                     GLfloat green,
                     GLfloat blue,
                     const std::string& tank_type,
                     const std::string& ai_type,
                     const std::string& name,
                     char team_label,
                     const std::string& player_type,
                     int starting_cash) {
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;

    currentCash = starting_cash;
    currentWait = 0;
    this->teamLabel = team_label;

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
    this->tankType = tank_type;
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

PlayerCPU::~PlayerCPU() {
    delete currentTank;

    // before we delete inventory, where should we store the inventory? figure
    // this out later
    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) delete currentItems[i];
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) delete currentWeapons[i];
}

void PlayerCPU::updateTank(
        /* Pass in all paramaters that are associated with a tank */) {}
Tank* PlayerCPU::getCurrentTank() { return currentTank; }
std::string PlayerCPU::getTankType() { return this->tankType; }
Item** PlayerCPU::getCurrentItems() { return currentItems; }
Weapon** PlayerCPU::getCurrentWeapons() { return currentWeapons; }
std::string PlayerCPU::getAI_Type() { return this->ai_type; }
void PlayerCPU::setAI_Type(const std::string& ai_type) {
    this->ai_type = ai_type;
}
std::string PlayerCPU::getPlayer_Type() { return this->player_type; }
void PlayerCPU::setPlayer_Type(const std::string& player_type) {
    this->player_type = player_type;
}
GLfloat PlayerCPU::getRed() { return color[0]; }
GLfloat PlayerCPU::getGreen() { return color[1]; }
GLfloat PlayerCPU::getBlue() { return color[2]; }
std::string PlayerCPU::getPlayerName() { return this->player_type; }
int PlayerCPU::getCash() { return currentCash; }
void PlayerCPU::setCash(int cash) { currentCash = cash; }
float PlayerCPU::getCurrentWait() { return currentWait; }
void PlayerCPU::setCurrentWait(float time) { currentWait = time; }
char PlayerCPU::getTeamLabel() { return this->teamLabel; }
void PlayerCPU::setTeamLabel(char t) { this->teamLabel = t; }
Weapon* PlayerCPU::getLoadedWeapon() { return loadedWeapon; }
void PlayerCPU::setLoadedWeapon(Weapon* wpn) { loadedWeapon = wpn; }
void PlayerCPU::setPlayerName(const std::string& name) {
    printf("\nYou're trying to set a name for CPU. It won't happen, sorry.\n");
    this->name = "CPU";
}
void PlayerCPU::setWeapons(Weapon** weapon_set) {
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        currentWeapons[i] = weapon_set[i];
    }
}
void PlayerCPU::setItems(Item** item_set) {
    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) {
        currentItems[i] = item_set[i];
    }
}
void PlayerCPU::setTankType(const std::string& tank_type) {
    this->tankType = tank_type;
    if (currentTank != nullptr) delete currentTank;
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