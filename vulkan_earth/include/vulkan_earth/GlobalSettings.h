#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include <string>

const int num_options = 23;
const int max_num_players = 10;

class TerrainMaker;

class GlobalSettings {
public:
    GlobalSettings();
    ~GlobalSettings();
    void setVariables(const std::string& global_options,
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
    int getPlayerCount();
    int getRoundCount();
    void setCurrentTerrain(TerrainMaker* new_terrain);
    TerrainMaker* getCurrentTerrain();

private:
    std::string options[num_options];

    /*	HARDWARE SUBMENU	*/
    std::string game_speed;
    /*	ECONOMICS SUBMENU	*/
    std::string interest_rate;
    std::string cash_at_start;
    std::string computers_buy;
    std::string free_market;
    std::string scoring_mode;
    /*	PHYSICS SUBMENU	*/
    std::string air_viscosity;
    std::string gravity;
    std::string tanks_fall;
    /*	LANDSCAPE SUBMENU	*/
    std::string hillyness;
    std::string hill_height;
    std::string hill_girth;
    /*	PLAY SETTINGS SUBMENU	*/
    std::string teams;
    std::string status_bar;
    std::string play_order;
    std::string fast_computers;
    std::string talking_tanks;
    std::string talk_probability;
    /*	WEAPONS SUBMENU	*/
    std::string arms_level;
    std::string bomb_icon;
    std::string tunneling;
    std::string scale;
    std::string trace_path;
    /*	PLAYER AND ROUND DATA	*/
    int player_count;
    int round_count;

    void printSelf(int index);
    void copyData();

    TerrainMaker* current_terrain;
};

#endif /*	GLOBAL_SETTINGS_H	*/
