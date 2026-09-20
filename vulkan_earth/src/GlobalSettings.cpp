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
    this->player_count = 2;
    this->round_count = 1;

    this->game_speed = "0.5x";
    this->interest_rate = "0.01";
    this->cash_at_start = "$1000";
    this->computers_buy = "false";
    this->free_market = "false";
    this->scoring_mode = "Weak Sauce";
    this->air_viscosity = "Low";
    this->gravity = "0.1";
    this->tanks_fall = "false";
    this->hillyness = "5";
    this->hill_height = "5";
    this->hill_girth = "5";
    this->teams = "Allowed";
    this->status_bar = "false";
    this->play_order = "Sequential";
    this->fast_computers = "false";
    this->talking_tanks = "Yes";
    this->talk_probability = "0.1";
    this->arms_level = "1";
    this->bomb_icon = "Small";
    this->tunneling = "false";
    this->scale = "Small";
    this->trace_path = "false";
}

GlobalSettings::~GlobalSettings() = default;

void GlobalSettings::setVariables(const std::string& global_options,
                                  const std::string& round_and_player_count) {
    for (int i = 0; i < NUM_OPTIONS; i++) {
        this->options[i].clear();
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
                this->options[cur_token_count] = data;
                cur_token_count++;
            }
        }
        cur_char++;
    }
    this->copyData();

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
                    if (!(ss1 >> this->player_count)) this->player_count = 0;
                } else if (cur_token_count1 == 1) {
                    stringstream ss1(data);
                    if (!(ss1 >> this->round_count)) this->round_count = 0;
                }
                cur_token_count1++;
            }
        }
        cur_char1++;
    }

    /*	EXIT IF ERROR	*/
    if (this->round_count == 0 || this->player_count == 0) {
        exit(0);
    }

    // this->printSelf(-1);
}

void GlobalSettings::copyData() {
    this->game_speed = this->options[0];
    this->interest_rate = this->options[1];
    this->cash_at_start = this->options[2];
    this->computers_buy = this->options[3];
    this->free_market = this->options[4];
    this->scoring_mode = this->options[5];
    this->air_viscosity = this->options[6];
    this->gravity = this->options[7];
    this->tanks_fall = this->options[8];
    this->hillyness = this->options[9];
    this->hill_height = this->options[10];
    this->hill_girth = this->options[11];
    this->teams = this->options[12];
    this->status_bar = this->options[13];
    this->play_order = this->options[14];
    this->fast_computers = this->options[15];
    this->talking_tanks = this->options[16];
    this->talk_probability = this->options[17];
    this->arms_level = this->options[18];
    this->bomb_icon = this->options[19];
    this->tunneling = this->options[20];
    this->scale = this->options[21];
    this->trace_path = this->options[22];
}

void GlobalSettings::printSelf(int index) {
    printf("Number of Players == %d\n", this->player_count);
    printf("Number of Rounds == %d\n", this->round_count);

    printf("game speed = %s\n", this->game_speed.c_str());
    printf("interest rate = %s\n", this->interest_rate.c_str());
    printf("cash at start = %s\n", this->cash_at_start.c_str());
    printf("computers buy = %s\n", this->computers_buy.c_str());
    printf("free market = %s\n", this->free_market.c_str());
    printf("scoring mode = %s\n", this->scoring_mode.c_str());
    printf("air viscosity = %s\n", this->air_viscosity.c_str());
    printf("gravity = %s\n", this->gravity.c_str());
    printf("tanks fall = %s\n", this->tanks_fall.c_str());
    printf("hillyness = %s\n", this->hillyness.c_str());
    printf("hill_height = %s\n", this->hill_height.c_str());
    printf("hill girth = %s\n", this->hill_girth.c_str());
    printf("teams = %s\n", this->teams.c_str());
    printf("status bar = %s\n", this->status_bar.c_str());
    printf("play order = %s\n", this->play_order.c_str());
    printf("fast computers = %s\n", this->fast_computers.c_str());
    printf("talking tanks = %s\n", this->talking_tanks.c_str());
    printf("talk probability = %s\n", this->talk_probability.c_str());
    printf("arms level = %s\n", this->arms_level.c_str());
    printf("bomb icon = %s\n", this->bomb_icon.c_str());
    printf("tunneling = %s\n", this->tunneling.c_str());
    printf("scale = %s\n", this->scale.c_str());
    printf("trace path = %s\n", this->trace_path.c_str());
}

std::string GlobalSettings::getGame_Speed() { return this->game_speed; }
std::string GlobalSettings::getInterest_Rate() { return this->interest_rate; }
std::string GlobalSettings::getCash_At_Start() { return this->cash_at_start; }
std::string GlobalSettings::getComputers_Buy() { return this->computers_buy; }
std::string GlobalSettings::getFree_Market() { return this->free_market; }
std::string GlobalSettings::getScoring_Mode() { return this->scoring_mode; }
std::string GlobalSettings::getAir_Viscosity() { return this->air_viscosity; }
std::string GlobalSettings::getGravity() { return this->gravity; }
std::string GlobalSettings::getTanks_Fall() { return this->tanks_fall; }
std::string GlobalSettings::getHillyness() { return this->hillyness; }
std::string GlobalSettings::getHill_Height() { return this->hill_height; }
std::string GlobalSettings::getHill_Girth() { return this->hill_girth; }
std::string GlobalSettings::getTeams() { return this->teams; }
std::string GlobalSettings::getStatus_bar() { return this->status_bar; }
std::string GlobalSettings::getPlay_Order() { return this->play_order; }
std::string GlobalSettings::getFast_Computers() {
    return this->fast_computers;
}
std::string GlobalSettings::getTalking_Tanks() { return this->talking_tanks; }
std::string GlobalSettings::getTalk_Probability() {
    return this->talk_probability;
}
std::string GlobalSettings::getArms_Level() { return this->arms_level; }
std::string GlobalSettings::getBomb_Icon() { return this->bomb_icon; }
std::string GlobalSettings::getTunneling() { return this->tunneling; }
std::string GlobalSettings::getScale() { return this->scale; }
std::string GlobalSettings::getTrace_Path() { return this->trace_path; }
int GlobalSettings::getPlayer_Count() { return this->player_count; }
int GlobalSettings::getRound_Count() { return this->round_count; }
void GlobalSettings::setCurrentTerrain(TerrainMaker* newTerrain) {
    this->currentTerrain = newTerrain;
}
TerrainMaker* GlobalSettings::getCurrentTerrain() {
    return this->currentTerrain;
}
