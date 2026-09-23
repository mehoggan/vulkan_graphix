#include "ReadyMenu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "ControlItem.h"
#include "ControlItemSelectionBox.h"
#include "ControlItemSliderbar.h"
#include "ControlItemTextField.h"
#include "GlobalSettings.h"
#include "ImageObject.h"
#include "MainMenu.h"
#include "MainMenuButton.h"
#include "Player.h"
#include "PlayerCPU.h"
#include "PlayerFactory.h"
#include "PlayerHuman.h"
#include "ShopMenu.h"
#include "Tank.h"
#include "TankA.h"
#include "TankB.h"
#include "TankC.h"
#include "TankD.h"
#include "TankE.h"
#include "TankF.h"
#include "TankG.h"
#include "TankH.h"
#include "TerrainMaker.h"
#include "TextObject.h"
#include "Vertex.h"
#include "MacroCrtdbg.h"
#include "PossibleGameStates.h"
#include "Sound.h"

extern void playMusic(int music);

using namespace std;

ReadyMenu::ReadyMenu() = default;
ReadyMenu::ReadyMenu(GLfloat new_width,
                     GLfloat new_height,
                     GLfloat new_percent_border,
                     GlobalSettings* new_global_settings,
                     PlayerFactory* new_player_factory,
                     int* game_state) {
    start_music_played = false;
    global_settings = new_global_settings;
    player_factory = new_player_factory;
    current_game_state = game_state;

    num_players = max_num_players;
    for (int i = 0; i < num_buttons; i++) {
        buttons[i] = nullptr;
    }
    for (int i = 0; i < num_stat_images; i++) {
        stat_images[i] = nullptr;
    }
    for (int i = 0; i < num_control_items; i++) {
        control_items[i] = nullptr;
    }

    button_pressed = nullptr;
    width = new_width;
    height = new_height;
    percent_border = new_percent_border;
    pos[0] = pos[1] = pos[2] = 0;
    color[0] = color[1] = color[2] = 1;
    color[3] = 1;
    tank_prv_scr_pos[0] = width * 0.07;
    tank_prv_scr_pos[1] = height * 0.40;
    tank_prv_scr_pos[2] = 0.1;
    tank_prv_scr_width = width * 0.35;
    tank_prv_scr_height = height * 0.45;
    tank_prv_scr_color[0] = tank_prv_scr_color[1] = tank_prv_scr_color[2] = 0;
    prv_scr_color_control = 1;

    caption = "Player 1";
    current_player_index = 0;

    /*BUTTONS AND CONTROL ITEMS*/
    buttons[0] = new MainMenuButton(
            0,
            pos[0] - width * 0.35,
            pos[1] + height * 0.35,
            1,
            player_factory->collectPlayerColor(current_player_index)[0],
            player_factory->collectPlayerColor(current_player_index)[1],
            player_factory->collectPlayerColor(current_player_index)[2],
            0.1 * (width),
            0.04 * (height),
            "CPU",
            nullptr);

    buttons[0]->pressButton();
    Mix_HaltChannel(0);
    buttons[0]->activateSubMenu();
    buttons[1] = new MainMenuButton(
            1,
            pos[0] - width * 0.2,
            pos[1] + height * 0.35,
            1,
            player_factory->collectPlayerColor(current_player_index)[0],
            player_factory->collectPlayerColor(current_player_index)[1],
            player_factory->collectPlayerColor(current_player_index)[2],
            0.1 * (width),
            0.04 * (height),
            "HUMAN",
            nullptr);
    buttons[2] = new MainMenuButton(2,
                                    pos[0] + width * 0.125,
                                    pos[1] - height * 0.35,
                                    1,
                                    0.65,
                                    0.15,
                                    0.15,
                                    0.1 * (width),
                                    0.04 * (height),
                                    "Back",
                                    nullptr);
    buttons[3] = new MainMenuButton(3,
                                    pos[0] + width * 0.275,
                                    pos[1] - height * 0.35,
                                    1,
                                    0.75,
                                    0.75,
                                    0.75,
                                    0.1 * (width),
                                    0.04 * (height),
                                    "Next",
                                    nullptr);
    control_items[0] = new ControlItemSelectionBox(
            pos[0] - width * 0.375,
            pos[1] + height * 0.275,
            1,
            0.55,
            0.55,
            0.55,
            0.175 * (width),
            0.04 * (height),
            "AI",
            "Moron/Tosser/Cyborg/Shooter/Chooser/Poolshark/Spoiler/Unknown/");
    control_items[1] = new ControlItemSliderbar(
            pos[0] - width * 0.35,
            pos[1] - height * 0.24,
            1,
            0.55,
            0.55,
            0.55,
            0.7 * (width),
            0.06 * (height),
            "Tank Type",
            "Rhinoxx/Hellfire/HeavyD/Panzer/Eggroid/Behemoth/Cubix/Predator/",
            0);
    control_items[2] = new ControlItemSelectionBox(pos[0] - width * 0.175,
                                                   pos[1] + height * 0.275,
                                                   1,
                                                   0.55,
                                                   0.55,
                                                   0.55,
                                                   0.1 * (width),
                                                   0.04 * (height),
                                                   "Team",
                                                   "-/1/2/3/4/5/");
    text_field = new ControlItemTextField(pos[0] - width * 0.375,
                                          pos[1] + height * 0.275,
                                          1,
                                          1.0,
                                          1.0,
                                          1.0,
                                          0.175 * (width),
                                          0.04 * (height));

    /*LABEL PLACEMENT*/
    player_page_num = new TextObject(caption,
                                     pos[0] - width * 0.25,
                                     pos[1] + height * 0.4,
                                     (pos[2] + 1),
                                     GLUT_BITMAP_TIMES_ROMAN_24,
                                     0.0f,
                                     0.0f,
                                     0.0f);

    for (int i = 0; i < num_tank_stats; i++) {
        std::string stat;
        if (i == 0)
            stat = "Power:";
        else if (i == 1)
            stat = "Armor:";
        else if (i == 2)
            stat = "Speed:";
        else
            stat = "Meh...:";
        GLfloat stat_label_x_pos = pos[0] - width * 0.385;
        GLfloat stat_label_y_pos =
                pos[1] + height * 0.12 - height * (i * 0.07);
        tank_stat_labels[i] = new TextObject(stat,
                                             stat_label_x_pos,
                                             stat_label_y_pos,
                                             (pos[2] + 1),
                                             GLUT_BITMAP_TIMES_ROMAN_24,
                                             0.0f,
                                             0.0f,
                                             0.0f);
    }

    // TANKS
    tanks[0] = new TankA(0, 0, 0);
    tanks[1] = new TankB(0, 0, 0);
    tanks[2] = new TankC(0, 0, 0);
    tanks[3] = new TankD(0, 0, 0);
    tanks[4] = new TankE(0, 0, 0);
    tanks[5] = new TankF(0, 0, 0);
    tanks[6] = new TankG(0, 0, 0);
    tanks[7] = new TankH(0, 0, 0);
    tank_angle = 0;

    for (int i = 0; i < num_tank_types; i++) {
        tanks[i]->setTankPos(0, 0, 0);
    }

    // STAT IMAGES
    GLfloat img_start_pos_x = pos[0] - width * 0.325;
    for (int i = 0; i < num_stat_images; i++) {
        // For Off Lights
        if (i < 30) {
            // For Power Lights
            if (i < 10) {
                stat_images[i] = new ImageObject(
                        img_start_pos_x + width * (i % 10) * 0.03,
                        pos[1] + new_height * 0.14 -
                                height * ((i / 10) * 0.07),
                        pos[2] + 0.5,
                        width * 0.03,
                        height * 0.032,
                        0.0006 * (width),
                        64,
                        64,
                        "lightOff.raw");
            }
            // For Armor Lights
            else if (i < 20) {
                stat_images[i] = new ImageObject(
                        img_start_pos_x + width * (i % 10) * 0.03,
                        pos[1] + new_height * 0.14 -
                                height * ((i / 10) * 0.07),
                        pos[2] + 0.5,
                        width * 0.03,
                        height * 0.032,
                        0.0006 * (width),
                        64,
                        64,
                        "lightOff.raw");
            }
            // For Speed Lights
            else {
                stat_images[i] = new ImageObject(
                        img_start_pos_x + width * (i % 10) * 0.03,
                        pos[1] + new_height * 0.14 -
                                height * ((i / 10) * 0.07),
                        pos[2] + 0.5,
                        width * 0.03,
                        height * 0.032,
                        0.0006 * (width),
                        64,
                        64,
                        "lightOff.raw");
            }
        }
        // For On Lights
        else {
            // For Power Lights
            if (i < 40) {
                stat_images[i] = new ImageObject(
                        img_start_pos_x + width * (i % 10) * 0.03,
                        pos[1] + new_height * 0.14 -
                                height * (((i - 30) / 10) * 0.07),
                        pos[2] + 1,
                        width * 0.03,
                        height * 0.032,
                        0.0006 * (width),
                        64,
                        64,
                        "lightRed.raw");
            }
            // For Armor Lights
            else if (i < 50) {
                stat_images[i] = new ImageObject(
                        img_start_pos_x + width * (i % 10) * 0.03,
                        pos[1] + new_height * 0.14 -
                                height * (((i - 30) / 10) * 0.07),
                        pos[2] + 1,
                        width * 0.03,
                        height * 0.032,
                        0.0006 * (width),
                        64,
                        64,
                        "lightBlue.raw");
            }
            // For Speed Lights
            else {
                stat_images[i] = new ImageObject(
                        img_start_pos_x + width * (i % 10) * 0.03,
                        pos[1] + new_height * 0.14 -
                                height * (((i - 30) / 10) * 0.07),
                        pos[2] + 1,
                        width * 0.03,
                        height * 0.032,
                        0.0006 * (width),
                        64,
                        64,
                        "lightGreen.raw");
            }
        }
    }

    updatePageInfo();
}

