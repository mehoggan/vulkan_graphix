#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include "GlobalSettings.h"
#include "TerrainMaker.h"
#include "macro_crtdbg.h"

using namespace std;

GlobalSettings::GlobalSettings() {
	for(int i = 0; i < NUM_OPTIONS; i++) {
		this->options[i] = new char[256];
		memset(this->options[i],0,256);
	}
	
	this->player_count = 2;
	this->round_count = 1;
	
	this->game_speed="0.5x";
	this->interest_rate="0.01";
	this->cash_at_start="$1000";
	this->computers_buy="false";
	this->free_market="false";
	this->scoring_mode="Weak Sauce";
	this->air_viscosity="Low";
	this->gravity="0.1";
	this->tanks_fall="false";
	this->hillyness="5";
	this->hill_height="5";
	this->hill_girth="5";
	this->teams="Allowed";
	this->status_bar="false";
	this->play_order="Sequential";
	this->fast_computers="false";
	this->talking_tanks="Yes";
	this->talk_probability="0.1";
	this->arms_level="1";
	this->bomb_icon="Small";
	this->tunneling="false";
	this->scale="Small";
	this->trace_path="false";
}

GlobalSettings::~GlobalSettings() {
	for(int i=0 ; i<NUM_OPTIONS ; i++){
		delete[] this->options[i];
	}
}

void GlobalSettings::setVariables(const char* global_options, const char* round_and_player_count) {
	for(int i = 0; i < NUM_OPTIONS; i++) {
		memset(this->options[i],0,256);
	}

	int index = 0;
	int chars_in_global_options = strlen(global_options);
	int cur_char = 0;
	int cur_token_count = 0;
	while(cur_char < chars_in_global_options) {
		if(*(global_options + cur_char) == '/') {
			cur_char++;
			char DATA[256];
			memset(DATA,0,256);
			int index = 0;
			while((*(global_options + cur_char) != '/') && (cur_char < chars_in_global_options)) {
				DATA[index++]=*(global_options + cur_char);
				cur_char++;
			}
			cur_char--;
			if(	(strcmp(DATA,"") !=0) && ((strcmp(DATA,"Hardware") !=0)) && ((strcmp(DATA,"Economics") !=0)) && 
						((strcmp(DATA,"Physics") !=0)) && ((strcmp(DATA,"Landscape") !=0)) && ((strcmp(DATA,"Game Options") !=0))
						&& ((strcmp(DATA,"Weapons") !=0))&& ((strcmp(DATA,"Button") !=0))) {
				strcpy(this->options[cur_token_count],DATA);
				cur_token_count++;
			}
		}
		cur_char++;
	}
	this->copyData();
	
	/*	GET NUMBER OF PLAYERS AND ROUNDS	*/
	//*
	int chars_in_round_and_player_count = strlen(round_and_player_count);
	int cur_char1 = 0;
	int cur_token_count1 = 0;
	while(cur_char1 < chars_in_round_and_player_count) {
		if(*(round_and_player_count + cur_char1) == '/') {
			cur_char1++;
			char DATA[256];
			memset(DATA,0,256);
			int index = 0;
			while((round_and_player_count[cur_char1] != '/') && (cur_char1 < chars_in_round_and_player_count)) {
				DATA[index++]=round_and_player_count[cur_char1];
				cur_char1++;
			}
			cur_char1--;
			if(	(strcmp(DATA,"") !=0) && ((strcmp(DATA,"Player Count") !=0)) && ((strcmp(DATA,"Round Count")))) {
				if(cur_token_count1 == 0) {
					char* pout1 = DATA;
					stringstream ss1(pout1);
					if(!(ss1 >> this->player_count)) this->player_count=0;
				}else if(cur_token_count1 == 1) {
					char* pout1 = DATA;
					stringstream ss1(pout1);
					if(!(ss1 >> this->round_count)) this->round_count=0;
				}
				cur_token_count1++;
			}
		}
		cur_char1++;
	}
	
	/*	EXIT IF ERROR	*/
	if(this->round_count==0 || this->player_count==0) {
		exit(0);
	}
	
	//this->printSelf(-1);
}

