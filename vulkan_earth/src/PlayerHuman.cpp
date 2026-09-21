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

PlayerHuman::PlayerHuman(GLfloat red,
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

PlayerHuman::~PlayerHuman() {
    delete current_tank;

    // before we delete inventory, where should we store the inventory? figure
    // out this later
    for (int i = 0; i < player_max_items; i++) {
        if (current_items[i] != nullptr) {
            delete current_items[i];
            current_items[i] = nullptr;
        }
    }
    for (int i = 0; i < player_max_weapons; i++) {
        if (current_weapons[i] != nullptr) {
            delete current_weapons[i];
            current_items[i] = nullptr;
        }
    }
}
void drawHUD() {}
void PlayerHuman::updateTank(
        /* Pass in all paramaters that are associated with a tank */) {}
Tank* PlayerHuman::getCurrentTank() { return current_tank; }
std::string PlayerHuman::getTankType() { return tank_type; }
Item** PlayerHuman::getCurrentItems() { return current_items; }
Weapon** PlayerHuman::getCurrentWeapons() { return current_weapons; }
std::string PlayerHuman::getAiType() { return ai_type; }
void PlayerHuman::setAiType(const std::string& new_ai_type) {
    ai_type = new_ai_type;
}
std::string PlayerHuman::getPlayerType() { return player_type; }
void PlayerHuman::setPlayerType(const std::string& new_player_type) {
    player_type = new_player_type;
}
std::string PlayerHuman::getPlayerName() { return name; }
void PlayerHuman::setPlayerName(const std::string& new_name) {
    name = new_name;
}
int PlayerHuman::getCash() { return current_cash; }
void PlayerHuman::setCash(int cash) { current_cash = cash; }
GLfloat PlayerHuman::getRed() { return color[0]; }
GLfloat PlayerHuman::getGreen() { return color[1]; }
GLfloat PlayerHuman::getBlue() { return color[2]; }
float PlayerHuman::getCurrentWait() { return current_wait; }
void PlayerHuman::setCurrentWait(float time) { current_wait = time; }
char PlayerHuman::getTeamLabel() { return team_label; }
void PlayerHuman::setTeamLabel(char t) { team_label = t; }
Weapon* PlayerHuman::getLoadedWeapon() { return loaded_weapon; }
void PlayerHuman::setLoadedWeapon(Weapon* wpn) { loaded_weapon = wpn; }
GLfloat* PlayerHuman::getBalisticMatrix() { return balistic_matrix; }
void PlayerHuman::setItems(Item** item_set) {
    for (int i = 0; i < player_max_items; i++) {
        current_items[i] = item_set[i];
    }
}
void PlayerHuman::setTankType(const std::string& new_tank_type) {
    tank_type = new_tank_type;
    delete current_tank;
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
        printf("ERROR <PlayerHuman::setTankType(const std::string&)>: Wrong "
               "Tank Type!!\n");
        current_tank = nullptr;
    }
}
void PlayerHuman::setWeapons(Weapon** weapon_set) {
    for (int i = 0; i < player_max_weapons; i++) {
        current_weapons[i] = weapon_set[i];
    }
}