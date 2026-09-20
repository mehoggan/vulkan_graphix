#ifndef PLAYER_FACTORY_H
#define PLAYER_FACTORY_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

class Player;
class GlobalSettings;

const int MAX_NUMBER_OF_PLAYERS = 10;

class PlayerFactory {
	public:
		PlayerFactory();
		PlayerFactory(GlobalSettings* game_global_settings);
		~PlayerFactory();
		void setNumberofPlayers(int number_of_players);
		int getNumberofPlayers();
		void initializePlayerDataBase();
		void updatePlayerBasicStrings(	char* player_type, char* ai_type, char* name, char teamLabel,
										char* tank_type, int player_number);
		GLfloat* collectPlayerColor(int i);
		Player* getPlayer(int i);
	private:
		Player** player_set;
		GlobalSettings* game_global_settings;
		int number_of_players;
		int prev_number_of_players;
		GLfloat playerColor[10][4];
};

#endif /*	PLAYER_FACTORY_H	*/