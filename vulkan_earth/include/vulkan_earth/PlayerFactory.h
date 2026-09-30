#ifndef PLAYER_FACTORY_H
#define PLAYER_FACTORY_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string>

class Player;
class GlobalSettings;

const int max_number_of_players = 10;

class PlayerFactory {
public:
    PlayerFactory();
    PlayerFactory(GlobalSettings* new_game_global_settings);
    ~PlayerFactory();
    void setNumberofPlayers(int new_number_of_players);
    int getNumberofPlayers();
    void initializePlayerDataBase();
    void updatePlayerBasicStrings(const std::string& player_type,
                                  const std::string& ai_type,
                                  const std::string& name,
                                  char team_label,
                                  const std::string& tank_type,
                                  int player_number);
    GLfloat* collectPlayerColor(int i);
    Player* getPlayer(int i);

private:
    Player** player_set;
    GlobalSettings* game_global_settings;
    int number_of_players;
    int prev_number_of_players;
    GLfloat player_color[10][4];
};

#endif /*	PLAYER_FACTORY_H	*/