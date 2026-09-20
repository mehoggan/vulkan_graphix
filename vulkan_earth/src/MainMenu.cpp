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
                   GLfloat percentBorder,
                   GlobalSettings* global_settings,
                   PlayerFactory* player_factory,
                   int* gameState) {
    this->global_settings = global_settings;
    this->player_factory = player_factory;
    this->currentGameState = gameState;

    for (int x = 0; x < NUM_BUTTON; x++) {
        this->buttons[x] = nullptr;
    }
    for (int x = 0; x < NUM_SUBMENUS; x++) {
        this->submenus[x] = nullptr;
    }
    for (int x = 0; x < NUM_IMAGES; x++) {
        this->images[x] = nullptr;
    }
    for (int x = 0; x < NUM_ARROW_BUTTONS; x++) {
        this->arrowsbutton[x] = nullptr;
    }
    this->buttonPressed = nullptr;
    this->activeSubMenu = nullptr;
    this->arrowButtonPressed = nullptr;
    this->width = width;
    this->height = height;
    this->percentBorder = percentBorder;
    this->color[0] = this->color[1] = this->color[2] = 1;
    this->color[3] = 1;
    this->pos[0] = this->pos[1] = this->pos[2] = 0;

    /*	BUTTON 0 AND ITS SUBMENU	*/
    this->submenus[0] = new SubMenuSelectTanks(
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
    this->buttons[0] =
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
                               this->submenus[0]);
    /*	ARROW BUTTON 1 NUMBER OF PLAYERS	*/
    this->arrowsbutton[0] = new ControlItemSelectionBox(
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
    this->arrowsbutton[1] = new ControlItemSelectionBox(
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
    this->submenus[1] =
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
    this->buttons[1] =
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
                               this->submenus[1]);
    /*	BUTTON 2 AND ITS SUBMENU	*/
    this->submenus[2] =
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
    this->buttons[2] =
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
                               this->submenus[2]);
    /*	BUTTON 3 AND ITS SUBMENU	*/
    this->submenus[3] =
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
    this->buttons[3] =
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
                               this->submenus[3]);
    /*	BUTTON 4 AND ITS SUBMENU	*/
    this->submenus[4] =
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
    this->buttons[4] =
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
                               this->submenus[4]);
    /*	BUTTON 5 AND ITS SUBMENU	*/
    this->submenus[5] =
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
    this->buttons[5] =
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
                               this->submenus[5]);
    /*	BUTTON 6 AND ITS SUBMENU	*/
    this->submenus[6] = new SubMenuPlayOptions(
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
    this->buttons[6] =
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
                               this->submenus[6]);
    /*	BUTTON 7 AND ITS SUBMENU	*/
    this->submenus[7] =
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
    this->buttons[7] =
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
                               this->submenus[7]);
    /*	BUTTON 8 SAVE OPTIONS	*/
    this->buttons[8] =
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
    this->buttons[9] =
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
    this->images[0] =
            new ImageObject((-1 * this->width / 2.0 + .30 * this->width),
                            ((this->height / 2.0) - .17 * this->height),
                            2.0f,
                            0.65f * this->width,
                            0.75 * this->height,
                            .006 * (this->width),
                            1280,
                            1024,
                            "SplashScreen.raw");
    this->images[1] =
            new ImageObject((-1 * this->width / 2.0 + .30 * this->width),
                            ((this->height / 2.0) - .03 * this->height),
                            2.0f,
                            0.65f * this->width,
                            0.12 * this->height,
                            .006 * (this->width),
                            800,
                            150,
                            "vulkanEarthTitle.raw");
    global_settings->setCurrentTerrain(
            (static_cast<SubMenuLandscape*>(this->submenus[5]))->tm);
}

MainMenu::~MainMenu() {
    for (int i = 0; i < NUM_IMAGES; i++) delete images[i];
    for (int i = 0; i < NUM_BUTTON; i++) delete buttons[i];
    for (int i = 0; i < NUM_SUBMENUS; i++) delete submenus[i];
    for (int i = 0; i < NUM_ARROW_BUTTONS; i++) delete this->arrowsbutton[i];
}

