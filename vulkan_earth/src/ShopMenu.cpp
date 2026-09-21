#include "ShopMenu.h"
#include "ControlItem.h"
#include "ControlItemButton.h"
#include "ControlItemGrid.h"
#include "GlobalSettings.h"
#include "ImageObject.h"
#include "Item.h"
#include "ItemAntiAcid.h"
#include "ItemBigRepair.h"
#include "ItemCloak.h"
#include "ItemDoubleAction.h"
#include "ItemExtraBattery.h"
#include "ItemFloat.h"
#include "ItemShield.h"
#include "ItemSmallRepair.h"
#include "Player.h"
#include "PlayerFactory.h"
#include "TextObject.h"
#include "Weapon.h"
#include "WeaponAcid.h"
#include "WeaponAtom.h"
#include "WeaponBFB.h"
#include "WeaponEMP.h"
#include "WeaponMFB.h"
#include "WeaponNuke.h"
#include "WeaponPadlock.h"
#include "WeaponRevive.h"
#include "WeaponTeleport.h"
#include "WeaponThor.h"
#include "macro_crtdbg.h"
#include "possibleGameStates.h"
#include "sound.h"

extern void playSFX(int sfx);
extern void playMusic(int music);

ShopMenu::ShopMenu() = default;
ShopMenu::ShopMenu(GLfloat width,
                   GLfloat height,
                   GLfloat percent_border,
                   GlobalSettings* global_settings,
                   PlayerFactory* player_factory,
                   int* game_state) {
    this->global_settings = global_settings;
    this->player_factory = player_factory;
    current_game_state = game_state;
    num_players = global_settings->getPlayerCount();
    current_player_index = 0;
    current_player_balance = 0;

    for (int i = 0; i < inven_grid_row; i++) {
        inven_wpns[i] = nullptr;
        inven_items[i] = nullptr;
        label_inven_wpn_remains[i] = nullptr;
        label_inven_item_remains[i] = nullptr;
        img_inven_wpns[i] = nullptr;
        img_inven_items[i] = nullptr;
    }

    this->width = width;
    this->height = height;
    this->percent_border = percent_border;
    pos[0] = pos[1] = pos[2] = 0;

    grids[0] = new ControlItemGrid(pos[0] - this->width * 0.4,
                                   pos[1] + this->height * 0.28,
                                   pos[2],
                                   this->width * 0.3,
                                   this->height * 0.3,
                                   shop_grid_row,
                                   shop_grid_col,
                                   0.72,
                                   0.25,
                                   0.41,
                                   false,
                                   false);
    grids[1] = new ControlItemGrid(pos[0] + this->width * 0.175,
                                   pos[1] + this->height * 0.30,
                                   pos[2],
                                   this->width * 0.12,
                                   this->height * 0.4,
                                   inven_grid_row,
                                   inven_grid_col,
                                   0.25,
                                   0.7,
                                   0.43,
                                   true,
                                   true);
    buttons[0] = new ControlItemButton(nullptr,
                                       pos[0] - this->width * 0.34,
                                       pos[1] + this->height * 0.35,
                                       pos[2] + 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       0.075 * (this->width),
                                       0.04 * (this->height),
                                       "Weapon");
    buttons[0]->setToggled(true);
    buttons[0]->updateButtonState();

    buttons[1] = new ControlItemButton(nullptr,
                                       pos[0] - this->width * 0.235,
                                       pos[1] + this->height * 0.35,
                                       pos[2] + 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       0.075 * (this->width),
                                       0.04 * (this->height),
                                       "Item");
    buttons[2] = new ControlItemButton(nullptr,
                                       pos[0] - this->width * 0.2,
                                       pos[1] - this->height * 0.175,
                                       pos[2] + 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       0.075 * (this->width),
                                       0.04 * (this->height),
                                       "Buy");
    buttons[3] = new ControlItemButton(nullptr,
                                       pos[0] + this->width * 0.275,
                                       pos[1] - this->height * 0.175,
                                       pos[2] + 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       0.075 * (this->width),
                                       0.04 * (this->height),
                                       "Sell");
    buttons[4] = new ControlItemButton(nullptr,
                                       pos[0] + this->width * 0.15,
                                       pos[1] - this->height * 0.365,
                                       pos[2] + 0.5,
                                       0.65,
                                       0.15,
                                       0.15,
                                       0.135 * (this->width),
                                       0.04 * (this->height),
                                       "Finish Shopping");

    /*LABEL PLACEMENT*/
    label_wpn =
            new TextObject("Weapon",
                           grids[1]->getXPos() + grids[1]->getWidth() * 0.05,
                           grids[1]->getYPos() + 15,
                           (pos[2] + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    label_item =
            new TextObject("Item",
                           grids[1]->getXPos() + grids[1]->getWidth() / 1.6,
                           grids[1]->getYPos() + 15,
                           (pos[2] + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    label_player_num = new TextObject("Player 1 Balance:",
                                      pos[0] - this->width * 0.35,
                                      pos[1] - this->height * 0.39,
                                      (pos[2] + 1),
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0.0f,
                                      0.0f,
                                      0.0f);
    label_discription = new TextObject("",
                                       pos[0] - this->width * 0.4,
                                       pos[1] - this->height * 0.1,
                                       pos[2] + 1,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
    label_buy_price = new TextObject("",
                                     pos[0] - this->width * 0.35,
                                     pos[1] - this->height * 0.2,
                                     pos[2] + 1,
                                     GLUT_BITMAP_TIMES_ROMAN_24,
                                     0.0f,
                                     0.0f,
                                     0.0f);
    label_sell_price = new TextObject("$ 0",
                                      pos[0] + this->width * 0.15,
                                      pos[1] - this->height * 0.2,
                                      pos[2] + 1,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0.0f,
                                      0.0f,
                                      0.0f);
    std::string balance = "$ " + std::to_string(current_player_balance);
    label_player_balance = new TextObject(balance,
                                          pos[0] - this->width * 0.2,
                                          pos[1] - this->height * 0.39,
                                          (pos[2] + 1),
                                          GLUT_BITMAP_TIMES_ROMAN_24,
                                          0.0f,
                                          0.0f,
                                          0.0f);

    /*Weapons and Items Creation*/
    shop_wpns[0] = new WeaponMFB(0);
    shop_wpns[1] = new WeaponBFB(1);
    shop_wpns[2] = new WeaponAcid(2);
    shop_wpns[3] = new WeaponThor(3);
    shop_wpns[4] = new WeaponEMP(4);
    shop_wpns[5] = new WeaponPadlock(5);
    shop_wpns[6] = new WeaponRevive(6);
    shop_wpns[7] = new WeaponTeleport(7);
    shop_wpns[8] = new WeaponAtom(8);
    shop_wpns[9] = new WeaponNuke(9);

    shop_items[0] = new ItemSmallRepair(0);
    shop_items[1] = new ItemBigRepair(1);
    shop_items[2] = new ItemAntiAcid(2);
    shop_items[3] = new ItemDoubleAction(3);
    shop_items[4] = new ItemShield(4);
    shop_items[5] = new ItemExtraBattery(5);
    shop_items[6] = new ItemCloak(6);
    shop_items[7] = new ItemFloat(7);

    displayCurrentPlayerInfo();

    /*Images and remainsLabels Creation*/
    for (int i = 0; i < num_sales_weapon; i++) {
        img_shop_wpns[i] = new ImageObject(0,
                                           0,
                                           2.0f,
                                           100,
                                           100,
                                           .0006 * (this->width),
                                           256,
                                           256,
                                           shop_wpns[i]->getImageFileName());
        grids[0]->setImageSizeToCell(img_shop_wpns[i], 0.8);

        std::string remain =
                "x " + std::to_string(shop_wpns[i]->getPackageNum());
        label_shop_wpn_remains[i] = new TextObject(
                remain, 0, 0, 0, GLUT_BITMAP_TIMES_ROMAN_24, 0.6f, 0.3f, 0.4f);
    }
    for (int i = 0; i < num_sales_item; i++) {
        img_shop_items[i] = new ImageObject(0,
                                            0,
                                            2.0f,
                                            100,
                                            100,
                                            .0006 * (this->width),
                                            256,
                                            256,
                                            shop_items[i]->getImageFileName());
        grids[0]->setImageSizeToCell(img_shop_items[i], 0.8);

        std::string remain =
                "x " + std::to_string(shop_items[i]->getPackageNum());
        label_shop_item_remains[i] = new TextObject(
                remain, 0, 0, 0, GLUT_BITMAP_TIMES_ROMAN_24, 0.6f, 0.3f, 0.4f);
    }

    /*Set position of images and remainsLabels to shopping cells*/
    int index = 0;
    for (int r = 0; r < shop_grid_row; r++) {
        for (int c = 0; c < shop_grid_col; c++) {
            if (index < num_sales_weapon) {
                grids[0]->placeImageToCell(img_shop_wpns[index], r, c);
                grids[0]->placeTextToCell(label_shop_wpn_remains[index], r, c);
            }
            if (index < num_sales_item) {
                grids[0]->placeImageToCell(img_shop_items[index], r, c);
                grids[0]->placeTextToCell(
                        label_shop_item_remains[index], r, c);
            }
            index++;
        }
    }
}

ShopMenu::~ShopMenu() {
    delete grids[0];
    delete grids[1];
    for (int i = 0; i < 5; i++) delete buttons[i];
    delete label_wpn;
    delete label_item;
    delete label_player_num;
    delete label_player_balance;
    delete label_discription;
    delete label_buy_price;
    delete label_sell_price;
    for (int i = 0; i < num_sales_weapon; i++) {
        delete shop_wpns[i];
        delete img_shop_wpns[i];
        delete label_shop_wpn_remains[i];
    }
    for (int i = 0; i < num_sales_item; i++) {
        delete shop_items[i];
        delete img_shop_items[i];
        delete label_shop_item_remains[i];
    }
    for (int i = 0; i < inven_grid_row; i++) {
        delete img_inven_wpns[i];
        delete label_inven_wpn_remains[i];
        delete img_inven_items[i];
        delete label_inven_item_remains[i];
    }
}

/*GETTERS & SETTERS*/
void ShopMenu::updateNumPlayers(int n) { num_players = n; }

void ShopMenu::saveCurrentPlayerInfo() {
    // save currentPlayerBalance and the inventory (Weapon, Item objects, and
    // remainings)
    this->player_factory->getPlayer(current_player_index)
            ->setCash(current_player_balance);
    this->player_factory->getPlayer(current_player_index)
            ->setWeapons(inven_wpns);
    this->player_factory->getPlayer(current_player_index)
            ->setItems(inven_items);

    // clear inventory slots for the next player
    for (int i = 0; i < inven_grid_row; i++) {
        if (img_inven_wpns[i] != nullptr) {
            delete img_inven_wpns[i];
            delete label_inven_wpn_remains[i];
            img_inven_wpns[i] = nullptr;
            label_inven_wpn_remains[i] = nullptr;
            inven_wpns[i] = nullptr;
        }
        if (img_inven_items[i] != nullptr) {
            delete img_inven_items[i];
            delete label_inven_item_remains[i];
            img_inven_items[i] = nullptr;
            label_inven_item_remains[i] = nullptr;
            inven_items[i] = nullptr;
        }
    }

    buttons[1]->setToggled(false);
    buttons[1]->updateButtonState();
    buttons[0]->setToggled(true);
    buttons[0]->updateButtonState();
}

void ShopMenu::displayCurrentPlayerInfo() {
    while ((current_player_index < num_players) &&
           (this->player_factory->getPlayer(current_player_index)
                    ->getPlayerType() == "CPU")) {
        current_player_index++;
    }

    if (current_player_index < num_players) {
        for (int i = 0; i < inven_grid_row; i++) {
            inven_wpns[i] = player_factory->getPlayer(current_player_index)
                                    ->getCurrentWeapons()[i];
            if (inven_wpns[i]) {
                img_inven_wpns[i] =
                        new ImageObject(0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        256,
                                        256,
                                        inven_wpns[i]->getImageFileName());
                grids[1]->setImageSizeToCell(img_inven_wpns[i], 0.8);
                grids[1]->placeImageToCell(img_inven_wpns[i], i, 0);

                std::string remain =
                        "x " + std::to_string(inven_wpns[i]->getRemaining());
                delete label_inven_wpn_remains[i];
                label_inven_wpn_remains[i] =
                        new TextObject(remain,
                                       0,
                                       0,
                                       0,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.6f,
                                       0.3f,
                                       0.4f);
                grids[1]->placeTextToCell(label_inven_wpn_remains[i], i, 0);
            }

            inven_items[i] = player_factory->getPlayer(current_player_index)
                                     ->getCurrentItems()[i];
            if (inven_items[i]) {
                img_inven_items[i] =
                        new ImageObject(0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        256,
                                        256,
                                        inven_items[i]->getImageFileName());
                grids[1]->setImageSizeToCell(img_inven_items[i], 0.8);
                grids[1]->placeImageToCell(img_inven_items[i], i, 1);

                std::string remain =
                        "x " + std::to_string(inven_items[i]->getRemaining());
                delete label_inven_item_remains[i];
                label_inven_item_remains[i] =
                        new TextObject(remain,
                                       0,
                                       0,
                                       0,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.6f,
                                       0.3f,
                                       0.4f);
                grids[1]->placeTextToCell(label_inven_item_remains[i], i, 1);
            }
        }

        // Set next player number label
        delete label_player_num;
        std::string str = "Player " +
                          std::to_string(current_player_index + 1) +
                          " Balance:";
        label_player_num = new TextObject(str,
                                          pos[0] - this->width * 0.35,
                                          pos[1] - this->height * 0.39,
                                          (pos[2] + 1),
                                          GLUT_BITMAP_TIMES_ROMAN_24,
                                          0.0f,
                                          0.0f,
                                          0.0f);

        // Set next player balance label
        current_player_balance =
                player_factory->getPlayer(current_player_index)->getCash();
        delete label_player_balance;
        std::string balance = "$ " + std::to_string(current_player_balance);
        label_player_balance = new TextObject(balance,
                                              pos[0] - this->width * 0.2,
                                              pos[1] - this->height * 0.39,
                                              (pos[2] + 1),
                                              GLUT_BITMAP_TIMES_ROMAN_24,
                                              0.0f,
                                              0.0f,
                                              0.0f);

        // Set next player inventory

    } else {
        // printDebugInfo();
        current_player_index = 0;
        Mix_HaltMusic();
        *current_game_state = GAME_PLAY;
    }
}

void ShopMenu::printDebugInfo() {
    // PLAYER INSPECTION DEBUG
    //***********************
    printf("\n*************************************");
    for (int i = 0; i < player_factory->getNumberofPlayers(); i++) {
        printf("\n\nPlayer%d", i + 1);
        printf("\nPlayer Type: %s",
               player_factory->getPlayer(i)->getPlayerType().c_str());
        printf("\nAI Difficulty: %s",
               player_factory->getPlayer(i)->getAiType().c_str());
        printf("\nPlayer Name: %s",
               player_factory->getPlayer(i)->getPlayerName().c_str());
        printf("\nTeam Number: %c",
               player_factory->getPlayer(i)->getTeamLabel());
        printf("\nTank Type: %s",
               player_factory->getPlayer(i)->getTankType().c_str());
        printf("\nCurrent Money: %d", player_factory->getPlayer(i)->getCash());
        for (int j = 0; j < inven_grid_row; j++) {
            if (player_factory->getPlayer(i)->getCurrentWeapons()[j]) {
                printf("\nWeapon Slot %i: %s",
                       j,
                       player_factory->getPlayer(i)
                               ->getCurrentWeapons()[j]
                               ->getDescription()
                               .c_str());
                printf("\nWeapon Slot %i amount: %i",
                       j,
                       player_factory->getPlayer(i)
                               ->getCurrentWeapons()[j]
                               ->getRemaining());
            }
        }
        for (int j = 0; j < inven_grid_row; j++) {
            if (player_factory->getPlayer(i)->getCurrentItems()[j]) {
                printf("\nItem Slot %i: %s",
                       j,
                       player_factory->getPlayer(i)
                               ->getCurrentItems()[j]
                               ->getDescription()
                               .c_str());
                printf("\nItem Slot %i amount: %i",
                       j,
                       player_factory->getPlayer(i)
                               ->getCurrentItems()[j]
                               ->getRemaining());
            }
        }
    }
    printf("\n\n*************************************\n\n");
    //**************************
}

void ShopMenu::updateBuyDiscriptLabel() {
    bool* selected_cells = grids[0]->getSelectedCells();
    if (buttons[0]->isToggled()) {
        for (int i = 0; i < num_sales_weapon; i++) {
            if (selected_cells[i]) {
                delete label_discription;
                delete label_buy_price;
                label_discription =
                        new TextObject(shop_wpns[i]->getDescription(),
                                       pos[0] - this->width * 0.4,
                                       pos[1] - this->height * 0.1,
                                       pos[2] + 1,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                std::string price =
                        "$ " + std::to_string(shop_wpns[i]->getPrice());
                label_buy_price = new TextObject(price,
                                                 pos[0] - this->width * 0.35,
                                                 pos[1] - this->height * 0.2,
                                                 pos[2] + 1,
                                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                                 0.0f,
                                                 0.0f,
                                                 0.0f);
                break;
            } else {
                delete label_discription;
                delete label_buy_price;
                label_discription =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
                label_buy_price =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
            }
        }
    } else {
        for (int i = 0; i < num_sales_item; i++) {
            if (selected_cells[i]) {
                delete label_discription;
                delete label_buy_price;
                label_discription =
                        new TextObject(shop_items[i]->getDescription(),
                                       pos[0] - this->width * 0.4,
                                       pos[1] - this->height * 0.1,
                                       pos[2] + 1,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                std::string price =
                        "$ " + std::to_string(shop_items[i]->getPrice());
                label_buy_price = new TextObject(price,
                                                 pos[0] - this->width * 0.35,
                                                 pos[1] - this->height * 0.2,
                                                 pos[2] + 1,
                                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                                 0.0f,
                                                 0.0f,
                                                 0.0f);
                break;
            } else {
                delete label_discription;
                delete label_buy_price;
                label_discription =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
                label_buy_price =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
            }
        }
    }
}

void ShopMenu::updateSellLabel() {
    bool* selected_cells = grids[1]->getSelectedCells();
    int total_sell = 0;

    for (int i = 0; i < inven_grid_row * 2; i++) {
        if (selected_cells[i] && inven_wpns[i / 2] != nullptr &&
            i % 2 == 0) {  //	i%2 == 0 is weapon inventory
            total_sell +=
                    static_cast<int>(((inven_wpns[i / 2]->getPrice() /
                                       inven_wpns[i / 2]->getPackageNum()) /
                                      1.5) *
                                     inven_wpns[i / 2]->getRemaining());
        }
        if (selected_cells[i] && inven_items[i / 2] != nullptr && i % 2 == 1) {
            total_sell +=
                    static_cast<int>(((inven_items[i / 2]->getPrice() /
                                       inven_items[i / 2]->getPackageNum()) /
                                      1.5) *
                                     inven_items[i / 2]->getRemaining());
        }
    }

    delete label_sell_price;
    std::string price = "$ " + std::to_string(total_sell);
    label_sell_price = new TextObject(price,
                                      pos[0] + this->width * 0.15,
                                      pos[1] - this->height * 0.2,
                                      pos[2] + 1,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0.0f,
                                      0.0f,
                                      0.0f);
}

void ShopMenu::buyHandler() {
    int inven_i;
    bool* selected_cells = grids[0]->getSelectedCells();

    if (buttons[0]->isToggled()) {
        for (int i = 0; i < num_sales_weapon; i++) {
            if (selected_cells[i]) {
                for (inven_i = 0; inven_i < inven_grid_row; inven_i++) {
                    if ((inven_wpns[inven_i] == nullptr) ||
                        (shop_wpns[i]->getUNIQUEIDENTIFIER() ==
                         inven_wpns[inven_i]->getUNIQUEIDENTIFIER()))
                        break;
                }
                if ((current_player_balance >= shop_wpns[i]->getPrice()) &&
                    (inven_i < inven_grid_row)) {
                    if ((inven_wpns[inven_i] == nullptr)) {
                        playSFX(TRANSACTION);
                        img_inven_wpns[inven_i] = new ImageObject(
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                256,
                                256,
                                shop_wpns[i]->getImageFileName());
                        grids[1]->setImageSizeToCell(img_inven_wpns[inven_i],
                                                     0.8);
                        grids[1]->placeImageToCell(
                                img_inven_wpns[inven_i], inven_i, 0);
                        inven_wpns[inven_i] =
                                shop_wpns[i]->getWeaponInstance();
                        current_player_balance -= shop_wpns[i]->getPrice();
                    } else {
                        if (inven_wpns[inven_i]->getRemaining() <
                            inven_wpns[inven_i]->getMaxStack()) {
                            playSFX(TRANSACTION);
                            current_player_balance -= shop_wpns[i]->getPrice();
                            inven_wpns[inven_i]->setRemaining(
                                    inven_wpns[inven_i]->getRemaining() +
                                    shop_wpns[i]->getPackageNum());
                            if (inven_wpns[inven_i]->getRemaining() >
                                inven_wpns[inven_i]->getMaxStack())
                                inven_wpns[inven_i]->setRemaining(
                                        inven_wpns[inven_i]->getMaxStack());
                        } else {
                            playSFX(INVALID_CLICK);
                        }
                    }

                    delete label_player_balance;
                    std::string balance =
                            "$ " + std::to_string(current_player_balance);
                    label_player_balance =
                            new TextObject(balance,
                                           pos[0] - this->width * 0.2,
                                           pos[1] - this->height * 0.39,
                                           (pos[2] + 1),
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.0f,
                                           0.0f,
                                           0.0f);

                    std::string remain =
                            "x " +
                            std::to_string(
                                    inven_wpns[inven_i]->getRemaining());
                    delete label_inven_wpn_remains[inven_i];
                    label_inven_wpn_remains[inven_i] =
                            new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
                    grids[1]->placeTextToCell(
                            label_inven_wpn_remains[inven_i], inven_i, 0);
                } else {
                    playSFX(INVALID_CLICK);
                }
                break;
            }
        }
    } else {
        for (int i = 0; i < num_sales_item; i++) {
            if (selected_cells[i]) {
                for (inven_i = 0; inven_i < inven_grid_row; inven_i++) {
                    if ((inven_items[inven_i] == nullptr) ||
                        (shop_items[i]->getUNIQUEIDENTIFIER() ==
                         inven_items[inven_i]->getUNIQUEIDENTIFIER()))
                        break;
                }
                if (current_player_balance >= shop_items[i]->getPrice() &&
                    inven_i < inven_grid_row) {
                    if (inven_items[inven_i] == nullptr) {
                        playSFX(TRANSACTION);
                        img_inven_items[inven_i] = new ImageObject(
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                256,
                                256,
                                shop_items[i]->getImageFileName());
                        grids[1]->setImageSizeToCell(img_inven_items[inven_i],
                                                     0.8);
                        grids[1]->placeImageToCell(
                                img_inven_items[inven_i], inven_i, 1);
                        inven_items[inven_i] =
                                shop_items[i]->getItemInstance();
                        current_player_balance -= shop_items[i]->getPrice();
                    } else {
                        if (inven_items[inven_i]->getRemaining() <
                            inven_items[inven_i]->getMaxStack()) {
                            playSFX(TRANSACTION);
                            current_player_balance -=
                                    shop_items[i]->getPrice();
                            inven_items[inven_i]->setRemaining(
                                    inven_items[inven_i]->getRemaining() +
                                    shop_items[i]->getPackageNum());
                            if (inven_items[inven_i]->getRemaining() >
                                inven_items[inven_i]->getMaxStack())
                                inven_items[inven_i]->setRemaining(
                                        inven_items[inven_i]->getMaxStack());
                        } else {
                            playSFX(INVALID_CLICK);
                        }
                    }

                    delete label_player_balance;
                    std::string balance =
                            "$ " + std::to_string(current_player_balance);
                    label_player_balance =
                            new TextObject(balance,
                                           pos[0] - this->width * 0.2,
                                           pos[1] - this->height * 0.39,
                                           (pos[2] + 1),
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.0f,
                                           0.0f,
                                           0.0f);

                    std::string remain =
                            "x " +
                            std::to_string(
                                    inven_items[inven_i]->getRemaining());
                    delete label_inven_item_remains[inven_i];
                    label_inven_item_remains[inven_i] =
                            new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
                    grids[1]->placeTextToCell(
                            label_inven_item_remains[inven_i], inven_i, 1);
                } else {
                    playSFX(INVALID_CLICK);
                }
                break;
            }
        }
    }
}

void ShopMenu::sellHandler() {
    bool* selected_cells = grids[1]->getSelectedCells();
    int total_sell = 0;

    for (int i = 0; i < inven_grid_row * 2; i++) {
        if (selected_cells[i] && inven_wpns[i / 2] != nullptr &&
            i % 2 == 0) {  // i%2 == 0 is weapon inventory
            total_sell +=
                    static_cast<int>(((inven_wpns[i / 2]->getPrice() /
                                       inven_wpns[i / 2]->getPackageNum()) /
                                      1.5) *
                                     inven_wpns[i / 2]->getRemaining());
            delete img_inven_wpns[i / 2];
            delete label_inven_wpn_remains[i / 2];
            delete inven_wpns[i / 2];
            img_inven_wpns[i / 2] = nullptr;
            label_inven_wpn_remains[i / 2] = nullptr;
            inven_wpns[i / 2] = nullptr;
        }
        if (selected_cells[i] && inven_items[i / 2] != nullptr && i % 2 == 1) {
            total_sell +=
                    static_cast<int>(((inven_items[i / 2]->getPrice() /
                                       inven_items[i / 2]->getPackageNum()) /
                                      1.5) *
                                     inven_items[i / 2]->getRemaining());
            delete img_inven_items[i / 2];
            delete label_inven_item_remains[i / 2];
            delete inven_items[i / 2];
            img_inven_items[i / 2] = nullptr;
            label_inven_item_remains[i / 2] = nullptr;
            inven_items[i / 2] = nullptr;
        }
    }

    if (total_sell != 0) {
        playSFX(TRANSACTION);
        current_player_balance += total_sell;
        delete label_player_balance;
        std::string balance = "$ " + std::to_string(current_player_balance);
        label_player_balance = new TextObject(balance,
                                              pos[0] - this->width * 0.2,
                                              pos[1] - this->height * 0.39,
                                              (pos[2] + 1),
                                              GLUT_BITMAP_TIMES_ROMAN_24,
                                              0.0f,
                                              0.0f,
                                              0.0f);
    }
}

void ShopMenu::draw() {
    playMusic(shopmenu);
    glPushMatrix();
    /*	ALWAYS START AT UPPER LEFT CORNER -> LOWER LEFT CORNER -> LOWER RIGHT
     * CORNER -> UPPER RIGHT	*/
    glBegin(GL_QUADS);
    glColor3f(0.85f, 0.85f, 0.85f);
    glVertex3f(-1 * (this->width / 2.0), (this->height / 2.0), 0); /*	|\ 	*/
    glVertex3f(
            -1 * (this->width / 2.0), -1 * (this->height / 2.0), 0); /*	| |	*/
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percent_border * (this->height)),
            -1 * (this->height / 2.0) +
                    (this->percent_border * (this->height)),
            0); /*	|/ 	*/
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percent_border * (this->height)),
            (this->height / 2.0) - (this->percent_border * (this->height)),
            0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.80f, 0.80f);
    glVertex3f(-1 * (this->width / 2.0), (this->height / 2.0), 0); /*_____ */
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percent_border * (this->height)),
            (this->height / 2.0) - (this->percent_border * (this->height)),
            0); /*\	  / */
    glVertex3f((this->width / 2.0) - (this->percent_border * (this->height)),
               (this->height / 2.0) - (this->percent_border * (this->height)),
               0); /* ---	*/
    glVertex3f((this->width / 2.0), (this->height / 2.0), 0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.75f, 0.75f, 0.75f);
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percent_border * (this->height)),
            (this->height / 2.0) - (this->percent_border * (this->height)),
            0); /*_____ */
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percent_border * (this->height)),
            -1 * (this->height / 2.0) +
                    (this->percent_border * (this->height)),
            0); /*|	  | */
    glVertex3f((this->width / 2.0) - (this->percent_border * (this->height)),
               -1 * (this->height / 2.0) +
                       (this->percent_border * (this->height)),
               0); /*----- */
    glVertex3f((this->width / 2.0) - (this->percent_border * (this->height)),
               (this->height / 2.0) - (this->percent_border * (this->height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.45f, 0.45f, 0.45f);
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percent_border * (this->height)),
            -1 * (this->height / 2.0) +
                    (this->percent_border * (this->height)),
            0); /* ___  */
    glVertex3f(-1 * (this->width / 2.0),
               -1 * (this->height / 2.0),
               0);                                                 /*/	  \ */
    glVertex3f((this->width / 2.0), -1 * (this->height / 2.0), 0); /*----- */
    glVertex3f((this->width / 2.0) - (this->percent_border * (this->height)),
               -1 * (this->height / 2.0) +
                       (this->percent_border * (this->height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.40f, 0.40f, 0.40f);
    glVertex3f((this->width / 2.0) - (this->percent_border * (this->height)),
               (this->height / 2.0) - (this->percent_border * (this->height)),
               0); /*	 /| 	*/
    glVertex3f((this->width / 2.0) - (this->percent_border * (this->height)),
               -1 * (this->height / 2.0) +
                       (this->percent_border * (this->height)),
               0); /*	| | 	*/
    glVertex3f(
            (this->width / 2.0), -1 * (this->height / 2.0), 0); /* 	 \| 	*/
    glVertex3f((this->width / 2.0), (this->height / 2.0), 0);
    glEnd();

    // Draw separating lines
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(pos[0] + this->width * 0.04 - 1,
               pos[1] + this->height * 0.45,
               pos[2] + 1);
    glVertex3f(pos[0] + this->width * 0.04 - 1,
               pos[1] - this->height * 0.277,
               pos[2] + 1);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(pos[0] + this->width * 0.04,
               pos[1] + this->height * 0.45,
               pos[2] + 1);
    glVertex3f(pos[0] + this->width * 0.04,
               pos[1] - this->height * 0.277,
               pos[2] + 1);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(pos[0] + this->width * 0.04 + 1,
               pos[1] + this->height * 0.45,
               pos[2] + 1);
    glVertex3f(pos[0] + this->width * 0.04 + 1,
               pos[1] - this->height * 0.277,
               pos[2] + 1);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(pos[0] - this->width * 0.45,
               pos[1] - this->height * 0.29 + 1,
               pos[2] + 1);
    glVertex3f(pos[0] + this->width * 0.45,
               pos[1] - this->height * 0.29 + 1,
               pos[2] + 1);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(pos[0] - this->width * 0.45,
               pos[1] - this->height * 0.29,
               pos[2] + 1);
    glVertex3f(pos[0] + this->width * 0.45,
               pos[1] - this->height * 0.29,
               pos[2] + 1);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(pos[0] - this->width * 0.45,
               pos[1] - this->height * 0.29 - 1,
               pos[2] + 1);
    glVertex3f(pos[0] + this->width * 0.45,
               pos[1] - this->height * 0.29 - 1,
               pos[2] + 1);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(grids[1]->getXPos() + grids[1]->getWidth() * 0.5,
               grids[1]->getYPos() + 35,
               pos[2] + 1);
    glVertex3f(grids[1]->getXPos() + grids[1]->getWidth() * 0.5,
               grids[1]->getYPos() + 5,
               pos[2] + 1);
    glEnd();

    grids[0]->draw();
    grids[1]->draw();
    buttons[0]->draw();
    buttons[1]->draw();
    buttons[2]->draw();
    buttons[3]->draw();
    buttons[4]->draw();
    label_wpn->draw();
    label_item->draw();
    label_player_num->draw();
    label_player_balance->draw();
    label_discription->draw();
    label_buy_price->draw();
    label_sell_price->draw();

    if (buttons[0]->isToggled()) {
        for (int i = 0; i < num_sales_weapon; i++) {
            img_shop_wpns[i]->draw();
            label_shop_wpn_remains[i]->draw();
        }
    } else {
        for (int i = 0; i < num_sales_item; i++) {
            img_shop_items[i]->draw();
            label_shop_item_remains[i]->draw();
        }
    }

    for (int i = 0; i < inven_grid_row; i++) {
        if (img_inven_wpns[i] != nullptr) {
            img_inven_wpns[i]->draw();
            label_inven_wpn_remains[i]->draw();
        }
        if (img_inven_items[i] != nullptr) {
            img_inven_items[i]->draw();
            label_inven_item_remains[i]->draw();
        }
    }
    glPopMatrix();
}

void ShopMenu::buttonTest(int x, int y, int button_down) {
    grids[0]->mouseClickEvent(x, y, button_down, true);
    updateBuyDiscriptLabel();
    grids[1]->mouseClickEvent(x, y, button_down, true);
    updateSellLabel();

    if ((x >= (buttons[0]->getXPos()) &&
         x <= ((buttons[0]->getXPos()) + (buttons[0]->getWidth()))) &&
        (y <= (buttons[0]->getYPos()) &&
         y >= ((buttons[0]->getYPos()) - (buttons[0]->getHeight())))) {
        if (button_down) playSFX(SMALL_CLICK);
        grids[0]->deselectAllCells();
        delete label_discription;
        delete label_buy_price;
        label_discription = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        label_buy_price = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        buttons[0]->mouseClickEvent(x, y, button_down, true);
        if (buttons[0]->isToggled()) {
            buttons[1]->setToggled(false);
            buttons[0]->updateButtonState();
            buttons[1]->updateButtonState();
        }
    }

    if ((x >= (buttons[1]->getXPos()) &&
         x <= ((buttons[1]->getXPos()) + (buttons[1]->getWidth()))) &&
        (y <= (buttons[1]->getYPos()) &&
         y >= ((buttons[1]->getYPos()) - (buttons[1]->getHeight())))) {
        if (button_down) playSFX(SMALL_CLICK);
        grids[0]->deselectAllCells();
        delete label_discription;
        delete label_buy_price;
        label_discription = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        label_buy_price = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        buttons[1]->mouseClickEvent(x, y, button_down, true);
        if (buttons[1]->isToggled()) {
            buttons[0]->setToggled(false);
            buttons[0]->updateButtonState();
            buttons[1]->updateButtonState();
        }
    }

    buttons[2]->mouseClickEvent(x, y, button_down, true);
    if (buttons[2]->isToggled()) {
        buyHandler();
        buttons[2]->setToggled(false);
    }

    buttons[3]->mouseClickEvent(x, y, button_down, true);
    if (buttons[3]->isToggled()) {
        sellHandler();
        grids[1]->deselectAllCells();
        buttons[3]->setToggled(false);
    }

    buttons[4]->mouseClickEvent(x, y, button_down, true);
    if (buttons[4]->isToggled()) {
        playSFX(BIG_CLICK);
        grids[0]->deselectAllCells();
        grids[1]->deselectAllCells();
        buttons[4]->setToggled(false);
        saveCurrentPlayerInfo();
        current_player_index++;
        displayCurrentPlayerInfo();
    }
}
