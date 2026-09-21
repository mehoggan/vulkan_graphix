#ifndef PLAYER_CPU_H
#define PLAYER_CPU_H

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

class PlayerCPU : public Player {
public:
    PlayerCPU();
    PlayerCPU(GLfloat red, GLfloat green, GLfloat blue);
    PlayerCPU(GLfloat red,
              GLfloat green,
              GLfloat blue,
              const std::string& tank_type,
              const std::string& ai_type,
              const std::string& name,
              char team_name,
              const std::string& player_type,
              int starting_cash);
    ~PlayerCPU() override;
    void updateTank(
            /* Pass in all paramaters that are associated with a tank */)
            override;
    Tank* getCurrentTank() override;
    std::string getTankType() override;
    void setTankType(const std::string& tank_type) override;
    Item** getCurrentItems() override;
    void setItems(Item** item_set) override;
    Weapon** getCurrentWeapons() override;
    void setWeapons(Weapon** weapon_set) override;
    std::string getAiType() override;
    void setAiType(const std::string& ai_type) override;
    std::string getPlayerType() override;
    std::string getPlayerName() override;
    void setPlayerName(const std::string& name) override;
    void setPlayerType(const std::string& player_type) override;
    int getCash() override;
    void setCash(int money) override;
    float getCurrentWait() override;
    void setCurrentWait(float time) override;
    GLfloat getRed() override;
    GLfloat getGreen() override;
    GLfloat getBlue() override;
    char getTeamLabel() override;
    void setTeamLabel(char t) override;
    Weapon* getLoadedWeapon() override;
    void setLoadedWeapon(Weapon* wpn) override;

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

#endif /*	PLAYER_CPU_H	*/