#include "Inventory.h"
#include <math.h>
#include "ControlItem.h"
#include "ControlItemGrid.h"
#include "ImageObject.h"
#include "Item.h"
#include "Player.h"
#include "PlayerHuman.h"
#include "TextObject.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

Inventory::Inventory() = default;
Inventory::Inventory(GLfloat x, GLfloat y, int w, int h) {
    xPos = x - x * 0.01;
    yPos = y - y * 0.01;
    this->width = w + w * 0.01;
    this->height = h + h * 0.01;
    invenGrid = new ControlItemGrid(-width / 4.0,
                                    height / 4.0,
                                    1,
                                    width * 0.5,
                                    height * 0.4,
                                    2,
                                    5,
                                    0.5,
                                    0.5,
                                    1,
                                    true,
                                    false);
    for (int i = 0; i < PLAYER_MAX_WEAPONS + PLAYER_MAX_ITEMS; i++) {
        imgInven[i] = nullptr;
        remainings[i] = nullptr;
    }
    title = new TextObject("Inventory",
                           -width / 3.3,
                           height / 2.5,
                           1,
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0,
                           0,
                           0);
    explain = new TextObject(
            "(Press Enter to load/unload a weapon or use an item)",
            -width / 3.3,
            height / 3.0,
            1,
            GLUT_BITMAP_TIMES_ROMAN_24,
            0,
            0,
            0);
    descript = new TextObject("",
                              -width / 3.6,
                              -height / 3.0,
                              1,
                              GLUT_BITMAP_TIMES_ROMAN_24,
                              0,
                              0,
                              0);
    selectCellRow = 0;
    selectCellCol = 0;
}
Inventory::~Inventory() {
    delete invenGrid;
    for (int i = 0; i < PLAYER_MAX_WEAPONS + PLAYER_MAX_ITEMS; i++) {
        if (imgInven[i]) {
            delete imgInven[i];
            delete remainings[i];
        }
    }
    delete title;
    delete explain;
    delete descript;
}

void Inventory::setupInventory(Player* player) {
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        weapons[i] = player->getCurrentWeapons()[i];
        items[i] = player->getCurrentItems()[i];
    }
    for (int i = 0; i < PLAYER_MAX_WEAPONS; i++) {
        // If Player Has A Weapon
        if (weapons[i] != nullptr) {
            if (imgInven[i]) {
                delete imgInven[i];
                delete remainings[i];
            }
            imgInven[i] = new ImageObject(0,
                                          0,
                                          0,
                                          0,
                                          0,
                                          0,
                                          256,
                                          256,
                                          weapons[i]->getImageFileName());
            invenGrid->setImageSizeToCell(this->imgInven[i], 0.8);
            std::string remain =
                    "x " + std::to_string(weapons[i]->getRemaining());
            remainings[i] = new TextObject(remain,
                                           0,
                                           0,
                                           0,
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.6f,
                                           0.3f,
                                           0.4f);
        } else {
            if (imgInven[i]) {
                delete imgInven[i];
                delete remainings[i];
            }
            imgInven[i] = nullptr;
            remainings[i] = nullptr;
        }
    }
    for (int i = 0; i < PLAYER_MAX_ITEMS; i++) {
        // If Player Has An Item
        if (items[i] != nullptr) {
            if (imgInven[PLAYER_MAX_WEAPONS + i]) {
                delete imgInven[PLAYER_MAX_WEAPONS + i];
                delete remainings[PLAYER_MAX_WEAPONS + i];
            }
            imgInven[PLAYER_MAX_WEAPONS + i] = new ImageObject(
                    0, 0, 0, 0, 0, 0, 256, 256, items[i]->getImageFileName());
            invenGrid->setImageSizeToCell(
                    this->imgInven[PLAYER_MAX_WEAPONS + i], 0.8);
            std::string remain =
                    "x " + std::to_string(items[i]->getRemaining());
            remainings[PLAYER_MAX_WEAPONS + i] =
                    new TextObject(remain,
                                   0,
                                   0,
                                   0,
                                   GLUT_BITMAP_TIMES_ROMAN_24,
                                   0.6f,
                                   0.3f,
                                   0.4f);
        } else {
            if (imgInven[PLAYER_MAX_WEAPONS + i]) {
                delete imgInven[PLAYER_MAX_WEAPONS + i];
                delete remainings[PLAYER_MAX_WEAPONS + i];
            }
            imgInven[PLAYER_MAX_WEAPONS + i] = nullptr;
            remainings[PLAYER_MAX_WEAPONS + i] = nullptr;
        }
    }
    // Place Images and remaining labels to the cells
    int index = 0;
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < PLAYER_MAX_WEAPONS; c++) {
            if (imgInven[index] != nullptr) {
                invenGrid->placeImageToCell(imgInven[index], r, c);
                invenGrid->placeTextToCell(remainings[index], r, c);
            }
            index++;
        }
    }
    selectCellRow = 0;
    selectCellCol = 0;
    invenGrid->selectCell(selectCellRow, selectCellCol);
    if (weapons[0] != nullptr) {
        delete descript;
        descript = new TextObject(weapons[0]->getDescription(),
                                  -width / 3.6,
                                  -height / 3.0,
                                  1,
                                  GLUT_BITMAP_TIMES_ROMAN_24,
                                  0,
                                  0,
                                  0);
    } else {
        delete descript;
        descript = new TextObject("",
                                  -width / 3.6,
                                  -height / 3.0,
                                  1,
                                  GLUT_BITMAP_TIMES_ROMAN_24,
                                  0,
                                  0,
                                  0);
    }
}