GLfloat* MainMenu::getPos() { return &(this->pos[0]); }
GLfloat MainMenu::getHeight() { return this->width; }
void MainMenu::setHeight(GLfloat height) { this->height = height; }
GLfloat MainMenu::getWidth() { return this->height; }
void MainMenu::setWidth(GLfloat width) { this->width = width; }
GLfloat* MainMenu::getColor() { return &(this->color[0]); }
SubMenu* MainMenu::getSubMenuI(int I) { return this->submenus[I]; }
SubMenu* MainMenu::getActiveSubMenu() { return this->activeSubMenu; }
SubMenuLandscape* MainMenu::getSubMenuLandscape() {
    return static_cast<SubMenuLandscape*>(this->submenus[5]);
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
        if (this->images[i]) {
            this->images[i]->draw();
        }
    }
    for (int i = 0; i < NUM_ARROW_BUTTONS; i++) {
        if (this->arrowsbutton[i]) {
            this->arrowsbutton[i]->draw();
        }
    }
    for (int x = 0; x < NUM_BUTTON; x++) {
        if (this->buttons[x]) {
            this->buttons[x]->draw();
        }
    }

    glPopMatrix();
}

void MainMenu::buttonTest(int x, int y, int buttonDown) {
    if (buttonDown) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_BUTTON;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF CLICK LANDS IN BUTTON I
            if (buttons[button_i]) {  // JUST TO MAKE SURE
                if ((x >= this->buttons[button_i]->getXPos()) &&
                    (x <= (this->buttons[button_i]->getXPos() +
                           this->buttons[button_i]->getWidth())) &&
                    (y <= this->buttons[button_i]->getYPos()) &&
                    (y >= (this->buttons[button_i]->getYPos() -
                           this->buttons[button_i]->getHeight()))) {
                    buttons[button_i]->pressButton();  // PRESS BUTTON
                    this->buttonPressed =
                            buttons[button_i];  // KEEP TRACK OF WHICH BUTTON
                                                // WAS PRESSED
                }
            }
            if (button_i < NUM_ARROW_BUTTONS) {
                if (arrowsbutton[button_i]) {  // IF YOU DID NOT CLICK A BUTTON
                                               // PERHAPS YOU CLICKED A ARROW
                                               // BUTTON???
                    if ((x >= this->arrowsbutton[button_i]->getXPos()) &&
                        (x <= (this->arrowsbutton[button_i]->getXPos() +
                               this->arrowsbutton[button_i]->getWidth())) &&
                        (y <= this->arrowsbutton[button_i]->getYPos()) &&
                        (y >= (this->arrowsbutton[button_i]->getYPos() -
                               this->arrowsbutton[button_i]->getHeight()))) {
                        arrowsbutton[button_i]->mouseClickEvent(
                                x,
                                y,
                                buttonDown,
                                true);  // YOU PRESSED OVER A ARROWBUTTON
                        this->arrowButtonPressed = arrowsbutton[button_i];
                    }
                }
            }
        }
    } else if (!buttonDown) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                               // GOES UP
        if (this->buttonPressed !=
            nullptr) {  // IF THE LEFT CLICK WAS VALID AND INSIDE A BUTTON
                        // CHECK TO SEE IF YOU ARE STILL OVER SAME BUTTON
            if ((x >= this->buttonPressed->getXPos()) &&
                (x <= (this->buttonPressed->getXPos() +
                       this->buttonPressed->getWidth())) &&
                (y <= this->buttonPressed->getYPos()) &&
                (y >= (this->buttonPressed->getYPos() -
                       this->buttonPressed->getHeight()))) {
                if (this->buttonPressed
                            ->isActive()) {  // IF OVER SAME BUTTON MAKE SURE
                                             // IT WAS NOT ALREADY ACTIVATED
                                             // SOME BUTTONS HAVE EXTRA ACTIONS
                                             // LIKE SAVING DATA TO XML FILES
                    if (this->buttonPressed->getUNIQUEIDENTIFIER() == 0) {
                        this->collectData();  // ONE OF THE BUTTONS PRESSED WAS
                                              // START OR SAVE SETTINGS
                        this->player_factory->setNumberofPlayers(
                                this->global_settings->getPlayer_Count());
                        this->player_factory->initializePlayerDataBase();
                        this->global_settings->setCurrentTerrain(
                                this->getSubMenuLandscape()->tm);
                    }
                    if (this->buttonPressed->getUNIQUEIDENTIFIER() == 8) {
                        this->collectData();
                        this->player_factory->setNumberofPlayers(
                                this->global_settings->getPlayer_Count());
                    } else {
                        this->buttonPressed
                                ->deactivateSubMenu();  // IF SO DEACTIVATE
                                                        // SUBMENU ATTACHED TO
                                                        // BUTTON
                        this->activeSubMenu =
                                nullptr;  // GET RID OF ACTIVE SUBMENU
                        this->buttonPressed
                                ->depressButton();  // DEPRESS THE BUTTON
                                                    // (CHANGE DRAWING MODE)
                    }
                } else {  // IF BUTTON WAS NOT ALREADY ACTIVATED
                          // SOME BUTTONS HAVE EXTRA ACTIONS LIKE SAVING DATA
                          // TO XML FILES
                    if (this->buttonPressed->getUNIQUEIDENTIFIER() ==
                        QUIT) {  // IF QUIT BUTTON PRESSED (QUIT DEFINED AT
                                 // VERY TOP)
                        this->buttonPressed
                                ->depressButton();  // DEPRESS THE QUIT BUTTON
                        *(this->currentGameState) = QUIT_GAME;
                    }
                    if (this->buttonPressed->getUNIQUEIDENTIFIER() == 0) {
                        this->collectData();  // ONE OF THE BUTTONS PRESSED WAS
                                              // START OR SAVE SETTINGS
                        this->player_factory->setNumberofPlayers(
                                this->global_settings->getPlayer_Count());
                        this->player_factory->initializePlayerDataBase();
                        this->global_settings->setCurrentTerrain(
                                this->getSubMenuLandscape()->tm);
                        Mix_HaltMusic();
                        *currentGameState = READY_MENU;
                    }
                    if (this->buttonPressed->getUNIQUEIDENTIFIER() == 8) {
                        this->collectData();
                    } else {
                        for (int i = 0; i < NUM_BUTTON;
                             i++) {            // DEACTIVATE ALL OTHER MENUS
                            if (buttons[i]) {  // SAFEGUARD AGAINST NULL
                                               // POINTER
                                this->buttons[i]->deactivateSubMenu();
                            }
                        }
                        if (this->buttonPressed->getUNIQUEIDENTIFIER() != 0) {
                            this->buttonPressed
                                    ->activateSubMenu();  // ACTIVATE THE
                                                          // SUBMENU ASSOCIATED
                                                          // WITH THE BUTTON
                                                          // PRESSED ABOVE
                            this->activeSubMenu =
                                    this->buttonPressed
                                            ->getSubMenu();  // SET THE ACTIVE
                                                             // SUBMENU
                        }
                    }
                }
                this->buttonPressed
                        ->depressButton();  // WHENEVER YOU RELEASE MOUSEBUTTON
                                            // DEPRESS THE BUTTON YOU MIGHT
                                            // HAVE PUSHED
                this->buttonPressed =
                        nullptr;  // YOU HANDLED THE BUTTON NOW CLEAR IT
            } else {              // IF YOU RELEASE OUTSIDE ALL BUTTONS
                this->buttonPressed->depressButton();  // DEPRESS BUTTON YOU
                                                       // MIGHT HAVE CLICKED
                this->buttonPressed = nullptr;  // NO BUTTON REALLY CLICKED
                                                // (MUST REMAIN OVER BUTTON)

                // WTF, THIS CAUSED A BUG! WHY WAS IT THERE?!
                // this->activeSubMenu=NULL;
                // //GET RID OF ACTIVE SUBMENU JUST IN CASE (in case of what?)
            }
        } else if (this->arrowButtonPressed !=
                   nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW
                               // BUTTON
                               // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= this->arrowButtonPressed->getXPos()) &&
                (x <= (this->arrowButtonPressed->getXPos() +
                       this->arrowButtonPressed->getWidth())) &&
                (y <= this->arrowButtonPressed->getYPos()) &&
                (y >= (this->arrowButtonPressed->getYPos() -
                       this->arrowButtonPressed->getHeight()))) {
                this->arrowButtonPressed->mouseClickEvent(
                        x,
                        y,
                        buttonDown,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
            } else {
                this->arrowButtonPressed->mouseClickEvent(
                        x,
                        y,
                        buttonDown,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                this->arrowButtonPressed = nullptr;
            }
        }
    }
    // Sub Menu Button Test
    if (this->activeSubMenu != nullptr) {
        this->activeSubMenu->subMenuMouseTest(x, y, buttonDown);
    }
}

void MainMenu::collectData() {
    std::string optionsarray;
    for (int x = 2; x < NUM_SUBMENUS;
         x++) {  // SOUND AND START GAME ARE 1 AND 0 RESPECTIVLY
        if (this->submenus[x]) {
            optionsarray += this->submenus[x]->collectData();
        }
    }

    std::string playercount = "/Player Count/";
    if (this->arrowsbutton[0]) {
        playercount += arrowsbutton[0]->collectData();
        playercount += "/";
    }

    this->global_settings->setVariables(optionsarray, playercount);
}