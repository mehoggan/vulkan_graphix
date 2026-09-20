#ifndef PLAYER_CPU_H
#define PLAYER_CPU_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
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
              char* tankType,
              char* ai_type,
              char* name,
              char teamName,
              char* player_type,
              int startingCash);
    ~PlayerCPU() override;
    void updateTank(
            /* Pass in all paramaters that are associated with a tank */)
            override;
    Tank* getCurrentTank() override;
    char* getTankType() override;
    void setTankType(char* tankType) override;
    Item** getCurrentItems() override;
    void setItems(Item** itemSet) override;
    Weapon** getCurrentWeapons() override;
    void setWeapons(Weapon** weaponSet) override;
    char* getAI_Type() override;
    void setAI_Type(char* ai_type) override;
    char* getPlayer_Type() override;
    char* getPlayerName() override;
    void setPlayerName(char* name) override;
    void setPlayer_Type(char* player_type) override;
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
    Tank* currentTank;
    int currentCash;
    char tankType[25];
    Item* currentItems[PLAYER_MAX_ITEMS];
    Weapon* currentWeapons[PLAYER_MAX_WEAPONS];
    char ai_type[128];
    char name[128];
    char player_type[128];
    GLfloat color[4];
    float currentWait;
    char teamLabel;
    Weapon* loadedWeapon;
};

#endif /*	PLAYER_CPU_H	*/