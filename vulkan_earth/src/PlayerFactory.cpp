#include "PlayerFactory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "GlobalSettings.h"
#include "Player.h"
#include "PlayerCPU.h"
#include "PlayerHuman.h"
#include "Tank.h"
#include "TankA.h"
#include "macro_crtdbg.h"

PlayerFactory::PlayerFactory() = default;

PlayerFactory::PlayerFactory(GlobalSettings* game_global_settings) {
    this->game_global_settings = game_global_settings;
    this->number_of_players = 2;
    prev_number_of_players = this->number_of_players;
    player_set = new Player*[max_number_of_players]; /*	10 IS THE MAXIMUM
                                                        NUMBER OF PLAYERS	*/
    for (int p = 0; p < max_number_of_players; p++) {
        player_set[p] = nullptr;
    }
    for (int p = 0; p < this->number_of_players; p++) {
        if (!(player_set[p])) {
            player_set[p] = new PlayerCPU(
                    player_color[p][0],
                    player_color[p][1],
                    player_color[p][2],
                    "Rhinoxx",
                    "Shooter",
                    "",
                    '-',
                    "CPU",
                    atoi(this->game_global_settings->getCashAtStart()
                                 .c_str()));
        }
    }
    // Set Players' Colors
    player_color[0][0] = 0.95;
    player_color[0][1] = 0.25;
    player_color[0][2] = 0.25;
    player_color[1][0] = 0.85;
    player_color[1][1] = 0.45;
    player_color[1][2] = 0.25;
    player_color[2][0] = 0.75;
    player_color[2][1] = 0.75;
    player_color[2][2] = 0.15;
    player_color[3][0] = 0.25;
    player_color[3][1] = 0.85;
    player_color[3][2] = 0.25;
    player_color[4][0] = 0.05;
    player_color[4][1] = 0.55;
    player_color[4][2] = 0.05;
    player_color[5][0] = 0.25;
    player_color[5][1] = 0.25;
    player_color[5][2] = 0.95;
    player_color[6][0] = 0.25;
    player_color[6][1] = 0.15;
    player_color[6][2] = 0.65;
    player_color[7][0] = 0.75;
    player_color[7][1] = 0.25;
    player_color[7][2] = 0.75;
    player_color[8][0] = 0.25;
    player_color[8][1] = 0.25;
    player_color[8][2] = 0.25;
    player_color[9][0] = 0.95;
    player_color[9][1] = 0.95;
    player_color[9][2] = 0.95;
    for (int x = 0; x < 10; x++) {
        player_color[x][3] = 1.0;
    }
}

PlayerFactory::~PlayerFactory() {
    for (int i = 0; i < number_of_players; i++) delete player_set[i];
    delete[] player_set;
}

void PlayerFactory::setNumberofPlayers(int number_of_players) {
    this->number_of_players = number_of_players;
}
int PlayerFactory::getNumberofPlayers() { return this->number_of_players; }

void PlayerFactory::initializePlayerDataBase() {
    int change_in_number_of_players =
            this->number_of_players - prev_number_of_players;
    if (!change_in_number_of_players) { /* No Need To Initialize Or Remove
                                           Players	*/
        prev_number_of_players = this->number_of_players;
    } else if (change_in_number_of_players <
               0) { /* Number of Players Decreased Remove Players	*/
        for (int p = prev_number_of_players - 1;
             p > (prev_number_of_players + change_in_number_of_players) - 1;
             p--) {
            delete player_set[p];
            player_set[p] = nullptr;
        }
        prev_number_of_players = this->number_of_players;
    } else if (change_in_number_of_players >
               0) { /* Number of Players Increased	Add Players*/
        for (int p = prev_number_of_players; p < (this->number_of_players);
             p++) {
            player_set[p] = new PlayerCPU(
                    player_color[p][0],
                    player_color[p][1],
                    player_color[p][2],
                    "Rhinoxx",
                    "Shooter",
                    "",
                    '-',
                    "CPU",
                    atoi(this->game_global_settings->getCashAtStart()
                                 .c_str()));
        }
        prev_number_of_players = this->number_of_players;
    }
}

void PlayerFactory::updatePlayerBasicStrings(const std::string& player_type,
                                             const std::string& ai_type,
                                             const std::string& name,
                                             char team_label,
                                             const std::string& tank_type,
                                             int player_number) {
    if (player_type == "CPU") {
        delete player_set[player_number];
        player_set[player_number] = new PlayerCPU(
                player_color[player_number][0],
                player_color[player_number][1],
                player_color[player_number][2],
                tank_type,
                ai_type,
                name,
                team_label,
                player_type,
                atoi(this->game_global_settings->getCashAtStart().c_str()));
    } else if (player_type == "HUMAN") {
        if (player_set[player_number]->getPlayerType() == "CPU") {
            delete player_set[player_number];
            player_set[player_number] = new PlayerHuman(
                    player_color[player_number][0],
                    player_color[player_number][1],
                    player_color[player_number][2],
                    tank_type,
                    ai_type,
                    name,
                    team_label,
                    player_type,
                    atoi(this->game_global_settings->getCashAtStart()
                                 .c_str()));
        } else {
            player_set[player_number]->setPlayerType(player_type);
            player_set[player_number]->setPlayerName(name);
            player_set[player_number]->setTeamLabel(team_label);
            player_set[player_number]->setTankType(tank_type);
        }
    }

    if (player_set[player_number]->getCurrentTank() != nullptr) {
        player_set[player_number]->getCurrentTank()->changeHeadTexture(
                player_number);
    }
}
GLfloat* PlayerFactory::collectPlayerColor(int i) { return player_color[i]; }

Player* PlayerFactory::getPlayer(int i) { return player_set[i]; }