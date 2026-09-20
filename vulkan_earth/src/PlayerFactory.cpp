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
    this->prev_number_of_players = this->number_of_players;
    this->player_set =
            new Player*[MAX_NUMBER_OF_PLAYERS]; /*	10 IS THE MAXIMUM NUMBER OF
                                                   PLAYERS	*/
    for (int p = 0; p < MAX_NUMBER_OF_PLAYERS; p++) {
        this->player_set[p] = nullptr;
    }
    for (int p = 0; p < this->number_of_players; p++) {
        if (!(this->player_set[p])) {
            this->player_set[p] = new PlayerCPU(
                    playerColor[p][0],
                    playerColor[p][1],
                    playerColor[p][2],
                    "Rhinoxx",
                    "Shooter",
                    "",
                    '-',
                    "CPU",
                    atoi(this->game_global_settings->getCash_At_Start()
                                 .c_str()));
        }
    }
    // Set Players' Colors
    playerColor[0][0] = 0.95;
    playerColor[0][1] = 0.25;
    playerColor[0][2] = 0.25;
    playerColor[1][0] = 0.85;
    playerColor[1][1] = 0.45;
    playerColor[1][2] = 0.25;
    playerColor[2][0] = 0.75;
    playerColor[2][1] = 0.75;
    playerColor[2][2] = 0.15;
    playerColor[3][0] = 0.25;
    playerColor[3][1] = 0.85;
    playerColor[3][2] = 0.25;
    playerColor[4][0] = 0.05;
    playerColor[4][1] = 0.55;
    playerColor[4][2] = 0.05;
    playerColor[5][0] = 0.25;
    playerColor[5][1] = 0.25;
    playerColor[5][2] = 0.95;
    playerColor[6][0] = 0.25;
    playerColor[6][1] = 0.15;
    playerColor[6][2] = 0.65;
    playerColor[7][0] = 0.75;
    playerColor[7][1] = 0.25;
    playerColor[7][2] = 0.75;
    playerColor[8][0] = 0.25;
    playerColor[8][1] = 0.25;
    playerColor[8][2] = 0.25;
    playerColor[9][0] = 0.95;
    playerColor[9][1] = 0.95;
    playerColor[9][2] = 0.95;
    for (int x = 0; x < 10; x++) {
        playerColor[x][3] = 1.0;
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
            this->number_of_players - this->prev_number_of_players;
    if (!change_in_number_of_players) { /* No Need To Initialize Or Remove
                                           Players	*/
        this->prev_number_of_players = this->number_of_players;
    } else if (change_in_number_of_players <
               0) { /* Number of Players Decreased Remove Players	*/
        for (int p = this->prev_number_of_players - 1;
             p >
             (this->prev_number_of_players + change_in_number_of_players) - 1;
             p--) {
            delete this->player_set[p];
            this->player_set[p] = nullptr;
        }
        this->prev_number_of_players = this->number_of_players;
    } else if (change_in_number_of_players >
               0) { /* Number of Players Increased	Add Players*/
        for (int p = this->prev_number_of_players;
             p < (this->number_of_players);
             p++) {
            this->player_set[p] = new PlayerCPU(
                    playerColor[p][0],
                    playerColor[p][1],
                    playerColor[p][2],
                    "Rhinoxx",
                    "Shooter",
                    "",
                    '-',
                    "CPU",
                    atoi(this->game_global_settings->getCash_At_Start()
                                 .c_str()));
        }
        this->prev_number_of_players = this->number_of_players;
    }
}

void PlayerFactory::updatePlayerBasicStrings(const std::string& player_type,
                                             const std::string& ai_type,
                                             const std::string& name,
                                             char teamLabel,
                                             const std::string& tank_type,
                                             int player_number) {
    if (player_type == "CPU") {
        delete this->player_set[player_number];
        this->player_set[player_number] = new PlayerCPU(
                playerColor[player_number][0],
                playerColor[player_number][1],
                playerColor[player_number][2],
                tank_type,
                ai_type,
                name,
                teamLabel,
                player_type,
                atoi(this->game_global_settings->getCash_At_Start().c_str()));
    } else if (player_type == "HUMAN") {
        if (player_set[player_number]->getPlayer_Type() == "CPU") {
            delete this->player_set[player_number];
            this->player_set[player_number] = new PlayerHuman(
                    playerColor[player_number][0],
                    playerColor[player_number][1],
                    playerColor[player_number][2],
                    tank_type,
                    ai_type,
                    name,
                    teamLabel,
                    player_type,
                    atoi(this->game_global_settings->getCash_At_Start()
                                 .c_str()));
        } else {
            this->player_set[player_number]->setPlayer_Type(player_type);
            this->player_set[player_number]->setPlayerName(name);
            this->player_set[player_number]->setTeamLabel(teamLabel);
            this->player_set[player_number]->setTankType(tank_type);
        }
    }

    if (this->player_set[player_number]->getCurrentTank() != nullptr) {
        this->player_set[player_number]->getCurrentTank()->changeHeadTexture(
                player_number);
    }
}
GLfloat* PlayerFactory::collectPlayerColor(int i) {
    return this->playerColor[i];
}

Player* PlayerFactory::getPlayer(int i) { return player_set[i]; }