ReadyMenu::~ReadyMenu() {
    for (int i = 0; i < num_buttons; i++) delete buttons[i];
    for (int i = 0; i < num_stat_images; i++) delete stat_images[i];
    for (int i = 0; i < num_control_items; i++) delete control_items[i];
    for (int i = 0; i < num_tank_types; i++) delete tanks[i];
    for (int i = 0; i < num_tank_stats; i++) delete tank_stat_labels[i];
    delete text_field;
    delete player_page_num;
}

// GETTERS & SETTERS //
GLfloat* ReadyMenu::getPos() { return &(pos[0]); }
GLfloat ReadyMenu::getWidth() { return width; }
GLfloat ReadyMenu::getHeight() { return height; }
GLfloat* ReadyMenu::getColor() { return &(color[0]); }
void ReadyMenu::setWidth(GLfloat new_width) { width = new_width; }
void ReadyMenu::setHeight(GLfloat new_height) { height = new_height; }
void ReadyMenu::updateNumPlayers(int n) { num_players = n; }
void ReadyMenu::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
}

void ReadyMenu::saveCurrentPlayerData() {
    std::string aitype = control_items[0]->collectData();
    std::string name = text_field->collectData();
    std::string tank = control_items[1]->collectData();
    char team_label = (control_items[2]->collectData()[0]);

    if (buttons[0]->isActive()) {
        player_factory->updatePlayerBasicStrings(
                "CPU", aitype, "", team_label, tank, current_player_index);
    } else {
        player_factory->updatePlayerBasicStrings(
                "HUMAN", "", name, team_label, tank, current_player_index);
        text_field->clearTextBuffer();
        text_field->deactivate();
    }
}

