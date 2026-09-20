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
#include "macro_crtdbg.h"
#include "possibleGameStates.h"
#include "sound.h"

extern void playMusic(int music);

using namespace std;

ReadyMenu::ReadyMenu() = default;
ReadyMenu::ReadyMenu(GLfloat width,
                     GLfloat height,
                     GLfloat percentBorder,
                     GlobalSettings* global_settings,
                     PlayerFactory* player_factory,
                     int* gameState) {
    startMusicPlayed = false;
    this->global_settings = global_settings;
    this->player_factory = player_factory;
    currentGameState = gameState;

    numPlayers = MAX_NUM_PLAYERS;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i] = nullptr;
    }
    for (int i = 0; i < NUM_STAT_IMAGES; i++) {
        statImages[i] = nullptr;
    }
    for (int i = 0; i < NUM_CONTROL_ITEMS; i++) {
        controlItems[i] = nullptr;
    }

    buttonPressed = nullptr;
    this->width = width;
    this->height = height;
    this->percentBorder = percentBorder;
    pos[0] = pos[1] = pos[2] = 0;
    color[0] = color[1] = color[2] = 1;
    color[3] = 1;
    tankPrvScrPos[0] = this->width * 0.07;
    tankPrvScrPos[1] = this->height * 0.40;
    tankPrvScrPos[2] = 0.1;
    tankPrvScrWidth = this->width * 0.35;
    tankPrvScrHeight = this->height * 0.45;
    tankPrvScrColor[0] = tankPrvScrColor[1] = tankPrvScrColor[2] = 0;
    prvScrColorControl = 1;

    caption = "Player 1";
    currentPlayerIndex = 0;

    /*BUTTONS AND CONTROL ITEMS*/
    buttons[0] = new MainMenuButton(
            0,
            pos[0] - this->width * 0.35,
            pos[1] + this->height * 0.35,
            1,
            this->player_factory->collectPlayerColor(currentPlayerIndex)[0],
            this->player_factory->collectPlayerColor(currentPlayerIndex)[1],
            this->player_factory->collectPlayerColor(currentPlayerIndex)[2],
            0.1 * (this->width),
            0.04 * (this->height),
            "CPU",
            nullptr);

    buttons[0]->pressButton();
    Mix_HaltChannel(0);
    buttons[0]->activateSubMenu();
    buttons[1] = new MainMenuButton(
            1,
            pos[0] - this->width * 0.2,
            pos[1] + this->height * 0.35,
            1,
            this->player_factory->collectPlayerColor(currentPlayerIndex)[0],
            this->player_factory->collectPlayerColor(currentPlayerIndex)[1],
            this->player_factory->collectPlayerColor(currentPlayerIndex)[2],
            0.1 * (this->width),
            0.04 * (this->height),
            "HUMAN",
            nullptr);
    buttons[2] = new MainMenuButton(2,
                                    pos[0] + this->width * 0.125,
                                    pos[1] - this->height * 0.35,
                                    1,
                                    0.65,
                                    0.15,
                                    0.15,
                                    0.1 * (this->width),
                                    0.04 * (this->height),
                                    "Back",
                                    nullptr);
    buttons[3] = new MainMenuButton(3,
                                    pos[0] + this->width * 0.275,
                                    pos[1] - this->height * 0.35,
                                    1,
                                    0.75,
                                    0.75,
                                    0.75,
                                    0.1 * (this->width),
                                    0.04 * (this->height),
                                    "Next",
                                    nullptr);
    controlItems[0] = new ControlItemSelectionBox(
            pos[0] - this->width * 0.375,
            pos[1] + this->height * 0.275,
            1,
            0.55,
            0.55,
            0.55,
            0.175 * (this->width),
            0.04 * (this->height),
            "AI",
            "Moron/Tosser/Cyborg/Shooter/Chooser/Poolshark/Spoiler/Unknown/");
    controlItems[1] = new ControlItemSliderbar(
            pos[0] - this->width * 0.35,
            pos[1] - this->height * 0.24,
            1,
            0.55,
            0.55,
            0.55,
            0.7 * (this->width),
            0.06 * (this->height),
            "Tank Type",
            "Rhinoxx/Hellfire/HeavyD/Panzer/Eggroid/Behemoth/Cubix/Predator/",
            0);
    controlItems[2] =
            new ControlItemSelectionBox(pos[0] - this->width * 0.175,
                                        pos[1] + this->height * 0.275,
                                        1,
                                        0.55,
                                        0.55,
                                        0.55,
                                        0.1 * (this->width),
                                        0.04 * (this->height),
                                        "Team",
                                        "-/1/2/3/4/5/");
    textField = new ControlItemTextField(pos[0] - this->width * 0.375,
                                         pos[1] + this->height * 0.275,
                                         1,
                                         1.0,
                                         1.0,
                                         1.0,
                                         0.175 * (this->width),
                                         0.04 * (this->height));

    /*LABEL PLACEMENT*/
    playerPageNum = new TextObject(caption,
                                   pos[0] - this->width * 0.25,
                                   pos[1] + this->height * 0.4,
                                   (pos[2] + 1),
                                   GLUT_BITMAP_TIMES_ROMAN_24,
                                   0.0f,
                                   0.0f,
                                   0.0f);

    for (int i = 0; i < NUM_TANK_STATS; i++) {
        std::string stat;
        if (i == 0)
            stat = "Power:";
        else if (i == 1)
            stat = "Armor:";
        else if (i == 2)
            stat = "Speed:";
        else
            stat = "Meh...:";
        GLfloat stat_label_x_pos = pos[0] - this->width * 0.385;
        GLfloat stat_label_y_pos =
                pos[1] + this->height * 0.12 - this->height * (i * 0.07);
        tankStatLabels[i] = new TextObject(stat,
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
    tankAngle = 0;

    for (int i = 0; i < NUM_TANK_TYPES; i++) {
        tanks[i]->setTankPos(0, 0, 0);
    }

    // STAT IMAGES
    GLfloat img_start_pos_x = pos[0] - this->width * 0.325;
    for (int i = 0; i < NUM_STAT_IMAGES; i++) {
        // For Off Lights
        if (i < 30) {
            // For Power Lights
            if (i < 10) {
                statImages[i] = new ImageObject(
                        img_start_pos_x + this->width * (i % 10) * 0.03,
                        pos[1] + height * 0.14 -
                                this->height * ((i / 10) * 0.07),
                        pos[2] + 0.5,
                        this->width * 0.03,
                        this->height * 0.032,
                        0.0006 * (this->width),
                        64,
                        64,
                        "lightOff.raw");
            }
            // For Armor Lights
            else if (i < 20) {
                statImages[i] = new ImageObject(
                        img_start_pos_x + this->width * (i % 10) * 0.03,
                        pos[1] + height * 0.14 -
                                this->height * ((i / 10) * 0.07),
                        pos[2] + 0.5,
                        this->width * 0.03,
                        this->height * 0.032,
                        0.0006 * (this->width),
                        64,
                        64,
                        "lightOff.raw");
            }
            // For Speed Lights
            else {
                statImages[i] = new ImageObject(
                        img_start_pos_x + this->width * (i % 10) * 0.03,
                        pos[1] + height * 0.14 -
                                this->height * ((i / 10) * 0.07),
                        pos[2] + 0.5,
                        this->width * 0.03,
                        this->height * 0.032,
                        0.0006 * (this->width),
                        64,
                        64,
                        "lightOff.raw");
            }
        }
        // For On Lights
        else {
            // For Power Lights
            if (i < 40) {
                statImages[i] = new ImageObject(
                        img_start_pos_x + this->width * (i % 10) * 0.03,
                        pos[1] + height * 0.14 -
                                this->height * (((i - 30) / 10) * 0.07),
                        pos[2] + 1,
                        this->width * 0.03,
                        this->height * 0.032,
                        0.0006 * (this->width),
                        64,
                        64,
                        "lightRed.raw");
            }
            // For Armor Lights
            else if (i < 50) {
                statImages[i] = new ImageObject(
                        img_start_pos_x + this->width * (i % 10) * 0.03,
                        pos[1] + height * 0.14 -
                                this->height * (((i - 30) / 10) * 0.07),
                        pos[2] + 1,
                        this->width * 0.03,
                        this->height * 0.032,
                        0.0006 * (this->width),
                        64,
                        64,
                        "lightBlue.raw");
            }
            // For Speed Lights
            else {
                statImages[i] = new ImageObject(
                        img_start_pos_x + this->width * (i % 10) * 0.03,
                        pos[1] + height * 0.14 -
                                this->height * (((i - 30) / 10) * 0.07),
                        pos[2] + 1,
                        this->width * 0.03,
                        this->height * 0.032,
                        0.0006 * (this->width),
                        64,
                        64,
                        "lightGreen.raw");
            }
        }
    }

    updatePageInfo();
}

ReadyMenu::~ReadyMenu() {
    for (int i = 0; i < NUM_BUTTONS; i++) delete buttons[i];
    for (int i = 0; i < NUM_STAT_IMAGES; i++) delete statImages[i];
    for (int i = 0; i < NUM_CONTROL_ITEMS; i++) delete controlItems[i];
    for (int i = 0; i < NUM_TANK_TYPES; i++) delete tanks[i];
    for (int i = 0; i < NUM_TANK_STATS; i++) delete tankStatLabels[i];
    delete textField;
    delete playerPageNum;
}

// GETTERS & SETTERS //
GLfloat* ReadyMenu::getPos() { return &(pos[0]); }
GLfloat ReadyMenu::getWidth() { return this->width; }
GLfloat ReadyMenu::getHeight() { return this->height; }
GLfloat* ReadyMenu::getColor() { return &(color[0]); }
void ReadyMenu::setWidth(GLfloat width) { this->width = width; }
void ReadyMenu::setHeight(GLfloat height) { this->height = height; }
void ReadyMenu::updateNumPlayers(int n) { numPlayers = n; }
void ReadyMenu::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
}

