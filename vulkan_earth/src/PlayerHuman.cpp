#include "vulkan_earth/PlayerHuman.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/TankA.h"
#include "vulkan_earth/TankB.h"
#include "vulkan_earth/TankC.h"
#include "vulkan_earth/TankD.h"
#include "vulkan_earth/TankE.h"
#include "vulkan_earth/TankF.h"
#include "vulkan_earth/TankG.h"
#include "vulkan_earth/TankH.h"
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/MacroCrtdbg.h"

PlayerHuman::PlayerHuman() = default;

PlayerHuman::PlayerHuman(float red, float green, float blue) {
  m_color[0] = red;
  m_color[1] = green;
  m_color[2] = blue;
  m_color[3] = 1.0;

  m_current_tank = new TankA(0, 0, 0);
  for (std::int32_t i = 0; i < player_max_items; i++) {
    m_current_items[i] = nullptr;
  }
  for (std::int32_t i = 0; i < player_max_weapons; i++) {
    m_current_weapons[i] = nullptr;
  }

  m_tank_type = "Rhinoxx";
  m_ai_type = "Shooter";
  m_player_type = "CPU";
  m_name = "";

  m_team_label = '-';
  m_loaded_weapon = nullptr;
}

PlayerHuman::PlayerHuman(float red,
    float green,
    float blue,
    const std::string& new_tank_type,
    const std::string& new_ai_type,
    const std::string& new_name,
    char new_team_label,
    const std::string& new_player_type,
    std::int32_t starting_cash) {
  m_color[0] = red;
  m_color[1] = green;
  m_color[2] = blue;
  m_color[3] = 1.0;

  m_current_cash = starting_cash;
  m_current_wait = 0;
  m_team_label = new_team_label;

  for (std::int32_t i = 0; i < player_max_items; i++) {
    m_current_items[i] = nullptr;
  }
  for (std::int32_t i = 0; i < player_max_weapons; i++) {
    m_current_weapons[i] = nullptr;
  }
  m_loaded_weapon = nullptr;

  m_ai_type = new_ai_type;
  m_player_type = new_player_type;
  m_name = new_name;
  m_tank_type = new_tank_type;
  if (m_tank_type == "Rhinoxx")
    m_current_tank = new TankA(0, 0, 0);
  else if (m_tank_type == "Hellfire")
    m_current_tank = new TankB(0, 0, 0);
  else if (m_tank_type == "HeavyD")
    m_current_tank = new TankC(0, 0, 0);
  else if (m_tank_type == "Panzer")
    m_current_tank = new TankD(0, 0, 0);
  else if (m_tank_type == "Eggroid")
    m_current_tank = new TankE(0, 0, 0);
  else if (m_tank_type == "Behemoth")
    m_current_tank = new TankF(0, 0, 0);
  else if (m_tank_type == "Cubix")
    m_current_tank = new TankG(0, 0, 0);
  else if (m_tank_type == "Predator")
    m_current_tank = new TankH(0, 0, 0);
  else {
    printf("ERROR: Wrong Tank Type!!\n");
    m_current_tank = nullptr;
  }
}

PlayerHuman::~PlayerHuman() {
  delete m_current_tank;

  // before we delete inventory, where should we store the inventory? figure
  // out this later
  for (std::int32_t i = 0; i < player_max_items; i++) {
    if (m_current_items[i] != nullptr) {
      delete m_current_items[i];
      m_current_items[i] = nullptr;
    }
  }
  for (std::int32_t i = 0; i < player_max_weapons; i++) {
    if (m_current_weapons[i] != nullptr) {
      delete m_current_weapons[i];
      m_current_items[i] = nullptr;
    }
  }
}
void drawHUD() {}
void PlayerHuman::updateTank(
    /* Pass in all paramaters that are associated with a tank */) {}
Tank* PlayerHuman::getCurrentTank() { return m_current_tank; }
std::string PlayerHuman::getTankType() { return m_tank_type; }
Item** PlayerHuman::getCurrentItems() { return m_current_items; }
Weapon** PlayerHuman::getCurrentWeapons() { return m_current_weapons; }
std::string PlayerHuman::getAiType() { return m_ai_type; }
void PlayerHuman::setAiType(const std::string& new_ai_type) {
  m_ai_type = new_ai_type;
}
std::string PlayerHuman::getPlayerType() { return m_player_type; }
void PlayerHuman::setPlayerType(const std::string& new_player_type) {
  m_player_type = new_player_type;
}
std::string PlayerHuman::getPlayerName() { return m_name; }
void PlayerHuman::setPlayerName(const std::string& new_name) {
  m_name = new_name;
}
std::int32_t PlayerHuman::getCash() { return m_current_cash; }
void PlayerHuman::setCash(std::int32_t cash) { m_current_cash = cash; }
float PlayerHuman::getRed() { return m_color[0]; }
float PlayerHuman::getGreen() { return m_color[1]; }
float PlayerHuman::getBlue() { return m_color[2]; }
float PlayerHuman::getCurrentWait() { return m_current_wait; }
void PlayerHuman::setCurrentWait(float time) { m_current_wait = time; }
char PlayerHuman::getTeamLabel() { return m_team_label; }
void PlayerHuman::setTeamLabel(char t) { m_team_label = t; }
Weapon* PlayerHuman::getLoadedWeapon() { return m_loaded_weapon; }
void PlayerHuman::setLoadedWeapon(Weapon* wpn) { m_loaded_weapon = wpn; }
float* PlayerHuman::getBalisticMatrix() { return m_balistic_matrix; }
void PlayerHuman::setItems(Item** item_set) {
  for (std::int32_t i = 0; i < player_max_items; i++) {
    m_current_items[i] = item_set[i];
  }
}
void PlayerHuman::setTankType(const std::string& new_tank_type) {
  m_tank_type = new_tank_type;
  delete m_current_tank;
  if (m_tank_type == "Rhinoxx")
    m_current_tank = new TankA(0, 0, 0);
  else if (m_tank_type == "Hellfire")
    m_current_tank = new TankB(0, 0, 0);
  else if (m_tank_type == "HeavyD")
    m_current_tank = new TankC(0, 0, 0);
  else if (m_tank_type == "Panzer")
    m_current_tank = new TankD(0, 0, 0);
  else if (m_tank_type == "Eggroid")
    m_current_tank = new TankE(0, 0, 0);
  else if (m_tank_type == "Behemoth")
    m_current_tank = new TankF(0, 0, 0);
  else if (m_tank_type == "Cubix")
    m_current_tank = new TankG(0, 0, 0);
  else if (m_tank_type == "Predator")
    m_current_tank = new TankH(0, 0, 0);
  else {
    printf(
        "ERROR <PlayerHuman::setTankType(const std::string&)>: Wrong "
        "Tank Type!!\n");
    m_current_tank = nullptr;
  }
}
void PlayerHuman::setWeapons(Weapon** weapon_set) {
  for (std::int32_t i = 0; i < player_max_weapons; i++) {
    m_current_weapons[i] = weapon_set[i];
  }
}