void ReadyMenu::showPreviousPlayerPage() {
    if (current_player_index == 0) {
        Mix_FadeOutMusic(300);
        Mix_HaltMusic();
        *current_game_state = MAIN_MENU;
        start_music_played = false;

    } else {
        if (current_player_index == 1) {
            buttons[2]->setColor(0.65, 0.15, 0.15);
            text_field->setTextBuffer("");
        }
        buttons[3]->setLabel("Next");
        buttons[3]->setColor(0.75, 0.75, 0.75);
        current_player_index--;
        setPlayerPageNum(current_player_index);
        text_field->setTextBuffer("");
        updatePageInfo();
    }
}

void ReadyMenu::showNextPlayerPage() {
    if (current_player_index + 1 == num_players) {
        Mix_FadeOutMusic(300);
        Mix_HaltMusic();
        *current_game_state = SHOP_MENU;
        text_field->setTextBuffer("");
    } else {
        if (current_player_index + 1 == num_players - 1) {
            buttons[3]->setLabel("Done");
            buttons[3]->setColor(0.65, 0.15, 0.15);
        }
        buttons[2]->setColor(0.75, 0.75, 0.75);
        current_player_index++;
        setPlayerPageNum(current_player_index);
        text_field->setTextBuffer("");
        updatePageInfo();
    }
}

