#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include <string>

const int NUM_OPTIONS = 23;
const int MAX_NUM_PLAYERS = 10;

class TerrainMaker;

class GlobalSettings {
public:
    GlobalSettings();
    ~GlobalSettings();
    void setVariables(const std::string& global_options,
                      const std::string& round_and_player_count);
    std::string getGame_Speed();
    std::string getInterest_Rate();
    std::string getCash_At_Start();
    std::string getComputers_Buy();
    std::string getFree_Market();
    std::string getScoring_Mode();
    std::string getAir_Viscosity();
    std::string getGravity();
    std::string getTanks_Fall();
    std::string getHillyness();
    std::string getHill_Height();
    std::string getHill_Girth();
    std::string getTeams();
    std::string getStatus_bar();
    std::string getPlay_Order();
    std::string getFast_Computers();
    std::string getTalking_Tanks();
    std::string getTalk_Probability();
    std::string getArms_Level();
    std::string getBomb_Icon();
    std::string getTunneling();
    std::string getScale();
    std::string getTrace_Path();
    int getPlayer_Count();
    int getRound_Count();
    void setCurrentTerrain(TerrainMaker* newTerrain);
    TerrainMaker* getCurrentTerrain();

private:
    std::string options[NUM_OPTIONS];

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

    TerrainMaker* currentTerrain;
};

#endif /*	GLOBAL_SETTINGS_H	*/
