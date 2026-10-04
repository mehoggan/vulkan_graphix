#include "vulkan_earth/ShopMenu.h"
#include <cstdint>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemButton.h"
#include "vulkan_earth/ControlItemGrid.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/GlobalSettings.h"
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/Item.h"
#include "vulkan_earth/ItemAntiAcid.h"
#include "vulkan_earth/ItemBigRepair.h"
#include "vulkan_earth/ItemCloak.h"
#include "vulkan_earth/ItemDoubleAction.h"
#include "vulkan_earth/ItemExtraBattery.h"
#include "vulkan_earth/ItemFloat.h"
#include "vulkan_earth/ItemShield.h"
#include "vulkan_earth/ItemSmallRepair.h"
#include "vulkan_earth/Player.h"
#include "vulkan_earth/PlayerFactory.h"
#include "vulkan_earth/PossibleGameStates.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/WeaponAcid.h"
#include "vulkan_earth/WeaponAtom.h"
#include "vulkan_earth/WeaponBFB.h"
#include "vulkan_earth/WeaponEMP.h"
#include "vulkan_earth/WeaponMFB.h"
#include "vulkan_earth/WeaponNuke.h"
#include "vulkan_earth/WeaponPadlock.h"
#include "vulkan_earth/WeaponRevive.h"
#include "vulkan_earth/WeaponTeleport.h"
#include "vulkan_earth/WeaponThor.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);
extern void playMusic(std::int32_t music);

