#include "MainMenu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "GlobalSettings.h"
#include "ImageObject.h"
#include "MainMenuButton.h"
#include "PlayerFactory.h"
#include "SubMenu.h"
#include "SubMenuEconomics.h"
#include "SubMenuHardware.h"
#include "SubMenuLandscape.h"
#include "SubMenuPhysics.h"
#include "SubMenuPlayOptions.h"
#include "SubMenuSelectTanks.h"
#include "SubMenuSound.h"
#include "SubMenuTest.h"
#include "SubMenuWeapons.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "possibleGameStates.h"
#include "sound.h"

extern void playMusic(int music);

MainMenu::MainMenu() = default;

MainMenu::MainMenu(GLfloat width,
                   GLfloat height,
                   GLfloat percent_border,
                   GlobalSettings* global_settings,
                   PlayerFactory* player_factory,
                   int* game_state) {
    this->global_settings = global_settings;
    this->player_factory = player_factory;
    currentGameState = game_state;

    for (int x = 0; x < NUM_BUTTON; x++) {
        buttons[x] = nullptr;
    }
    for (int x = 0; x < NUM_SUBMENUS; x++) {
        submenus[x] = nullptr;
    }
    for (int x = 0; x < NUM_IMAGES; x++) {
        images[x] = nullptr;
    }
    for (int x = 0; x < NUM_ARROW_BUTTONS; x++) {
        arrowsbutton[x] = nullptr;
    }
    buttonPressed = nullptr;
    activeSubMenu = nullptr;
    arrowButtonPressed = nullptr;
    this->width = width;
    this->height = height;
    this->percentBorder = percent_border;
    color[0] = color[1] = color[2] = 1;
    color[3] = 1;
    pos[0] = pos[1] = pos[2] = 0;

    /*	BUTTON 0 AND ITS SUBMENU	*/
    submenus[0] = new SubMenuSelectTanks(
            0,
            (-1 * this->width / 2.0 + .30 * this->width),
            ((this->height / 2.0) - .17 * this->height),
            3.0f,
            0.75f,
            0.75f,
            0.75f,
            0.65f * this->width,
            0.75 * this->height,
            "Select Your Tanks",
            .006);
    buttons[0] =
            new MainMenuButton(0,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .05 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Start",
                               submenus[0]);
    /*	ARROW BUTTON 1 NUMBER OF PLAYERS	*/
    arrowsbutton[0] = new ControlItemSelectionBox(
            -1 * this->width / 2.0 + .05 * this->width,
            ((this->height / 2.0) - .10 * this->height),
            1.0f,
            0.5f,
            0.5f,
            0.5f,
            0.2 * this->width,
            0.04 * (this->height),
            "# of Players",
            "2/3/4/5/6/7/8/9/10/");
    /*	ARROW BUTTON 2	NUMBER OF ROUNDS	*/
    arrowsbutton[1] = new ControlItemSelectionBox(
            -1 * this->width / 2.0 + .05 * this->width,
            ((this->height / 2.0) - .15 * this->height),
            1.0f,
            0.5f,
            0.5f,
            0.5f,
            0.2 * this->width,
            0.04 * (this->height),
            "# of Rounds",
            "1/2/3/4/5/6/7/8/9/");
    /*	BUTTON 1 AND ITS SUBMENU	*/
    submenus[1] =
            new SubMenuSound(1,
                             (-1 * this->width / 2.0 + .30 * this->width),
                             ((this->height / 2.0) - .17 * this->height),
                             3.0f,
                             0.75f,
                             0.75f,
                             0.75f,
                             0.65f * this->width,
                             0.75 * this->height,
                             "Sound Options Menu",
                             .006);
    buttons[1] =
            new MainMenuButton(1,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .20 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Sound Options",
                               submenus[1]);
    /*	BUTTON 2 AND ITS SUBMENU	*/
    submenus[2] =
            new SubMenuHardware(2,
                                (-1 * this->width / 2.0 + .30 * this->width),
                                ((this->height / 2.0) - .17 * this->height),
                                3.0f,
                                0.75f,
                                0.75f,
                                0.75f,
                                0.65f * this->width,
                                0.75 * this->height,
                                "Hardware Options Menu",
                                .006);
    buttons[2] =
            new MainMenuButton(2,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .25 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Hardware Options",
                               submenus[2]);
    /*	BUTTON 3 AND ITS SUBMENU	*/
    submenus[3] =
            new SubMenuEconomics(3,
                                 (-1 * this->width / 2.0 + .30 * this->width),
                                 ((this->height / 2.0) - .17 * this->height),
                                 3.0f,
                                 0.75f,
                                 0.75f,
                                 0.75f,
                                 0.65f * this->width,
                                 0.75 * this->height,
                                 "Economics Options Menu",
                                 .006);
    buttons[3] =
            new MainMenuButton(3,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .30 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Economics",
                               submenus[3]);
    /*	BUTTON 4 AND ITS SUBMENU	*/
    submenus[4] =
            new SubMenuPhysics(4,
                               (-1 * this->width / 2.0 + .30 * this->width),
                               ((this->height / 2.0) - .17 * this->height),
                               3.0f,
                               0.75f,
                               0.75f,
                               0.75f,
                               0.65f * this->width,
                               0.75 * this->height,
                               "Physics Options Menu",
                               .006);
    buttons[4] =
            new MainMenuButton(4,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .35 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Physics",
                               submenus[4]);
    /*	BUTTON 5 AND ITS SUBMENU	*/
    submenus[5] =
            new SubMenuLandscape(5,
                                 (-1 * this->width / 2.0 + .30 * this->width),
                                 ((this->height / 2.0) - .17 * this->height),
                                 3.0f,
                                 0.75f,
                                 0.75f,
                                 0.75f,
                                 0.65f * this->width,
                                 0.75 * this->height,
                                 "Landscape Options Menu",
                                 .006);
    buttons[5] =
            new MainMenuButton(5,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .40 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Landscape",
                               submenus[5]);
    /*	BUTTON 6 AND ITS SUBMENU	*/
    submenus[6] = new SubMenuPlayOptions(
            6,
            (-1 * this->width / 2.0 + .30 * this->width),
            ((this->height / 2.0) - .17 * this->height),
            3.0f,
            0.75f,
            0.75f,
            0.75f,
            0.65f * this->width,
            0.75 * this->height,
            "Play Settings Options Menu",
            .006);
    buttons[6] =
            new MainMenuButton(6,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .45 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Play Settings",
                               submenus[6]);
    /*	BUTTON 7 AND ITS SUBMENU	*/
    submenus[7] =
            new SubMenuWeapons(7,
                               (-1 * this->width / 2.0 + .30 * this->width),
                               ((this->height / 2.0) - .17 * this->height),
                               3.0f,
                               0.75f,
                               0.75f,
                               0.75f,
                               0.65f * this->width,
                               0.75 * this->height,
                               "Weapons Option Menu",
                               .006);
    buttons[7] =
            new MainMenuButton(7,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .50 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Weapons",
                               submenus[7]);
    /*	BUTTON 8 SAVE OPTIONS	*/
    buttons[8] =
            new MainMenuButton(8,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .55 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Save Settings",
                               nullptr);

    /*	QUIT BUTTON AND IMAGES	*/
    buttons[9] =
            new MainMenuButton(9,
                               (-1 * this->width / 2.0 + .05 * this->width),
                               ((this->height / 2.0) - .87 * this->height),
                               1.0f,
                               0.75f,
                               0.0f,
                               0.0f,
                               0.20f * (this->width),
                               0.04f * (this->height),
                               "Quit",
                               nullptr);
    images[0] = new ImageObject((-1 * this->width / 2.0 + .30 * this->width),
                                ((this->height / 2.0) - .17 * this->height),
                                2.0f,
                                0.65f * this->width,
                                0.75 * this->height,
                                .006 * (this->width),
                                1280,
                                1024,
                                "SplashScreen.raw");
    images[1] = new ImageObject((-1 * this->width / 2.0 + .30 * this->width),
                                ((this->height / 2.0) - .03 * this->height),
                                2.0f,
                                0.65f * this->width,
                                0.12 * this->height,
                                .006 * (this->width),
                                800,
                                150,
                                "vulkanEarthTitle.raw");
    global_settings->setCurrentTerrain(
            (static_cast<SubMenuLandscape*>(submenus[5]))->tm);
}