void ReadyMenu::updatePageInfo() {
    for (int i = 0; i < num_tank_types; i++) {
        tanks[i]->changeHeadTexture(current_player_index);
    }
    std::string player_type =
            player_factory->getPlayer(current_player_index)->getPlayerType();

    // test to see if it's CPU
    if (player_type == "CPU") {
        buttons[0]->pressButton();
        Mix_HaltChannel(0);
        buttons[0]->activateSubMenu();
        buttons[1]->depressButton();
        buttons[1]->deactivateSubMenu();

        std::string ai_type =
                player_factory->getPlayer(current_player_index)->getAiType();
        int i = 0;
        while (control_items[0]->collectData() != ai_type) {
            control_items[0]->setOptionText(i);
            i++;
        }
    } else {
        std::string name = player_factory->getPlayer(current_player_index)
                                   ->getPlayerName();
        buttons[1]->pressButton();
        Mix_HaltChannel(0);
        buttons[1]->activateSubMenu();
        buttons[0]->depressButton();
        buttons[0]->deactivateSubMenu();

        text_field->setTextBuffer(name);
    }

    if (player_factory->getPlayer(current_player_index)->getTeamLabel() ==
        '-') {
        control_items[2]->setOptionText(0);
    } else {
        control_items[2]->setOptionText(
                player_factory->getPlayer(current_player_index)
                        ->getTeamLabel() -
                48);
    }

    std::string tank_type =
            player_factory->getPlayer(current_player_index)->getTankType();
    int i = 0;
    while (control_items[1]->collectData() != tank_type) {
        control_items[1]->setOptionText(i);
        i++;
    }

    buttons[0]->setColor(
            player_factory->collectPlayerColor(current_player_index)[0],
            player_factory->collectPlayerColor(current_player_index)[1],
            player_factory->collectPlayerColor(current_player_index)[2]);
    buttons[1]->setColor(
            player_factory->collectPlayerColor(current_player_index)[0],
            player_factory->collectPlayerColor(current_player_index)[1],
            player_factory->collectPlayerColor(current_player_index)[2]);

    tank_prv_scr_color[0] =
            player_factory->collectPlayerColor(current_player_index)[0];
    tank_prv_scr_color[1] =
            player_factory->collectPlayerColor(current_player_index)[1];
    tank_prv_scr_color[2] =
            player_factory->collectPlayerColor(current_player_index)[2];
}

void ReadyMenu::setPlayerPageNum(int i) {
    delete player_page_num;
    caption = "Player " + std::to_string(i + 1);
    GLfloat label_x_pos = pos[0] - width * 0.25;
    GLfloat label_y_pos = pos[1] + height * 0.4;
    player_page_num = new TextObject(caption,
                                     label_x_pos,
                                     label_y_pos,
                                     (pos[2] + 1),
                                     GLUT_BITMAP_TIMES_ROMAN_24,
                                     0.0f,
                                     0.0f,
                                     0.0f);
}