ShopMenu::ShopMenu() = default;
ShopMenu::ShopMenu(float new_width,
                   float new_height,
                   float new_percent_border,
                   GlobalSettings* new_global_settings,
                   PlayerFactory* new_player_factory,
                   std::int32_t* game_state) {
    m_global_settings = new_global_settings;
    m_player_factory = new_player_factory;
    m_current_game_state = game_state;
    m_num_players = new_global_settings->getPlayerCount();
    m_current_player_index = 0;
    m_current_player_balance = 0;

    for (std::int32_t i = 0; i < inven_grid_row; i++) {
        m_inven_wpns[i] = nullptr;
        m_inven_items[i] = nullptr;
        m_label_inven_wpn_remains[i] = nullptr;
        m_label_inven_item_remains[i] = nullptr;
        m_img_inven_wpns[i] = nullptr;
        m_img_inven_items[i] = nullptr;
    }

    m_width = new_width;
    m_height = new_height;
    m_percent_border = new_percent_border;
    m_pos[0] = m_pos[1] = m_pos[2] = 0;

    m_grids[0] = new ControlItemGrid(m_pos[0] - m_width * 0.4,
                                     m_pos[1] + m_height * 0.28,
                                     m_pos[2],
                                     m_width * 0.3,
                                     m_height * 0.3,
                                     shop_grid_row,
                                     shop_grid_col,
                                     0.72,
                                     0.25,
                                     0.41,
                                     false,
                                     false);
    m_grids[1] = new ControlItemGrid(m_pos[0] + m_width * 0.175,
                                     m_pos[1] + m_height * 0.30,
                                     m_pos[2],
                                     m_width * 0.12,
                                     m_height * 0.4,
                                     inven_grid_row,
                                     inven_grid_col,
                                     0.25,
                                     0.7,
                                     0.43,
                                     true,
                                     true);
    m_buttons[0] = new ControlItemButton(nullptr,
                                         m_pos[0] - m_width * 0.34,
                                         m_pos[1] + m_height * 0.35,
                                         m_pos[2] + 0.5,
                                         0.75,
                                         0.75,
                                         0.75,
                                         0.075 * (m_width),
                                         0.04 * (m_height),
                                         "Weapon");
    m_buttons[0]->setToggled(true);
    m_buttons[0]->updateButtonState();

    m_buttons[1] = new ControlItemButton(nullptr,
                                         m_pos[0] - m_width * 0.235,
                                         m_pos[1] + m_height * 0.35,
                                         m_pos[2] + 0.5,
                                         0.75,
                                         0.75,
                                         0.75,
                                         0.075 * (m_width),
                                         0.04 * (m_height),
                                         "Item");
    m_buttons[2] = new ControlItemButton(nullptr,
                                         m_pos[0] - m_width * 0.2,
                                         m_pos[1] - m_height * 0.175,
                                         m_pos[2] + 0.5,
                                         0.75,
                                         0.75,
                                         0.75,
                                         0.075 * (m_width),
                                         0.04 * (m_height),
                                         "Buy");
    m_buttons[3] = new ControlItemButton(nullptr,
                                         m_pos[0] + m_width * 0.275,
                                         m_pos[1] - m_height * 0.175,
                                         m_pos[2] + 0.5,
                                         0.75,
                                         0.75,
                                         0.75,
                                         0.075 * (m_width),
                                         0.04 * (m_height),
                                         "Sell");
    m_buttons[4] = new ControlItemButton(nullptr,
                                         m_pos[0] + m_width * 0.15,
                                         m_pos[1] - m_height * 0.365,
                                         m_pos[2] + 0.5,
                                         0.65,
                                         0.15,
                                         0.15,
                                         0.135 * (m_width),
                                         0.04 * (m_height),
                                         "Finish Shopping");

    /*LABEL PLACEMENT*/
    m_label_wpn = new TextObject(
            "Weapon",
            m_grids[1]->getXPos() + m_grids[1]->getWidth() * 0.05,
            m_grids[1]->getYPos() + 15,
            (m_pos[2] + 1),
            vulkan_earth::FontId::TimesRoman24,
            0.0f,
            0.0f,
            0.0f);
    m_label_item = new TextObject(
            "Item",
            m_grids[1]->getXPos() + m_grids[1]->getWidth() / 1.6,
            m_grids[1]->getYPos() + 15,
            (m_pos[2] + 1),
            vulkan_earth::FontId::TimesRoman24,
            0.0f,
            0.0f,
            0.0f);
    m_label_player_num = new TextObject("Player 1 Balance:",
                                        m_pos[0] - m_width * 0.35,
                                        m_pos[1] - m_height * 0.39,
                                        (m_pos[2] + 1),
                                        vulkan_earth::FontId::TimesRoman24,
                                        0.0f,
                                        0.0f,
                                        0.0f);
    m_label_discription = new TextObject("",
                                         m_pos[0] - m_width * 0.4,
                                         m_pos[1] - m_height * 0.1,
                                         m_pos[2] + 1,
                                         vulkan_earth::FontId::TimesRoman24,
                                         0.0f,
                                         0.0f,
                                         0.0f);
    m_label_buy_price = new TextObject("",
                                       m_pos[0] - m_width * 0.35,
                                       m_pos[1] - m_height * 0.2,
                                       m_pos[2] + 1,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
    m_label_sell_price = new TextObject("$ 0",
                                        m_pos[0] + m_width * 0.15,
                                        m_pos[1] - m_height * 0.2,
                                        m_pos[2] + 1,
                                        vulkan_earth::FontId::TimesRoman24,
                                        0.0f,
                                        0.0f,
                                        0.0f);
    std::string balance = "$ " + std::to_string(m_current_player_balance);
    m_label_player_balance = new TextObject(balance,
                                            m_pos[0] - m_width * 0.2,
                                            m_pos[1] - m_height * 0.39,
                                            (m_pos[2] + 1),
                                            vulkan_earth::FontId::TimesRoman24,
                                            0.0f,
                                            0.0f,
                                            0.0f);

    /*Weapons and Items Creation*/
    m_shop_wpns[0] = new WeaponMFB(0);
    m_shop_wpns[1] = new WeaponBFB(1);
    m_shop_wpns[2] = new WeaponAcid(2);
    m_shop_wpns[3] = new WeaponThor(3);
    m_shop_wpns[4] = new WeaponEMP(4);
    m_shop_wpns[5] = new WeaponPadlock(5);
    m_shop_wpns[6] = new WeaponRevive(6);
    m_shop_wpns[7] = new WeaponTeleport(7);
    m_shop_wpns[8] = new WeaponAtom(8);
    m_shop_wpns[9] = new WeaponNuke(9);

    m_shop_items[0] = new ItemSmallRepair(0);
    m_shop_items[1] = new ItemBigRepair(1);
    m_shop_items[2] = new ItemAntiAcid(2);
    m_shop_items[3] = new ItemDoubleAction(3);
    m_shop_items[4] = new ItemShield(4);
    m_shop_items[5] = new ItemExtraBattery(5);
    m_shop_items[6] = new ItemCloak(6);
    m_shop_items[7] = new ItemFloat(7);

    displayCurrentPlayerInfo();

    /*Images and remainsLabels Creation*/
    for (std::int32_t i = 0; i < num_sales_weapon; i++) {
        m_img_shop_wpns[i] =
                new ImageObject(0,
                                0,
                                2.0f,
                                100,
                                100,
                                .0006 * (m_width),
                                256,
                                256,
                                m_shop_wpns[i]->getImageFileName());
        m_grids[0]->setImageSizeToCell(m_img_shop_wpns[i], 0.8);

        std::string remain =
                "x " + std::to_string(m_shop_wpns[i]->getPackageNum());
        m_label_shop_wpn_remains[i] =
                new TextObject(remain,
                               0,
                               0,
                               0,
                               vulkan_earth::FontId::TimesRoman24,
                               0.6f,
                               0.3f,
                               0.4f);
    }
    for (std::int32_t i = 0; i < num_sales_item; i++) {
        m_img_shop_items[i] =
                new ImageObject(0,
                                0,
                                2.0f,
                                100,
                                100,
                                .0006 * (m_width),
                                256,
                                256,
                                m_shop_items[i]->getImageFileName());
        m_grids[0]->setImageSizeToCell(m_img_shop_items[i], 0.8);

        std::string remain =
                "x " + std::to_string(m_shop_items[i]->getPackageNum());
        m_label_shop_item_remains[i] =
                new TextObject(remain,
                               0,
                               0,
                               0,
                               vulkan_earth::FontId::TimesRoman24,
                               0.6f,
                               0.3f,
                               0.4f);
    }

    /*Set position of images and remainsLabels to shopping cells*/
    std::int32_t index = 0;
    for (std::int32_t r = 0; r < shop_grid_row; r++) {
        for (std::int32_t c = 0; c < shop_grid_col; c++) {
            if (index < num_sales_weapon) {
                m_grids[0]->placeImageToCell(m_img_shop_wpns[index], r, c);
                m_grids[0]->placeTextToCell(
                        m_label_shop_wpn_remains[index], r, c);
            }
            if (index < num_sales_item) {
                m_grids[0]->placeImageToCell(m_img_shop_items[index], r, c);
                m_grids[0]->placeTextToCell(
                        m_label_shop_item_remains[index], r, c);
            }
            index++;
        }
    }
}

ShopMenu::~ShopMenu() {
    delete m_grids[0];
    delete m_grids[1];
    for (std::int32_t i = 0; i < 5; i++) delete m_buttons[i];
    delete m_label_wpn;
    delete m_label_item;
    delete m_label_player_num;
    delete m_label_player_balance;
    delete m_label_discription;
    delete m_label_buy_price;
    delete m_label_sell_price;
    for (std::int32_t i = 0; i < num_sales_weapon; i++) {
        delete m_shop_wpns[i];
        delete m_img_shop_wpns[i];
        delete m_label_shop_wpn_remains[i];
    }
    for (std::int32_t i = 0; i < num_sales_item; i++) {
        delete m_shop_items[i];
        delete m_img_shop_items[i];
        delete m_label_shop_item_remains[i];
    }
    for (std::int32_t i = 0; i < inven_grid_row; i++) {
        delete m_img_inven_wpns[i];
        delete m_label_inven_wpn_remains[i];
        delete m_img_inven_items[i];
        delete m_label_inven_item_remains[i];
    }
}

/*GETTERS & SETTERS*/
void ShopMenu::updateNumPlayers(std::int32_t n) { m_num_players = n; }

void ShopMenu::saveCurrentPlayerInfo() {
    // save currentPlayerBalance and the inventory (Weapon, Item objects, and
    // remainings)
    m_player_factory->getPlayer(m_current_player_index)
            ->setCash(m_current_player_balance);
    m_player_factory->getPlayer(m_current_player_index)
            ->setWeapons(m_inven_wpns);
    m_player_factory->getPlayer(m_current_player_index)
            ->setItems(m_inven_items);

    // clear inventory slots for the next player
    for (std::int32_t i = 0; i < inven_grid_row; i++) {
        if (m_img_inven_wpns[i] != nullptr) {
            delete m_img_inven_wpns[i];
            delete m_label_inven_wpn_remains[i];
            m_img_inven_wpns[i] = nullptr;
            m_label_inven_wpn_remains[i] = nullptr;
            m_inven_wpns[i] = nullptr;
        }
        if (m_img_inven_items[i] != nullptr) {
            delete m_img_inven_items[i];
            delete m_label_inven_item_remains[i];
            m_img_inven_items[i] = nullptr;
            m_label_inven_item_remains[i] = nullptr;
            m_inven_items[i] = nullptr;
        }
    }

    m_buttons[1]->setToggled(false);
    m_buttons[1]->updateButtonState();
    m_buttons[0]->setToggled(true);
    m_buttons[0]->updateButtonState();
}

void ShopMenu::displayCurrentPlayerInfo() {
    while ((m_current_player_index < m_num_players) &&
           (m_player_factory->getPlayer(m_current_player_index)
                    ->getPlayerType() == "CPU")) {
        m_current_player_index++;
    }

    if (m_current_player_index < m_num_players) {
        for (std::int32_t i = 0; i < inven_grid_row; i++) {
            m_inven_wpns[i] =
                    m_player_factory->getPlayer(m_current_player_index)
                            ->getCurrentWeapons()[i];
            if (m_inven_wpns[i]) {
                m_img_inven_wpns[i] =
                        new ImageObject(0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        256,
                                        256,
                                        m_inven_wpns[i]->getImageFileName());
                m_grids[1]->setImageSizeToCell(m_img_inven_wpns[i], 0.8);
                m_grids[1]->placeImageToCell(m_img_inven_wpns[i], i, 0);

                std::string remain =
                        "x " + std::to_string(m_inven_wpns[i]->getRemaining());
                delete m_label_inven_wpn_remains[i];
                m_label_inven_wpn_remains[i] =
                        new TextObject(remain,
                                       0,
                                       0,
                                       0,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0.6f,
                                       0.3f,
                                       0.4f);
                m_grids[1]->placeTextToCell(
                        m_label_inven_wpn_remains[i], i, 0);
            }

            m_inven_items[i] =
                    m_player_factory->getPlayer(m_current_player_index)
                            ->getCurrentItems()[i];
            if (m_inven_items[i]) {
                m_img_inven_items[i] =
                        new ImageObject(0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        256,
                                        256,
                                        m_inven_items[i]->getImageFileName());
                m_grids[1]->setImageSizeToCell(m_img_inven_items[i], 0.8);
                m_grids[1]->placeImageToCell(m_img_inven_items[i], i, 1);

                std::string remain =
                        "x " +
                        std::to_string(m_inven_items[i]->getRemaining());
                delete m_label_inven_item_remains[i];
                m_label_inven_item_remains[i] =
                        new TextObject(remain,
                                       0,
                                       0,
                                       0,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0.6f,
                                       0.3f,
                                       0.4f);
                m_grids[1]->placeTextToCell(
                        m_label_inven_item_remains[i], i, 1);
            }
        }

        // Set next player number label
        delete m_label_player_num;
        std::string label_text = "Player " +
                                 std::to_string(m_current_player_index + 1) +
                                 " Balance:";
        m_label_player_num = new TextObject(label_text,
                                            m_pos[0] - m_width * 0.35,
                                            m_pos[1] - m_height * 0.39,
                                            (m_pos[2] + 1),
                                            vulkan_earth::FontId::TimesRoman24,
                                            0.0f,
                                            0.0f,
                                            0.0f);

        // Set next player balance label
        m_current_player_balance =
                m_player_factory->getPlayer(m_current_player_index)->getCash();
        delete m_label_player_balance;
        std::string balance = "$ " + std::to_string(m_current_player_balance);
        m_label_player_balance =
                new TextObject(balance,
                               m_pos[0] - m_width * 0.2,
                               m_pos[1] - m_height * 0.39,
                               (m_pos[2] + 1),
                               vulkan_earth::FontId::TimesRoman24,
                               0.0f,
                               0.0f,
                               0.0f);

        // Set next player inventory

    } else {
        // printDebugInfo();
        m_current_player_index = 0;
        Mix_HaltMusic();
        *m_current_game_state = GAME_PLAY;
    }
}

void ShopMenu::printDebugInfo() {
    // PLAYER INSPECTION DEBUG
    //***********************
    printf("\n*************************************");
    for (std::int32_t i = 0; i < m_player_factory->getNumberofPlayers(); i++) {
        printf("\n\nPlayer%d", i + 1);
        printf("\nPlayer Type: %s",
               m_player_factory->getPlayer(i)->getPlayerType().c_str());
        printf("\nAI Difficulty: %s",
               m_player_factory->getPlayer(i)->getAiType().c_str());
        printf("\nPlayer Name: %s",
               m_player_factory->getPlayer(i)->getPlayerName().c_str());
        printf("\nTeam Number: %c",
               m_player_factory->getPlayer(i)->getTeamLabel());
        printf("\nTank Type: %s",
               m_player_factory->getPlayer(i)->getTankType().c_str());
        printf("\nCurrent Money: %d",
               m_player_factory->getPlayer(i)->getCash());
        for (std::int32_t j = 0; j < inven_grid_row; j++) {
            if (m_player_factory->getPlayer(i)->getCurrentWeapons()[j]) {
                printf("\nWeapon Slot %i: %s",
                       j,
                       m_player_factory->getPlayer(i)
                               ->getCurrentWeapons()[j]
                               ->getDescription()
                               .c_str());
                printf("\nWeapon Slot %i amount: %i",
                       j,
                       m_player_factory->getPlayer(i)
                               ->getCurrentWeapons()[j]
                               ->getRemaining());
            }
        }
        for (std::int32_t j = 0; j < inven_grid_row; j++) {
            if (m_player_factory->getPlayer(i)->getCurrentItems()[j]) {
                printf("\nItem Slot %i: %s",
                       j,
                       m_player_factory->getPlayer(i)
                               ->getCurrentItems()[j]
                               ->getDescription()
                               .c_str());
                printf("\nItem Slot %i amount: %i",
                       j,
                       m_player_factory->getPlayer(i)
                               ->getCurrentItems()[j]
                               ->getRemaining());
            }
        }
    }
    printf("\n\n*************************************\n\n");
    //**************************
}

void ShopMenu::updateBuyDiscriptLabel() {
    bool* selected_cells = m_grids[0]->getSelectedCells();
    if (m_buttons[0]->isToggled()) {
        for (std::int32_t i = 0; i < num_sales_weapon; i++) {
            if (selected_cells[i]) {
                delete m_label_discription;
                delete m_label_buy_price;
                m_label_discription =
                        new TextObject(m_shop_wpns[i]->getDescription(),
                                       m_pos[0] - m_width * 0.4,
                                       m_pos[1] - m_height * 0.1,
                                       m_pos[2] + 1,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                std::string price =
                        "$ " + std::to_string(m_shop_wpns[i]->getPrice());
                m_label_buy_price =
                        new TextObject(price,
                                       m_pos[0] - m_width * 0.35,
                                       m_pos[1] - m_height * 0.2,
                                       m_pos[2] + 1,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                break;
            } else {
                delete m_label_discription;
                delete m_label_buy_price;
                m_label_discription =
                        new TextObject("",
                                       0,
                                       0,
                                       0,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0,
                                       0,
                                       0);
                m_label_buy_price =
                        new TextObject("",
                                       0,
                                       0,
                                       0,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0,
                                       0,
                                       0);
            }
        }
    } else {
        for (std::int32_t i = 0; i < num_sales_item; i++) {
            if (selected_cells[i]) {
                delete m_label_discription;
                delete m_label_buy_price;
                m_label_discription =
                        new TextObject(m_shop_items[i]->getDescription(),
                                       m_pos[0] - m_width * 0.4,
                                       m_pos[1] - m_height * 0.1,
                                       m_pos[2] + 1,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                std::string price =
                        "$ " + std::to_string(m_shop_items[i]->getPrice());
                m_label_buy_price =
                        new TextObject(price,
                                       m_pos[0] - m_width * 0.35,
                                       m_pos[1] - m_height * 0.2,
                                       m_pos[2] + 1,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                break;
            } else {
                delete m_label_discription;
                delete m_label_buy_price;
                m_label_discription =
                        new TextObject("",
                                       0,
                                       0,
                                       0,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0,
                                       0,
                                       0);
                m_label_buy_price =
                        new TextObject("",
                                       0,
                                       0,
                                       0,
                                       vulkan_earth::FontId::TimesRoman24,
                                       0,
                                       0,
                                       0);
            }
        }
    }
}

void ShopMenu::updateSellLabel() {
    bool* selected_cells = m_grids[1]->getSelectedCells();
    std::int32_t total_sell = 0;

    for (std::int32_t i = 0; i < inven_grid_row * 2; i++) {
        if (selected_cells[i] && m_inven_wpns[i / 2] != nullptr &&
            i % 2 == 0) {  //	i%2 == 0 is weapon inventory
            total_sell += static_cast<std::int32_t>(
                    ((m_inven_wpns[i / 2]->getPrice() /
                      m_inven_wpns[i / 2]->getPackageNum()) /
                     1.5) *
                    m_inven_wpns[i / 2]->getRemaining());
        }
        if (selected_cells[i] && m_inven_items[i / 2] != nullptr &&
            i % 2 == 1) {
            total_sell += static_cast<std::int32_t>(
                    ((m_inven_items[i / 2]->getPrice() /
                      m_inven_items[i / 2]->getPackageNum()) /
                     1.5) *
                    m_inven_items[i / 2]->getRemaining());
        }
    }

    delete m_label_sell_price;
    std::string price = "$ " + std::to_string(total_sell);
    m_label_sell_price = new TextObject(price,
                                        m_pos[0] + m_width * 0.15,
                                        m_pos[1] - m_height * 0.2,
                                        m_pos[2] + 1,
                                        vulkan_earth::FontId::TimesRoman24,
                                        0.0f,
                                        0.0f,
                                        0.0f);
}

void ShopMenu::buyHandler() {
    std::int32_t inven_i;
    bool* selected_cells = m_grids[0]->getSelectedCells();

    if (m_buttons[0]->isToggled()) {
        for (std::int32_t i = 0; i < num_sales_weapon; i++) {
            if (selected_cells[i]) {
                for (inven_i = 0; inven_i < inven_grid_row; inven_i++) {
                    if ((m_inven_wpns[inven_i] == nullptr) ||
                        (m_shop_wpns[i]->getUNIQUEIDENTIFIER() ==
                         m_inven_wpns[inven_i]->getUNIQUEIDENTIFIER()))
                        break;
                }
                if ((m_current_player_balance >= m_shop_wpns[i]->getPrice()) &&
                    (inven_i < inven_grid_row)) {
                    if (m_inven_wpns[inven_i] == nullptr) {
                        playSFX(TRANSACTION);
                        m_img_inven_wpns[inven_i] = new ImageObject(
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                256,
                                256,
                                m_shop_wpns[i]->getImageFileName());
                        m_grids[1]->setImageSizeToCell(
                                m_img_inven_wpns[inven_i], 0.8);
                        m_grids[1]->placeImageToCell(
                                m_img_inven_wpns[inven_i], inven_i, 0);
                        m_inven_wpns[inven_i] =
                                m_shop_wpns[i]->getWeaponInstance();
                        m_current_player_balance -= m_shop_wpns[i]->getPrice();
                    } else {
                        if (m_inven_wpns[inven_i]->getRemaining() <
                            m_inven_wpns[inven_i]->getMaxStack()) {
                            playSFX(TRANSACTION);
                            m_current_player_balance -=
                                    m_shop_wpns[i]->getPrice();
                            m_inven_wpns[inven_i]->setRemaining(
                                    m_inven_wpns[inven_i]->getRemaining() +
                                    m_shop_wpns[i]->getPackageNum());
                            if (m_inven_wpns[inven_i]->getRemaining() >
                                m_inven_wpns[inven_i]->getMaxStack())
                                m_inven_wpns[inven_i]->setRemaining(
                                        m_inven_wpns[inven_i]->getMaxStack());
                        } else {
                            playSFX(INVALID_CLICK);
                        }
                    }

                    delete m_label_player_balance;
                    std::string balance =
                            "$ " + std::to_string(m_current_player_balance);
                    m_label_player_balance =
                            new TextObject(balance,
                                           m_pos[0] - m_width * 0.2,
                                           m_pos[1] - m_height * 0.39,
                                           (m_pos[2] + 1),
                                           vulkan_earth::FontId::TimesRoman24,
                                           0.0f,
                                           0.0f,
                                           0.0f);

                    std::string remain =
                            "x " +
                            std::to_string(
                                    m_inven_wpns[inven_i]->getRemaining());
                    delete m_label_inven_wpn_remains[inven_i];
                    m_label_inven_wpn_remains[inven_i] =
                            new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           vulkan_earth::FontId::TimesRoman24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
                    m_grids[1]->placeTextToCell(
                            m_label_inven_wpn_remains[inven_i], inven_i, 0);
                } else {
                    playSFX(INVALID_CLICK);
                }
                break;
            }
        }
    } else {
        for (std::int32_t i = 0; i < num_sales_item; i++) {
            if (selected_cells[i]) {
                for (inven_i = 0; inven_i < inven_grid_row; inven_i++) {
                    if ((m_inven_items[inven_i] == nullptr) ||
                        (m_shop_items[i]->getUNIQUEIDENTIFIER() ==
                         m_inven_items[inven_i]->getUNIQUEIDENTIFIER()))
                        break;
                }
                if (m_current_player_balance >= m_shop_items[i]->getPrice() &&
                    inven_i < inven_grid_row) {
                    if (m_inven_items[inven_i] == nullptr) {
                        playSFX(TRANSACTION);
                        m_img_inven_items[inven_i] = new ImageObject(
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                256,
                                256,
                                m_shop_items[i]->getImageFileName());
                        m_grids[1]->setImageSizeToCell(
                                m_img_inven_items[inven_i], 0.8);
                        m_grids[1]->placeImageToCell(
                                m_img_inven_items[inven_i], inven_i, 1);
                        m_inven_items[inven_i] =
                                m_shop_items[i]->getItemInstance();
                        m_current_player_balance -=
                                m_shop_items[i]->getPrice();
                    } else {
                        if (m_inven_items[inven_i]->getRemaining() <
                            m_inven_items[inven_i]->getMaxStack()) {
                            playSFX(TRANSACTION);
                            m_current_player_balance -=
                                    m_shop_items[i]->getPrice();
                            m_inven_items[inven_i]->setRemaining(
                                    m_inven_items[inven_i]->getRemaining() +
                                    m_shop_items[i]->getPackageNum());
                            if (m_inven_items[inven_i]->getRemaining() >
                                m_inven_items[inven_i]->getMaxStack())
                                m_inven_items[inven_i]->setRemaining(
                                        m_inven_items[inven_i]->getMaxStack());
                        } else {
                            playSFX(INVALID_CLICK);
                        }
                    }

                    delete m_label_player_balance;
                    std::string balance =
                            "$ " + std::to_string(m_current_player_balance);
                    m_label_player_balance =
                            new TextObject(balance,
                                           m_pos[0] - m_width * 0.2,
                                           m_pos[1] - m_height * 0.39,
                                           (m_pos[2] + 1),
                                           vulkan_earth::FontId::TimesRoman24,
                                           0.0f,
                                           0.0f,
                                           0.0f);

                    std::string remain =
                            "x " +
                            std::to_string(
                                    m_inven_items[inven_i]->getRemaining());
                    delete m_label_inven_item_remains[inven_i];
                    m_label_inven_item_remains[inven_i] =
                            new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           vulkan_earth::FontId::TimesRoman24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
                    m_grids[1]->placeTextToCell(
                            m_label_inven_item_remains[inven_i], inven_i, 1);
                } else {
                    playSFX(INVALID_CLICK);
                }
                break;
            }
        }
    }
}

void ShopMenu::sellHandler() {
    bool* selected_cells = m_grids[1]->getSelectedCells();
    std::int32_t total_sell = 0;

    for (std::int32_t i = 0; i < inven_grid_row * 2; i++) {
        if (selected_cells[i] && m_inven_wpns[i / 2] != nullptr &&
            i % 2 == 0) {  // i%2 == 0 is weapon inventory
            total_sell += static_cast<std::int32_t>(
                    ((m_inven_wpns[i / 2]->getPrice() /
                      m_inven_wpns[i / 2]->getPackageNum()) /
                     1.5) *
                    m_inven_wpns[i / 2]->getRemaining());
            delete m_img_inven_wpns[i / 2];
            delete m_label_inven_wpn_remains[i / 2];
            delete m_inven_wpns[i / 2];
            m_img_inven_wpns[i / 2] = nullptr;
            m_label_inven_wpn_remains[i / 2] = nullptr;
            m_inven_wpns[i / 2] = nullptr;
        }
        if (selected_cells[i] && m_inven_items[i / 2] != nullptr &&
            i % 2 == 1) {
            total_sell += static_cast<std::int32_t>(
                    ((m_inven_items[i / 2]->getPrice() /
                      m_inven_items[i / 2]->getPackageNum()) /
                     1.5) *
                    m_inven_items[i / 2]->getRemaining());
            delete m_img_inven_items[i / 2];
            delete m_label_inven_item_remains[i / 2];
            delete m_inven_items[i / 2];
            m_img_inven_items[i / 2] = nullptr;
            m_label_inven_item_remains[i / 2] = nullptr;
            m_inven_items[i / 2] = nullptr;
        }
    }

    if (total_sell != 0) {
        playSFX(TRANSACTION);
        m_current_player_balance += total_sell;
        delete m_label_player_balance;
        std::string balance = "$ " + std::to_string(m_current_player_balance);
        m_label_player_balance =
                new TextObject(balance,
                               m_pos[0] - m_width * 0.2,
                               m_pos[1] - m_height * 0.39,
                               (m_pos[2] + 1),
                               vulkan_earth::FontId::TimesRoman24,
                               0.0f,
                               0.0f,
                               0.0f);
    }
}

void ShopMenu::draw(render::RenderContext& context) {
    playMusic(shopmenu);
    using Vec3 = math::Vec3<float>;
    using Vec4 = math::Vec4<float>;
    // The whole-window background panel (see appendMenuPanel()), then the
    // black separating lines (each three pixels thick, as three lines).
    if (m_panel_mesh.triangles().empty() || m_built_width != m_width ||
        m_built_height != m_height) {
        m_panel_mesh.clear();
        vulkan_earth::appendMenuPanel(
                m_panel_mesh, m_width, m_height, m_percent_border);
        const Vec4 black(0, 0, 0, 1);
        for (float offset : {-1.0f, 0.0f, 1.0f}) {
            m_panel_mesh.addLine(Vec3(m_pos[0] + m_width * 0.04 + offset,
                                      m_pos[1] + m_height * 0.45,
                                      m_pos[2] + 1),
                                 Vec3(m_pos[0] + m_width * 0.04 + offset,
                                      m_pos[1] - m_height * 0.277,
                                      m_pos[2] + 1),
                                 black);
        }
        for (float offset : {1.0f, 0.0f, -1.0f}) {
            m_panel_mesh.addLine(Vec3(m_pos[0] - m_width * 0.45,
                                      m_pos[1] - m_height * 0.29 + offset,
                                      m_pos[2] + 1),
                                 Vec3(m_pos[0] + m_width * 0.45,
                                      m_pos[1] - m_height * 0.29 + offset,
                                      m_pos[2] + 1),
                                 black);
        }
        m_panel_mesh.addLine(
                Vec3(m_grids[1]->getXPos() + m_grids[1]->getWidth() * 0.5,
                     m_grids[1]->getYPos() + 35,
                     m_pos[2] + 1),
                Vec3(m_grids[1]->getXPos() + m_grids[1]->getWidth() * 0.5,
                     m_grids[1]->getYPos() + 5,
                     m_pos[2] + 1),
                black);
        m_built_width = m_width;
        m_built_height = m_height;
    }
    context.draw(m_panel_mesh);

    m_grids[0]->draw(context);
    m_grids[1]->draw(context);
    m_buttons[0]->draw(context);
    m_buttons[1]->draw(context);
    m_buttons[2]->draw(context);
    m_buttons[3]->draw(context);
    m_buttons[4]->draw(context);
    m_label_wpn->draw(context);
    m_label_item->draw(context);
    m_label_player_num->draw(context);
    m_label_player_balance->draw(context);
    m_label_discription->draw(context);
    m_label_buy_price->draw(context);
    m_label_sell_price->draw(context);

    if (m_buttons[0]->isToggled()) {
        for (std::int32_t i = 0; i < num_sales_weapon; i++) {
            m_img_shop_wpns[i]->draw(context);
            m_label_shop_wpn_remains[i]->draw(context);
        }
    } else {
        for (std::int32_t i = 0; i < num_sales_item; i++) {
            m_img_shop_items[i]->draw(context);
            m_label_shop_item_remains[i]->draw(context);
        }
    }

    for (std::int32_t i = 0; i < inven_grid_row; i++) {
        if (m_img_inven_wpns[i] != nullptr) {
            m_img_inven_wpns[i]->draw(context);
            m_label_inven_wpn_remains[i]->draw(context);
        }
        if (m_img_inven_items[i] != nullptr) {
            m_img_inven_items[i]->draw(context);
            m_label_inven_item_remains[i]->draw(context);
        }
    }
}

void ShopMenu::buttonTest(std::int32_t x,
                          std::int32_t y,
                          std::int32_t button_down) {
    m_grids[0]->mouseClickEvent(x, y, button_down, true);
    updateBuyDiscriptLabel();
    m_grids[1]->mouseClickEvent(x, y, button_down, true);
    updateSellLabel();

    if ((x >= (m_buttons[0]->getXPos()) &&
         x <= ((m_buttons[0]->getXPos()) + (m_buttons[0]->getWidth()))) &&
        (y <= (m_buttons[0]->getYPos()) &&
         y >= ((m_buttons[0]->getYPos()) - (m_buttons[0]->getHeight())))) {
        if (button_down) playSFX(SMALL_CLICK);
        m_grids[0]->deselectAllCells();
        delete m_label_discription;
        delete m_label_buy_price;
        m_label_discription = new TextObject(
                "", 0, 0, 0, vulkan_earth::FontId::TimesRoman24, 0, 0, 0);
        m_label_buy_price = new TextObject(
                "", 0, 0, 0, vulkan_earth::FontId::TimesRoman24, 0, 0, 0);
        m_buttons[0]->mouseClickEvent(x, y, button_down, true);
        if (m_buttons[0]->isToggled()) {
            m_buttons[1]->setToggled(false);
            m_buttons[0]->updateButtonState();
            m_buttons[1]->updateButtonState();
        }
    }

    if ((x >= (m_buttons[1]->getXPos()) &&
         x <= ((m_buttons[1]->getXPos()) + (m_buttons[1]->getWidth()))) &&
        (y <= (m_buttons[1]->getYPos()) &&
         y >= ((m_buttons[1]->getYPos()) - (m_buttons[1]->getHeight())))) {
        if (button_down) playSFX(SMALL_CLICK);
        m_grids[0]->deselectAllCells();
        delete m_label_discription;
        delete m_label_buy_price;
        m_label_discription = new TextObject(
                "", 0, 0, 0, vulkan_earth::FontId::TimesRoman24, 0, 0, 0);
        m_label_buy_price = new TextObject(
                "", 0, 0, 0, vulkan_earth::FontId::TimesRoman24, 0, 0, 0);
        m_buttons[1]->mouseClickEvent(x, y, button_down, true);
        if (m_buttons[1]->isToggled()) {
            m_buttons[0]->setToggled(false);
            m_buttons[0]->updateButtonState();
            m_buttons[1]->updateButtonState();
        }
    }

    m_buttons[2]->mouseClickEvent(x, y, button_down, true);
    if (m_buttons[2]->isToggled()) {
        buyHandler();
        m_buttons[2]->setToggled(false);
    }

    m_buttons[3]->mouseClickEvent(x, y, button_down, true);
    if (m_buttons[3]->isToggled()) {
        sellHandler();
        m_grids[1]->deselectAllCells();
        m_buttons[3]->setToggled(false);
    }

    m_buttons[4]->mouseClickEvent(x, y, button_down, true);
    if (m_buttons[4]->isToggled()) {
        playSFX(BIG_CLICK);
        m_grids[0]->deselectAllCells();
        m_grids[1]->deselectAllCells();
        m_buttons[4]->setToggled(false);
        saveCurrentPlayerInfo();
        m_current_player_index++;
        displayCurrentPlayerInfo();
    }
}