MainMenu::~MainMenu() {
    for (int i = 0; i < NUM_IMAGES; i++) delete images[i];
    for (int i = 0; i < NUM_BUTTON; i++) delete buttons[i];
    for (int i = 0; i < NUM_SUBMENUS; i++) delete submenus[i];
    for (int i = 0; i < NUM_ARROW_BUTTONS; i++) delete arrowsbutton[i];
}

GLfloat* MainMenu::getPos() { return &(pos[0]); }
GLfloat MainMenu::getHeight() { return this->width; }
void MainMenu::setHeight(GLfloat height) { this->height = height; }
GLfloat MainMenu::getWidth() { return this->height; }
void MainMenu::setWidth(GLfloat width) { this->width = width; }
GLfloat* MainMenu::getColor() { return &(color[0]); }
SubMenu* MainMenu::getSubMenuI(int i) { return submenus[i]; }
SubMenu* MainMenu::getActiveSubMenu() { return activeSubMenu; }
SubMenuLandscape* MainMenu::getSubMenuLandscape() {
    return static_cast<SubMenuLandscape*>(submenus[5]);
}

void MainMenu::draw() {
    playMusic(MAINMENU);
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

    for (int i = 0; i < NUM_IMAGES; i++) {
        if (images[i]) {
            images[i]->draw();
        }
    }
    for (int i = 0; i < NUM_ARROW_BUTTONS; i++) {
        if (arrowsbutton[i]) {
            arrowsbutton[i]->draw();
        }
    }
    for (int x = 0; x < NUM_BUTTON; x++) {
        if (buttons[x]) {
            buttons[x]->draw();
        }
    }

    glPopMatrix();
}