void ReadyMenu::buttonTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < num_buttons;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF CLICK LANDS IN BUTTON I
            if (buttons[button_i]) {  // JUST TO MAKE SURE
                if ((x >= buttons[button_i]->getXPos()) &&
                    (x <= (buttons[button_i]->getXPos() +
                           buttons[button_i]->getWidth())) &&
                    (y <= buttons[button_i]->getYPos()) &&
                    (y >= (buttons[button_i]->getYPos() -
                           buttons[button_i]->getHeight()))) {
                    buttons[button_i]->pressButton();    // PRESS BUTTON
                    button_pressed = buttons[button_i];  // KEEP TRACK OF WHICH
                                                         // BUTTON WAS PRESSED
                }
            }
        }
        for (int control_i = 0; control_i < num_control_items;
             control_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                             // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                             // CLICKED A ARROW BUTTON???
            if ((x >= control_items[control_i]->getXPos()) &&
                (x <= (control_items[control_i]->getXPos() +
                       control_items[control_i]->getWidth())) &&
                (y <= control_items[control_i]->getYPos()) &&
                (y >= (control_items[control_i]->getYPos() -
                       control_items[control_i]->getHeight()))) {
                control_items[control_i]->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
            }
        }
        if ((x >= text_field->getXPos()) &&
            (x <= (text_field->getXPos() + text_field->getWidth())) &&
            (y <= text_field->getYPos()) &&
            (y >= (text_field->getYPos() - text_field->getHeight()))) {
            text_field->mouseClickEvent(x, y, button_down, true);
        } else {
            text_field->mouseClickEvent(x, y, button_down, false);
        }
    }

    else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                              // GOES UP
        if (text_field->isTextFieldActive()) text_field->deactivate();
        if (button_pressed !=
            nullptr) {  // IF THE LEFT CLICK WAS VALID AND INSIDE A BUTTON
                        // CHECK TO SEE IF YOU ARE STILL OVER SAME BUTTON
            if ((x >= button_pressed->getXPos()) &&
                (x <=
                 (button_pressed->getXPos() + button_pressed->getWidth())) &&
                (y <= button_pressed->getYPos()) &&
                (y >=
                 (button_pressed->getYPos() - button_pressed->getHeight()))) {
                if (button_pressed->getUNIQUEIDENTIFIER() ==
                    0) {  // YOU CLICKED CPU TOGGLE BUTTON
                    buttons[1]->depressButton();
                    buttons[1]->deactivateSubMenu();
                    buttons[0]->activateSubMenu();
                    tanks[5]->getBaseHP();
                    button_pressed = nullptr;
                } else if (button_pressed->getUNIQUEIDENTIFIER() ==
                           1) {  // YOU CLICKED HUMAN TOGGLE BUTTON
                    buttons[0]->deactivateSubMenu();
                    buttons[0]->depressButton();
                    buttons[1]->activateSubMenu();
                    button_pressed = nullptr;
                } else if (button_pressed->getUNIQUEIDENTIFIER() ==
                           2) {  // YOU CLICKED BACK BUTTON
                    saveCurrentPlayerData();
                    showPreviousPlayerPage();
                    button_pressed->depressButton();
                    button_pressed = nullptr;
                } else if (button_pressed->getUNIQUEIDENTIFIER() ==
                           3) {  // YOU CLICKED NEXT BUTTON
                    saveCurrentPlayerData();
                    showNextPlayerPage();
                    button_pressed->depressButton();
                    button_pressed = nullptr;
                }
            } else {  // IF YOU RELEASE OUTSIDE ALL BUTTONS
                if (!button_pressed
                             ->isActive())  // IF THE BUTTON PRESSED IS NOT THE
                                            // BUTTON THAT'S TOGGLED
                {
                    button_pressed->depressButton();
                    button_pressed = nullptr;
                }
            }
        }
        for (int control_i = 0; control_i < num_control_items;
             control_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                             // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                             // CLICKED A ARROW BUTTON???
            if ((x >= control_items[control_i]->getXPos()) &&
                (x <= (control_items[control_i]->getXPos() +
                       control_items[control_i]->getWidth())) &&
                (y <= control_items[control_i]->getYPos()) &&
                (y >= (control_items[control_i]->getYPos() -
                       control_items[control_i]->getHeight()))) {
                if (control_i == 0 && buttons[control_i + 1]->isActive())
                    ;  // IF HUMAN BUTTON IS TOGGLED, SELECTION BUTTON SHOULD
                       // DO NOTHING
                else {
                    control_items[control_i]->mouseClickEvent(
                            x, y, button_down, true);
                }
            } else {
                if (control_i == 0 && buttons[control_i + 1]->isActive())
                    ;  // IF HUMAN BUTTON IS TOGGLED, SELECTION BUTTON SHOULD
                       // DO NOTHING
                else {
                    control_items[control_i]->mouseClickEvent(
                            x, y, button_down, false);
                }
            }
        }
        if ((x >= text_field->getXPos()) &&
            (x <= (text_field->getXPos() + text_field->getWidth())) &&
            (y <= text_field->getYPos()) &&
            (y >= (text_field->getYPos() - text_field->getHeight()))) {
            text_field->mouseClickEvent(
                    x,
                    y,
                    button_down,
                    true);  // YOU PRESSED OVER A ARROWBUTTON
        }
    }
}

