#include "vulkan_earth/GlobalSettings.h"
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include "vulkan_earth/TerrainMaker.h"

using namespace std;

GlobalSettings::GlobalSettings() {
  m_player_count = 2;
  m_round_count = 1;

  m_game_speed = "0.5x";
  m_interest_rate = "0.01";
  m_cash_at_start = "$1000";
  m_computers_buy = "false";
  m_free_market = "false";
  m_scoring_mode = "Weak Sauce";
  m_air_viscosity = "Low";
  m_gravity = "0.1";
  m_tanks_fall = "false";
  m_hillyness = "5";
  m_hill_height = "5";
  m_hill_girth = "5";
  m_teams = "Allowed";
  m_status_bar = "false";
  m_play_order = "Sequential";
  m_fast_computers = "false";
  m_talking_tanks = "Yes";
  m_talk_probability = "0.1";
  m_arms_level = "1";
  m_bomb_icon = "Small";
  m_tunneling = "false";
  m_scale = "Small";
  m_trace_path = "false";
}

GlobalSettings::~GlobalSettings() = default;

void GlobalSettings::setVariables(const std::string& global_options,
    const std::string& round_and_player_count) {
  for (std::int32_t i = 0; i < num_options; i++) {
    m_options[i].clear();
  }

  std::int32_t cur_char = 0;
  std::int32_t cur_token_count = 0;
  while (cur_char < static_cast<std::int32_t>(global_options.size())) {
    if (global_options[cur_char] == '/') {
      cur_char++;
      std::string data;
      while (cur_char < static_cast<std::int32_t>(global_options.size()) &&
          global_options[cur_char] != '/') {
        data += global_options[cur_char];
        cur_char++;
      }
      cur_char--;
      if (!data.empty() && data != "Hardware" && data != "Economics" &&
          data != "Physics" && data != "Landscape" && data != "Game Options" &&
          data != "Weapons" && data != "Button") {
        m_options[cur_token_count] = data;
        cur_token_count++;
      }
    }
    cur_char++;
  }
  copyData();

  /*	GET NUMBER OF PLAYERS AND ROUNDS	*/
  std::int32_t cur_char1 = 0;
  std::int32_t cur_token_count1 = 0;
  while (
      cur_char1 < static_cast<std::int32_t>(round_and_player_count.size())) {
    if (round_and_player_count[cur_char1] == '/') {
      cur_char1++;
      std::string data;
      while (cur_char1 <
              static_cast<std::int32_t>(round_and_player_count.size()) &&
          round_and_player_count[cur_char1] != '/') {
        data += round_and_player_count[cur_char1];
        cur_char1++;
      }
      cur_char1--;
      if (!data.empty() && data != "Player Count" && data != "Round Count") {
        if (cur_token_count1 == 0) {
          stringstream ss1(data);
          if (!(ss1 >> m_player_count)) m_player_count = 0;
        } else if (cur_token_count1 == 1) {
          stringstream ss1(data);
          if (!(ss1 >> m_round_count)) m_round_count = 0;
        }
        cur_token_count1++;
      }
    }
    cur_char1++;
  }

  /*	EXIT IF ERROR	*/
  if (m_round_count == 0 || m_player_count == 0) {
    exit(0);
  }

  // printSelf(-1);
}

void GlobalSettings::copyData() {
  m_game_speed = m_options[0];
  m_interest_rate = m_options[1];
  m_cash_at_start = m_options[2];
  m_computers_buy = m_options[3];
  m_free_market = m_options[4];
  m_scoring_mode = m_options[5];
  m_air_viscosity = m_options[6];
  m_gravity = m_options[7];
  m_tanks_fall = m_options[8];
  m_hillyness = m_options[9];
  m_hill_height = m_options[10];
  m_hill_girth = m_options[11];
  m_teams = m_options[12];
  m_status_bar = m_options[13];
  m_play_order = m_options[14];
  m_fast_computers = m_options[15];
  m_talking_tanks = m_options[16];
  m_talk_probability = m_options[17];
  m_arms_level = m_options[18];
  m_bomb_icon = m_options[19];
  m_tunneling = m_options[20];
  m_scale = m_options[21];
  m_trace_path = m_options[22];
}

