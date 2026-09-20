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
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;

    this->currentTank = new TankA(0, 0, 0);

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

PlayerCPU::~PlayerCPU() {
    delete currentTank;

    // before we delete inventory, where should we store the inventory? figure
    // this out later
    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) delete currentItems[i];
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) delete currentWeapons[i];
}

void PlayerCPU::updateTank(
        /* Pass in all paramaters that are associated with a tank */) {}
Tank* PlayerCPU::getCurrentTank() { return this->currentTank; }
std::string PlayerCPU::getTankType() { return this->tankType; }
Item** PlayerCPU::getCurrentItems() { return this->currentItems; }
Weapon** PlayerCPU::getCurrentWeapons() { return this->currentWeapons; }
std::string PlayerCPU::getAI_Type() { return this->ai_type; }
void PlayerCPU::setAI_Type(const std::string& ai_type) {
    this->ai_type = ai_type;
}
std::string PlayerCPU::getPlayer_Type() { return this->player_type; }
void PlayerCPU::setPlayer_Type(const std::string& player_type) {
    this->player_type = player_type;
}
GLfloat PlayerCPU::getRed() { return this->color[0]; }
GLfloat PlayerCPU::getGreen() { return this->color[1]; }
GLfloat PlayerCPU::getBlue() { return this->color[2]; }
std::string PlayerCPU::getPlayerName() { return this->player_type; }
int PlayerCPU::getCash() { return this->currentCash; }
void PlayerCPU::setCash(int cash) { this->currentCash = cash; }
float PlayerCPU::getCurrentWait() { return this->currentWait; }
void PlayerCPU::setCurrentWait(float time) { this->currentWait = time; }
char PlayerCPU::getTeamLabel() { return this->teamLabel; }
void PlayerCPU::setTeamLabel(char t) { this->teamLabel = t; }
Weapon* PlayerCPU::getLoadedWeapon() { return loadedWeapon; }
void PlayerCPU::setLoadedWeapon(Weapon* wpn) { loadedWeapon = wpn; }
void PlayerCPU::setPlayerName(const std::string& name) {
    printf("\nYou're trying to set a name for CPU. It won't happen, sorry.\n");
    this->name = "CPU";
}
void PlayerCPU::setWeapons(Weapon** weaponSet) {
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        this->currentWeapons[i] = weaponSet[i];
    }
}
void PlayerCPU::setItems(Item** itemSet) {
    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) {
        this->currentItems[i] = itemSet[i];
    }
}
void PlayerCPU::setTankType(const std::string& tankType) {
    this->tankType = tankType;
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