void ReadyMenu::updateMouse(int x, int y) {
    control_items[1]->updateMouse(x, y);
}

void ReadyMenu::draw() {
    if (!start_music_played) {
        playMusic(readymenu_start);
        start_music_played = true;
    }
    playMusic(readymenu_loop);
    glPushMatrix();
    /*	ALWAYS START AT UPPER LEFT CORNER -> LOWER LEFT CORNER -> LOWER RIGHT
     * CORNER -> UPPER RIGHT	*/
    glBegin(GL_QUADS);
    glColor3f(0.85f, 0.85f, 0.85f);
    glVertex3f(-1 * (width / 2.0), (height / 2.0), 0);      /*	|\ 	*/
    glVertex3f(-1 * (width / 2.0), -1 * (height / 2.0), 0); /*	| |	*/
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0); /*	|/ 	*/
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.80f, 0.80f);
    glVertex3f(-1 * (width / 2.0), (height / 2.0), 0); /*_____ */
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0); /*\	  / */
    glVertex3f((width / 2.0) - (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0); /* ---	*/
    glVertex3f((width / 2.0), (height / 2.0), 0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.75f, 0.75f, 0.75f);
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0); /*_____ */
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0); /*|	  | */
    glVertex3f((width / 2.0) - (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0); /*----- */
    glVertex3f((width / 2.0) - (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.45f, 0.45f, 0.45f);
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0);                                          /* ___  */
    glVertex3f(-1 * (width / 2.0), -1 * (height / 2.0), 0); /*/	  \ */
    glVertex3f((width / 2.0), -1 * (height / 2.0), 0);      /*----- */
    glVertex3f((width / 2.0) - (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.40f, 0.40f, 0.40f);
    glVertex3f((width / 2.0) - (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0); /*	 /| 	*/
    glVertex3f((width / 2.0) - (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0);                                     /*	| | 	*/
    glVertex3f((width / 2.0), -1 * (height / 2.0), 0); /* 	 \| 	*/
    glVertex3f((width / 2.0), (height / 2.0), 0);
    glEnd();

    // Draw tank preview screen
    glBegin(GL_QUADS);
    glColor4f(0.45, 0.45, 0.45, 1);
    glVertex3f(tank_prv_scr_pos[0], tank_prv_scr_pos[1], tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] - 6,
               tank_prv_scr_pos[1] + 6,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] + tank_prv_scr_width + 6,
               tank_prv_scr_pos[1] + 6,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] + tank_prv_scr_width,
               tank_prv_scr_pos[1],
               tank_prv_scr_pos[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.4, 0.4, 0.4, 1);
    glVertex3f(tank_prv_scr_pos[0] - 6,
               tank_prv_scr_pos[1] + 6,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] - 6,
               tank_prv_scr_pos[1] - tank_prv_scr_height - 6,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0],
               tank_prv_scr_pos[1] - tank_prv_scr_height,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0], tank_prv_scr_pos[1], tank_prv_scr_pos[2]);
    glEnd();

    /* Middle pane replaced by viewport
    glBegin(GL_QUADS);
        glColor4f(tankPrvScrColor[0], tankPrvScrColor[1], tankPrvScrColor[2],
    1); glVertex3f(tankPrvScrPos[0], tankPrvScrPos[1],
    tankPrvScrPos[2]); glVertex3f(tankPrvScrPos[0],
    tankPrvScrPos[1]-tankPrvScrHeight, tankPrvScrPos[2]);
        glVertex3f(tankPrvScrPos[0]+tankPrvScrWidth,
    tankPrvScrPos[1]-tankPrvScrHeight, tankPrvScrPos[2]);
        glVertex3f(tankPrvScrPos[0]+tankPrvScrWidth,
    tankPrvScrPos[1], tankPrvScrPos[2]); glEnd();*/

    glBegin(GL_QUADS);
    glColor4f(0.8, 0.8, 0.8, 1);
    glVertex3f(tank_prv_scr_pos[0] - 6,
               tank_prv_scr_pos[1] - tank_prv_scr_height - 6,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] + tank_prv_scr_width + 6,
               tank_prv_scr_pos[1] - tank_prv_scr_height - 6,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] + tank_prv_scr_width,
               tank_prv_scr_pos[1] - tank_prv_scr_height,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0],
               tank_prv_scr_pos[1] - tank_prv_scr_height,
               tank_prv_scr_pos[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.85, 0.85, 0.85, 1);
    glVertex3f(tank_prv_scr_pos[0] + tank_prv_scr_width,
               tank_prv_scr_pos[1],
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] + tank_prv_scr_width + 6,
               tank_prv_scr_pos[1] + 6,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] + tank_prv_scr_width + 6,
               tank_prv_scr_pos[1] - tank_prv_scr_height - 6,
               tank_prv_scr_pos[2]);
    glVertex3f(tank_prv_scr_pos[0] + tank_prv_scr_width,
               tank_prv_scr_pos[1] + -tank_prv_scr_height,
               tank_prv_scr_pos[2]);
    glEnd();

    for (int i = 0; i < num_tank_stats; i++) {
        tank_stat_labels[i]->draw();
    }
    for (int i = 0; i < num_buttons; i++) {
        buttons[i]->draw();
    }
    if (buttons[0]->isActive()) {
        control_items[0]->draw();
    } else {
        text_field->draw();
    }
    control_items[1]->draw();
    control_items[2]->draw();
    player_page_num->draw();

    // Flashing color effect in the tank preview screen
    if (0 <= tank_prv_scr_color[0] &&
        tank_prv_scr_color[0] <=
                player_factory->collectPlayerColor(current_player_index)[0] *
                        1.12)
        tank_prv_scr_color[0] +=
                (tank_prv_scr_color[0] + 0.1) / 100 * prv_scr_color_control;
    if (0 <= tank_prv_scr_color[1] &&
        tank_prv_scr_color[1] <=
                player_factory->collectPlayerColor(current_player_index)[1] *
                        1.12)
        tank_prv_scr_color[1] +=
                (tank_prv_scr_color[1] + 0.1) / 100 * prv_scr_color_control;
    if (0 <= tank_prv_scr_color[2] &&
        tank_prv_scr_color[2] <=
                player_factory->collectPlayerColor(current_player_index)[2] *
                        1.12)
        tank_prv_scr_color[2] +=
                (tank_prv_scr_color[2] + 0.1) / 100 * prv_scr_color_control;
    if (tank_prv_scr_color[0] + tank_prv_scr_color[1] + tank_prv_scr_color[2] <
        0) {
        tank_prv_scr_color[0] = tank_prv_scr_color[1] = tank_prv_scr_color[2] =
                0;
        prv_scr_color_control = 1;
    }

    if (tank_prv_scr_color[0] + tank_prv_scr_color[1] + tank_prv_scr_color[2] >
        (player_factory->collectPlayerColor(current_player_index)[0] +
         player_factory->collectPlayerColor(current_player_index)[1] +
         player_factory->collectPlayerColor(current_player_index)[2]) *
                1.12) {
        tank_prv_scr_color[0] =
                player_factory->collectPlayerColor(current_player_index)[0] *
                1.11;
        tank_prv_scr_color[1] =
                player_factory->collectPlayerColor(current_player_index)[1] *
                1.11;
        tank_prv_scr_color[2] =
                player_factory->collectPlayerColor(current_player_index)[2] *
                1.11;
        prv_scr_color_control = -1;
    }
    // Draw Stat Images
    for (int i = 0; i < num_stat_images; i++) {
        if (i < 30) {
            stat_images[i]->draw();
        } else {
            if (i < 40) {
                if (control_items[1]->collectData() == "Rhinoxx" &&
                    i - 30 < tanks[0]->getBasePower()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Hellfire" &&
                           i - 30 < tanks[1]->getBasePower()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "HeavyD" &&
                           i - 30 < tanks[2]->getBasePower()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Panzer" &&
                           i - 30 < tanks[3]->getBasePower()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Eggroid" &&
                           i - 30 < tanks[4]->getBasePower()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Behemoth" &&
                           i - 30 < tanks[5]->getBasePower()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Cubix" &&
                           i - 30 < tanks[6]->getBasePower()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Predator" &&
                           i - 30 < tanks[7]->getBasePower()) {
                    stat_images[i]->draw();
                }
            } else if (i < 50) {
                if (control_items[1]->collectData() == "Rhinoxx" &&
                    i - 40 < tanks[0]->getBaseArmor()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Hellfire" &&
                           i - 40 < tanks[1]->getBaseArmor()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "HeavyD" &&
                           i - 40 < tanks[2]->getBaseArmor()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Panzer" &&
                           i - 40 < tanks[3]->getBaseArmor()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Eggroid" &&
                           i - 40 < tanks[4]->getBaseArmor()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Behemoth" &&
                           i - 40 < tanks[5]->getBaseArmor()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Cubix" &&
                           i - 40 < tanks[6]->getBaseArmor()) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Predator" &&
                           i - 40 < tanks[7]->getBaseArmor()) {
                    stat_images[i]->draw();
                }
            } else {
                if (control_items[1]->collectData() == "Rhinoxx" &&
                    i - 50 < tanks[0]->getBaseSpeed() / 10) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Hellfire" &&
                           i - 50 < tanks[1]->getBaseSpeed() / 10) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "HeavyD" &&
                           i - 50 < tanks[2]->getBaseSpeed() / 10) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Panzer" &&
                           i - 50 < tanks[3]->getBaseSpeed() / 10) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Eggroid" &&
                           i - 50 < tanks[4]->getBaseSpeed() / 10) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Behemoth" &&
                           i - 50 < tanks[5]->getBaseSpeed() / 10) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Cubix" &&
                           i - 50 < tanks[6]->getBaseSpeed() / 10) {
                    stat_images[i]->draw();
                } else if (control_items[1]->collectData() == "Predator" &&
                           i - 50 < tanks[7]->getBaseSpeed() / 10) {
                    stat_images[i]->draw();
                }
            }
        }
    }

    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(tank_prv_scr_pos[0] + getWidth() / 2,
               tank_prv_scr_height + 1,
               tank_prv_scr_width,
               tank_prv_scr_height);
    gluPerspective(
            60.0,
            (static_cast<float>(width) / (1.5 * static_cast<float>(height))),
            1,
            199999999);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScissor(tank_prv_scr_pos[0] + getWidth() / 2,
              tank_prv_scr_height + 1,
              tank_prv_scr_width,
              tank_prv_scr_height);
    glClearColor(tank_prv_scr_color[0],
                 tank_prv_scr_color[1],
                 tank_prv_scr_color[2],
                 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // gluLookAt(	0,0,400,	0, 0, 0,		0.0f,1.0f,0.0f);
    gluLookAt(0, 200, 500, 0, 0, 0, 0.0f, 1.0f, 0.0f);

    // Draw Tanks
    glTranslatef(pos[0], pos[1] - tank_prv_scr_height * 0.2, pos[2]);
    glRotatef(tank_angle, 0, 1, 0);
    if (control_items[1]->collectData() == "Rhinoxx")
        tanks[0]->draw();
    else if (control_items[1]->collectData() == "Hellfire")
        tanks[1]->draw();
    else if (control_items[1]->collectData() == "HeavyD")
        tanks[2]->draw();
    else if (control_items[1]->collectData() == "Panzer")
        tanks[3]->draw();
    else if (control_items[1]->collectData() == "Eggroid")
        tanks[4]->draw();
    else if (control_items[1]->collectData() == "Behemoth")
        tanks[5]->draw();
    else if (control_items[1]->collectData() == "Cubix")
        tanks[6]->draw();
    else if (control_items[1]->collectData() == "Predator")
        tanks[7]->draw();
    else {
        printf("ERROR: Unkown tank type\n");
        glutSolidSphere(100, 30, 30);
    }

    tank_angle += 0.25f;
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT1);
    glEnable(GL_LIGHT0);
    glPopMatrix();

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

void ReadyMenu::keyTest(unsigned char key) {
    if (text_field->isTextFieldActive()) {
        text_field->keyHandler(key);
    }
}