void GlobalSettings::copyData() {
	this->game_speed=this->options[0];
	this->interest_rate=this->options[1];
	this->cash_at_start=this->options[2];
	this->computers_buy=this->options[3];
	this->free_market=this->options[4];
	this->scoring_mode=this->options[5];
	this->air_viscosity=this->options[6];
	this->gravity=this->options[7];
	this->tanks_fall=this->options[8];
	this->hillyness=this->options[9];
	this->hill_height=this->options[10];
	this->hill_girth=this->options[11];
	this->teams=this->options[12];
	this->status_bar=this->options[13];
	this->play_order=this->options[14];
	this->fast_computers=this->options[15];
	this->talking_tanks=this->options[16];
	this->talk_probability=this->options[17];
	this->arms_level=this->options[18];
	this->bomb_icon=this->options[19];
	this->tunneling=this->options[20];
	this->scale=this->options[21];
	this->trace_path=this->options[22];
}

void GlobalSettings::printSelf(int index) {
	printf("Number of Players == %d\n", this->player_count);
	printf("Number of Rounds == %d\n", this->round_count);

	printf("game speed = %s\n",this->game_speed);
	printf("interest rate = %s\n",this->interest_rate);
	printf("cash at start = %s\n",this->cash_at_start);
	printf("computers buy = %s\n",this->computers_buy);
	printf("free market = %s\n",this->free_market);
	printf("scoring mode = %s\n",this->scoring_mode);
	printf("air viscosity = %s\n",this->air_viscosity);
	printf("gravity = %s\n",this->gravity);
	printf("tanks fall = %s\n",this->tanks_fall);
	printf("hillyness = %s\n",this->hillyness);
	printf("hill_height = %s\n",this->hill_height);
	printf("hill girth = %s\n",this->hill_girth);
	printf("teams = %s\n",this->teams);
	printf("status bar = %s\n",this->status_bar);
	printf("play order = %s\n",this->play_order);
	printf("fast computers = %s\n",this->fast_computers);
	printf("talking tanks = %s\n",this->talking_tanks);
	printf("talk probability = %s\n",this->talk_probability);
	printf("arms level = %s\n",this->arms_level);
	printf("bomb icon = %s\n",this->bomb_icon);
	printf("tunneling = %s\n",this->tunneling);
	printf("scale = %s\n",this->scale);
	printf("trace path = %s\n",this->trace_path);
}

char* GlobalSettings::getGame_Speed() { return this->game_speed; }
char* GlobalSettings::getInterest_Rate() { return this->interest_rate; }
char* GlobalSettings::getCash_At_Start() { return this->cash_at_start; }
char* GlobalSettings::getComputers_Buy() { return this->computers_buy; }
char* GlobalSettings::getFree_Market() { return this->free_market; }
char* GlobalSettings::getScoring_Mode() { return this->scoring_mode; }
char* GlobalSettings::getAir_Viscosity() { return this->air_viscosity; }
char* GlobalSettings::getGravity() { return this->gravity; }
char* GlobalSettings::getTanks_Fall() { return this->tanks_fall; }
char* GlobalSettings::getHillyness() { return this->hillyness; }
char* GlobalSettings::getHill_Height() { return this->hill_height; }
char* GlobalSettings::getHill_Girth() { return this->hill_girth; }
char* GlobalSettings::getTeams() { return this->teams; }
char* GlobalSettings::getStatus_bar() { return this->status_bar; }
char* GlobalSettings::getPlay_Order() { return this->play_order; }
char* GlobalSettings::getFast_Computers() { return this->fast_computers; }
char* GlobalSettings::getTalking_Tanks() { return this->talking_tanks; }
char* GlobalSettings::getTalk_Probability() { return this->talk_probability; }
char* GlobalSettings::getArms_Level() { return this->arms_level; }
char* GlobalSettings::getBomb_Icon() { return this->bomb_icon; }
char* GlobalSettings::getTunneling() { return this->tunneling; }
char* GlobalSettings::getScale() { return this->scale; }
char* GlobalSettings::getTrace_Path() { return this->trace_path; }
int GlobalSettings::getPlayer_Count() { return this->player_count; }
int GlobalSettings::getRound_Count() { return this->round_count; }
void GlobalSettings::setCurrentTerrain(TerrainMaker* newTerrain){this->currentTerrain=newTerrain;}
TerrainMaker* GlobalSettings::getCurrentTerrain(){return this->currentTerrain;}
