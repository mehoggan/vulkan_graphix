#ifndef PLAYER_HUMAN_H
#define PLAYER_HUMAN_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "Player.h"

class Tank;
class Item;
class Weapon;
class PlayerFactory;

class PlayerHuman : public Player {
public:
    PlayerHuman();
    PlayerHuman(GLfloat red, GLfloat green, GLfloat blue);
    PlayerHuman(GLfloat red,
                GLfloat green,
                GLfloat blue,
                const std::string& new_tank_type,
                const std::string& new_ai_type,
                const std::string& new_name,
                char new_team_label,
                const std::string& new_player_type,
                int starting_cash);
    ~PlayerHuman() override;
    void updateTank(
            /* Pass in all paramaters that are associated with a tank */)
            override;
    Tank* getCurrentTank() override;
    std::string getTankType() override;
    void setTankType(const std::string& new_tank_type) override;
    Item** getCurrentItems() override;
    void setItems(Item** item_set) override;
    Weapon** getCurrentWeapons() override;
    void setWeapons(Weapon** weapon_set) override;
    std::string getAiType() override;
    void setAiType(const std::string& new_ai_type) override;
    std::string getPlayerType() override;
    void setPlayerType(const std::string& new_player_type) override;
    std::string getPlayerName() override;
    void setPlayerName(const std::string& new_name) override;
    int getCash() override;
    void setCash(int cash) override;
    float getCurrentWait() override;
    void setCurrentWait(float time) override;
    GLfloat getRed() override;
    GLfloat getGreen() override;
    GLfloat getBlue() override;
    char getTeamLabel() override;
    void setTeamLabel(char t) override;
    Weapon* getLoadedWeapon() override;
    void setLoadedWeapon(Weapon* wpn) override;
    void selectTarget(PlayerFactory* pf);
    Tank* getCurrentTarget(int i);
    Vector getEnemyPosition();
    GLfloat* getBalisticMatrix();
    void updateBalsticMatrix();

private:
    Tank* current_tank;
    int current_cash;
    std::string tank_type;
    Item* current_items[player_max_items];
    Weapon* current_weapons[player_max_weapons];
    std::string ai_type;
    std::string name;
    std::string player_type;
    GLfloat color[4];
    float current_wait;
    char team_label;
    Weapon* loaded_weapon;
};

#endif /*	PLAYER_HUMAN_H	*/