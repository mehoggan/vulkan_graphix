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
                const std::string& tankType,
                const std::string& ai_type,
                const std::string& name,
                char teamLabel,
                const std::string& player_type,
                int startingCash);
    ~PlayerHuman() override;
    void updateTank(
            /* Pass in all paramaters that are associated with a tank */)
            override;
    Tank* getCurrentTank() override;
    std::string getTankType() override;
    void setTankType(const std::string& tankType) override;
    Item** getCurrentItems() override;
    void setItems(Item** itemSet) override;
    Weapon** getCurrentWeapons() override;
    void setWeapons(Weapon** weaponSet) override;
    std::string getAI_Type() override;
    void setAI_Type(const std::string& ai_type) override;
    std::string getPlayer_Type() override;
    void setPlayer_Type(const std::string& player_type) override;
    std::string getPlayerName() override;
    void setPlayerName(const std::string& name) override;
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
    void selectTarget(PlayerFactory* pf);
    Tank* getCurrentTarget(int i);
    Vector getEnemyPosition();
    GLfloat* getBalisticMatrix();
    void updateBalsticMatrix();

private:
    Tank* currentTank;
    int currentCash;
    std::string tankType;
    Item* currentItems[PLAYER_MAX_ITEMS];
    Weapon* currentWeapons[PLAYER_MAX_WEAPONS];
    std::string ai_type;
    std::string name;
    std::string player_type;
    GLfloat color[4];
    float currentWait;
    char teamLabel;
    Weapon* loadedWeapon;
};

#endif /*	PLAYER_HUMAN_H	*/