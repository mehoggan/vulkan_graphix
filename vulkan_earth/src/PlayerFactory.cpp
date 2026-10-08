#include "vulkan_earth/PlayerFactory.h"
#include <stdlib.h>
#include <cstdint>
#include <string>
#include "vulkan_earth/GlobalSettings.h"
#include "vulkan_earth/Player.h"
#include "vulkan_earth/PlayerCPU.h"
#include "vulkan_earth/PlayerHuman.h"
#include "vulkan_earth/Tank.h"

PlayerFactory::PlayerFactory() = default;

PlayerFactory::PlayerFactory(GlobalSettings* new_game_global_settings) {
  m_game_global_settings = new_game_global_settings;
  m_number_of_players = 2;
  m_prev_number_of_players = m_number_of_players;
  m_player_set = new Player*[max_number_of_players]; /*	10 IS THE MAXIMUM
                                                      NUMBER OF PLAYERS	*/
  for (std::int32_t p = 0; p < max_number_of_players; p++) {
    m_player_set[p] = nullptr;
  }
  for (std::int32_t p = 0; p < m_number_of_players; p++) {
    if (!(m_player_set[p])) {
      m_player_set[p] = new PlayerCPU(m_player_color[p][0],
          m_player_color[p][1],
          m_player_color[p][2],
          "Rhinoxx",
          "Shooter",
          "",
          '-',
          "CPU",
          atoi(m_game_global_settings->getCashAtStart().c_str()));
    }
  }
  // Set Players' Colors
  m_player_color[0][0] = 0.95;
  m_player_color[0][1] = 0.25;
  m_player_color[0][2] = 0.25;
  m_player_color[1][0] = 0.85;
  m_player_color[1][1] = 0.45;
  m_player_color[1][2] = 0.25;
  m_player_color[2][0] = 0.75;
  m_player_color[2][1] = 0.75;
  m_player_color[2][2] = 0.15;
  m_player_color[3][0] = 0.25;
  m_player_color[3][1] = 0.85;
  m_player_color[3][2] = 0.25;
  m_player_color[4][0] = 0.05;
  m_player_color[4][1] = 0.55;
  m_player_color[4][2] = 0.05;
  m_player_color[5][0] = 0.25;
  m_player_color[5][1] = 0.25;
  m_player_color[5][2] = 0.95;
  m_player_color[6][0] = 0.25;
  m_player_color[6][1] = 0.15;
  m_player_color[6][2] = 0.65;
  m_player_color[7][0] = 0.75;
  m_player_color[7][1] = 0.25;
  m_player_color[7][2] = 0.75;
  m_player_color[8][0] = 0.25;
  m_player_color[8][1] = 0.25;
  m_player_color[8][2] = 0.25;
  m_player_color[9][0] = 0.95;
  m_player_color[9][1] = 0.95;
  m_player_color[9][2] = 0.95;
  for (std::int32_t x = 0; x < 10; x++) {
    m_player_color[x][3] = 1.0;
  }
}

PlayerFactory::~PlayerFactory() {
  for (std::int32_t i = 0; i < m_number_of_players; i++)
    delete m_player_set[i];
  delete[] m_player_set;
}

void PlayerFactory::setNumberofPlayers(std::int32_t new_number_of_players) {
  m_number_of_players = new_number_of_players;
}
std::int32_t PlayerFactory::getNumberofPlayers() {
  return m_number_of_players;
}

void PlayerFactory::initializePlayerDataBase() {
  std::int32_t change_in_number_of_players =
      m_number_of_players - m_prev_number_of_players;
  if (!change_in_number_of_players) { /* No Need To Initialize Or Remove
                                         Players	*/
    m_prev_number_of_players = m_number_of_players;
  } else if (change_in_number_of_players <
      0) { /* Number of Players Decreased Remove Players	*/
    for (std::int32_t p = m_prev_number_of_players - 1;
        p > (m_prev_number_of_players + change_in_number_of_players) - 1;
        p--) {
      delete m_player_set[p];
      m_player_set[p] = nullptr;
    }
    m_prev_number_of_players = m_number_of_players;
  } else if (change_in_number_of_players >
      0) { /* Number of Players Increased	Add Players*/
    for (std::int32_t p = m_prev_number_of_players; p < (m_number_of_players);
        p++) {
      m_player_set[p] = new PlayerCPU(m_player_color[p][0],
          m_player_color[p][1],
          m_player_color[p][2],
          "Rhinoxx",
          "Shooter",
          "",
          '-',
          "CPU",
          atoi(m_game_global_settings->getCashAtStart().c_str()));
    }
    m_prev_number_of_players = m_number_of_players;
  }
}

void PlayerFactory::updatePlayerBasicStrings(const std::string& player_type,
    const std::string& ai_type,
    const std::string& name,
    char team_label,
    const std::string& tank_type,
    std::int32_t player_number) {
  if (player_type == "CPU") {
    delete m_player_set[player_number];
    m_player_set[player_number] =
        new PlayerCPU(m_player_color[player_number][0],
            m_player_color[player_number][1],
            m_player_color[player_number][2],
            tank_type,
            ai_type,
            name,
            team_label,
            player_type,
            atoi(m_game_global_settings->getCashAtStart().c_str()));
  } else if (player_type == "HUMAN") {
    if (m_player_set[player_number]->getPlayerType() == "CPU") {
      delete m_player_set[player_number];
      m_player_set[player_number] =
          new PlayerHuman(m_player_color[player_number][0],
              m_player_color[player_number][1],
              m_player_color[player_number][2],
              tank_type,
              ai_type,
              name,
              team_label,
              player_type,
              atoi(m_game_global_settings->getCashAtStart().c_str()));
    } else {
      m_player_set[player_number]->setPlayerType(player_type);
      m_player_set[player_number]->setPlayerName(name);
      m_player_set[player_number]->setTeamLabel(team_label);
      m_player_set[player_number]->setTankType(tank_type);
    }
  }

  if (m_player_set[player_number]->getCurrentTank() != nullptr) {
    m_player_set[player_number]->getCurrentTank()->changeHeadTexture(
        player_number);
  }
}
float* PlayerFactory::collectPlayerColor(std::int32_t i) {
  return m_player_color[i];
}

Player* PlayerFactory::getPlayer(std::int32_t i) { return m_player_set[i]; }