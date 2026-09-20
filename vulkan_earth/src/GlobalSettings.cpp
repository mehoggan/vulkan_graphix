#include "GlobalSettings.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>
#include "TerrainMaker.h"
#include "macro_crtdbg.h"

using namespace std;

GlobalSettings::GlobalSettings() {
    player_count = 2;
    round_count = 1;

    game_speed = "0.5x";
    interest_rate = "0.01";
    cash_at_start = "$1000";
    computers_buy = "false";
    free_market = "false";
    scoring_mode = "Weak Sauce";
    air_viscosity = "Low";
    gravity = "0.1";
    tanks_fall = "false";
    hillyness = "5";
    hill_height = "5";
    hill_girth = "5";
    teams = "Allowed";
    status_bar = "false";
    play_order = "Sequential";
    fast_computers = "false";
    talking_tanks = "Yes";
    talk_probability = "0.1";
    arms_level = "1";
    bomb_icon = "Small";
    tunneling = "false";
    scale = "Small";
    trace_path = "false";
}

GlobalSettings::~GlobalSettings() = default;

void GlobalSettings::setVariables(const std::string& global_options,
                                  const std::string& round_and_player_count) {
    for (int i = 0; i < NUM_OPTIONS; i++) {
        options[i].clear();
    }

    int cur_char = 0;
    int cur_token_count = 0;
    while (cur_char < static_cast<int>(global_options.size())) {
        if (global_options[cur_char] == '/') {
            cur_char++;
            std::string data;
            while (cur_char < static_cast<int>(global_options.size()) &&
                   global_options[cur_char] != '/') {
                data += global_options[cur_char];
                cur_char++;
            }
            cur_char--;
            if (data != "" && data != "Hardware" && data != "Economics" &&
                data != "Physics" && data != "Landscape" &&
                data != "Game Options" && data != "Weapons" &&
                data != "Button") {
                options[cur_token_count] = data;
                cur_token_count++;
            }
        }
        cur_char++;
    }
    copyData();

    /*	GET NUMBER OF PLAYERS AND ROUNDS	*/
    int cur_char1 = 0;
    int cur_token_count1 = 0;
    while (cur_char1 < static_cast<int>(round_and_player_count.size())) {
        if (round_and_player_count[cur_char1] == '/') {
            cur_char1++;
            std::string data;
            while (cur_char1 <
                           static_cast<int>(round_and_player_count.size()) &&
                   round_and_player_count[cur_char1] != '/') {
                data += round_and_player_count[cur_char1];
                cur_char1++;
            }
            cur_char1--;
            if (data != "" && data != "Player Count" &&
                data != "Round Count") {
                if (cur_token_count1 == 0) {
                    stringstream ss1(data);
                    if (!(ss1 >> player_count)) player_count = 0;
                } else if (cur_token_count1 == 1) {
                    stringstream ss1(data);
                    if (!(ss1 >> round_count)) round_count = 0;
                }
                cur_token_count1++;
            }
        }
        cur_char1++;
    }

    /*	EXIT IF ERROR	*/
    if (round_count == 0 || player_count == 0) {
        exit(0);
    }

    // printSelf(-1);
}

void GlobalSettings::copyData() {
    game_speed = options[0];
    interest_rate = options[1];
    cash_at_start = options[2];
    computers_buy = options[3];
    free_market = options[4];
    scoring_mode = options[5];
    air_viscosity = options[6];
    gravity = options[7];
    tanks_fall = options[8];
    hillyness = options[9];
    hill_height = options[10];
    hill_girth = options[11];
    teams = options[12];
    status_bar = options[13];
    play_order = options[14];
    fast_computers = options[15];
    talking_tanks = options[16];
    talk_probability = options[17];
    arms_level = options[18];
    bomb_icon = options[19];
    tunneling = options[20];
    scale = options[21];
    trace_path = options[22];
}

void GlobalSettings::printSelf(int index) {
    printf("Number of Players == %d\n", player_count);
    printf("Number of Rounds == %d\n", round_count);

    printf("game speed = %s\n", game_speed.c_str());
    printf("interest rate = %s\n", interest_rate.c_str());
    printf("cash at start = %s\n", cash_at_start.c_str());
    printf("computers buy = %s\n", computers_buy.c_str());
    printf("free market = %s\n", free_market.c_str());
    printf("scoring mode = %s\n", scoring_mode.c_str());
    printf("air viscosity = %s\n", air_viscosity.c_str());
    printf("gravity = %s\n", gravity.c_str());
    printf("tanks fall = %s\n", tanks_fall.c_str());
    printf("hillyness = %s\n", hillyness.c_str());
    printf("hill_height = %s\n", hill_height.c_str());
    printf("hill girth = %s\n", hill_girth.c_str());
    printf("teams = %s\n", teams.c_str());
    printf("status bar = %s\n", status_bar.c_str());
    printf("play order = %s\n", play_order.c_str());
    printf("fast computers = %s\n", fast_computers.c_str());
    printf("talking tanks = %s\n", talking_tanks.c_str());
    printf("talk probability = %s\n", talk_probability.c_str());
    printf("arms level = %s\n", arms_level.c_str());
    printf("bomb icon = %s\n", bomb_icon.c_str());
    printf("tunneling = %s\n", tunneling.c_str());
    printf("scale = %s\n", scale.c_str());
    printf("trace path = %s\n", trace_path.c_str());
}

std::string GlobalSettings::getGame_Speed() { return game_speed; }
std::string GlobalSettings::getInterest_Rate() { return interest_rate; }
std::string GlobalSettings::getCash_At_Start() { return cash_at_start; }
std::string GlobalSettings::getComputers_Buy() { return computers_buy; }
std::string GlobalSettings::getFree_Market() { return free_market; }
std::string GlobalSettings::getScoring_Mode() { return scoring_mode; }
std::string GlobalSettings::getAir_Viscosity() { return air_viscosity; }
std::string GlobalSettings::getGravity() { return gravity; }
std::string GlobalSettings::getTanks_Fall() { return tanks_fall; }
std::string GlobalSettings::getHillyness() { return hillyness; }
std::string GlobalSettings::getHill_Height() { return hill_height; }
std::string GlobalSettings::getHill_Girth() { return hill_girth; }
std::string GlobalSettings::getTeams() { return teams; }
std::string GlobalSettings::getStatus_bar() { return status_bar; }
std::string GlobalSettings::getPlay_Order() { return play_order; }
std::string GlobalSettings::getFast_Computers() { return fast_computers; }
std::string GlobalSettings::getTalking_Tanks() { return talking_tanks; }
std::string GlobalSettings::getTalk_Probability() { return talk_probability; }
std::string GlobalSettings::getArms_Level() { return arms_level; }
std::string GlobalSettings::getBomb_Icon() { return bomb_icon; }
std::string GlobalSettings::getTunneling() { return tunneling; }
std::string GlobalSettings::getScale() { return scale; }
std::string GlobalSettings::getTrace_Path() { return trace_path; }
int GlobalSettings::getPlayer_Count() { return player_count; }
int GlobalSettings::getRound_Count() { return round_count; }
void GlobalSettings::setCurrentTerrain(TerrainMaker* newTerrain) {
    currentTerrain = newTerrain;
}
TerrainMaker* GlobalSettings::getCurrentTerrain() { return currentTerrain; }