void Inventory::handleInventory(Player* currentPlayer, int invenIndex) {
    // IF PLAYER HAS SHOT WITH A SPECIAL WEAPON
    if (invenIndex < PLAYER_MAX_WEAPONS) {
        if (currentPlayer->getLoadedWeapon() != nullptr) {
            if (currentPlayer->getLoadedWeapon()->getRemaining() > 1) {
                currentPlayer->getLoadedWeapon()->setRemaining(
                        currentPlayer->getLoadedWeapon()->getRemaining() - 1);
            } else {
                int index;
                for (index = 0; index < PLAYER_MAX_WEAPONS; index++) {
                    if (currentPlayer->getCurrentWeapons()[index] == nullptr) {
                        // do nothing
                    } else if (currentPlayer->getCurrentWeapons()[index]
                                       ->getUNIQUEIDENTIFIER() ==
                               currentPlayer->getLoadedWeapon()
                                       ->getUNIQUEIDENTIFIER()) {
                        delete currentPlayer->getLoadedWeapon();
                        currentPlayer->getCurrentWeapons()[index] = nullptr;
                        currentPlayer->setLoadedWeapon(nullptr);
                        break;
                    }
                }
            }
        }
    }
    // IF PLAYER HAS USED AN ITEM
    else {
        if (currentPlayer->getCurrentItems()[invenIndex - PLAYER_MAX_WEAPONS]
                    ->getRemaining() > 1) {
            currentPlayer->getCurrentItems()[invenIndex - PLAYER_MAX_WEAPONS]
                    ->setRemaining(
                            currentPlayer
                                    ->getCurrentItems()[invenIndex -
                                                        PLAYER_MAX_WEAPONS]
                                    ->getRemaining() -
                            1);
        } else {
            delete currentPlayer
                    ->getCurrentItems()[invenIndex - PLAYER_MAX_WEAPONS];
            currentPlayer->getCurrentItems()[invenIndex - PLAYER_MAX_WEAPONS] =
                    nullptr;
        }
    }
}

int Inventory::getSelectedIndex() {
    return PLAYER_MAX_WEAPONS * selectCellRow + selectCellCol;
}

void Inventory::keyHandler(int key) {
    // LEFT KEY
    if (key == 1 && selectCellCol > 0) {
        selectCellCol--;
        invenGrid->selectCell(selectCellRow, selectCellCol);
    }
    // UP KEY
    else if (key == 2 && selectCellRow > 0) {
        selectCellRow--;
        invenGrid->selectCell(selectCellRow, selectCellCol);
    }
    // RIGHT KEY
    else if (key == 3 && selectCellCol < PLAYER_MAX_WEAPONS - 1) {
        selectCellCol++;
        invenGrid->selectCell(selectCellRow, selectCellCol);
    }
    // DOWN KEY
    else if (key == 4 && selectCellRow < 1) {
        selectCellRow++;
        invenGrid->selectCell(selectCellRow, selectCellCol);
    }

    // IF ONE OF WEAPONS IS SELECTED
    if (selectCellRow == 0) {
        if (weapons[selectCellCol] != nullptr) {
            delete descript;
            descript = new TextObject(weapons[selectCellCol]->getDescription(),
                                      -width / 3.6,
                                      -height / 3.0,
                                      1,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0,
                                      0,
                                      0);
        } else {
            delete descript;
            descript = new TextObject("",
                                      -width / 3.6,
                                      -height / 3.0,
                                      1,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0,
                                      0,
                                      0);
        }
    }
    // IF ONE OF ITEMS IS SELECTED
    else {
        if (items[selectCellCol] != nullptr) {
            delete descript;
            descript = new TextObject(items[selectCellCol]->getDescription(),
                                      -width / 3.6,
                                      -height / 3.0,
                                      1,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0,
                                      0,
                                      0);
        } else {
            delete descript;
            descript = new TextObject("",
                                      -width / 3.6,
                                      -height / 3.0,
                                      1,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0,
                                      0,
                                      0);
        }
    }
}

void Inventory::draw() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(xPos, yPos, width, height);
    gluPerspective(
            60.0,
            (static_cast<float>(width) / (1.5 * static_cast<float>(height))),
            1,
            199999999);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScissor(xPos, yPos, width, height);
    glClearColor(0.75, 0.75, 0.75, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int distance = static_cast<int>(glutGet(GLUT_WINDOW_HEIGHT) / 4 *
                                    tan(1.04719755));
    gluLookAt(0, 0, distance, 0, 0, 0, 0.0f, 1.0f, 0.0f);

    invenGrid->draw();
    title->draw();
    explain->draw();
    descript->draw();
    for (int i = 0; i < PLAYER_MAX_WEAPONS + PLAYER_MAX_ITEMS; i++) {
        if (imgInven[i] != nullptr) {
            imgInven[i]->draw();
            remainings[i]->draw();
        }
    }

    int win_width = glutGet(GLUT_WINDOW_WIDTH);
    int win_height = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, win_width, win_height);
    gluPerspective(
            60.0,
            static_cast<float>(win_width) / static_cast<float>(win_height),
            1.0,
            1000000.0);

    glMatrixMode(GL_MODELVIEW);
    glScissor(0, 0, win_width, win_height);
    glLoadIdentity();
}