void ReadyMenu::saveCurrentPlayerData() {
    std::string aitype = controlItems[0]->collectData();
    std::string name = textField->collectData();
    std::string tank = controlItems[1]->collectData();
    char team_label = (controlItems[2]->collectData()[0]);

    if (buttons[0]->isActive()) {
        this->player_factory->updatePlayerBasicStrings(
                "CPU", aitype, "", team_label, tank, currentPlayerIndex);
    } else {
        this->player_factory->updatePlayerBasicStrings(
                "HUMAN", "", name, team_label, tank, currentPlayerIndex);
        textField->clearTextBuffer();
        textField->deactivate();
    }
}

void ReadyMenu::showPreviousPlayerPage() {
    if (currentPlayerIndex == 0) {
        Mix_FadeOutMusic(300);
        Mix_HaltMusic();
        *currentGameState = MAIN_MENU;
        startMusicPlayed = false;

    } else {
        if (currentPlayerIndex == 1) {
            buttons[2]->setColor(0.65, 0.15, 0.15);
            textField->setTextBuffer("");
        }
        buttons[3]->setLabel("Next");
        buttons[3]->setColor(0.75, 0.75, 0.75);
        currentPlayerIndex--;
        setPlayerPageNum(currentPlayerIndex);
        textField->setTextBuffer("");
        updatePageInfo();
    }
}

