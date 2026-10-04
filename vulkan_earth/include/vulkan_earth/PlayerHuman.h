#ifndef VULKAN_EARTH_PLAYERHUMAN_H
#define VULKAN_EARTH_PLAYERHUMAN_H

#include <cstdint>
#include <string>
#include "vulkan_earth/Player.h"
#include "vulkan_graphix/Math/MathTypes.hpp"

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
    vulkan_graphix::Math::Vec3<float> getEnemyPosition();
    float* getBalisticMatrix();
    void updateBalsticMatrix();

private:
    Tank* m_current_tank;
    std::int32_t m_current_cash;
    std::string m_tank_type;
    Item* m_current_items[player_max_items];
    Weapon* m_current_weapons[player_max_weapons];
    std::string m_ai_type;
    std::string m_name;
    std::string m_player_type;
    float m_color[4];
    float m_current_wait;
    char m_team_label;
    Weapon* m_loaded_weapon;
};

#endif  // VULKAN_EARTH_PLAYERHUMAN_H