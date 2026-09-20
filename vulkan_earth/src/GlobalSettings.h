#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

const int NUM_OPTIONS = 23;
const int MAX_NUM_PLAYERS = 10;

class TerrainMaker;

class GlobalSettings {
	public:
		GlobalSettings();
		~GlobalSettings();
		void setVariables(const char* global_options, const char* round_and_player_count);
		char* getGame_Speed();
		char* getInterest_Rate();
		char* getCash_At_Start();
		char* getComputers_Buy();
		char* getFree_Market();
		char* getScoring_Mode();
		char* getAir_Viscosity();
		char* getGravity();
		char* getTanks_Fall();
		char* getHillyness();
		char* getHill_Height();
		char* getHill_Girth();
		char* getTeams();
		char* getStatus_bar();
		char* getPlay_Order();
		char* getFast_Computers();
		char* getTalking_Tanks();
		char* getTalk_Probability();
		char* getArms_Level();
		char* getBomb_Icon();
		char* getTunneling();
		char* getScale();
		char* getTrace_Path();
		int getPlayer_Count();
		int getRound_Count();
		void setCurrentTerrain(TerrainMaker* newTerrain);
		TerrainMaker* getCurrentTerrain();
	private:
		char* options[NUM_OPTIONS];
	
		/*	HARDWARE SUBMENU	*/
		char* game_speed;
		/*	ECONOMICS SUBMENU	*/
		char* interest_rate;
		char* cash_at_start;
		char* computers_buy;
		char* free_market;
		char* scoring_mode;
		/*	PHYSICS SUBMENU	*/
		char* air_viscosity;
		char* gravity;
		char* tanks_fall;
		/*	LANDSCAPE SUBMENU	*/
		char* hillyness;
		char* hill_height;
		char* hill_girth;
		/*	PLAY SETTINGS SUBMENU	*/
		char* teams;
		char* status_bar;
		char* play_order;
		char* fast_computers;
		char* talking_tanks;
		char* talk_probability;
		/*	WEAPONS SUBMENU	*/
		char* arms_level;
		char* bomb_icon;
		char* tunneling;
		char* scale;
		char* trace_path;
		/*	PLAYER AND ROUND DATA	*/
		int player_count;
		int round_count;
		
		void printSelf(int index);
		void copyData();
		
		TerrainMaker* currentTerrain;
};

#endif /*	GLOBAL_SETTINGS_H	*/