void ReadyMenu::showNextPlayerPage() {
    if (currentPlayerIndex + 1 == numPlayers) {
        Mix_FadeOutMusic(300);
        Mix_HaltMusic();
        *currentGameState = SHOP_MENU;
        textField->setTextBuffer("");
    } else {
        if (currentPlayerIndex + 1 == numPlayers - 1) {
            buttons[3]->setLabel("Done");
            buttons[3]->setColor(0.65, 0.15, 0.15);
        }
        buttons[2]->setColor(0.75, 0.75, 0.75);
        currentPlayerIndex++;
        setPlayerPageNum(currentPlayerIndex);
        textField->setTextBuffer("");
        updatePageInfo();
    }
}

void ReadyMenu::updatePageInfo() {
    for (int i = 0; i < NUM_TANK_TYPES; i++) {
        tanks[i]->changeHeadTexture(currentPlayerIndex);
    }
    std::string player_type =
            this->player_factory->getPlayer(currentPlayerIndex)
                    ->getPlayer_Type();

    // test to see if it's CPU
    if (player_type == "CPU") {
        buttons[0]->pressButton();
        Mix_HaltChannel(0);
        buttons[0]->activateSubMenu();
        buttons[1]->depressButton();
        buttons[1]->deactivateSubMenu();

        std::string ai_type =
                this->player_factory->getPlayer(currentPlayerIndex)
                        ->getAI_Type();
        int i = 0;
        while (controlItems[0]->collectData() != ai_type) {
            controlItems[0]->setOptionText(i);
            i++;
        }
    } else {
        std::string name = this->player_factory->getPlayer(currentPlayerIndex)
                                   ->getPlayerName();
        buttons[1]->pressButton();
        Mix_HaltChannel(0);
        buttons[1]->activateSubMenu();
        buttons[0]->depressButton();
        buttons[0]->deactivateSubMenu();

        textField->setTextBuffer(name);
    }

    if (this->player_factory->getPlayer(currentPlayerIndex)->getTeamLabel() ==
        '-') {
        controlItems[2]->setOptionText(0);
    } else {
        controlItems[2]->setOptionText(
                this->player_factory->getPlayer(currentPlayerIndex)
                        ->getTeamLabel() -
                48);
    }

    std::string tank_type =
            this->player_factory->getPlayer(currentPlayerIndex)->getTankType();
    int i = 0;
    while (controlItems[1]->collectData() != tank_type) {
        controlItems[1]->setOptionText(i);
        i++;
    }

    buttons[0]->setColor(
            this->player_factory->collectPlayerColor(currentPlayerIndex)[0],
            this->player_factory->collectPlayerColor(currentPlayerIndex)[1],
            this->player_factory->collectPlayerColor(currentPlayerIndex)[2]);
    buttons[1]->setColor(
            this->player_factory->collectPlayerColor(currentPlayerIndex)[0],
            this->player_factory->collectPlayerColor(currentPlayerIndex)[1],
            this->player_factory->collectPlayerColor(currentPlayerIndex)[2]);

    tankPrvScrColor[0] =
            this->player_factory->collectPlayerColor(currentPlayerIndex)[0];
    tankPrvScrColor[1] =
            this->player_factory->collectPlayerColor(currentPlayerIndex)[1];
    tankPrvScrColor[2] =
            this->player_factory->collectPlayerColor(currentPlayerIndex)[2];
}

