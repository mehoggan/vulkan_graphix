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
    currentGameState = game_state;
    numPlayers = global_settings->getPlayer_Count();
    currentPlayerIndex = 0;
    currentPlayerBalance = 0;

    for (int i = 0; i < INVEN_GRID_ROW; i++) {
        invenWpns[i] = nullptr;
        invenItems[i] = nullptr;
        labelInvenWpnRemains[i] = nullptr;
        labelInvenItemRemains[i] = nullptr;
        imgInvenWpns[i] = nullptr;
        imgInvenItems[i] = nullptr;
    }

    this->width = width;
    this->height = height;
    this->percentBorder = percent_border;
    pos[0] = pos[1] = pos[2] = 0;

    grids[0] = new ControlItemGrid(pos[0] - this->width * 0.4,
                                   pos[1] + this->height * 0.28,
                                   pos[2],
                                   this->width * 0.3,
                                   this->height * 0.3,
                                   SHOP_GRID_ROW,
                                   SHOP_GRID_COL,
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
                                   INVEN_GRID_ROW,
                                   INVEN_GRID_COL,
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
    labelWpn =
            new TextObject("Weapon",
                           grids[1]->getXPos() + grids[1]->getWidth() * 0.05,
                           grids[1]->getYPos() + 15,
                           (pos[2] + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    labelItem =
            new TextObject("Item",
                           grids[1]->getXPos() + grids[1]->getWidth() / 1.6,
                           grids[1]->getYPos() + 15,
                           (pos[2] + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    labelPlayerNum = new TextObject("Player 1 Balance:",
                                    pos[0] - this->width * 0.35,
                                    pos[1] - this->height * 0.39,
                                    (pos[2] + 1),
                                    GLUT_BITMAP_TIMES_ROMAN_24,
                                    0.0f,
                                    0.0f,
                                    0.0f);
    labelDiscription = new TextObject("",
                                      pos[0] - this->width * 0.4,
                                      pos[1] - this->height * 0.1,
                                      pos[2] + 1,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0.0f,
                                      0.0f,
                                      0.0f);
    labelBuyPrice = new TextObject("",
                                   pos[0] - this->width * 0.35,
                                   pos[1] - this->height * 0.2,
                                   pos[2] + 1,
                                   GLUT_BITMAP_TIMES_ROMAN_24,
                                   0.0f,
                                   0.0f,
                                   0.0f);
    labelSellPrice = new TextObject("$ 0",
                                    pos[0] + this->width * 0.15,
                                    pos[1] - this->height * 0.2,
                                    pos[2] + 1,
                                    GLUT_BITMAP_TIMES_ROMAN_24,
                                    0.0f,
                                    0.0f,
                                    0.0f);
    std::string balance = "$ " + std::to_string(currentPlayerBalance);
    labelPlayerBalance = new TextObject(balance,
                                        pos[0] - this->width * 0.2,
                                        pos[1] - this->height * 0.39,
                                        (pos[2] + 1),
                                        GLUT_BITMAP_TIMES_ROMAN_24,
                                        0.0f,
                                        0.0f,
                                        0.0f);

    /*Weapons and Items Creation*/
    shopWpns[0] = new WeaponMFB(0);
    shopWpns[1] = new WeaponBFB(1);
    shopWpns[2] = new WeaponAcid(2);
    shopWpns[3] = new WeaponThor(3);
    shopWpns[4] = new WeaponEMP(4);
    shopWpns[5] = new WeaponPadlock(5);
    shopWpns[6] = new WeaponRevive(6);
    shopWpns[7] = new WeaponTeleport(7);
    shopWpns[8] = new WeaponAtom(8);
    shopWpns[9] = new WeaponNuke(9);

    shopItems[0] = new ItemSmallRepair(0);
    shopItems[1] = new ItemBigRepair(1);
    shopItems[2] = new ItemAntiAcid(2);
    shopItems[3] = new ItemDoubleAction(3);
    shopItems[4] = new ItemShield(4);
    shopItems[5] = new ItemExtraBattery(5);
    shopItems[6] = new ItemCloak(6);
    shopItems[7] = new ItemFloat(7);

    displayCurrentPlayerInfo();

    /*Images and remainsLabels Creation*/
    for (int i = 0; i < NUM_SALES_WEAPON; i++) {
        imgShopWpns[i] = new ImageObject(0,
                                         0,
                                         2.0f,
                                         100,
                                         100,
                                         .0006 * (this->width),
                                         256,
                                         256,
                                         shopWpns[i]->getImageFileName());
        grids[0]->setImageSizeToCell(imgShopWpns[i], 0.8);

        std::string remain =
                "x " + std::to_string(shopWpns[i]->getPackageNum());
        labelShopWpnRemains[i] = new TextObject(
                remain, 0, 0, 0, GLUT_BITMAP_TIMES_ROMAN_24, 0.6f, 0.3f, 0.4f);
    }
    for (int i = 0; i < NUM_SALES_ITEM; i++) {
        imgShopItems[i] = new ImageObject(0,
                                          0,
                                          2.0f,
                                          100,
                                          100,
                                          .0006 * (this->width),
                                          256,
                                          256,
                                          shopItems[i]->getImageFileName());
        grids[0]->setImageSizeToCell(imgShopItems[i], 0.8);

        std::string remain =
                "x " + std::to_string(shopItems[i]->getPackageNum());
        labelShopItemRemains[i] = new TextObject(
                remain, 0, 0, 0, GLUT_BITMAP_TIMES_ROMAN_24, 0.6f, 0.3f, 0.4f);
    }

    /*Set position of images and remainsLabels to shopping cells*/
    int index = 0;
    for (int r = 0; r < SHOP_GRID_ROW; r++) {
        for (int c = 0; c < SHOP_GRID_COL; c++) {
            if (index < NUM_SALES_WEAPON) {
                grids[0]->placeImageToCell(imgShopWpns[index], r, c);
                grids[0]->placeTextToCell(labelShopWpnRemains[index], r, c);
            }
            if (index < NUM_SALES_ITEM) {
                grids[0]->placeImageToCell(imgShopItems[index], r, c);
                grids[0]->placeTextToCell(labelShopItemRemains[index], r, c);
            }
            index++;
        }
    }
}

ShopMenu::~ShopMenu() {
    delete grids[0];
    delete grids[1];
    for (int i = 0; i < 5; i++) delete buttons[i];
    delete labelWpn;
    delete labelItem;
    delete labelPlayerNum;
    delete labelPlayerBalance;
    delete labelDiscription;
    delete labelBuyPrice;
    delete labelSellPrice;
    for (int i = 0; i < NUM_SALES_WEAPON; i++) {
        delete shopWpns[i];
        delete imgShopWpns[i];
        delete labelShopWpnRemains[i];
    }
    for (int i = 0; i < NUM_SALES_ITEM; i++) {
        delete shopItems[i];
        delete imgShopItems[i];
        delete labelShopItemRemains[i];
    }
    for (int i = 0; i < INVEN_GRID_ROW; i++) {
        delete imgInvenWpns[i];
        delete labelInvenWpnRemains[i];
        delete imgInvenItems[i];
        delete labelInvenItemRemains[i];
    }
}

/*GETTERS & SETTERS*/
void ShopMenu::updateNumPlayers(int n) { numPlayers = n; }

void ShopMenu::saveCurrentPlayerInfo() {
    // save currentPlayerBalance and the inventory (Weapon, Item objects, and
    // remainings)
    this->player_factory->getPlayer(currentPlayerIndex)
            ->setCash(currentPlayerBalance);
    this->player_factory->getPlayer(currentPlayerIndex)->setWeapons(invenWpns);
    this->player_factory->getPlayer(currentPlayerIndex)->setItems(invenItems);

    // clear inventory slots for the next player
    for (int i = 0; i < INVEN_GRID_ROW; i++) {
        if (imgInvenWpns[i] != nullptr) {
            delete imgInvenWpns[i];
            delete labelInvenWpnRemains[i];
            imgInvenWpns[i] = nullptr;
            labelInvenWpnRemains[i] = nullptr;
            invenWpns[i] = nullptr;
        }
        if (imgInvenItems[i] != nullptr) {
            delete imgInvenItems[i];
            delete labelInvenItemRemains[i];
            imgInvenItems[i] = nullptr;
            labelInvenItemRemains[i] = nullptr;
            invenItems[i] = nullptr;
        }
    }

    buttons[1]->setToggled(false);
    buttons[1]->updateButtonState();
    buttons[0]->setToggled(true);
    buttons[0]->updateButtonState();
}

void ShopMenu::displayCurrentPlayerInfo() {
    while ((currentPlayerIndex < numPlayers) &&
           (this->player_factory->getPlayer(currentPlayerIndex)
                    ->getPlayer_Type() == "CPU")) {
        currentPlayerIndex++;
    }

    if (currentPlayerIndex < numPlayers) {
        for (int i = 0; i < INVEN_GRID_ROW; i++) {
            invenWpns[i] = player_factory->getPlayer(currentPlayerIndex)
                                   ->getCurrentWeapons()[i];
            if (invenWpns[i]) {
                imgInvenWpns[i] =
                        new ImageObject(0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        256,
                                        256,
                                        invenWpns[i]->getImageFileName());
                grids[1]->setImageSizeToCell(imgInvenWpns[i], 0.8);
                grids[1]->placeImageToCell(imgInvenWpns[i], i, 0);

                std::string remain =
                        "x " + std::to_string(invenWpns[i]->getRemaining());
                delete labelInvenWpnRemains[i];
                labelInvenWpnRemains[i] =
                        new TextObject(remain,
                                       0,
                                       0,
                                       0,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.6f,
                                       0.3f,
                                       0.4f);
                grids[1]->placeTextToCell(labelInvenWpnRemains[i], i, 0);
            }

            invenItems[i] = player_factory->getPlayer(currentPlayerIndex)
                                    ->getCurrentItems()[i];
            if (invenItems[i]) {
                imgInvenItems[i] =
                        new ImageObject(0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        256,
                                        256,
                                        invenItems[i]->getImageFileName());
                grids[1]->setImageSizeToCell(imgInvenItems[i], 0.8);
                grids[1]->placeImageToCell(imgInvenItems[i], i, 1);

                std::string remain =
                        "x " + std::to_string(invenItems[i]->getRemaining());
                delete labelInvenItemRemains[i];
                labelInvenItemRemains[i] =
                        new TextObject(remain,
                                       0,
                                       0,
                                       0,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.6f,
                                       0.3f,
                                       0.4f);
                grids[1]->placeTextToCell(labelInvenItemRemains[i], i, 1);
            }
        }

        // Set next player number label
        delete labelPlayerNum;
        std::string str = "Player " + std::to_string(currentPlayerIndex + 1) +
                          " Balance:";
        labelPlayerNum = new TextObject(str,
                                        pos[0] - this->width * 0.35,
                                        pos[1] - this->height * 0.39,
                                        (pos[2] + 1),
                                        GLUT_BITMAP_TIMES_ROMAN_24,
                                        0.0f,
                                        0.0f,
                                        0.0f);

        // Set next player balance label
        currentPlayerBalance =
                player_factory->getPlayer(currentPlayerIndex)->getCash();
        delete labelPlayerBalance;
        std::string balance = "$ " + std::to_string(currentPlayerBalance);
        labelPlayerBalance = new TextObject(balance,
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
        currentPlayerIndex = 0;
        Mix_HaltMusic();
        *currentGameState = GAME_PLAY;
    }
}

void ShopMenu::printDebugInfo() {
    // PLAYER INSPECTION DEBUG
    //***********************
    printf("\n*************************************");
    for (int i = 0; i < player_factory->getNumberofPlayers(); i++) {
        printf("\n\nPlayer%d", i + 1);
        printf("\nPlayer Type: %s",
               player_factory->getPlayer(i)->getPlayer_Type().c_str());
        printf("\nAI Difficulty: %s",
               player_factory->getPlayer(i)->getAI_Type().c_str());
        printf("\nPlayer Name: %s",
               player_factory->getPlayer(i)->getPlayerName().c_str());
        printf("\nTeam Number: %c",
               player_factory->getPlayer(i)->getTeamLabel());
        printf("\nTank Type: %s",
               player_factory->getPlayer(i)->getTankType().c_str());
        printf("\nCurrent Money: %d", player_factory->getPlayer(i)->getCash());
        for (int j = 0; j < INVEN_GRID_ROW; j++) {
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
        for (int j = 0; j < INVEN_GRID_ROW; j++) {
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
        for (int i = 0; i < NUM_SALES_WEAPON; i++) {
            if (selected_cells[i]) {
                delete labelDiscription;
                delete labelBuyPrice;
                labelDiscription =
                        new TextObject(shopWpns[i]->getDescription(),
                                       pos[0] - this->width * 0.4,
                                       pos[1] - this->height * 0.1,
                                       pos[2] + 1,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                std::string price =
                        "$ " + std::to_string(shopWpns[i]->getPrice());
                labelBuyPrice = new TextObject(price,
                                               pos[0] - this->width * 0.35,
                                               pos[1] - this->height * 0.2,
                                               pos[2] + 1,
                                               GLUT_BITMAP_TIMES_ROMAN_24,
                                               0.0f,
                                               0.0f,
                                               0.0f);
                break;
            } else {
                delete labelDiscription;
                delete labelBuyPrice;
                labelDiscription =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
                labelBuyPrice = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
            }
        }
    } else {
        for (int i = 0; i < NUM_SALES_ITEM; i++) {
            if (selected_cells[i]) {
                delete labelDiscription;
                delete labelBuyPrice;
                labelDiscription =
                        new TextObject(shopItems[i]->getDescription(),
                                       pos[0] - this->width * 0.4,
                                       pos[1] - this->height * 0.1,
                                       pos[2] + 1,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                std::string price =
                        "$ " + std::to_string(shopItems[i]->getPrice());
                labelBuyPrice = new TextObject(price,
                                               pos[0] - this->width * 0.35,
                                               pos[1] - this->height * 0.2,
                                               pos[2] + 1,
                                               GLUT_BITMAP_TIMES_ROMAN_24,
                                               0.0f,
                                               0.0f,
                                               0.0f);
                break;
            } else {
                delete labelDiscription;
                delete labelBuyPrice;
                labelDiscription =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
                labelBuyPrice = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
            }
        }
    }
}

void ShopMenu::updateSellLabel() {
    bool* selected_cells = grids[1]->getSelectedCells();
    int total_sell = 0;

    for (int i = 0; i < INVEN_GRID_ROW * 2; i++) {
        if (selected_cells[i] && invenWpns[i / 2] != nullptr &&
            i % 2 == 0) {  //	i%2 == 0 is weapon inventory
            total_sell +=
                    static_cast<int>(((invenWpns[i / 2]->getPrice() /
                                       invenWpns[i / 2]->getPackageNum()) /
                                      1.5) *
                                     invenWpns[i / 2]->getRemaining());
        }
        if (selected_cells[i] && invenItems[i / 2] != nullptr && i % 2 == 1) {
            total_sell +=
                    static_cast<int>(((invenItems[i / 2]->getPrice() /
                                       invenItems[i / 2]->getPackageNum()) /
                                      1.5) *
                                     invenItems[i / 2]->getRemaining());
        }
    }

    delete labelSellPrice;
    std::string price = "$ " + std::to_string(total_sell);
    labelSellPrice = new TextObject(price,
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
        for (int i = 0; i < NUM_SALES_WEAPON; i++) {
            if (selected_cells[i]) {
                for (inven_i = 0; inven_i < INVEN_GRID_ROW; inven_i++) {
                    if ((invenWpns[inven_i] == nullptr) ||
                        (shopWpns[i]->getUNIQUEIDENTIFIER() ==
                         invenWpns[inven_i]->getUNIQUEIDENTIFIER()))
                        break;
                }
                if ((currentPlayerBalance >= shopWpns[i]->getPrice()) &&
                    (inven_i < INVEN_GRID_ROW)) {
                    if ((invenWpns[inven_i] == nullptr)) {
                        playSFX(TRANSACTION);
                        imgInvenWpns[inven_i] = new ImageObject(
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                256,
                                256,
                                shopWpns[i]->getImageFileName());
                        grids[1]->setImageSizeToCell(imgInvenWpns[inven_i],
                                                     0.8);
                        grids[1]->placeImageToCell(
                                imgInvenWpns[inven_i], inven_i, 0);
                        invenWpns[inven_i] = shopWpns[i]->getWeaponInstance();
                        currentPlayerBalance -= shopWpns[i]->getPrice();
                    } else {
                        if (invenWpns[inven_i]->getRemaining() <
                            invenWpns[inven_i]->getMaxStack()) {
                            playSFX(TRANSACTION);
                            currentPlayerBalance -= shopWpns[i]->getPrice();
                            invenWpns[inven_i]->setRemaining(
                                    invenWpns[inven_i]->getRemaining() +
                                    shopWpns[i]->getPackageNum());
                            if (invenWpns[inven_i]->getRemaining() >
                                invenWpns[inven_i]->getMaxStack())
                                invenWpns[inven_i]->setRemaining(
                                        invenWpns[inven_i]->getMaxStack());
                        } else {
                            playSFX(INVALID_CLICK);
                        }
                    }

                    delete labelPlayerBalance;
                    std::string balance =
                            "$ " + std::to_string(currentPlayerBalance);
                    labelPlayerBalance =
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
                            std::to_string(invenWpns[inven_i]->getRemaining());
                    delete labelInvenWpnRemains[inven_i];
                    labelInvenWpnRemains[inven_i] =
                            new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
                    grids[1]->placeTextToCell(
                            labelInvenWpnRemains[inven_i], inven_i, 0);
                } else {
                    playSFX(INVALID_CLICK);
                }
                break;
            }
        }
    } else {
        for (int i = 0; i < NUM_SALES_ITEM; i++) {
            if (selected_cells[i]) {
                for (inven_i = 0; inven_i < INVEN_GRID_ROW; inven_i++) {
                    if ((invenItems[inven_i] == nullptr) ||
                        (shopItems[i]->getUNIQUEIDENTIFIER() ==
                         invenItems[inven_i]->getUNIQUEIDENTIFIER()))
                        break;
                }
                if (currentPlayerBalance >= shopItems[i]->getPrice() &&
                    inven_i < INVEN_GRID_ROW) {
                    if (invenItems[inven_i] == nullptr) {
                        playSFX(TRANSACTION);
                        imgInvenItems[inven_i] = new ImageObject(
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                256,
                                256,
                                shopItems[i]->getImageFileName());
                        grids[1]->setImageSizeToCell(imgInvenItems[inven_i],
                                                     0.8);
                        grids[1]->placeImageToCell(
                                imgInvenItems[inven_i], inven_i, 1);
                        invenItems[inven_i] = shopItems[i]->getItemInstance();
                        currentPlayerBalance -= shopItems[i]->getPrice();
                    } else {
                        if (invenItems[inven_i]->getRemaining() <
                            invenItems[inven_i]->getMaxStack()) {
                            playSFX(TRANSACTION);
                            currentPlayerBalance -= shopItems[i]->getPrice();
                            invenItems[inven_i]->setRemaining(
                                    invenItems[inven_i]->getRemaining() +
                                    shopItems[i]->getPackageNum());
                            if (invenItems[inven_i]->getRemaining() >
                                invenItems[inven_i]->getMaxStack())
                                invenItems[inven_i]->setRemaining(
                                        invenItems[inven_i]->getMaxStack());
                        } else {
                            playSFX(INVALID_CLICK);
                        }
                    }

                    delete labelPlayerBalance;
                    std::string balance =
                            "$ " + std::to_string(currentPlayerBalance);
                    labelPlayerBalance =
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
                                    invenItems[inven_i]->getRemaining());
                    delete labelInvenItemRemains[inven_i];
                    labelInvenItemRemains[inven_i] =
                            new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
                    grids[1]->placeTextToCell(
                            labelInvenItemRemains[inven_i], inven_i, 1);
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

    for (int i = 0; i < INVEN_GRID_ROW * 2; i++) {
        if (selected_cells[i] && invenWpns[i / 2] != nullptr &&
            i % 2 == 0) {  // i%2 == 0 is weapon inventory
            total_sell +=
                    static_cast<int>(((invenWpns[i / 2]->getPrice() /
                                       invenWpns[i / 2]->getPackageNum()) /
                                      1.5) *
                                     invenWpns[i / 2]->getRemaining());
            delete imgInvenWpns[i / 2];
            delete labelInvenWpnRemains[i / 2];
            delete invenWpns[i / 2];
            imgInvenWpns[i / 2] = nullptr;
            labelInvenWpnRemains[i / 2] = nullptr;
            invenWpns[i / 2] = nullptr;
        }
        if (selected_cells[i] && invenItems[i / 2] != nullptr && i % 2 == 1) {
            total_sell +=
                    static_cast<int>(((invenItems[i / 2]->getPrice() /
                                       invenItems[i / 2]->getPackageNum()) /
                                      1.5) *
                                     invenItems[i / 2]->getRemaining());
            delete imgInvenItems[i / 2];
            delete labelInvenItemRemains[i / 2];
            delete invenItems[i / 2];
            imgInvenItems[i / 2] = nullptr;
            labelInvenItemRemains[i / 2] = nullptr;
            invenItems[i / 2] = nullptr;
        }
    }

    if (total_sell != 0) {
        playSFX(TRANSACTION);
        currentPlayerBalance += total_sell;
        delete labelPlayerBalance;
        std::string balance = "$ " + std::to_string(currentPlayerBalance);
        labelPlayerBalance = new TextObject(balance,
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
    playMusic(SHOPMENU);
    glPushMatrix();
    /*	ALWAYS START AT UPPER LEFT CORNER -> LOWER LEFT CORNER -> LOWER RIGHT
     * CORNER -> UPPER RIGHT	*/
    glBegin(GL_QUADS);
    glColor3f(0.85f, 0.85f, 0.85f);
    glVertex3f(-1 * (this->width / 2.0), (this->height / 2.0), 0); /*	|\ 	*/
    glVertex3f(
            -1 * (this->width / 2.0), -1 * (this->height / 2.0), 0); /*	| |	*/
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percentBorder * (this->height)),
            -1 * (this->height / 2.0) + (this->percentBorder * (this->height)),
            0); /*	|/ 	*/
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percentBorder * (this->height)),
            (this->height / 2.0) - (this->percentBorder * (this->height)),
            0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.80f, 0.80f);
    glVertex3f(-1 * (this->width / 2.0), (this->height / 2.0), 0); /*_____ */
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percentBorder * (this->height)),
            (this->height / 2.0) - (this->percentBorder * (this->height)),
            0); /*\	  / */
    glVertex3f((this->width / 2.0) - (this->percentBorder * (this->height)),
               (this->height / 2.0) - (this->percentBorder * (this->height)),
               0); /* ---	*/
    glVertex3f((this->width / 2.0), (this->height / 2.0), 0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.75f, 0.75f, 0.75f);
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percentBorder * (this->height)),
            (this->height / 2.0) - (this->percentBorder * (this->height)),
            0); /*_____ */
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percentBorder * (this->height)),
            -1 * (this->height / 2.0) + (this->percentBorder * (this->height)),
            0); /*|	  | */
    glVertex3f(
            (this->width / 2.0) - (this->percentBorder * (this->height)),
            -1 * (this->height / 2.0) + (this->percentBorder * (this->height)),
            0); /*----- */
    glVertex3f((this->width / 2.0) - (this->percentBorder * (this->height)),
               (this->height / 2.0) - (this->percentBorder * (this->height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.45f, 0.45f, 0.45f);
    glVertex3f(
            -1 * (this->width / 2.0) + (this->percentBorder * (this->height)),
            -1 * (this->height / 2.0) + (this->percentBorder * (this->height)),
            0); /* ___  */
    glVertex3f(-1 * (this->width / 2.0),
               -1 * (this->height / 2.0),
               0);                                                 /*/	  \ */
    glVertex3f((this->width / 2.0), -1 * (this->height / 2.0), 0); /*----- */
    glVertex3f(
            (this->width / 2.0) - (this->percentBorder * (this->height)),
            -1 * (this->height / 2.0) + (this->percentBorder * (this->height)),
            0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.40f, 0.40f, 0.40f);
    glVertex3f((this->width / 2.0) - (this->percentBorder * (this->height)),
               (this->height / 2.0) - (this->percentBorder * (this->height)),
               0); /*	 /| 	*/
    glVertex3f(
            (this->width / 2.0) - (this->percentBorder * (this->height)),
            -1 * (this->height / 2.0) + (this->percentBorder * (this->height)),
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
    labelWpn->draw();
    labelItem->draw();
    labelPlayerNum->draw();
    labelPlayerBalance->draw();
    labelDiscription->draw();
    labelBuyPrice->draw();
    labelSellPrice->draw();

    if (buttons[0]->isToggled()) {
        for (int i = 0; i < NUM_SALES_WEAPON; i++) {
            imgShopWpns[i]->draw();
            labelShopWpnRemains[i]->draw();
        }
    } else {
        for (int i = 0; i < NUM_SALES_ITEM; i++) {
            imgShopItems[i]->draw();
            labelShopItemRemains[i]->draw();
        }
    }

    for (int i = 0; i < INVEN_GRID_ROW; i++) {
        if (imgInvenWpns[i] != nullptr) {
            imgInvenWpns[i]->draw();
            labelInvenWpnRemains[i]->draw();
        }
        if (imgInvenItems[i] != nullptr) {
            imgInvenItems[i]->draw();
            labelInvenItemRemains[i]->draw();
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
        delete labelDiscription;
        delete labelBuyPrice;
        labelDiscription = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        labelBuyPrice = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
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
        delete labelDiscription;
        delete labelBuyPrice;
        labelDiscription = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        labelBuyPrice = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
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
        currentPlayerIndex++;
        displayCurrentPlayerInfo();
    }
}
