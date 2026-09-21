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
    x_pos = x - x * 0.01;
    y_pos = y - y * 0.01;
    width = w + w * 0.01;
    height = h + h * 0.01;
    inven_grid = new ControlItemGrid(-width / 4.0,
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
    for (int i = 0; i < player_max_weapons + player_max_items; i++) {
        img_inven[i] = nullptr;
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
    select_cell_row = 0;
    select_cell_col = 0;
}
Inventory::~Inventory() {
    delete inven_grid;
    for (int i = 0; i < player_max_weapons + player_max_items; i++) {
        if (img_inven[i]) {
            delete img_inven[i];
            delete remainings[i];
        }
    }
    delete title;
    delete explain;
    delete descript;
}

void Inventory::setupInventory(Player* player) {
    for (int i = 0; i < player_max_weapons; i++) {
        weapons[i] = player->getCurrentWeapons()[i];
        items[i] = player->getCurrentItems()[i];
    }
    for (int i = 0; i < player_max_weapons; i++) {
        // If Player Has A Weapon
        if (weapons[i] != nullptr) {
            if (img_inven[i]) {
                delete img_inven[i];
                delete remainings[i];
            }
            img_inven[i] = new ImageObject(0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           256,
                                           256,
                                           weapons[i]->getImageFileName());
            inven_grid->setImageSizeToCell(img_inven[i], 0.8);
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
            if (img_inven[i]) {
                delete img_inven[i];
                delete remainings[i];
            }
            img_inven[i] = nullptr;
            remainings[i] = nullptr;
        }
    }
    for (int i = 0; i < player_max_items; i++) {
        // If Player Has An Item
        if (items[i] != nullptr) {
            if (img_inven[player_max_weapons + i]) {
                delete img_inven[player_max_weapons + i];
                delete remainings[player_max_weapons + i];
            }
            img_inven[player_max_weapons + i] = new ImageObject(
                    0, 0, 0, 0, 0, 0, 256, 256, items[i]->getImageFileName());
            inven_grid->setImageSizeToCell(img_inven[player_max_weapons + i],
                                           0.8);
            std::string remain =
                    "x " + std::to_string(items[i]->getRemaining());
            remainings[player_max_weapons + i] =
                    new TextObject(remain,
                                   0,
                                   0,
                                   0,
                                   GLUT_BITMAP_TIMES_ROMAN_24,
                                   0.6f,
                                   0.3f,
                                   0.4f);
        } else {
            if (img_inven[player_max_weapons + i]) {
                delete img_inven[player_max_weapons + i];
                delete remainings[player_max_weapons + i];
            }
            img_inven[player_max_weapons + i] = nullptr;
            remainings[player_max_weapons + i] = nullptr;
        }
    }
    // Place Images and remaining labels to the cells
    int index = 0;
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < player_max_weapons; c++) {
            if (img_inven[index] != nullptr) {
                inven_grid->placeImageToCell(img_inven[index], r, c);
                inven_grid->placeTextToCell(remainings[index], r, c);
            }
            index++;
        }
    }
    select_cell_row = 0;
    select_cell_col = 0;
    inven_grid->selectCell(select_cell_row, select_cell_col);
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

void Inventory::handleInventory(Player* current_player, int inven_index) {
    // IF PLAYER HAS SHOT WITH A SPECIAL WEAPON
    if (inven_index < player_max_weapons) {
        if (current_player->getLoadedWeapon() != nullptr) {
            if (current_player->getLoadedWeapon()->getRemaining() > 1) {
                current_player->getLoadedWeapon()->setRemaining(
                        current_player->getLoadedWeapon()->getRemaining() - 1);
            } else {
                int index;
                for (index = 0; index < player_max_weapons; index++) {
                    if (current_player->getCurrentWeapons()[index] ==
                        nullptr) {
                        // do nothing
                    } else if (current_player->getCurrentWeapons()[index]
                                       ->getUNIQUEIDENTIFIER() ==
                               current_player->getLoadedWeapon()
                                       ->getUNIQUEIDENTIFIER()) {
                        delete current_player->getLoadedWeapon();
                        current_player->getCurrentWeapons()[index] = nullptr;
                        current_player->setLoadedWeapon(nullptr);
                        break;
                    }
                }
            }
        }
    }
    // IF PLAYER HAS USED AN ITEM
    else {
        if (current_player->getCurrentItems()[inven_index - player_max_weapons]
                    ->getRemaining() > 1) {
            current_player->getCurrentItems()[inven_index - player_max_weapons]
                    ->setRemaining(
                            current_player
                                    ->getCurrentItems()[inven_index -
                                                        player_max_weapons]
                                    ->getRemaining() -
                            1);
        } else {
            delete current_player
                    ->getCurrentItems()[inven_index - player_max_weapons];
            current_player
                    ->getCurrentItems()[inven_index - player_max_weapons] =
                    nullptr;
        }
    }
}

int Inventory::getSelectedIndex() {
    return player_max_weapons * select_cell_row + select_cell_col;
}

void Inventory::keyHandler(int key) {
    // LEFT KEY
    if (key == 1 && select_cell_col > 0) {
        select_cell_col--;
        inven_grid->selectCell(select_cell_row, select_cell_col);
    }
    // UP KEY
    else if (key == 2 && select_cell_row > 0) {
        select_cell_row--;
        inven_grid->selectCell(select_cell_row, select_cell_col);
    }
    // RIGHT KEY
    else if (key == 3 && select_cell_col < player_max_weapons - 1) {
        select_cell_col++;
        inven_grid->selectCell(select_cell_row, select_cell_col);
    }
    // DOWN KEY
    else if (key == 4 && select_cell_row < 1) {
        select_cell_row++;
        inven_grid->selectCell(select_cell_row, select_cell_col);
    }

    // IF ONE OF WEAPONS IS SELECTED
    if (select_cell_row == 0) {
        if (weapons[select_cell_col] != nullptr) {
            delete descript;
            descript =
                    new TextObject(weapons[select_cell_col]->getDescription(),
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
        if (items[select_cell_col] != nullptr) {
            delete descript;
            descript = new TextObject(items[select_cell_col]->getDescription(),
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

    glViewport(x_pos, y_pos, width, height);
    gluPerspective(
            60.0,
            (static_cast<float>(width) / (1.5 * static_cast<float>(height))),
            1,
            199999999);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScissor(x_pos, y_pos, width, height);
    glClearColor(0.75, 0.75, 0.75, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int distance = static_cast<int>(glutGet(GLUT_WINDOW_HEIGHT) / 4 *
                                    tan(1.04719755));
    gluLookAt(0, 0, distance, 0, 0, 0, 0.0f, 1.0f, 0.0f);

    inven_grid->draw();
    title->draw();
    explain->draw();
    descript->draw();
    for (int i = 0; i < player_max_weapons + player_max_items; i++) {
        if (img_inven[i] != nullptr) {
            img_inven[i]->draw();
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