void ReadyMenu::setPlayerPageNum(int i) {
    delete playerPageNum;
    caption = "Player " + std::to_string(i + 1);
    GLfloat label_x_pos = pos[0] - this->width * 0.25;
    GLfloat label_y_pos = pos[1] + this->height * 0.4;
    playerPageNum = new TextObject(caption,
                                   label_x_pos,
                                   label_y_pos,
                                   (pos[2] + 1),
                                   GLUT_BITMAP_TIMES_ROMAN_24,
                                   0.0f,
                                   0.0f,
                                   0.0f);
}

void ReadyMenu::buttonTest(int x, int y, int buttonDown) {
    if (buttonDown) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_BUTTONS;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF CLICK LANDS IN BUTTON I
            if (buttons[button_i]) {  // JUST TO MAKE SURE
                if ((x >= buttons[button_i]->getXPos()) &&
                    (x <= (buttons[button_i]->getXPos() +
                           buttons[button_i]->getWidth())) &&
                    (y <= buttons[button_i]->getYPos()) &&
                    (y >= (buttons[button_i]->getYPos() -
                           buttons[button_i]->getHeight()))) {
                    buttons[button_i]->pressButton();   // PRESS BUTTON
                    buttonPressed = buttons[button_i];  // KEEP TRACK OF WHICH
                                                        // BUTTON WAS PRESSED
                }
            }
        }
        for (int control_i = 0; control_i < NUM_CONTROL_ITEMS;
             control_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                             // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                             // CLICKED A ARROW BUTTON???
            if ((x >= controlItems[control_i]->getXPos()) &&
                (x <= (controlItems[control_i]->getXPos() +
                       controlItems[control_i]->getWidth())) &&
                (y <= controlItems[control_i]->getYPos()) &&
                (y >= (controlItems[control_i]->getYPos() -
                       controlItems[control_i]->getHeight()))) {
                controlItems[control_i]->mouseClickEvent(
                        x,
                        y,
                        buttonDown,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
            }
        }
        if ((x >= textField->getXPos()) &&
            (x <= (textField->getXPos() + textField->getWidth())) &&
            (y <= textField->getYPos()) &&
            (y >= (textField->getYPos() - textField->getHeight()))) {
            textField->mouseClickEvent(x, y, buttonDown, true);
        } else {
            textField->mouseClickEvent(x, y, buttonDown, false);
        }
    }

    else if (!buttonDown) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                             // GOES UP
        if (textField->isTextFieldActive()) textField->deactivate();
        if (buttonPressed !=
            nullptr) {  // IF THE LEFT CLICK WAS VALID AND INSIDE A BUTTON
                        // CHECK TO SEE IF YOU ARE STILL OVER SAME BUTTON
            if ((x >= buttonPressed->getXPos()) &&
                (x <=
                 (buttonPressed->getXPos() + buttonPressed->getWidth())) &&
                (y <= buttonPressed->getYPos()) &&
                (y >=
                 (buttonPressed->getYPos() - buttonPressed->getHeight()))) {
                if (buttonPressed->getUNIQUEIDENTIFIER() ==
                    0) {  // YOU CLICKED CPU TOGGLE BUTTON
                    buttons[1]->depressButton();
                    buttons[1]->deactivateSubMenu();
                    buttons[0]->activateSubMenu();
                    tanks[5]->getBaseHP();
                    buttonPressed = nullptr;
                } else if (buttonPressed->getUNIQUEIDENTIFIER() ==
                           1) {  // YOU CLICKED HUMAN TOGGLE BUTTON
                    buttons[0]->deactivateSubMenu();
                    buttons[0]->depressButton();
                    buttons[1]->activateSubMenu();
                    buttonPressed = nullptr;
                } else if (buttonPressed->getUNIQUEIDENTIFIER() ==
                           2) {  // YOU CLICKED BACK BUTTON
                    saveCurrentPlayerData();
                    showPreviousPlayerPage();
                    buttonPressed->depressButton();
                    buttonPressed = nullptr;
                } else if (buttonPressed->getUNIQUEIDENTIFIER() ==
                           3) {  // YOU CLICKED NEXT BUTTON
                    saveCurrentPlayerData();
                    showNextPlayerPage();
                    buttonPressed->depressButton();
                    buttonPressed = nullptr;
                }
            } else {  // IF YOU RELEASE OUTSIDE ALL BUTTONS
                if (!buttonPressed
                             ->isActive())  // IF THE BUTTON PRESSED IS NOT THE
                                            // BUTTON THAT'S TOGGLED
                {
                    buttonPressed->depressButton();
                    buttonPressed = nullptr;
                }
            }
        }
        for (int control_i = 0; control_i < NUM_CONTROL_ITEMS;
             control_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                             // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                             // CLICKED A ARROW BUTTON???
            if ((x >= controlItems[control_i]->getXPos()) &&
                (x <= (controlItems[control_i]->getXPos() +
                       controlItems[control_i]->getWidth())) &&
                (y <= controlItems[control_i]->getYPos()) &&
                (y >= (controlItems[control_i]->getYPos() -
                       controlItems[control_i]->getHeight()))) {
                if (control_i == 0 && buttons[control_i + 1]->isActive())
                    ;  // IF HUMAN BUTTON IS TOGGLED, SELECTION BUTTON SHOULD
                       // DO NOTHING
                else {
                    controlItems[control_i]->mouseClickEvent(
                            x, y, buttonDown, true);
                }
            } else {
                if (control_i == 0 && buttons[control_i + 1]->isActive())
                    ;  // IF HUMAN BUTTON IS TOGGLED, SELECTION BUTTON SHOULD
                       // DO NOTHING
                else {
                    controlItems[control_i]->mouseClickEvent(
                            x, y, buttonDown, false);
                }
            }
        }
        if ((x >= textField->getXPos()) &&
            (x <= (textField->getXPos() + textField->getWidth())) &&
            (y <= textField->getYPos()) &&
            (y >= (textField->getYPos() - textField->getHeight()))) {
            textField->mouseClickEvent(
                    x, y, buttonDown, true);  // YOU PRESSED OVER A ARROWBUTTON
        }
    }
}

