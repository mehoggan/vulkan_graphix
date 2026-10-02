#ifndef PLAYER_HUMAN_H
#define PLAYER_HUMAN_H

#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <string>
#include "vulkan_earth/Player.h"

class Tank;
class Item;
class Weapon;
class PlayerFactory;

class PlayerHuman : public Player {
public:
    PlayerHuman();
    PlayerHuman(float red, float green, float blue);
    PlayerHuman(float red,
                float green,
                float blue,
                const std::string& new_tank_type,
                const std::string& new_ai_type,
                const std::string& new_name,
                char new_team_label,
                const std::string& new_player_type,
                std::int32_t starting_cash);
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
    std::int32_t getCash() override;
    void setCash(std::int32_t cash) override;
    float getCurrentWait() override;
    void setCurrentWait(float time) override;
    float getRed() override;
    float getGreen() override;
    float getBlue() override;
    char getTeamLabel() override;
    void setTeamLabel(char t) override;
    Weapon* getLoadedWeapon() override;
    void setLoadedWeapon(Weapon* wpn) override;
    void selectTarget(PlayerFactory* player_factory_ref);
    Tank* getCurrentTarget(std::int32_t i);
    Vector getEnemyPosition();
    float* getBalisticMatrix();
    void updateBalsticMatrix();

private:
    Tank* current_tank;
    std::int32_t current_cash;
    std::string tank_type;
    Item* current_items[player_max_items];
    Weapon* current_weapons[player_max_weapons];
    std::string ai_type;
    std::string name;
    std::string player_type;
    float color[4];
    float current_wait;
    char team_label;
    Weapon* loaded_weapon;
};

#endif /*	PLAYER_HUMAN_H	*/