void MainMenu::buttonTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_BUTTON;
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
            if (button_i < NUM_ARROW_BUTTONS) {
                if (arrowsbutton[button_i]) {  // IF YOU DID NOT CLICK A BUTTON
                                               // PERHAPS YOU CLICKED A ARROW
                                               // BUTTON???
                    if ((x >= arrowsbutton[button_i]->getXPos()) &&
                        (x <= (arrowsbutton[button_i]->getXPos() +
                               arrowsbutton[button_i]->getWidth())) &&
                        (y <= arrowsbutton[button_i]->getYPos()) &&
                        (y >= (arrowsbutton[button_i]->getYPos() -
                               arrowsbutton[button_i]->getHeight()))) {
                        arrowsbutton[button_i]->mouseClickEvent(
                                x,
                                y,
                                button_down,
                                true);  // YOU PRESSED OVER A ARROWBUTTON
                        arrowButtonPressed = arrowsbutton[button_i];
                    }
                }
            }
        }
    } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                                // GOES UP
        if (buttonPressed !=
            nullptr) {  // IF THE LEFT CLICK WAS VALID AND INSIDE A BUTTON
                        // CHECK TO SEE IF YOU ARE STILL OVER SAME BUTTON
            if ((x >= buttonPressed->getXPos()) &&
                (x <=
                 (buttonPressed->getXPos() + buttonPressed->getWidth())) &&
                (y <= buttonPressed->getYPos()) &&
                (y >=
                 (buttonPressed->getYPos() - buttonPressed->getHeight()))) {
                if (buttonPressed
                            ->isActive()) {  // IF OVER SAME BUTTON MAKE SURE
                                             // IT WAS NOT ALREADY ACTIVATED
                                             // SOME BUTTONS HAVE EXTRA ACTIONS
                                             // LIKE SAVING DATA TO XML FILES
                    if (buttonPressed->getUNIQUEIDENTIFIER() == 0) {
                        collectData();  // ONE OF THE BUTTONS PRESSED WAS
                                        // START OR SAVE SETTINGS
                        this->player_factory->setNumberofPlayers(
                                this->global_settings->getPlayer_Count());
                        this->player_factory->initializePlayerDataBase();
                        this->global_settings->setCurrentTerrain(
                                getSubMenuLandscape()->tm);
                    }
                    if (buttonPressed->getUNIQUEIDENTIFIER() == 8) {
                        collectData();
                        this->player_factory->setNumberofPlayers(
                                this->global_settings->getPlayer_Count());
                    } else {
                        buttonPressed
                                ->deactivateSubMenu();  // IF SO DEACTIVATE
                                                        // SUBMENU ATTACHED TO
                                                        // BUTTON
                        activeSubMenu = nullptr;  // GET RID OF ACTIVE SUBMENU
                        buttonPressed
                                ->depressButton();  // DEPRESS THE BUTTON
                                                    // (CHANGE DRAWING MODE)
                    }
                } else {  // IF BUTTON WAS NOT ALREADY ACTIVATED
                          // SOME BUTTONS HAVE EXTRA ACTIONS LIKE SAVING DATA
                          // TO XML FILES
                    if (buttonPressed->getUNIQUEIDENTIFIER() ==
                        QUIT) {  // IF QUIT BUTTON PRESSED (QUIT DEFINED AT
                                 // VERY TOP)
                        buttonPressed
                                ->depressButton();  // DEPRESS THE QUIT BUTTON
                        *(currentGameState) = QUIT_GAME;
                    }
                    if (buttonPressed->getUNIQUEIDENTIFIER() == 0) {
                        collectData();  // ONE OF THE BUTTONS PRESSED WAS
                                        // START OR SAVE SETTINGS
                        this->player_factory->setNumberofPlayers(
                                this->global_settings->getPlayer_Count());
                        this->player_factory->initializePlayerDataBase();
                        this->global_settings->setCurrentTerrain(
                                getSubMenuLandscape()->tm);
                        Mix_HaltMusic();
                        *currentGameState = READY_MENU;
                    }
                    if (buttonPressed->getUNIQUEIDENTIFIER() == 8) {
                        collectData();
                    } else {
                        for (int i = 0; i < NUM_BUTTON;
                             i++) {            // DEACTIVATE ALL OTHER MENUS
                            if (buttons[i]) {  // SAFEGUARD AGAINST NULL
                                               // POINTER
                                buttons[i]->deactivateSubMenu();
                            }
                        }
                        if (buttonPressed->getUNIQUEIDENTIFIER() != 0) {
                            buttonPressed
                                    ->activateSubMenu();  // ACTIVATE THE
                                                          // SUBMENU ASSOCIATED
                                                          // WITH THE BUTTON
                                                          // PRESSED ABOVE
                            activeSubMenu =
                                    buttonPressed
                                            ->getSubMenu();  // SET THE ACTIVE
                                                             // SUBMENU
                        }
                    }
                }
                buttonPressed
                        ->depressButton();  // WHENEVER YOU RELEASE MOUSEBUTTON
                                            // DEPRESS THE BUTTON YOU MIGHT
                                            // HAVE PUSHED
                buttonPressed =
                        nullptr;  // YOU HANDLED THE BUTTON NOW CLEAR IT
            } else {              // IF YOU RELEASE OUTSIDE ALL BUTTONS
                buttonPressed->depressButton();  // DEPRESS BUTTON YOU
                                                 // MIGHT HAVE CLICKED
                buttonPressed = nullptr;         // NO BUTTON REALLY CLICKED
                                                 // (MUST REMAIN OVER BUTTON)

                // WTF, THIS CAUSED A BUG! WHY WAS IT THERE?!
                // activeSubMenu=NULL;
                // //GET RID OF ACTIVE SUBMENU JUST IN CASE (in case of what?)
            }
        } else if (arrowButtonPressed !=
                   nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW
                               // BUTTON
                               // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= arrowButtonPressed->getXPos()) &&
                (x <= (arrowButtonPressed->getXPos() +
                       arrowButtonPressed->getWidth())) &&
                (y <= arrowButtonPressed->getYPos()) &&
                (y >= (arrowButtonPressed->getYPos() -
                       arrowButtonPressed->getHeight()))) {
                arrowButtonPressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
            } else {
                arrowButtonPressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                arrowButtonPressed = nullptr;
            }
        }
    }
    // Sub Menu Button Test
    if (activeSubMenu != nullptr) {
        activeSubMenu->subMenuMouseTest(x, y, button_down);
    }
}

void MainMenu::collectData() {
    std::string optionsarray;
    for (int x = 2; x < NUM_SUBMENUS;
         x++) {  // SOUND AND START GAME ARE 1 AND 0 RESPECTIVLY
        if (submenus[x]) {
            optionsarray += submenus[x]->collectData();
        }
    }

    std::string playercount = "/Player Count/";
    if (arrowsbutton[0]) {
        playercount += arrowsbutton[0]->collectData();
        playercount += "/";
    }

    this->global_settings->setVariables(optionsarray, playercount);
}