void ReadyMenu::updateMouse(int x, int y) {
    controlItems[1]->updateMouse(x, y);
}

void ReadyMenu::draw() {
    if (!startMusicPlayed) {
        playMusic(READYMENU_START);
        startMusicPlayed = true;
    }
    playMusic(READYMENU_LOOP);
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

    // Draw tank preview screen
    glBegin(GL_QUADS);
    glColor4f(0.45, 0.45, 0.45, 1);
    glVertex3f(tankPrvScrPos[0], tankPrvScrPos[1], tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] - 6, tankPrvScrPos[1] + 6, tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] + tankPrvScrWidth + 6,
               tankPrvScrPos[1] + 6,
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] + tankPrvScrWidth,
               tankPrvScrPos[1],
               tankPrvScrPos[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.4, 0.4, 0.4, 1);
    glVertex3f(tankPrvScrPos[0] - 6, tankPrvScrPos[1] + 6, tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] - 6,
               tankPrvScrPos[1] - tankPrvScrHeight - 6,
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0],
               tankPrvScrPos[1] - tankPrvScrHeight,
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0], tankPrvScrPos[1], tankPrvScrPos[2]);
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
    glVertex3f(tankPrvScrPos[0] - 6,
               tankPrvScrPos[1] - tankPrvScrHeight - 6,
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] + tankPrvScrWidth + 6,
               tankPrvScrPos[1] - tankPrvScrHeight - 6,
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] + tankPrvScrWidth,
               tankPrvScrPos[1] - tankPrvScrHeight,
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0],
               tankPrvScrPos[1] - tankPrvScrHeight,
               tankPrvScrPos[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.85, 0.85, 0.85, 1);
    glVertex3f(tankPrvScrPos[0] + tankPrvScrWidth,
               tankPrvScrPos[1],
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] + tankPrvScrWidth + 6,
               tankPrvScrPos[1] + 6,
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] + tankPrvScrWidth + 6,
               tankPrvScrPos[1] - tankPrvScrHeight - 6,
               tankPrvScrPos[2]);
    glVertex3f(tankPrvScrPos[0] + tankPrvScrWidth,
               tankPrvScrPos[1] + -tankPrvScrHeight,
               tankPrvScrPos[2]);
    glEnd();

    for (int i = 0; i < NUM_TANK_STATS; i++) {
        tankStatLabels[i]->draw();
    }
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i]->draw();
    }
    if (buttons[0]->isActive()) {
        controlItems[0]->draw();
    } else {
        textField->draw();
    }
    controlItems[1]->draw();
    controlItems[2]->draw();
    playerPageNum->draw();

    // Flashing color effect in the tank preview screen
    if (0 <= tankPrvScrColor[0] &&
        tankPrvScrColor[0] <= this->player_factory->collectPlayerColor(
                                      currentPlayerIndex)[0] *
                                      1.12)
        tankPrvScrColor[0] +=
                (tankPrvScrColor[0] + 0.1) / 100 * prvScrColorControl;
    if (0 <= tankPrvScrColor[1] &&
        tankPrvScrColor[1] <= this->player_factory->collectPlayerColor(
                                      currentPlayerIndex)[1] *
                                      1.12)
        tankPrvScrColor[1] +=
                (tankPrvScrColor[1] + 0.1) / 100 * prvScrColorControl;
    if (0 <= tankPrvScrColor[2] &&
        tankPrvScrColor[2] <= this->player_factory->collectPlayerColor(
                                      currentPlayerIndex)[2] *
                                      1.12)
        tankPrvScrColor[2] +=
                (tankPrvScrColor[2] + 0.1) / 100 * prvScrColorControl;
    if (tankPrvScrColor[0] + tankPrvScrColor[1] + tankPrvScrColor[2] < 0) {
        tankPrvScrColor[0] = tankPrvScrColor[1] = tankPrvScrColor[2] = 0;
        prvScrColorControl = 1;
    }

    if (tankPrvScrColor[0] + tankPrvScrColor[1] + tankPrvScrColor[2] >
        (this->player_factory->collectPlayerColor(currentPlayerIndex)[0] +
         this->player_factory->collectPlayerColor(currentPlayerIndex)[1] +
         this->player_factory->collectPlayerColor(currentPlayerIndex)[2]) *
                1.12) {
        tankPrvScrColor[0] = this->player_factory->collectPlayerColor(
                                     currentPlayerIndex)[0] *
                             1.11;
        tankPrvScrColor[1] = this->player_factory->collectPlayerColor(
                                     currentPlayerIndex)[1] *
                             1.11;
        tankPrvScrColor[2] = this->player_factory->collectPlayerColor(
                                     currentPlayerIndex)[2] *
                             1.11;
        prvScrColorControl = -1;
    }
    // Draw Stat Images
    for (int i = 0; i < NUM_STAT_IMAGES; i++) {
        if (i < 30) {
            statImages[i]->draw();
        } else {
            if (i < 40) {
                if (controlItems[1]->collectData() == "Rhinoxx" &&
                    i - 30 < tanks[0]->getBasePower()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Hellfire" &&
                           i - 30 < tanks[1]->getBasePower()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "HeavyD" &&
                           i - 30 < tanks[2]->getBasePower()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Panzer" &&
                           i - 30 < tanks[3]->getBasePower()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Eggroid" &&
                           i - 30 < tanks[4]->getBasePower()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Behemoth" &&
                           i - 30 < tanks[5]->getBasePower()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Cubix" &&
                           i - 30 < tanks[6]->getBasePower()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Predator" &&
                           i - 30 < tanks[7]->getBasePower()) {
                    statImages[i]->draw();
                }
            } else if (i < 50) {
                if (controlItems[1]->collectData() == "Rhinoxx" &&
                    i - 40 < tanks[0]->getBaseArmor()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Hellfire" &&
                           i - 40 < tanks[1]->getBaseArmor()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "HeavyD" &&
                           i - 40 < tanks[2]->getBaseArmor()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Panzer" &&
                           i - 40 < tanks[3]->getBaseArmor()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Eggroid" &&
                           i - 40 < tanks[4]->getBaseArmor()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Behemoth" &&
                           i - 40 < tanks[5]->getBaseArmor()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Cubix" &&
                           i - 40 < tanks[6]->getBaseArmor()) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Predator" &&
                           i - 40 < tanks[7]->getBaseArmor()) {
                    statImages[i]->draw();
                }
            } else {
                if (controlItems[1]->collectData() == "Rhinoxx" &&
                    i - 50 < tanks[0]->getBaseSpeed() / 10) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Hellfire" &&
                           i - 50 < tanks[1]->getBaseSpeed() / 10) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "HeavyD" &&
                           i - 50 < tanks[2]->getBaseSpeed() / 10) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Panzer" &&
                           i - 50 < tanks[3]->getBaseSpeed() / 10) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Eggroid" &&
                           i - 50 < tanks[4]->getBaseSpeed() / 10) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Behemoth" &&
                           i - 50 < tanks[5]->getBaseSpeed() / 10) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Cubix" &&
                           i - 50 < tanks[6]->getBaseSpeed() / 10) {
                    statImages[i]->draw();
                } else if (controlItems[1]->collectData() == "Predator" &&
                           i - 50 < tanks[7]->getBaseSpeed() / 10) {
                    statImages[i]->draw();
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

    glViewport(tankPrvScrPos[0] + getWidth() / 2,
               tankPrvScrHeight + 1,
               tankPrvScrWidth,
               tankPrvScrHeight);
    gluPerspective(
            60.0,
            (static_cast<float>(width) / (1.5 * static_cast<float>(height))),
            1,
            199999999);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScissor(tankPrvScrPos[0] + getWidth() / 2,
              tankPrvScrHeight + 1,
              tankPrvScrWidth,
              tankPrvScrHeight);
    glClearColor(
            tankPrvScrColor[0], tankPrvScrColor[1], tankPrvScrColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // gluLookAt(	0,0,400,	0, 0, 0,		0.0f,1.0f,0.0f);
    gluLookAt(0, 200, 500, 0, 0, 0, 0.0f, 1.0f, 0.0f);

    // Draw Tanks
    glTranslatef(pos[0], pos[1] - tankPrvScrHeight * 0.2, pos[2]);
    glRotatef(tankAngle, 0, 1, 0);
    if (controlItems[1]->collectData() == "Rhinoxx")
        tanks[0]->draw();
    else if (controlItems[1]->collectData() == "Hellfire")
        tanks[1]->draw();
    else if (controlItems[1]->collectData() == "HeavyD")
        tanks[2]->draw();
    else if (controlItems[1]->collectData() == "Panzer")
        tanks[3]->draw();
    else if (controlItems[1]->collectData() == "Eggroid")
        tanks[4]->draw();
    else if (controlItems[1]->collectData() == "Behemoth")
        tanks[5]->draw();
    else if (controlItems[1]->collectData() == "Cubix")
        tanks[6]->draw();
    else if (controlItems[1]->collectData() == "Predator")
        tanks[7]->draw();
    else {
        printf("ERROR: Unkown tank type\n");
        glutSolidSphere(100, 30, 30);
    }

    tankAngle += 0.25f;
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
    if (textField->isTextFieldActive()) {
        textField->keyHandler(key);
    }
}