#include "vulkan_earth/PlayerCPU.h"
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

PlayerCPU::PlayerCPU() = default;

PlayerCPU::PlayerCPU(float red, float green, float blue) {
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

PlayerCPU::PlayerCPU(float red,
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

PlayerCPU::~PlayerCPU() {
  delete m_current_tank;

  // before we delete inventory, where should we store the inventory? figure
  // this out later
  for (std::int32_t i = 0; i < player_max_items; i++)
    delete m_current_items[i];
  for (std::int32_t i = 0; i < player_max_weapons; i++)
    delete m_current_weapons[i];
}

void PlayerCPU::updateTank(
    /* Pass in all paramaters that are associated with a tank */) {}
Tank* PlayerCPU::getCurrentTank() { return m_current_tank; }
std::string PlayerCPU::getTankType() { return m_tank_type; }
Item** PlayerCPU::getCurrentItems() { return m_current_items; }
Weapon** PlayerCPU::getCurrentWeapons() { return m_current_weapons; }
std::string PlayerCPU::getAiType() { return m_ai_type; }
void PlayerCPU::setAiType(const std::string& new_ai_type) {
  m_ai_type = new_ai_type;
}
std::string PlayerCPU::getPlayerType() { return m_player_type; }
void PlayerCPU::setPlayerType(const std::string& new_player_type) {
  m_player_type = new_player_type;
}
float PlayerCPU::getRed() { return m_color[0]; }
float PlayerCPU::getGreen() { return m_color[1]; }
float PlayerCPU::getBlue() { return m_color[2]; }
std::string PlayerCPU::getPlayerName() { return m_player_type; }
std::int32_t PlayerCPU::getCash() { return m_current_cash; }
void PlayerCPU::setCash(std::int32_t cash) { m_current_cash = cash; }
float PlayerCPU::getCurrentWait() { return m_current_wait; }
void PlayerCPU::setCurrentWait(float time) { m_current_wait = time; }
char PlayerCPU::getTeamLabel() { return m_team_label; }
void PlayerCPU::setTeamLabel(char t) { m_team_label = t; }
Weapon* PlayerCPU::getLoadedWeapon() { return m_loaded_weapon; }
void PlayerCPU::setLoadedWeapon(Weapon* wpn) { m_loaded_weapon = wpn; }
void PlayerCPU::setPlayerName(const std::string& /*new_name*/) {
  printf(
      "\nYou're trying to set a new_name for CPU. It won't happen, "
      "sorry.\n");
  m_name = "CPU";
}
void PlayerCPU::setWeapons(Weapon** weapon_set) {
  for (std::int32_t i = 0; i < player_max_weapons; i++) {
    m_current_weapons[i] = weapon_set[i];
  }
}
void PlayerCPU::setItems(Item** item_set) {
  for (std::int32_t i = 0; i < player_max_items; i++) {
    m_current_items[i] = item_set[i];
  }
}
void PlayerCPU::setTankType(const std::string& new_tank_type) {
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
    printf("ERROR: Wrong Tank Type!!\n");
    m_current_tank = nullptr;
  }
}