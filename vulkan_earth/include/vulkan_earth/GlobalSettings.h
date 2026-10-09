#ifndef VULKAN_EARTH_GLOBALSETTINGS_H
#define VULKAN_EARTH_GLOBALSETTINGS_H

#include <cstdint>
#include <string>

const std::int32_t num_options = 23;
const std::int32_t max_num_players = 10;

class TerrainMaker;

class GlobalSettings {
public:
  GlobalSettings();
  ~GlobalSettings();
  void setVariables(
      const std::string& global_options,
      const std::string& round_and_player_count);
  std::string getGameSpeed();
  std::string getInterestRate();
  std::string getCashAtStart();
  std::string getComputersBuy();
  std::string getFreeMarket();
  std::string getScoringMode();
  std::string getAirViscosity();
  std::string getGravity();
  std::string getTanksFall();
  std::string getHillyness();
  std::string getHillHeight();
  std::string getHillGirth();
  std::string getTeams();
  std::string getStatusBar();
  std::string getPlayOrder();
  std::string getFastComputers();
  std::string getTalkingTanks();
  std::string getTalkProbability();
  std::string getArmsLevel();
  std::string getBombIcon();
  std::string getTunneling();
  std::string getScale();
  std::string getTracePath();
  std::int32_t getPlayerCount();
  std::int32_t getRoundCount();
  void setCurrentTerrain(TerrainMaker* new_terrain);
  TerrainMaker* getCurrentTerrain();

private:
  std::string m_options[num_options];

  /*	HARDWARE SUBMENU	*/
  std::string m_game_speed;
  /*	ECONOMICS SUBMENU	*/
  std::string m_interest_rate;
  std::string m_cash_at_start;
  std::string m_computers_buy;
  std::string m_free_market;
  std::string m_scoring_mode;
  /*	PHYSICS SUBMENU	*/
  std::string m_air_viscosity;
  std::string m_gravity;
  std::string m_tanks_fall;
  /*	LANDSCAPE SUBMENU	*/
  std::string m_hillyness;
  std::string m_hill_height;
  std::string m_hill_girth;
  /*	PLAY SETTINGS SUBMENU	*/
  std::string m_teams;
  std::string m_status_bar;
  std::string m_play_order;
  std::string m_fast_computers;
  std::string m_talking_tanks;
  std::string m_talk_probability;
  /*	WEAPONS SUBMENU	*/
  std::string m_arms_level;
  std::string m_bomb_icon;
  std::string m_tunneling;
  std::string m_scale;
  std::string m_trace_path;
  /*	PLAYER AND ROUND DATA	*/
  std::int32_t m_player_count;
  std::int32_t m_round_count;

  void printSelf(std::int32_t index);
  void copyData();

  TerrainMaker* m_current_terrain;
};

#endif  // VULKAN_EARTH_GLOBALSETTINGS_H