void GlobalSettings::printSelf(std::int32_t /*index*/) {
  printf("Number of Players == %d\n", m_player_count);
  printf("Number of Rounds == %d\n", m_round_count);

  printf("game speed = %s\n", m_game_speed.c_str());
  printf("interest rate = %s\n", m_interest_rate.c_str());
  printf("cash at start = %s\n", m_cash_at_start.c_str());
  printf("computers buy = %s\n", m_computers_buy.c_str());
  printf("free market = %s\n", m_free_market.c_str());
  printf("scoring mode = %s\n", m_scoring_mode.c_str());
  printf("air viscosity = %s\n", m_air_viscosity.c_str());
  printf("gravity = %s\n", m_gravity.c_str());
  printf("tanks fall = %s\n", m_tanks_fall.c_str());
  printf("hillyness = %s\n", m_hillyness.c_str());
  printf("hill_height = %s\n", m_hill_height.c_str());
  printf("hill girth = %s\n", m_hill_girth.c_str());
  printf("teams = %s\n", m_teams.c_str());
  printf("status bar = %s\n", m_status_bar.c_str());
  printf("play order = %s\n", m_play_order.c_str());
  printf("fast computers = %s\n", m_fast_computers.c_str());
  printf("talking tanks = %s\n", m_talking_tanks.c_str());
  printf("talk probability = %s\n", m_talk_probability.c_str());
  printf("arms level = %s\n", m_arms_level.c_str());
  printf("bomb icon = %s\n", m_bomb_icon.c_str());
  printf("tunneling = %s\n", m_tunneling.c_str());
  printf("scale = %s\n", m_scale.c_str());
  printf("trace path = %s\n", m_trace_path.c_str());
}

std::string GlobalSettings::getGameSpeed() { return m_game_speed; }
std::string GlobalSettings::getInterestRate() { return m_interest_rate; }
std::string GlobalSettings::getCashAtStart() { return m_cash_at_start; }
std::string GlobalSettings::getComputersBuy() { return m_computers_buy; }
std::string GlobalSettings::getFreeMarket() { return m_free_market; }
std::string GlobalSettings::getScoringMode() { return m_scoring_mode; }
std::string GlobalSettings::getAirViscosity() { return m_air_viscosity; }
std::string GlobalSettings::getGravity() { return m_gravity; }
std::string GlobalSettings::getTanksFall() { return m_tanks_fall; }
std::string GlobalSettings::getHillyness() { return m_hillyness; }
std::string GlobalSettings::getHillHeight() { return m_hill_height; }
std::string GlobalSettings::getHillGirth() { return m_hill_girth; }
std::string GlobalSettings::getTeams() { return m_teams; }
std::string GlobalSettings::getStatusBar() { return m_status_bar; }
std::string GlobalSettings::getPlayOrder() { return m_play_order; }
std::string GlobalSettings::getFastComputers() { return m_fast_computers; }
std::string GlobalSettings::getTalkingTanks() { return m_talking_tanks; }
std::string GlobalSettings::getTalkProbability() { return m_talk_probability; }
std::string GlobalSettings::getArmsLevel() { return m_arms_level; }
std::string GlobalSettings::getBombIcon() { return m_bomb_icon; }
std::string GlobalSettings::getTunneling() { return m_tunneling; }
std::string GlobalSettings::getScale() { return m_scale; }
std::string GlobalSettings::getTracePath() { return m_trace_path; }
std::int32_t GlobalSettings::getPlayerCount() { return m_player_count; }
std::int32_t GlobalSettings::getRoundCount() { return m_round_count; }
void GlobalSettings::setCurrentTerrain(TerrainMaker* new_terrain) {
  m_current_terrain = new_terrain;
}
TerrainMaker* GlobalSettings::getCurrentTerrain() { return m_current_terrain; }
