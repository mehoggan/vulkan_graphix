#include "PlayerCPU.h"
#include <string>
#include "Item.h"
#include "PlayerFactory.h"
#include "Weapon.h"
#include "Tank.h"
#include "TankA.h"
#include "TankB.h"
#include "TankC.h"
#include "TankD.h"
#include "TankE.h"
#include "TankF.h"
#include "TankG.h"
#include "TankH.h"
#include "MacroCrtdbg.h"

PlayerCPU::PlayerCPU() = default;

PlayerCPU::PlayerCPU(GLfloat red, GLfloat green, GLfloat blue) {
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;

    current_tank = new TankA(0, 0, 0);

    for (int i = 0; i < player_max_items; i++) {
        current_items[i] = nullptr;
    }
    for (int i = 0; i < player_max_weapons; i++) {
        current_weapons[i] = nullptr;
    }

    tank_type = "Rhinoxx";
    ai_type = "Shooter";
    player_type = "CPU";
    name = "";

    team_label = '-';
    loaded_weapon = nullptr;
}

PlayerCPU::PlayerCPU(GLfloat red,
                     GLfloat green,
                     GLfloat blue,
                     const std::string& new_tank_type,
                     const std::string& new_ai_type,
                     const std::string& new_name,
                     char new_team_label,
                     const std::string& new_player_type,
                     int starting_cash) {
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;

    current_cash = starting_cash;
    current_wait = 0;
    team_label = new_team_label;

    for (int i = 0; i < player_max_items; i++) {
        current_items[i] = nullptr;
    }
    for (int i = 0; i < player_max_weapons; i++) {
        current_weapons[i] = nullptr;
    }
    loaded_weapon = nullptr;

    ai_type = new_ai_type;
    player_type = new_player_type;
    name = new_name;
    tank_type = new_tank_type;
    if (tank_type == "Rhinoxx")
        current_tank = new TankA(0, 0, 0);
    else if (tank_type == "Hellfire")
        current_tank = new TankB(0, 0, 0);
    else if (tank_type == "HeavyD")
        current_tank = new TankC(0, 0, 0);
    else if (tank_type == "Panzer")
        current_tank = new TankD(0, 0, 0);
    else if (tank_type == "Eggroid")
        current_tank = new TankE(0, 0, 0);
    else if (tank_type == "Behemoth")
        current_tank = new TankF(0, 0, 0);
    else if (tank_type == "Cubix")
        current_tank = new TankG(0, 0, 0);
    else if (tank_type == "Predator")
        current_tank = new TankH(0, 0, 0);
    else {
        printf("ERROR: Wrong Tank Type!!\n");
        current_tank = nullptr;
    }
}

PlayerCPU::~PlayerCPU() {
    delete current_tank;

    // before we delete inventory, where should we store the inventory? figure
    // this out later
    for (int i = 0; i < player_max_items; i++) delete current_items[i];
    for (int i = 0; i < player_max_weapons; i++) delete current_weapons[i];
}

void PlayerCPU::updateTank(
        /* Pass in all paramaters that are associated with a tank */) {}
Tank* PlayerCPU::getCurrentTank() { return current_tank; }
std::string PlayerCPU::getTankType() { return tank_type; }
Item** PlayerCPU::getCurrentItems() { return current_items; }
Weapon** PlayerCPU::getCurrentWeapons() { return current_weapons; }
std::string PlayerCPU::getAiType() { return ai_type; }
void PlayerCPU::setAiType(const std::string& new_ai_type) {
    ai_type = new_ai_type;
}
std::string PlayerCPU::getPlayerType() { return player_type; }
void PlayerCPU::setPlayerType(const std::string& new_player_type) {
    player_type = new_player_type;
}
GLfloat PlayerCPU::getRed() { return color[0]; }
GLfloat PlayerCPU::getGreen() { return color[1]; }
GLfloat PlayerCPU::getBlue() { return color[2]; }
std::string PlayerCPU::getPlayerName() { return player_type; }
int PlayerCPU::getCash() { return current_cash; }
void PlayerCPU::setCash(int cash) { current_cash = cash; }
float PlayerCPU::getCurrentWait() { return current_wait; }
void PlayerCPU::setCurrentWait(float time) { current_wait = time; }
char PlayerCPU::getTeamLabel() { return team_label; }
void PlayerCPU::setTeamLabel(char t) { team_label = t; }
Weapon* PlayerCPU::getLoadedWeapon() { return loaded_weapon; }
void PlayerCPU::setLoadedWeapon(Weapon* wpn) { loaded_weapon = wpn; }
void PlayerCPU::setPlayerName(const std::string& new_name) {
    printf("\nYou're trying to set a new_name for CPU. It won't happen, "
           "sorry.\n");
    name = "CPU";
}
void PlayerCPU::setWeapons(Weapon** weapon_set) {
    for (int i = 0; i < player_max_weapons; i++) {
        current_weapons[i] = weapon_set[i];
    }
}
void PlayerCPU::setItems(Item** item_set) {
    for (int i = 0; i < player_max_items; i++) {
        current_items[i] = item_set[i];
    }
}
void PlayerCPU::setTankType(const std::string& new_tank_type) {
    tank_type = new_tank_type;
    if (current_tank != nullptr) delete current_tank;
    if (tank_type == "Rhinoxx")
        current_tank = new TankA(0, 0, 0);
    else if (tank_type == "Hellfire")
        current_tank = new TankB(0, 0, 0);
    else if (tank_type == "HeavyD")
        current_tank = new TankC(0, 0, 0);
    else if (tank_type == "Panzer")
        current_tank = new TankD(0, 0, 0);
    else if (tank_type == "Eggroid")
        current_tank = new TankE(0, 0, 0);
    else if (tank_type == "Behemoth")
        current_tank = new TankF(0, 0, 0);
    else if (tank_type == "Cubix")
        current_tank = new TankG(0, 0, 0);
    else if (tank_type == "Predator")
        current_tank = new TankH(0, 0, 0);
    else {
        printf("ERROR: Wrong Tank Type!!\n");
        current_tank = nullptr;
    }
}