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
                   GLfloat percentBorder,
                   GlobalSettings* global_settings,
                   PlayerFactory* player_factory,
                   int* gameState) {
    this->global_settings = global_settings;
    this->player_factory = player_factory;
    this->currentGameState = gameState;
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
    this->percentBorder = percentBorder;
    this->pos[0] = this->pos[1] = this->pos[2] = 0;

    grids[0] = new ControlItemGrid(this->pos[0] - this->width * 0.4,
                                   this->pos[1] + this->height * 0.28,
                                   this->pos[2],
                                   this->width * 0.3,
                                   this->height * 0.3,
                                   SHOP_GRID_ROW,
                                   SHOP_GRID_COL,
                                   0.72,
                                   0.25,
                                   0.41,
                                   false,
                                   false);
    grids[1] = new ControlItemGrid(this->pos[0] + this->width * 0.175,
                                   this->pos[1] + this->height * 0.30,
                                   this->pos[2],
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
                                       this->pos[0] - this->width * 0.34,
                                       this->pos[1] + this->height * 0.35,
                                       this->pos[2] + 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       0.075 * (this->width),
                                       0.04 * (this->height),
                                       "Weapon");
    buttons[0]->setToggled(true);
    buttons[0]->updateButtonState();

    buttons[1] = new ControlItemButton(nullptr,
                                       this->pos[0] - this->width * 0.235,
                                       this->pos[1] + this->height * 0.35,
                                       this->pos[2] + 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       0.075 * (this->width),
                                       0.04 * (this->height),
                                       "Item");
    buttons[2] = new ControlItemButton(nullptr,
                                       this->pos[0] - this->width * 0.2,
                                       this->pos[1] - this->height * 0.175,
                                       this->pos[2] + 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       0.075 * (this->width),
                                       0.04 * (this->height),
                                       "Buy");
    buttons[3] = new ControlItemButton(nullptr,
                                       this->pos[0] + this->width * 0.275,
                                       this->pos[1] - this->height * 0.175,
                                       this->pos[2] + 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       0.075 * (this->width),
                                       0.04 * (this->height),
                                       "Sell");
    buttons[4] = new ControlItemButton(nullptr,
                                       this->pos[0] + this->width * 0.15,
                                       this->pos[1] - this->height * 0.365,
                                       this->pos[2] + 0.5,
                                       0.65,
                                       0.15,
                                       0.15,
                                       0.135 * (this->width),
                                       0.04 * (this->height),
                                       "Finish Shopping");

    /*LABEL PLACEMENT*/
    this->labelWpn =
            new TextObject("Weapon",
                           grids[1]->getXPos() + grids[1]->getWidth() * 0.05,
                           grids[1]->getYPos() + 15,
                           (this->pos[2] + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    this->labelItem =
            new TextObject("Item",
                           grids[1]->getXPos() + grids[1]->getWidth() / 1.6,
                           grids[1]->getYPos() + 15,
                           (this->pos[2] + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    this->labelPlayerNum = new TextObject("Player 1 Balance:",
                                          this->pos[0] - this->width * 0.35,
                                          this->pos[1] - this->height * 0.39,
                                          (this->pos[2] + 1),
                                          GLUT_BITMAP_TIMES_ROMAN_24,
                                          0.0f,
                                          0.0f,
                                          0.0f);
    this->labelDiscription = new TextObject("",
                                            this->pos[0] - this->width * 0.4,
                                            this->pos[1] - this->height * 0.1,
                                            this->pos[2] + 1,
                                            GLUT_BITMAP_TIMES_ROMAN_24,
                                            0.0f,
                                            0.0f,
                                            0.0f);
    this->labelBuyPrice = new TextObject("",
                                         this->pos[0] - this->width * 0.35,
                                         this->pos[1] - this->height * 0.2,
                                         this->pos[2] + 1,
                                         GLUT_BITMAP_TIMES_ROMAN_24,
                                         0.0f,
                                         0.0f,
                                         0.0f);
    this->labelSellPrice = new TextObject("$ 0",
                                          this->pos[0] + this->width * 0.15,
                                          this->pos[1] - this->height * 0.2,
                                          this->pos[2] + 1,
                                          GLUT_BITMAP_TIMES_ROMAN_24,
                                          0.0f,
                                          0.0f,
                                          0.0f);
    char* balance = "";
    char add[20];
    sprintf(add, "$ %d", currentPlayerBalance);
    balance = add;
    this->labelPlayerBalance =
            new TextObject(balance,
                           this->pos[0] - this->width * 0.2,
                           this->pos[1] - this->height * 0.39,
                           (this->pos[2] + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);

    /*Weapons and Items Creation*/
    this->shopWpns[0] = new WeaponMFB(0);
    this->shopWpns[1] = new WeaponBFB(1);
    this->shopWpns[2] = new WeaponAcid(2);
    this->shopWpns[3] = new WeaponThor(3);
    this->shopWpns[4] = new WeaponEMP(4);
    this->shopWpns[5] = new WeaponPadlock(5);
    this->shopWpns[6] = new WeaponRevive(6);
    this->shopWpns[7] = new WeaponTeleport(7);
    this->shopWpns[8] = new WeaponAtom(8);
    this->shopWpns[9] = new WeaponNuke(9);

    this->shopItems[0] = new ItemSmallRepair(0);
    this->shopItems[1] = new ItemBigRepair(1);
    this->shopItems[2] = new ItemAntiAcid(2);
    this->shopItems[3] = new ItemDoubleAction(3);
    this->shopItems[4] = new ItemShield(4);
    this->shopItems[5] = new ItemExtraBattery(5);
    this->shopItems[6] = new ItemCloak(6);
    this->shopItems[7] = new ItemFloat(7);

    displayCurrentPlayerInfo();

    /*Images and remainsLabels Creation*/
    for (int i = 0; i < NUM_SALES_WEAPON; i++) {
        this->imgShopWpns[i] = new ImageObject(
                0,
                0,
                2.0f,
                100,
                100,
                .0006 * (this->width),
                256,
                256,
                const_cast<char*>(shopWpns[i]->getImageFileName()));
        grids[0]->setImageSizeToCell(this->imgShopWpns[i], 0.8);

        char* remain = "";
        char add[10];
        sprintf(add, "x %d", shopWpns[i]->getPackageNum());
        remain = add;
        this->labelShopWpnRemains[i] = new TextObject(
                remain, 0, 0, 0, GLUT_BITMAP_TIMES_ROMAN_24, 0.6f, 0.3f, 0.4f);
    }
    for (int i = 0; i < NUM_SALES_ITEM; i++) {
        this->imgShopItems[i] = new ImageObject(
                0,
                0,
                2.0f,
                100,
                100,
                .0006 * (this->width),
                256,
                256,
                const_cast<char*>(shopItems[i]->getImageFileName()));
        grids[0]->setImageSizeToCell(this->imgShopItems[i], 0.8);

        char* remain = "";
        char add[10];
        sprintf(add, "x %d", shopItems[i]->getPackageNum());
        remain = add;
        this->labelShopItemRemains[i] = new TextObject(
                remain, 0, 0, 0, GLUT_BITMAP_TIMES_ROMAN_24, 0.6f, 0.3f, 0.4f);
    }

    /*Set position of images and remainsLabels to shopping cells*/
    int index = 0;
    for (int r = 0; r < SHOP_GRID_ROW; r++) {
        for (int c = 0; c < SHOP_GRID_COL; c++) {
            if (index < NUM_SALES_WEAPON) {
                grids[0]->placeImageToCell(this->imgShopWpns[index], r, c);
                this->grids[0]->placeTextToCell(
                        this->labelShopWpnRemains[index], r, c);
            }
            if (index < NUM_SALES_ITEM) {
                grids[0]->placeImageToCell(this->imgShopItems[index], r, c);
                this->grids[0]->placeTextToCell(
                        this->labelShopItemRemains[index], r, c);
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
        delete this->shopWpns[i];
        delete this->imgShopWpns[i];
        delete this->labelShopWpnRemains[i];
    }
    for (int i = 0; i < NUM_SALES_ITEM; i++) {
        delete this->shopItems[i];
        delete this->imgShopItems[i];
        delete this->labelShopItemRemains[i];
    }
    for (int i = 0; i < INVEN_GRID_ROW; i++) {
        delete this->imgInvenWpns[i];
        delete this->labelInvenWpnRemains[i];
        delete this->imgInvenItems[i];
        delete this->labelInvenItemRemains[i];
    }
}

/*GETTERS & SETTERS*/
void ShopMenu::updateNumPlayers(int n) { this->numPlayers = n; }

void ShopMenu::saveCurrentPlayerInfo() {
    // save currentPlayerBalance and the inventory (Weapon, Item objects, and
    // remainings)
    this->player_factory->getPlayer(currentPlayerIndex)
            ->setCash(currentPlayerBalance);
    this->player_factory->getPlayer(currentPlayerIndex)->setWeapons(invenWpns);
    this->player_factory->getPlayer(currentPlayerIndex)->setItems(invenItems);

    // clear inventory slots for the next player
    for (int i = 0; i < INVEN_GRID_ROW; i++) {
        if (this->imgInvenWpns[i] != nullptr) {
            delete this->imgInvenWpns[i];
            delete this->labelInvenWpnRemains[i];
            this->imgInvenWpns[i] = nullptr;
            this->labelInvenWpnRemains[i] = nullptr;
            this->invenWpns[i] = nullptr;
        }
        if (this->imgInvenItems[i] != nullptr) {
            delete this->imgInvenItems[i];
            delete this->labelInvenItemRemains[i];
            this->imgInvenItems[i] = nullptr;
            this->labelInvenItemRemains[i] = nullptr;
            this->invenItems[i] = nullptr;
        }
    }

    this->buttons[1]->setToggled(false);
    buttons[1]->updateButtonState();
    this->buttons[0]->setToggled(true);
    buttons[0]->updateButtonState();
}

void ShopMenu::displayCurrentPlayerInfo() {
    while ((this->currentPlayerIndex < numPlayers) &&
           (strcmp(this->player_factory->getPlayer(currentPlayerIndex)
                           ->getPlayer_Type(),
                   "CPU") == 0)) {
        this->currentPlayerIndex++;
    }

    if (this->currentPlayerIndex < this->numPlayers) {
        for (int i = 0; i < INVEN_GRID_ROW; i++) {
            this->invenWpns[i] = player_factory->getPlayer(currentPlayerIndex)
                                         ->getCurrentWeapons()[i];
            if (this->invenWpns[i]) {
                this->imgInvenWpns[i] = new ImageObject(
                        0,
                        0,
                        0,
                        0,
                        0,
                        0,
                        256,
                        256,
                        const_cast<char*>(
                                this->invenWpns[i]->getImageFileName()));
                this->grids[1]->setImageSizeToCell(this->imgInvenWpns[i], 0.8);
                this->grids[1]->placeImageToCell(this->imgInvenWpns[i], i, 0);

                char* remain = "";
                char add1[10];
                sprintf(add1, "x %d", invenWpns[i]->getRemaining());
                remain = add1;
                delete this->labelInvenWpnRemains[i];
                this->labelInvenWpnRemains[i] =
                        new TextObject(remain,
                                       0,
                                       0,
                                       0,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.6f,
                                       0.3f,
                                       0.4f);
                this->grids[1]->placeTextToCell(
                        this->labelInvenWpnRemains[i], i, 0);
            }

            this->invenItems[i] = player_factory->getPlayer(currentPlayerIndex)
                                          ->getCurrentItems()[i];
            if (this->invenItems[i]) {
                this->imgInvenItems[i] = new ImageObject(
                        0,
                        0,
                        0,
                        0,
                        0,
                        0,
                        256,
                        256,
                        const_cast<char*>(
                                this->invenItems[i]->getImageFileName()));
                this->grids[1]->setImageSizeToCell(this->imgInvenItems[i],
                                                   0.8);
                this->grids[1]->placeImageToCell(this->imgInvenItems[i], i, 1);

                char* remain = "";
                char add1[10];
                sprintf(add1, "x %d", this->invenItems[i]->getRemaining());
                remain = add1;
                delete this->labelInvenItemRemains[i];
                this->labelInvenItemRemains[i] =
                        new TextObject(remain,
                                       0,
                                       0,
                                       0,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.6f,
                                       0.3f,
                                       0.4f);
                this->grids[1]->placeTextToCell(
                        this->labelInvenItemRemains[i], i, 1);
            }
        }

        // Set next player number label
        delete this->labelPlayerNum;
        char* str = "";
        char add[20];
        sprintf(add, "Player %d Balance:", currentPlayerIndex + 1);
        str = add;
        this->labelPlayerNum =
                new TextObject(str,
                               this->pos[0] - this->width * 0.35,
                               this->pos[1] - this->height * 0.39,
                               (this->pos[2] + 1),
                               GLUT_BITMAP_TIMES_ROMAN_24,
                               0.0f,
                               0.0f,
                               0.0f);

        // Set next player balance label
        currentPlayerBalance =
                player_factory->getPlayer(currentPlayerIndex)->getCash();
        delete this->labelPlayerBalance;
        char* balance = "";
        char add1[20];
        sprintf(add1, "$ %d", currentPlayerBalance);
        balance = add1;
        this->labelPlayerBalance =
                new TextObject(balance,
                               this->pos[0] - this->width * 0.2,
                               this->pos[1] - this->height * 0.39,
                               (this->pos[2] + 1),
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
               player_factory->getPlayer(i)->getPlayer_Type());
        printf("\nAI Difficulty: %s",
               player_factory->getPlayer(i)->getAI_Type());
        printf("\nPlayer Name: %s",
               player_factory->getPlayer(i)->getPlayerName());
        printf("\nTeam Number: %c",
               player_factory->getPlayer(i)->getTeamLabel());
        printf("\nTank Type: %s", player_factory->getPlayer(i)->getTankType());
        printf("\nCurrent Money: %d", player_factory->getPlayer(i)->getCash());
        for (int j = 0; j < INVEN_GRID_ROW; j++) {
            if (player_factory->getPlayer(i)->getCurrentWeapons()[j]) {
                printf("\nWeapon Slot %i: %s",
                       j,
                       player_factory->getPlayer(i)
                               ->getCurrentWeapons()[j]
                               ->getDescription());
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
                               ->getDescription());
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
    bool* selectedCells = grids[0]->getSelectedCells();
    if (this->buttons[0]->isToggled()) {
        for (int i = 0; i < NUM_SALES_WEAPON; i++) {
            if (selectedCells[i]) {
                delete this->labelDiscription;
                delete this->labelBuyPrice;
                this->labelDiscription = new TextObject(
                        const_cast<char*>(shopWpns[i]->getDescription()),
                        this->pos[0] - this->width * 0.4,
                        this->pos[1] - this->height * 0.1,
                        this->pos[2] + 1,
                        GLUT_BITMAP_TIMES_ROMAN_24,
                        0.0f,
                        0.0f,
                        0.0f);
                char* price = "";
                char add[20];
                sprintf(add, "$ %d", this->shopWpns[i]->getPrice());
                price = add;
                this->labelBuyPrice =
                        new TextObject(price,
                                       this->pos[0] - this->width * 0.35,
                                       this->pos[1] - this->height * 0.2,
                                       this->pos[2] + 1,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                break;
            } else {
                delete this->labelDiscription;
                delete this->labelBuyPrice;
                this->labelDiscription =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
                this->labelBuyPrice =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
            }
        }
    } else {
        for (int i = 0; i < NUM_SALES_ITEM; i++) {
            if (selectedCells[i]) {
                delete this->labelDiscription;
                delete this->labelBuyPrice;
                this->labelDiscription = new TextObject(
                        const_cast<char*>(shopItems[i]->getDescription()),
                        this->pos[0] - this->width * 0.4,
                        this->pos[1] - this->height * 0.1,
                        this->pos[2] + 1,
                        GLUT_BITMAP_TIMES_ROMAN_24,
                        0.0f,
                        0.0f,
                        0.0f);
                char* price = "";
                char add[20];
                sprintf(add, "$ %d", this->shopItems[i]->getPrice());
                price = add;
                this->labelBuyPrice =
                        new TextObject(price,
                                       this->pos[0] - this->width * 0.35,
                                       this->pos[1] - this->height * 0.2,
                                       this->pos[2] + 1,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.0f,
                                       0.0f,
                                       0.0f);
                break;
            } else {
                delete this->labelDiscription;
                delete this->labelBuyPrice;
                this->labelDiscription =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
                this->labelBuyPrice =
                        new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
            }
        }
    }
}

void ShopMenu::updateSellLabel() {
    bool* selectedCells = grids[1]->getSelectedCells();
    int totalSell = 0;

    for (int i = 0; i < INVEN_GRID_ROW * 2; i++) {
        if (selectedCells[i] && invenWpns[i / 2] != nullptr &&
            i % 2 == 0) {  //	i%2 == 0 is weapon inventory
            totalSell +=
                    static_cast<int>(((invenWpns[i / 2]->getPrice() /
                                       invenWpns[i / 2]->getPackageNum()) /
                                      1.5) *
                                     invenWpns[i / 2]->getRemaining());
        }
        if (selectedCells[i] && invenItems[i / 2] != nullptr && i % 2 == 1) {
            totalSell +=
                    static_cast<int>(((invenItems[i / 2]->getPrice() /
                                       invenItems[i / 2]->getPackageNum()) /
                                      1.5) *
                                     invenItems[i / 2]->getRemaining());
        }
    }

    delete this->labelSellPrice;
    char* price = "";
    char add[20];
    sprintf(add, "$ %d", totalSell);
    price = add;
    this->labelSellPrice = new TextObject(price,
                                          this->pos[0] + this->width * 0.15,
                                          this->pos[1] - this->height * 0.2,
                                          this->pos[2] + 1,
                                          GLUT_BITMAP_TIMES_ROMAN_24,
                                          0.0f,
                                          0.0f,
                                          0.0f);
}

void ShopMenu::buyHandler() {
    int inven_i;
    bool* selectedCells = grids[0]->getSelectedCells();

    if (this->buttons[0]->isToggled()) {
        for (int i = 0; i < NUM_SALES_WEAPON; i++) {
            if (selectedCells[i]) {
                for (inven_i = 0; inven_i < INVEN_GRID_ROW; inven_i++) {
                    if ((this->invenWpns[inven_i] == nullptr) ||
                        (this->shopWpns[i]->getUNIQUEIDENTIFIER() ==
                         this->invenWpns[inven_i]->getUNIQUEIDENTIFIER()))
                        break;
                }
                if ((this->currentPlayerBalance >=
                     this->shopWpns[i]->getPrice()) &&
                    (inven_i < INVEN_GRID_ROW)) {
                    if ((invenWpns[inven_i] == nullptr)) {
                        playSFX(TRANSACTION);
                        this->imgInvenWpns[inven_i] = new ImageObject(
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                256,
                                256,
                                const_cast<char*>(
                                        this->shopWpns[i]
                                                ->getImageFileName()));
                        this->grids[1]->setImageSizeToCell(
                                this->imgInvenWpns[inven_i], 0.8);
                        this->grids[1]->placeImageToCell(
                                this->imgInvenWpns[inven_i], inven_i, 0);
                        this->invenWpns[inven_i] =
                                this->shopWpns[i]->getWeaponInstance();
                        currentPlayerBalance -= shopWpns[i]->getPrice();
                    } else {
                        if (invenWpns[inven_i]->getRemaining() <
                            invenWpns[inven_i]->getMaxStack()) {
                            playSFX(TRANSACTION);
                            currentPlayerBalance -= shopWpns[i]->getPrice();
                            this->invenWpns[inven_i]->setRemaining(
                                    this->invenWpns[inven_i]->getRemaining() +
                                    this->shopWpns[i]->getPackageNum());
                            if (this->invenWpns[inven_i]->getRemaining() >
                                this->invenWpns[inven_i]->getMaxStack())
                                this->invenWpns[inven_i]->setRemaining(
                                        this->invenWpns[inven_i]
                                                ->getMaxStack());
                        } else {
                            playSFX(INVALID_CLICK);
                        }
                    }

                    delete this->labelPlayerBalance;
                    char* balance = "";
                    char add[20];
                    sprintf(add, "$ %d", currentPlayerBalance);
                    balance = add;
                    this->labelPlayerBalance =
                            new TextObject(balance,
                                           this->pos[0] - this->width * 0.2,
                                           this->pos[1] - this->height * 0.39,
                                           (this->pos[2] + 1),
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.0f,
                                           0.0f,
                                           0.0f);

                    char* remain = "";
                    char add1[10];
                    sprintf(add1, "x %d", invenWpns[inven_i]->getRemaining());
                    remain = add1;
                    delete this->labelInvenWpnRemains[inven_i];
                    this->labelInvenWpnRemains[inven_i] =
                            new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
                    this->grids[1]->placeTextToCell(
                            this->labelInvenWpnRemains[inven_i], inven_i, 0);
                } else {
                    playSFX(INVALID_CLICK);
                }
                break;
            }
        }
    } else {
        for (int i = 0; i < NUM_SALES_ITEM; i++) {
            if (selectedCells[i]) {
                for (inven_i = 0; inven_i < INVEN_GRID_ROW; inven_i++) {
                    if ((this->invenItems[inven_i] == nullptr) ||
                        (this->shopItems[i]->getUNIQUEIDENTIFIER() ==
                         this->invenItems[inven_i]->getUNIQUEIDENTIFIER()))
                        break;
                }
                if (this->currentPlayerBalance >=
                            this->shopItems[i]->getPrice() &&
                    inven_i < INVEN_GRID_ROW) {
                    if (invenItems[inven_i] == nullptr) {
                        playSFX(TRANSACTION);
                        this->imgInvenItems[inven_i] = new ImageObject(
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                256,
                                256,
                                const_cast<char*>(
                                        this->shopItems[i]
                                                ->getImageFileName()));
                        this->grids[1]->setImageSizeToCell(
                                this->imgInvenItems[inven_i], 0.8);
                        this->grids[1]->placeImageToCell(
                                this->imgInvenItems[inven_i], inven_i, 1);
                        this->invenItems[inven_i] =
                                this->shopItems[i]->getItemInstance();
                        currentPlayerBalance -= shopItems[i]->getPrice();
                    } else {
                        if (invenItems[inven_i]->getRemaining() <
                            invenItems[inven_i]->getMaxStack()) {
                            playSFX(TRANSACTION);
                            currentPlayerBalance -= shopItems[i]->getPrice();
                            this->invenItems[inven_i]->setRemaining(
                                    this->invenItems[inven_i]->getRemaining() +
                                    this->shopItems[i]->getPackageNum());
                            if (this->invenItems[inven_i]->getRemaining() >
                                this->invenItems[inven_i]->getMaxStack())
                                this->invenItems[inven_i]->setRemaining(
                                        this->invenItems[inven_i]
                                                ->getMaxStack());
                        } else {
                            playSFX(INVALID_CLICK);
                        }
                    }

                    delete this->labelPlayerBalance;
                    char* balance = "";
                    char add[20];
                    sprintf(add, "$ %d", currentPlayerBalance);
                    balance = add;
                    this->labelPlayerBalance =
                            new TextObject(balance,
                                           this->pos[0] - this->width * 0.2,
                                           this->pos[1] - this->height * 0.39,
                                           (this->pos[2] + 1),
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.0f,
                                           0.0f,
                                           0.0f);

                    char* remain = "";
                    char add1[10];
                    sprintf(add1,
                            "x %d",
                            this->invenItems[inven_i]->getRemaining());
                    remain = add1;
                    delete this->labelInvenItemRemains[inven_i];
                    this->labelInvenItemRemains[inven_i] =
                            new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
                    this->grids[1]->placeTextToCell(
                            this->labelInvenItemRemains[inven_i], inven_i, 1);
                } else {
                    playSFX(INVALID_CLICK);
                }
                break;
            }
        }
    }
}

void ShopMenu::sellHandler() {
    bool* selectedCells = grids[1]->getSelectedCells();
    int totalSell = 0;

    for (int i = 0; i < INVEN_GRID_ROW * 2; i++) {
        if (selectedCells[i] && invenWpns[i / 2] != nullptr &&
            i % 2 == 0) {  // i%2 == 0 is weapon inventory
            totalSell +=
                    static_cast<int>(((invenWpns[i / 2]->getPrice() /
                                       invenWpns[i / 2]->getPackageNum()) /
                                      1.5) *
                                     invenWpns[i / 2]->getRemaining());
            delete this->imgInvenWpns[i / 2];
            delete this->labelInvenWpnRemains[i / 2];
            delete this->invenWpns[i / 2];
            this->imgInvenWpns[i / 2] = nullptr;
            this->labelInvenWpnRemains[i / 2] = nullptr;
            this->invenWpns[i / 2] = nullptr;
        }
        if (selectedCells[i] && invenItems[i / 2] != nullptr && i % 2 == 1) {
            totalSell +=
                    static_cast<int>(((invenItems[i / 2]->getPrice() /
                                       invenItems[i / 2]->getPackageNum()) /
                                      1.5) *
                                     invenItems[i / 2]->getRemaining());
            delete this->imgInvenItems[i / 2];
            delete this->labelInvenItemRemains[i / 2];
            delete this->invenItems[i / 2];
            this->imgInvenItems[i / 2] = nullptr;
            this->labelInvenItemRemains[i / 2] = nullptr;
            this->invenItems[i / 2] = nullptr;
        }
    }

    if (totalSell != 0) {
        playSFX(TRANSACTION);
        currentPlayerBalance += totalSell;
        delete this->labelPlayerBalance;
        char* balance = "";
        char add[20];
        sprintf(add, "$ %d", currentPlayerBalance);
        balance = add;
        this->labelPlayerBalance =
                new TextObject(balance,
                               this->pos[0] - this->width * 0.2,
                               this->pos[1] - this->height * 0.39,
                               (this->pos[2] + 1),
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
    glVertex3f(this->pos[0] + this->width * 0.04 - 1,
               this->pos[1] + this->height * 0.45,
               this->pos[2] + 1);
    glVertex3f(this->pos[0] + this->width * 0.04 - 1,
               this->pos[1] - this->height * 0.277,
               this->pos[2] + 1);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(this->pos[0] + this->width * 0.04,
               this->pos[1] + this->height * 0.45,
               this->pos[2] + 1);
    glVertex3f(this->pos[0] + this->width * 0.04,
               this->pos[1] - this->height * 0.277,
               this->pos[2] + 1);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(this->pos[0] + this->width * 0.04 + 1,
               this->pos[1] + this->height * 0.45,
               this->pos[2] + 1);
    glVertex3f(this->pos[0] + this->width * 0.04 + 1,
               this->pos[1] - this->height * 0.277,
               this->pos[2] + 1);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(this->pos[0] - this->width * 0.45,
               this->pos[1] - this->height * 0.29 + 1,
               this->pos[2] + 1);
    glVertex3f(this->pos[0] + this->width * 0.45,
               this->pos[1] - this->height * 0.29 + 1,
               this->pos[2] + 1);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(this->pos[0] - this->width * 0.45,
               this->pos[1] - this->height * 0.29,
               this->pos[2] + 1);
    glVertex3f(this->pos[0] + this->width * 0.45,
               this->pos[1] - this->height * 0.29,
               this->pos[2] + 1);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(this->pos[0] - this->width * 0.45,
               this->pos[1] - this->height * 0.29 - 1,
               this->pos[2] + 1);
    glVertex3f(this->pos[0] + this->width * 0.45,
               this->pos[1] - this->height * 0.29 - 1,
               this->pos[2] + 1);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex3f(this->grids[1]->getXPos() + this->grids[1]->getWidth() * 0.5,
               this->grids[1]->getYPos() + 35,
               this->pos[2] + 1);
    glVertex3f(this->grids[1]->getXPos() + this->grids[1]->getWidth() * 0.5,
               this->grids[1]->getYPos() + 5,
               this->pos[2] + 1);
    glEnd();

    this->grids[0]->draw();
    this->grids[1]->draw();
    this->buttons[0]->draw();
    this->buttons[1]->draw();
    this->buttons[2]->draw();
    this->buttons[3]->draw();
    this->buttons[4]->draw();
    this->labelWpn->draw();
    this->labelItem->draw();
    this->labelPlayerNum->draw();
    this->labelPlayerBalance->draw();
    this->labelDiscription->draw();
    this->labelBuyPrice->draw();
    this->labelSellPrice->draw();

    if (this->buttons[0]->isToggled()) {
        for (int i = 0; i < NUM_SALES_WEAPON; i++) {
            this->imgShopWpns[i]->draw();
            this->labelShopWpnRemains[i]->draw();
        }
    } else {
        for (int i = 0; i < NUM_SALES_ITEM; i++) {
            this->imgShopItems[i]->draw();
            this->labelShopItemRemains[i]->draw();
        }
    }

    for (int i = 0; i < INVEN_GRID_ROW; i++) {
        if (this->imgInvenWpns[i] != nullptr) {
            this->imgInvenWpns[i]->draw();
            this->labelInvenWpnRemains[i]->draw();
        }
        if (this->imgInvenItems[i] != nullptr) {
            this->imgInvenItems[i]->draw();
            this->labelInvenItemRemains[i]->draw();
        }
    }
    glPopMatrix();
}

void ShopMenu::buttonTest(int x, int y, int buttonDown) {
    grids[0]->mouseClickEvent(x, y, buttonDown, true);
    updateBuyDiscriptLabel();
    grids[1]->mouseClickEvent(x, y, buttonDown, true);
    updateSellLabel();

    if ((x >= (buttons[0]->getXPos()) &&
         x <= ((buttons[0]->getXPos()) + (buttons[0]->getWidth()))) &&
        (y <= (buttons[0]->getYPos()) &&
         y >= ((buttons[0]->getYPos()) - (buttons[0]->getHeight())))) {
        if (buttonDown) playSFX(SMALL_CLICK);
        grids[0]->deselectAllCells();
        delete this->labelDiscription;
        delete this->labelBuyPrice;
        this->labelDiscription = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        this->labelBuyPrice = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        buttons[0]->mouseClickEvent(x, y, buttonDown, true);
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
        if (buttonDown) playSFX(SMALL_CLICK);
        grids[0]->deselectAllCells();
        delete this->labelDiscription;
        delete this->labelBuyPrice;
        this->labelDiscription = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        this->labelBuyPrice = new TextObject("", 0, 0, 0, nullptr, 0, 0, 0);
        buttons[1]->mouseClickEvent(x, y, buttonDown, true);
        if (buttons[1]->isToggled()) {
            buttons[0]->setToggled(false);
            buttons[0]->updateButtonState();
            buttons[1]->updateButtonState();
        }
    }

    buttons[2]->mouseClickEvent(x, y, buttonDown, true);
    if (buttons[2]->isToggled()) {
        buyHandler();
        buttons[2]->setToggled(false);
    }

    buttons[3]->mouseClickEvent(x, y, buttonDown, true);
    if (buttons[3]->isToggled()) {
        sellHandler();
        grids[1]->deselectAllCells();
        buttons[3]->setToggled(false);
    }

    buttons[4]->mouseClickEvent(x, y, buttonDown, true);
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
