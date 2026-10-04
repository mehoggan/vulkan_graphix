#include "vulkan_earth/ReadyMenu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/ControlItemSliderbar.h"
#include "vulkan_earth/ControlItemTextField.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/GlobalSettings.h"
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/MainMenu.h"
#include "vulkan_earth/MainMenuButton.h"
#include "vulkan_earth/Player.h"
#include "vulkan_earth/PlayerCPU.h"
#include "vulkan_earth/PlayerFactory.h"
#include "vulkan_earth/PlayerHuman.h"
#include "vulkan_earth/PossibleGameStates.h"
#include "vulkan_earth/ShopMenu.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/TankA.h"
#include "vulkan_earth/TankB.h"
#include "vulkan_earth/TankC.h"
#include "vulkan_earth/TankD.h"
#include "vulkan_earth/TankE.h"
#include "vulkan_earth/TankF.h"
#include "vulkan_earth/TankG.h"
#include "vulkan_earth/TankH.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Tools.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playMusic(std::int32_t music);

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

ReadyMenu::ReadyMenu() = default;
ReadyMenu::ReadyMenu(float new_width,
                     float new_height,
                     float new_percent_border,
                     GlobalSettings* new_global_settings,
                     PlayerFactory* new_player_factory,
                     std::int32_t* game_state) {
    start_music_played = false;
    global_settings = new_global_settings;
    player_factory = new_player_factory;
    current_game_state = game_state;

    num_players = max_num_players;
    for (std::int32_t i = 0; i < num_buttons; i++) {
        buttons[i] = nullptr;
    }
    for (std::int32_t i = 0; i < num_stat_images; i++) {
        stat_images[i] = nullptr;
    }
    for (std::int32_t i = 0; i < num_control_items; i++) {
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
                                     vulkan_earth::FontId::TimesRoman24,
                                     0.0f,
                                     0.0f,
                                     0.0f);

    for (std::int32_t i = 0; i < num_tank_stats; i++) {
        std::string stat;
        if (i == 0)
            stat = "Power:";
        else if (i == 1)
            stat = "Armor:";
        else if (i == 2)
            stat = "Speed:";
        else
            stat = "Meh...:";
        float stat_label_x_pos = pos[0] - width * 0.385;
        float stat_label_y_pos = pos[1] + height * 0.12 - height * (i * 0.07);
        tank_stat_labels[i] =
                new TextObject(stat,
                               stat_label_x_pos,
                               stat_label_y_pos,
                               (pos[2] + 1),
                               vulkan_earth::FontId::TimesRoman24,
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

    for (std::int32_t i = 0; i < num_tank_types; i++) {
        tanks[i]->setTankPos(0, 0, 0);
    }

    // STAT IMAGES
    float img_start_pos_x = pos[0] - width * 0.325;
    for (std::int32_t i = 0; i < num_stat_images; i++) {
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
    for (std::int32_t i = 0; i < num_buttons; i++) delete buttons[i];
    for (std::int32_t i = 0; i < num_stat_images; i++) delete stat_images[i];
    for (std::int32_t i = 0; i < num_control_items; i++)
        delete control_items[i];
    for (std::int32_t i = 0; i < num_tank_types; i++) delete tanks[i];
    for (std::int32_t i = 0; i < num_tank_stats; i++)
        delete tank_stat_labels[i];
    delete text_field;
    delete player_page_num;
}

// GETTERS & SETTERS //
float* ReadyMenu::getPos() { return &(pos[0]); }
float ReadyMenu::getWidth() { return width; }
float ReadyMenu::getHeight() { return height; }
float* ReadyMenu::getColor() { return &(color[0]); }
void ReadyMenu::setWidth(float new_width) { width = new_width; }
void ReadyMenu::setHeight(float new_height) { height = new_height; }
void ReadyMenu::updateNumPlayers(std::int32_t n) { num_players = n; }
void ReadyMenu::setColor(float r, float g, float b, float a) {
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
    for (std::int32_t i = 0; i < num_tank_types; i++) {
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
        std::int32_t i = 0;
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
    std::int32_t i = 0;
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

void ReadyMenu::setPlayerPageNum(std::int32_t i) {
    delete player_page_num;
    caption = "Player " + std::to_string(i + 1);
    float label_x_pos = pos[0] - width * 0.25;
    float label_y_pos = pos[1] + height * 0.4;
    player_page_num = new TextObject(caption,
                                     label_x_pos,
                                     label_y_pos,
                                     (pos[2] + 1),
                                     vulkan_earth::FontId::TimesRoman24,
                                     0.0f,
                                     0.0f,
                                     0.0f);
}

void ReadyMenu::buttonTest(std::int32_t x,
                           std::int32_t y,
                           std::int32_t button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (std::int32_t button_i = 0; button_i < num_buttons;
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
        for (std::int32_t control_i = 0; control_i < num_control_items;
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
        for (std::int32_t control_i = 0; control_i < num_control_items;
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

void ReadyMenu::updateMouse(std::int32_t x, std::int32_t y) {
    control_items[1]->updateMouse(x, y);
}

void ReadyMenu::draw(render::RenderContext& context) {
    if (!start_music_played) {
        playMusic(readymenu_start);
        start_music_played = true;
    }
    playMusic(readymenu_loop);
    using Vec3 = math::Vec3<float>;
    using Vec4 = math::Vec4<float>;
    // The whole-window background panel (see appendMenuPanel()), then the
    // tank preview screen's 6-pixel frame: top 0.45, left 0.4, bottom 0.8,
    // right 0.85 (its middle pane is the preview viewport drawn below).
    if (panel_mesh.triangles().empty() || built_width != width ||
        built_height != height) {
        panel_mesh.clear();
        vulkan_earth::appendMenuPanel(
                panel_mesh, width, height, percent_border);
        float const x = tank_prv_scr_pos[0];
        float const y = tank_prv_scr_pos[1];
        float const z = tank_prv_scr_pos[2];
        panel_mesh.addQuad({Vec3(x, y, z),
                            Vec3(x - 6, y + 6, z),
                            Vec3(x + tank_prv_scr_width + 6, y + 6, z),
                            Vec3(x + tank_prv_scr_width, y, z)},
                           Vec4(0.45, 0.45, 0.45, 1));
        panel_mesh.addQuad({Vec3(x - 6, y + 6, z),
                            Vec3(x - 6, y - tank_prv_scr_height - 6, z),
                            Vec3(x, y - tank_prv_scr_height, z),
                            Vec3(x, y, z)},
                           Vec4(0.4, 0.4, 0.4, 1));
        panel_mesh.addQuad(
                {Vec3(x - 6, y - tank_prv_scr_height - 6, z),
                 Vec3(x + tank_prv_scr_width + 6,
                      y - tank_prv_scr_height - 6,
                      z),
                 Vec3(x + tank_prv_scr_width, y - tank_prv_scr_height, z),
                 Vec3(x, y - tank_prv_scr_height, z)},
                Vec4(0.8, 0.8, 0.8, 1));
        panel_mesh.addQuad(
                {Vec3(x + tank_prv_scr_width, y, z),
                 Vec3(x + tank_prv_scr_width + 6, y + 6, z),
                 Vec3(x + tank_prv_scr_width + 6,
                      y - tank_prv_scr_height - 6,
                      z),
                 Vec3(x + tank_prv_scr_width, y + -tank_prv_scr_height, z)},
                Vec4(0.85, 0.85, 0.85, 1));
        built_width = width;
        built_height = height;
    }
    context.draw(panel_mesh);

    for (std::int32_t i = 0; i < num_tank_stats; i++) {
        tank_stat_labels[i]->draw(context);
    }
    for (std::int32_t i = 0; i < num_buttons; i++) {
        buttons[i]->draw(context);
    }
    if (buttons[0]->isActive()) {
        control_items[0]->draw(context);
    } else {
        text_field->draw(context);
    }
    control_items[1]->draw(context);
    control_items[2]->draw(context);
    player_page_num->draw(context);

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
    for (std::int32_t i = 0; i < num_stat_images; i++) {
        if (i < 30) {
            stat_images[i]->draw(context);
        } else {
            if (i < 40) {
                if (control_items[1]->collectData() == "Rhinoxx" &&
                    i - 30 < tanks[0]->getBasePower()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Hellfire" &&
                           i - 30 < tanks[1]->getBasePower()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "HeavyD" &&
                           i - 30 < tanks[2]->getBasePower()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Panzer" &&
                           i - 30 < tanks[3]->getBasePower()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Eggroid" &&
                           i - 30 < tanks[4]->getBasePower()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Behemoth" &&
                           i - 30 < tanks[5]->getBasePower()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Cubix" &&
                           i - 30 < tanks[6]->getBasePower()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Predator" &&
                           i - 30 < tanks[7]->getBasePower()) {
                    stat_images[i]->draw(context);
                }
            } else if (i < 50) {
                if (control_items[1]->collectData() == "Rhinoxx" &&
                    i - 40 < tanks[0]->getBaseArmor()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Hellfire" &&
                           i - 40 < tanks[1]->getBaseArmor()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "HeavyD" &&
                           i - 40 < tanks[2]->getBaseArmor()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Panzer" &&
                           i - 40 < tanks[3]->getBaseArmor()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Eggroid" &&
                           i - 40 < tanks[4]->getBaseArmor()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Behemoth" &&
                           i - 40 < tanks[5]->getBaseArmor()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Cubix" &&
                           i - 40 < tanks[6]->getBaseArmor()) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Predator" &&
                           i - 40 < tanks[7]->getBaseArmor()) {
                    stat_images[i]->draw(context);
                }
            } else {
                if (control_items[1]->collectData() == "Rhinoxx" &&
                    i - 50 < tanks[0]->getBaseSpeed() / 10) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Hellfire" &&
                           i - 50 < tanks[1]->getBaseSpeed() / 10) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "HeavyD" &&
                           i - 50 < tanks[2]->getBaseSpeed() / 10) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Panzer" &&
                           i - 50 < tanks[3]->getBaseSpeed() / 10) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Eggroid" &&
                           i - 50 < tanks[4]->getBaseSpeed() / 10) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Behemoth" &&
                           i - 50 < tanks[5]->getBaseSpeed() / 10) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Cubix" &&
                           i - 50 < tanks[6]->getBaseSpeed() / 10) {
                    stat_images[i]->draw(context);
                } else if (control_items[1]->collectData() == "Predator" &&
                           i - 50 < tanks[7]->getBaseSpeed() / 10) {
                    stat_images[i]->draw(context);
                }
            }
        }
    }

    // The live tank preview, in its own viewport (glViewport()'s float ->
    // int truncation kept), cleared to the flashing preview color.
    render::Rect const preview = vulkan_earth::glRect(
            static_cast<std::int32_t>(tank_prv_scr_pos[0] + getWidth() / 2),
            static_cast<std::int32_t>(tank_prv_scr_height + 1),
            static_cast<std::int32_t>(tank_prv_scr_width),
            static_cast<std::int32_t>(tank_prv_scr_height));
    context.setViewport(preview);
    context.clearColorAndDepth(Vec4(tank_prv_scr_color[0],
                                    tank_prv_scr_color[1],
                                    tank_prv_scr_color[2],
                                    1));

    // gluLookAt(	0,0,400,	0, 0, 0,		0.0f,1.0f,0.0f);
    math::Mat4<float> view = glm::lookAt(
            Vec3(0, 200, 500), Vec3(0, 0, 0), Vec3(0.0f, 1.0f, 0.0f));

    // Draw Tanks
    view = glm::translate(
            view,
            math::Vec3<float>(
                    pos[0], pos[1] - tank_prv_scr_height * 0.2, pos[2]));
    view = glm::rotate(view,
                       glm::radians(static_cast<float>(tank_angle)),
                       math::Vec3<float>(0, 1, 0));
    context.setCamera(vulkan_graphix::Tools::getPerspectiveProjectionMatrix(
                              (static_cast<float>(width) /
                               (1.5 * static_cast<float>(height))),
                              60.0,
                              1,
                              2.0e8f),
                      view);
    if (control_items[1]->collectData() == "Rhinoxx")
        tanks[0]->draw(context);
    else if (control_items[1]->collectData() == "Hellfire")
        tanks[1]->draw(context);
    else if (control_items[1]->collectData() == "HeavyD")
        tanks[2]->draw(context);
    else if (control_items[1]->collectData() == "Panzer")
        tanks[3]->draw(context);
    else if (control_items[1]->collectData() == "Eggroid")
        tanks[4]->draw(context);
    else if (control_items[1]->collectData() == "Behemoth")
        tanks[5]->draw(context);
    else if (control_items[1]->collectData() == "Cubix")
        tanks[6]->draw(context);
    else if (control_items[1]->collectData() == "Predator")
        tanks[7]->draw(context);
    else {
        printf("ERROR: Unkown tank type\n");
        // glutSolidSphere(100, 30, 30)
        context.drawMesh(render::Renderer::instance().sphere(30, 30),
                         vulkan_earth::pipelines().flat_color,
                         nullptr,
                         glm::scale(math::Mat4<float>(1.0f),
                                    math::Vec3<float>(100, 100, 100)),
                         Vec4(1, 1, 1, 1));
    }

    tank_angle += 0.25f;

    vulkan_earth::resetToFullWindow(context);
}

void ReadyMenu::keyTest(std::uint8_t key) {
    if (text_field->isTextFieldActive()) {
        text_field->keyHandler(key);
    }
}