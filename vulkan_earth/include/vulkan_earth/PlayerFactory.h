#ifndef VULKAN_EARTH_PLAYERFACTORY_H
#define VULKAN_EARTH_PLAYERFACTORY_H

#include <cstdint>
#include <string>

class Player;
class GlobalSettings;

const std::int32_t max_number_of_players = 10;

class PlayerFactory {
public:
  PlayerFactory();
  PlayerFactory(GlobalSettings* new_game_global_settings);
  ~PlayerFactory();
  void setNumberofPlayers(std::int32_t new_number_of_players);
  std::int32_t getNumberofPlayers();
  void initializePlayerDataBase();
  void updatePlayerBasicStrings(const std::string& player_type,
      const std::string& ai_type,
      const std::string& name,
      char team_label,
      const std::string& tank_type,
      std::int32_t player_number);
  float* collectPlayerColor(std::int32_t i);
  Player* getPlayer(std::int32_t i);

private:
  Player** m_player_set;
  GlobalSettings* m_game_global_settings;
  std::int32_t m_number_of_players;
  std::int32_t m_prev_number_of_players;
  float m_player_color[10][4];
};

#endif  // VULKAN_EARTH_PLAYERFACTORY_H