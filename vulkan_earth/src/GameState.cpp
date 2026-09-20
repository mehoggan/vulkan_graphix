#include "GameState.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <errno.h>
#include <cstdio>
#include <iostream>
#include <string>
#include "ChaseCam.h"
#include "Explosion.h"
#include "GlobalSettings.h"
#include "ImageObject.h"
#include "Inventory.h"
#include "Item.h"
#include "Normal.h"
#include "Player.h"
#include "PlayerFactory.h"
#include "Projectile.h"
#include "SkyboxFactory.h"
#include "SpecialEffect.h"
#include "Tank.h"
#include "TerrainMaker.h"
#include "TextObject.h"
#include "VBOShaderLibrary.h"
#include "Vector.h"
#include "Vertex.h"
#include "Water.h"
#include "Weapon.h"
#include "WorldCam.h"
#include "macro_crtdbg.h"
#include "math.h"
#include "possibleGameStates.h"
#include "sound.h"
#include "time.h"

/* Later, when a round is finished, make sure all human and/or cpu players must
 * unload their weapons. Call player(i)->setLoadedWeapon(NULL)*/
/* For the same reason, initialize all the durations of status of tanks*/

extern void playSFX(int sfx);
extern void playMusic(int music);

using namespace std;

GameState::GameState() = default;

GameState::GameState(int width,
                     int height,
                     PlayerFactory* player_factory,
                     GlobalSettings* global_settings,
                     int* currentGameState) {
    this->SCALE_GRAVITY = 30;
    this->BALISTIC_SCALAR = 50;
    this->gravity = -9.8 * SCALE_GRAVITY;

    this->timer = 0;

    this->currentGameState = currentGameState;
    needHelp = false;
    startMusicPlayed = false;
    prevMusicVolume = 0;
    this->specialEffectTimer = 0;
    this->radiusOfCurrentExplosion = 0;
    this->specialEffectType = 0;
    this->special_effects = nullptr;
    this->drawHitBox = false;

    for (int x = 0; x < 256; x++) {
        this->keyMonitor[x] = 0;
    }

    this->selectedWeaponImg = nullptr;
    this->selectedWeaponRemain = nullptr;
    this->weaponSlot = new ImageObject(0,
                                       0,
                                       2,
                                       width * 0.08,
                                       height * 0.11,
                                       0,
                                       1024,
                                       1024,
                                       "TestImage.raw");
    srand(time(nullptr));
    this->player_factory = player_factory;
    this->global_settings = global_settings;
    this->width = width;
    this->height = height;
    this->playerCam = false;
    this->chaseCamActive = false;
    this->projectile = nullptr;
    this->worldCam = new WorldCam(
            0,
            20000,
            this->global_settings->getCurrentTerrain()->getActualSize() / 2);

    this->gameSubState = PASS_TIME;
    this->currentPlayerIndex = 0;
    this->currentPlayer = player_factory->getPlayer(currentPlayerIndex);

    // PLACE TANKS
    for (int i = 0; i < global_settings->getPlayer_Count(); i++) {
        float x, y, z;
        int size = static_cast<int>(
                global_settings->getCurrentTerrain()->getActualSize());
        int scale = static_cast<int>(
                global_settings->getCurrentTerrain()->getScale());

        x = 5 + rand() % ((size / scale) - 10);
        z = 5 + rand() % ((size / scale) - 10);

        Normal n =
                global_settings->getCurrentTerrain()->getTriangleNormal(x, z);

        if (player_factory->getPlayer(i)->getCurrentTank()) {
            player_factory->getPlayer(i)->getCurrentTank()->orientTank(&n);
        } else {
            cout << "I lost a tank, HOW???" << endl;
        }

        // delete n;

        /*	FINALLY POSITION TANKS	*/
        y = global_settings->getCurrentTerrain()->getHeightAt(z * scale,
                                                              x * scale);
        player_factory->getPlayer(i)->getCurrentTank()->setTankPos(
                x * scale, y, z * scale);
        this->numberOfPlayers = global_settings->getPlayer_Count();
    }

    //*
    if (this->global_settings) {
    } else if (!global_settings) {
        printf("Quiting because Global Settings Does Not Exist\n");
        exit(0);
    } else if (!(this->global_settings->getCurrentTerrain())) {
        printf("The terrain does not exist\n");
        exit(0);
    } else {
    }
    //*/

    /************************************************************************************/
    /*	NOTE TERRAIN IS 256 X 256 WITH SCALE OF 100 --> TERRAIN IS OF SIZE
     * 655360000	*/
    /*	MID POINT OF THE TERRAIN WOULD RESULT IN OPENGL COORD (12800,12800) AT
     * MIDDLE	*/
    /*	OF TERRAIN. THUS IF YOU START AT (-9000,-9000) THE RADIUS BEING
     * 30829.8556 		*/
    /*	FROM YOUR START LOCATION TO THE CENTER OF THE TERRAIN.
     */
    /************************************************************************************/

    this->offset =
            this->global_settings->getCurrentTerrain()->getActualSize() / 2.0;
    this->cameraX = 4000;
    this->cameraY = 15000;
    this->cameraZ = 4000;
    //
    this->camera_radius = sqrt(
            (pow(static_cast<double>(this->cameraX - this->offset), 2.0)) +
            (pow(static_cast<double>(this->cameraY), 2.0)) +
            (pow(static_cast<double>(this->cameraZ - this->offset), 2.0)));
    this->plane_radius = sqrt(
            (pow(static_cast<double>(this->cameraX - this->offset), 2.0)) +
            (pow(static_cast<double>(this->cameraZ - this->offset), 2.0)));
    this->currentTankTheta = 0.0;
    this->currentWorldTheta = 0.0;
    this->currentTankPhi =
            45.0;  // NOTE THIS NEEDS TO BE UPDATED TO FIRST PLAYERS ANGLE

    skybox_factory = new SkyboxFactory(2048);
    ocean = new Water(400, 512);
    sfxRandom = 0;
    this->projectile_fired = false;

    inventory = new Inventory(
            width * 0.25, height * 0.25, width * 0.5, height * 0.5);

    // Creating models for projectile.
    // IMPORTANT: Be careful about the order. It should match with the order in
    // that the weapons are created in the ShopMenu constructor
    if (!(VBOShaderLibrary::InitGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::AreVBOsSupported())) {
        exit(1);
    }
    for (int i = 0; i < MAX_PROJECTILE_MODELS; i++) {
        projectileModels[i] = new VBOShaderLibrary();
        projectileModels[i]->getVBOPointerFunctions();
        projectileModels[i]->loadShaders("VertexTank.vs", "FragmentTank.vs");
    }

    projectileModels[10]->loadClientData("Projectiles/projectileDefault.ogl");
    projectileModels[10]->LoadTexture(
            "Projectiles/projectileDefault.raw", 512, 512);

    projectileModels[0]->loadClientData("Projectiles/projectileBFB.ogl");
    projectileModels[0]->LoadTexture(
            "Projectiles/projectileMFB.raw", 512, 512);

    projectileModels[1]->loadClientData("Projectiles/projectileBFB.ogl");
    projectileModels[1]->LoadTexture(
            "Projectiles/projectileBFB.raw", 512, 512);

    projectileModels[2]->loadClientData("Projectiles/projectileAcid.ogl");
    projectileModels[2]->LoadTexture(
            "Projectiles/projectileAcid.raw", 512, 512);

    projectileModels[3]->loadClientData("Projectiles/projectileThor.ogl");
    projectileModels[3]->LoadTexture(
            "Projectiles/projectileThor.raw", 512, 512);

    projectileModels[4]->loadClientData("Projectiles/projectileEMP.ogl");
    projectileModels[4]->LoadTexture(
            "Projectiles/projectileEMP.raw", 512, 512);

    projectileModels[5]->loadClientData("Projectiles/projectileDefault.ogl");
    projectileModels[5]->LoadTexture(
            "Projectiles/projectilePadlock.raw", 512, 512);

    projectileModels[6]->loadClientData("Projectiles/projectileDefault.ogl");
    projectileModels[6]->LoadTexture(
            "Projectiles/projectileRevive.raw", 512, 512);

    projectileModels[7]->loadClientData("Projectiles/projectileDefault.ogl");
    projectileModels[7]->LoadTexture(
            "Projectiles/projectileTeleport.raw", 512, 512);

    projectileModels[8]->loadClientData("Projectiles/projectileDefault.ogl");
    projectileModels[8]->LoadTexture(
            "Projectiles/projectileAtom.raw", 512, 512);

    projectileModels[9]->loadClientData("Projectiles/projectileNuke.ogl");
    projectileModels[9]->LoadTexture(
            "Projectiles/projectileNuke.raw", 512, 512);

    manual = new ImageObject(width * -0.175,
                             height * 0.25,
                             2,
                             width * 0.35,
                             height * 0.55,
                             width * 0.004,
                             1024,
                             1024,
                             "manual.raw");

    /* AI VARIABLES	*/
    this->tank_reachable =
            new bool*[this->player_factory->getNumberofPlayers()];
    this->tankList = new Tank**[this->player_factory->getNumberofPlayers()];
    this->distance_to_target =
            new GLfloat*[this->player_factory->getNumberofPlayers()];
    for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
        this->tankList[i] =
                new Tank*[this->player_factory->getNumberofPlayers()];
        this->distance_to_target[i] =
                new GLfloat[this->player_factory->getNumberofPlayers()];
        this->tank_reachable[i] =
                new bool[this->player_factory->getNumberofPlayers()];
        for (int j = 0; j < this->player_factory->getNumberofPlayers(); j++) {
            this->tankList[i][j] =
                    this->player_factory->getPlayer(j)->getCurrentTank();
            this->distance_to_target[i][j] = 1E+37;  // MAX FLOAT
            this->tank_reachable[i][j] = true;
        }
    }
    cout << "Done Building Table" << endl;
    /* END OF AI VARIABLES */

    for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
        this->player_factory->getPlayer(i)->setLoadedWeapon(nullptr);
        this->player_factory->getPlayer(i)->getCurrentTank()->initDuration();
        this->player_factory->getPlayer(i)->getCurrentTank()->updateHitBox();
    }

    for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
        this->player_factory->getPlayer(i)->setGameState(this);
    }
}

GameState::~GameState() {
    Mix_HaltMusic();
    delete ocean;
    delete skybox_factory;
    delete worldCam;
    delete inventory;
    delete selectedWeaponImg;
    delete selectedWeaponRemain;
    delete weaponSlot;
    for (int x = 0; x < MAX_PROJECTILE_MODELS; x++) {
        delete projectileModels[x];
    }
    delete manual;
    for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
        delete[] tank_reachable[i];
    }
    delete[] tank_reachable;
    for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
        delete[] tankList[i];
    }
    delete[] tankList;
    for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
        delete[] distance_to_target[i];
    }
    delete[] distance_to_target;
}

GLfloat GameState::calcDistanceBetweenVertices(Vertex* v0, Vertex* v1) {
    return static_cast<GLfloat>(sqrt(pow((static_cast<double>(v0->coordX) -
                                          static_cast<double>(v1->coordX)),
                                         2.0) +
                                     pow((static_cast<double>(v0->coordZ) -
                                          static_cast<double>(v1->coordZ)),
                                         2.0)));
}

void GameState::timerEvent(GLfloat timer) {}

void GameState::NormalizeVector(Vector* v) {
    GLfloat mag = sqrt(v->compoX * v->compoX + v->compoY * v->compoY +
                       v->compoZ * v->compoZ);
    if (mag != 0) {
        v->compoX /= mag;
        v->compoY /= mag;
        v->compoZ /= mag;
    }
}

//!!!!!//
GLfloat GameState::calcAngleBetweenVectors(Vector one, Vector two) {
    this->NormalizeVector(&one);
    this->NormalizeVector(&two);
    errno = 0;
    GLfloat TT = 3.141592653f;
    GLfloat u[3] = {one.compoX, one.compoY, one.compoZ};
    GLfloat v[3] = {two.compoX, two.compoY, two.compoZ};
    GLfloat angle =
            acos(u[0] * v[0] + u[1] * v[1] + u[2] * v[2]) * (180.0 / TT);
    if (errno) {
        return .01;
    }
    return angle;
}
//!!!!!//

void GameState::calcNormalVector(Vertex* v0,
                                 Vertex* v1,
                                 Vertex* v2,
                                 Normal* n) {
    GLfloat u[3] = {(v1->coordX - v0->coordX),
                    (v1->coordY / 100 - v0->coordY / 100),
                    (v1->coordZ - v0->coordZ)};
    GLfloat v[3] = {(v2->coordX - v0->coordX),
                    (v2->coordY / 100 - v0->coordY / 100),
                    (v2->coordZ - v0->coordZ)};

    n->compoX = u[1] * v[2] - v[1] * u[2];
    n->compoY = u[2] * v[0] - u[0] * v[2];
    n->compoZ = u[0] * v[1] - v[0] * u[1];

    GLfloat mag = static_cast<GLfloat>(
            sqrt(pow(static_cast<double>(n->compoX), 2.0) +
                 pow(static_cast<double>(n->compoY), 2.0) +
                 pow(static_cast<double>(n->compoZ), 2.0)));
    if (mag != 0) {
        n->compoX /= mag;
        n->compoY /= mag;
        n->compoZ /= mag;
    }
}

void GameState::update() {
    // Game logic
    if (this->gameSubState == PLAYER_CONTROL) {
        // QUICK FIX FOR TURRET

        if (player_factory->getPlayer(currentPlayerIndex)->getCurrentWait() >
            0) {
            gameSubState = PASS_TIME;
        }

        if (strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
            this->handlePlayerControlUpdates();
            if (this->currentPlayer->getCurrentTank()->isAlive()) {
                this->controlAI();
            } else {
                cout << "Tank " << this->currentPlayerIndex << " is dead"
                     << endl;
            }
            this->updateWorldCam();
        } else if (strcmp(currentPlayer->getPlayer_Type(), "HUMAN") == 0) {
            this->handlePlayerControlUpdates();
            this->updateWorldCam();
        }
    }
    // else
    if (this->gameSubState == PASS_TIME) {
        this->handlePassTime();
    }

    // projectile update/collision/special effect
    if (this->gameSubState == PROJECTILE) {
        this->handleProjectileState();
        this->updateWorldCam();
    }
    if (this->gameSubState == SPECIAL_EFFECT) {
        this->handleSpecialEffectState();
    }
}

void GameState::draw() {
    playBackgroundSounds();
    this->update();

    if (this->gameSubState == PROJECTILE ||
        this->gameSubState == SPECIAL_EFFECT) {
        if (this->chaseCamActive) this->projectile->chaseView();
    }
    if (!this->chaseCamActive) {
        if (!playerCam) {
            this->worldCam->view();
        } else {
            const GLfloat* turretMatrix =
                    this->currentPlayer->getCurrentTank()->getTurretMatrix();
            const GLfloat* headMatrix =
                    this->currentPlayer->getCurrentTank()->getHeadMatrix();
            gluLookAt(headMatrix[12] + headMatrix[8] * 4000,
                      headMatrix[13] + 2000,
                      headMatrix[14] + headMatrix[10] * 4000,
                      headMatrix[12] - headMatrix[8] * 3000,
                      headMatrix[13] + headMatrix[9] * 0,
                      headMatrix[14] - headMatrix[10] * 3000,
                      0,
                      1,
                      0);
        }
    }

    this->global_settings->getCurrentTerrain()->draw();
    this->skybox_factory->draw();

    if (strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
        if (this->currentPlayer->getDrawDebugLinesandPlanes()) {
            this->currentPlayer->drawTestLinesandPlanes();
        }
    }

    if (this->gameSubState != SPECIAL_EFFECT) {
        if (this->projectile) {
            this->projectile->draw();
        }
    }

    glPushMatrix();
    glTranslatef(-256.00f * 400, -5000.00f, -256.00f * 400);
    this->ocean->draw();
    glPopMatrix();

    // DRAW TANKS
    glColor3f(player_factory->getPlayer(currentPlayerIndex)->getRed(),
              player_factory->getPlayer(currentPlayerIndex)->getGreen(),
              player_factory->getPlayer(currentPlayerIndex)->getBlue());
    for (int i = 0; i < global_settings->getPlayer_Count(); i++) {
        if (this->player_factory->getPlayer(i)
                    ->getCurrentTank()
                    ->getDurationCloak() == 0) {
            this->player_factory->getPlayer(i)->getCurrentTank()->draw();
            if (this->drawHitBox) {
                this->player_factory->getPlayer(i)
                        ->getCurrentTank()
                        ->drawTankHitBox();
            }
        }
    }

    // DRAW EXPLOSION/SPECIAL EFFECT
    // this->specialEffectsCount =
    // (int)(SPECIAL_EFFECT_TIME_LIMIT/TIME_DIVISORS);
    if (this->gameSubState == SPECIAL_EFFECT) {
        if (this->specialEffectType == EXPLOSION) {
            for (int x = 0; x < this->specialEffectsCount; x++) {
                if (this->specialEffectTimer > 0) {
                    this->special_effects[0]->draw();
                }
                if (this->specialEffectTimer > 10) {
                    this->special_effects[1]->draw();
                }
                if (this->specialEffectTimer > 20) {
                    this->special_effects[2]->draw();
                }
            }
        }
    }

    if (this->gameSubState == PLAYER_CONTROL) {
        if (strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
            const GLfloat* turretMatrix =
                    this->currentPlayer->getCurrentTank()->getTurretMatrix();
            const GLfloat* bodyMatrix =
                    this->currentPlayer->getCurrentTank()->getBodyMatrix();
            glLineWidth(1000);
            glBegin(GL_LINES);
            glVertex3f(turretMatrix[12], turretMatrix[13], turretMatrix[14]);
            glVertex3f(bodyMatrix[12], bodyMatrix[13], bodyMatrix[14]);
            glEnd();
            glLineWidth(1);
        }
    }

    if (currentPlayer->getCurrentTank()->getDurationEMP() > 0) {
        // the current player is in effect of EMP, so do not draw HUD and
        // minimap. He doesn't deserve them.
    } else {
        drawHUD();
        drawMinimap();
    }
    if (this->gameSubState == INVENTORY) {
        inventory->draw();
    } else if (this->gameSubState == HELP) {
        drawHelp();
    }
}

void GameState::drawHUD() {
    if (playerCam || chaseCamActive) {
        this->worldCam->setShakeCam(0);
    }
    glPushMatrix();
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -800.0f);

    float newX = GLUT_SCREEN_WIDTH / 2 +
                 800 * tan(30 * PI / 180);  // this is wrong, change later
    float newY = GLUT_SCREEN_HEIGHT / 2 + 800 * tan(30 * PI / 180);

    // Current Player Name
    if (this->gameSubState == PLAYER_CONTROL) {
        glColor3f(player_factory->getPlayer(currentPlayerIndex)->getRed(),
                  player_factory->getPlayer(currentPlayerIndex)->getGreen(),
                  player_factory->getPlayer(currentPlayerIndex)->getBlue());
        drawHUDText(player_factory->getPlayer(this->currentPlayerIndex)
                            ->getPlayerName(),
                    0,
                    0.75 * newY);
    }

    // List of Players and health/delay/team
    glColor3f(1, 1, 1);
    drawHUDText("Wait", -0.99 * newX, 0.75 * newY);
    drawHUDText("HP", -0.87 * newX, 0.75 * newY);
    drawHUDText("Player Name", -0.76 * newX, 0.75 * newY);
    drawHUDText("Team", -0.525 * newX, 0.75 * newY);
    for (int i = 0; i < player_factory->getNumberofPlayers(); i++) {
        glColor3f(player_factory->getPlayer(i)->getRed(),
                  player_factory->getPlayer(i)->getGreen(),
                  player_factory->getPlayer(i)->getBlue());
        drawHUDText(player_factory->getPlayer(i)->getPlayerName(),
                    -0.76 * newX,
                    0.70 * newY - 0.05 * newY * i);
        char* team = "";
        char add[2];
        add[0] = player_factory->getPlayer(i)->getTeamLabel();
        add[1] = '\0';
        team = add;
        glColor3f(1, 1, 1);
        drawHUDText(team, -0.475 * newX, 0.70 * newY - 0.05 * newY * i);
        char buffer[128];
        memset(buffer, 0, 128);
        drawHUDText((sprintf(buffer,
                             "%d",
                             static_cast<int>(player_factory->getPlayer(i)
                                                      ->getCurrentWait())),
                     buffer),
                    -0.95 * newX,
                    0.70 * newY - 0.05 * newY * i);
        if (!player_factory->getPlayer(i)->getCurrentTank()->isAlive()) {
            glColor3f(1, 0, 0);
        }
        if (player_factory->getPlayer(i)
                    ->getCurrentTank()
                    ->getDurationShield() > 0) {
            glColor3f(0, 0, 1);
        }
        if (player_factory->getPlayer(i)->getCurrentTank()->getDurationAcid() >
            0) {
            glColor3f(0, 1, 0);
        }
        drawHUDText((sprintf(buffer,
                             "%d",
                             player_factory->getPlayer(i)
                                     ->getCurrentTank()
                                     ->getHP()),
                     buffer),
                    -0.87 * newX,
                    0.70 * newY - 0.05 * newY * i);
    }

    // Power Output
    if (this->gameSubState == PLAYER_CONTROL) {
        // Text
        GLfloat powerRatio = player_factory->getPlayer(currentPlayerIndex)
                                     ->getCurrentTank()
                                     ->getCurrentPower() /
                             10.0;
        char buffer[128];
        memset(buffer, 0, 128);
        glColor3f(1, 1, 1);
        drawHUDText("Power:", 0.7 * newX, -0.7 * newY);
        drawHUDText(
                (sprintf(buffer, "%d", static_cast<int>(powerRatio * 1000)),
                 buffer),
                0.95 * newX,
                -0.7 * newY);
        // show previous power
        char buffer1[128];
        memset(buffer1, 0, 128);
        glColor3f(0.55, 0.55, 0.55);
        drawHUDText("Power:", 0.7 * newX, -0.65 * newY);
        drawHUDText((sprintf(buffer1,
                             "%d",
                             player_factory->getPlayer(currentPlayerIndex)
                                     ->getCurrentTank()
                                     ->getPreviousPower()),
                     buffer1),
                    0.95 * newX,
                    -0.65 * newY);

        // Angle
        char buffer2[128];
        memset(buffer2, 0, 128);
        glColor3f(1, 1, 1);
        drawHUDText("Angle:", 0.7 * newX, -0.8 * newY);
        drawHUDText(
                (sprintf(buffer2,
                         "%d",
                         static_cast<int>(
                                 player_factory->getPlayer(currentPlayerIndex)
                                         ->getCurrentTank()
                                         ->getTurretDegrees() +
                                 1)),
                 buffer2),
                0.95 * newX,
                -0.8 * newY);
        // show previous angle
        char buffer3[128];
        memset(buffer3, 0, 128);
        glColor3f(0.55, 0.55, 0.55);
        drawHUDText("Angle:", 0.7 * newX, -0.75 * newY);
        drawHUDText((sprintf(buffer3,
                             "%d",
                             player_factory->getPlayer(currentPlayerIndex)
                                     ->getCurrentTank()
                                     ->getPreviousAngle()),
                     buffer3),
                    0.95 * newX,
                    -0.75 * newY);

        // Show Weapon Slot
        weaponSlot->setXpos(-newX * 0.7);
        weaponSlot->setYpos(-0.6 * newY);
        weaponSlot->setZpos(2);
        weaponSlot->draw();
        if (currentPlayer->getLoadedWeapon() != nullptr) {
            selectedWeaponImg->setXpos(weaponSlot->getXpos() * 0.965);
            selectedWeaponImg->setYpos(weaponSlot->getYpos() * 0.985);
            selectedWeaponImg->setZpos(20);
            selectedWeaponRemain->setXpos(weaponSlot->getXpos() * 0.965);
            selectedWeaponRemain->setYpos(weaponSlot->getYpos() * 1.3);
            selectedWeaponRemain->setZpos(22);
            selectedWeaponImg->draw();
            selectedWeaponRemain->draw();
        }

        // Display Need Help?
        if (needHelp) {
            glColor3f(1, 1, 0);
            drawHUDText("Press F1 for help", -1.3 * newX, -0.5 * newY);
        }

        // Graphic meter: Power
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, GLUT_SCREEN_WIDTH, 0, GLUT_SCREEN_HEIGHT, 1, 2000000);
        glMatrixMode(GL_MODELVIEW);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH),
                   0.05 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH),
                   0.08 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) + (0.1 * GLUT_SCREEN_WIDTH),
                   0.08 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) + (0.1 * GLUT_SCREEN_WIDTH),
                   0.05 * GLUT_SCREEN_HEIGHT,
                   2);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);
        glColor3f(powerRatio, 1 - powerRatio, 0);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH),
                   0.05 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH),
                   0.08 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH) +
                           (powerRatio * 0.2) * GLUT_SCREEN_WIDTH,
                   0.08 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH) +
                           (powerRatio * 0.2) * GLUT_SCREEN_WIDTH,
                   0.05 * GLUT_SCREEN_HEIGHT,
                   2);
        glEnd();
        glPopMatrix();

        // Graphic meter: Health
        GLfloat healthRatio =
                static_cast<float>(
                        player_factory->getPlayer(currentPlayerIndex)
                                ->getCurrentTank()
                                ->getHP()) /
                static_cast<float>(
                        player_factory->getPlayer(currentPlayerIndex)
                                ->getCurrentTank()
                                ->getArmor() *
                        100);
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, GLUT_SCREEN_WIDTH, 0, GLUT_SCREEN_HEIGHT, 1, 2000000);
        glMatrixMode(GL_MODELVIEW);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH),
                   0.90 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH),
                   0.93 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) + (0.1 * GLUT_SCREEN_WIDTH),
                   0.93 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) + (0.1 * GLUT_SCREEN_WIDTH),
                   0.90 * GLUT_SCREEN_HEIGHT,
                   2);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);
        glColor3f(1 - healthRatio, healthRatio, 0);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH),
                   0.90 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH),
                   0.93 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH) +
                           (healthRatio * 0.2) * GLUT_SCREEN_WIDTH,
                   0.93 * GLUT_SCREEN_HEIGHT,
                   2);
        glVertex3f((GLUT_SCREEN_WIDTH / 2) - (0.1 * GLUT_SCREEN_WIDTH) +
                           (healthRatio * 0.2) * GLUT_SCREEN_WIDTH,
                   0.90 * GLUT_SCREEN_HEIGHT,
                   2);
        glEnd();
        glPopMatrix();
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glPopMatrix();
}

void GameState::drawHUDText(char* input, GLfloat x, GLfloat y) {
    char* string_iterator = input;
    GLfloat xPos = x;
    while (*string_iterator != '\0') {
        int step = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,
                                   *(string_iterator));
        glRasterPos2f(xPos, y);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *(string_iterator));
        xPos += step;
        string_iterator++;
    }
}

void GameState::updateMouse(int x, int y) {
    if (playerCam) {
        float tHeight =
                this->global_settings->getCurrentTerrain()->getHeightAt(
                        this->currentPlayer->getCurrentTank()
                                        ->getHeadMatrix()[12] -
                                50 * this->currentPlayer->getCurrentTank()
                                                ->getHeadMatrix()[8],
                        this->currentPlayer->getCurrentTank()
                                        ->getHeadMatrix()[14] -
                                50 * this->currentPlayer->getCurrentTank()
                                                ->getHeadMatrix()[10]);
        float tank_radius = 400.0f;

        if (y < oldMouseY) {
            this->cameraY -= 10;
        } else if (y > oldMouseY) {
            this->cameraY += 10;
        }

    } else {
        if (this->currentWorldTheta >= 360.0) {
            this->currentWorldTheta -= 360.0;
        } else if (this->currentWorldTheta < 0.0) {
            this->currentWorldTheta += 360.0;
        }
        if (x < oldMouseX) {
            this->currentWorldTheta -= 1.0;
            this->cameraX =
                    (this->plane_radius) *
                            (cos(this->currentWorldTheta * (PI / 180))) +
                    this->offset;
            this->cameraZ =
                    (this->plane_radius) *
                            (sin(this->currentWorldTheta * (PI / 180))) +
                    this->offset;
        } else if (x > oldMouseX) {
            this->currentWorldTheta += 1.0;
            this->cameraX =
                    (this->plane_radius) *
                            (cos(this->currentWorldTheta * (PI / 180))) +
                    this->offset;
            this->cameraZ =
                    (this->plane_radius) *
                            (sin(this->currentWorldTheta * (PI / 180))) +
                    this->offset;
        }
    }
    this->oldMouseX = x;
    this->oldMouseY = y;
}

void GameState::useTurn() {
    this->gameSubState = PLAYER_CONTROL;
    if (currentPlayer->getCurrentTank()->getDurationDoubleAction() == 0) {
        this->currentPlayer->getCurrentTank()->setDurationAllPassTurn();
        this->currentPlayer->setCurrentWait(
                150 - this->currentPlayer->getCurrentTank()->getBaseSpeed());
    } else {
        currentPlayer->getCurrentTank()->setDurationDoubleAction(
                currentPlayer->getCurrentTank()->getDurationDoubleAction() -
                1);
    }

    if (playerCam) {
        toggleCamera();
    }
}

// call this function every time someone is dead (or gets damaged).
// NOTE: call this once when game starts to check the case in which everyone is
// on the same team. Return values: -1==Draw game, 0==Nobody's won, n==Solo
// player num, n+100==Team num
int GameState::getWinner() {
    int winner = 0;

    for (int i = 0; i < player_factory->getNumberofPlayers(); i++) {
        if (player_factory->getPlayer(i)->getCurrentTank()->getHP() > 0) {
            // CHECK FOR SOLO WINNER
            if (player_factory->getPlayer(i)->getTeamLabel() == '-' &&
                winner == 0) {
                winner = i + 1;
            }
            // CHECK FOR TEAM WINNER
            else if ((player_factory->getPlayer(i)->getTeamLabel() - 48) +
                                     100 ==
                             winner ||
                     winner == 0) {
                winner = (player_factory->getPlayer(i)->getTeamLabel() - 48) +
                         100;
            }
            // NOBODY HAS WON YET
            else {
                return 0;
            }
        } else {
            if (winner != 0)
                return winner;
            else
                return -1;  // Everyone is DEAD?! Draw Game!
        }
    }
    return 0;
}

void GameState::toggleCamera() {
    if (this->gameSubState == PROJECTILE) {
        if (this->chaseCamActive) {
            this->chaseCamActive = false;
            Mix_HaltChannel(1);
            this->playerCam = false;
        } else {
            this->chaseCamActive = true;
            Mix_HaltChannel(1);
            playSFX(BOMB_FLY);
            this->playerCam = false;
        }
    } else if (playerCam) {
        this->currentTankTheta = 0.0f;
        playerCam = false;
    } else {
        playerCam = true;
        this->cameraX =
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[12] +
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[8] *
                        1000;
        this->cameraY =
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[13] +
                350;
        this->cameraZ =
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[14] +
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[10] *
                        1000;
    }
}

void GameState::debugMode() {
    /*THIS IS DEBUG TEXT REMOVE LATER THIS JUST HELPS TO SEE IF TANKS ARE
     * ORIENTED CORRECTLY*/
    //*
    Normal n = this->currentPlayer->getCurrentTank()->getAlignmentVector();
    glColor3f(0.00, 0.50, 0.50);
    glBegin(GL_LINES);
    glVertex3f(
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[12],
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[13] + 100,
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[14]);
    glVertex3f(this->currentPlayer->getCurrentTank()->getHeadMatrix()[12] +
                       800 * n.compoX,
               this->currentPlayer->getCurrentTank()->getHeadMatrix()[13] +
                       100 + 800 * n.compoY,
               this->currentPlayer->getCurrentTank()->getHeadMatrix()[14] +
                       800 * n.compoZ);
    glEnd();
    //*/
    //*/
    glColor3f(0.00, 0.00, 1.00);
    glBegin(GL_LINES);
    glVertex3f(
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[12],
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[13] + 100,
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[14]);
    glVertex3f(this->currentPlayer->getCurrentTank()->getHeadMatrix()[12] +
                       1000 * (this->currentPlayer->getCurrentTank()
                                       ->getHeadMatrix()[4]),
               this->currentPlayer->getCurrentTank()->getHeadMatrix()[13] +
                       100 +
                       1000 * (this->currentPlayer->getCurrentTank()
                                       ->getHeadMatrix()[5]),
               this->currentPlayer->getCurrentTank()->getHeadMatrix()[14] +
                       1000 * (this->currentPlayer->getCurrentTank()
                                       ->getHeadMatrix()[6]));
    glEnd();
    //*/
    //*/
    Normal m = this->currentPlayer->getCurrentTank()->getRotateAbout();
    glColor3f(0.75, 0.50, 0.50);
    glBegin(GL_LINES);
    glVertex3f(
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[12],
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[13] + 100,
            this->currentPlayer->getCurrentTank()->getHeadMatrix()[14]);
    glVertex3f(this->currentPlayer->getCurrentTank()->getHeadMatrix()[12] +
                       1000 * m.compoX,
               this->currentPlayer->getCurrentTank()->getHeadMatrix()[13] +
                       100 + 1000 * m.compoY,
               this->currentPlayer->getCurrentTank()->getHeadMatrix()[14] +
                       1000 * m.compoZ);
    glEnd();
    //*/
    /*END OF TANK ORIENTATION DEBUGGING*/
}

void GameState::drawMinimap() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(width - width / 5.8,
               height - height / 4.8,
               width / 6.0,
               height / 5.0);
    glOrtho(-18000, 18000, -15000, 15000, 1, 20000000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScissor(width - width / 5.8,
              height - height / 4.8,
              width / 6.0,
              height / 5.0);
    glClearColor(.22, .65, .60, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int size = static_cast<int>(
            global_settings->getCurrentTerrain()->getActualSize());

    gluLookAt(size / 2,
              size + (size / width),
              size / 2,
              size / 2,
              0,
              size / 2,
              1.0f,
              0.0f,
              0.0f);

    global_settings->getCurrentTerrain()->draw();

    // Draw Tank Marks
    for (int i = 0; i < player_factory->getNumberofPlayers(); i++) {
        if (player_factory->getPlayer(i)
                    ->getCurrentTank()
                    ->getDurationCloak() == 0) {
            glBegin(GL_TRIANGLES);
            glColor3f(player_factory->collectPlayerColor(i)[0],
                      player_factory->collectPlayerColor(i)[1],
                      player_factory->collectPlayerColor(i)[2]);
            glVertex3f(player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[12] -
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[8] *
                                       900,
                       player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[13] +
                               12000,
                       player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[14] -
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[10] *
                                       900);
            glVertex3f(player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[12] +
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[0] *
                                       700 +
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[8] *
                                       900,
                       player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[13] +
                               12000,
                       player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[14] +
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[2] *
                                       700 +
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[10] *
                                       900);
            glVertex3f(player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[12] -
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[0] *
                                       700 +
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[8] *
                                       900,
                       player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[13] +
                               12000,
                       player_factory->getPlayer(i)
                                       ->getCurrentTank()
                                       ->getHeadMatrix()[14] -
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[2] *
                                       700 +
                               player_factory->getPlayer(i)
                                               ->getCurrentTank()
                                               ->getHeadMatrix()[10] *
                                       900);
            glEnd();
        }
    }
    // Draw Current Player Tank's Aiming Line
    glBegin(GL_QUADS);
    glColor4f(player_factory->collectPlayerColor(currentPlayerIndex)[0],
              player_factory->collectPlayerColor(currentPlayerIndex)[1],
              player_factory->collectPlayerColor(currentPlayerIndex)[2],
              0.50);
    glVertex3f(
            currentPlayer->getCurrentTank()->getHeadMatrix()[12] +
                    currentPlayer->getCurrentTank()->getHeadMatrix()[0] * 60,
            currentPlayer->getCurrentTank()->getHeadMatrix()[13] + 10000,
            currentPlayer->getCurrentTank()->getHeadMatrix()[14] +
                    currentPlayer->getCurrentTank()->getHeadMatrix()[2] * 60);
    glVertex3f(
            currentPlayer->getCurrentTank()->getHeadMatrix()[12] +
                    currentPlayer->getCurrentTank()->getHeadMatrix()[0] * 60 -
                    currentPlayer->getCurrentTank()->getHeadMatrix()[8] *
                            24000,
            currentPlayer->getCurrentTank()->getHeadMatrix()[13] + 10000,
            currentPlayer->getCurrentTank()->getHeadMatrix()[14] +
                    currentPlayer->getCurrentTank()->getHeadMatrix()[2] * 60 -
                    currentPlayer->getCurrentTank()->getHeadMatrix()[10] *
                            24000);
    glVertex3f(
            currentPlayer->getCurrentTank()->getHeadMatrix()[12] -
                    currentPlayer->getCurrentTank()->getHeadMatrix()[0] * 60 -
                    currentPlayer->getCurrentTank()->getHeadMatrix()[8] *
                            24000,
            currentPlayer->getCurrentTank()->getHeadMatrix()[13] + 10000,
            currentPlayer->getCurrentTank()->getHeadMatrix()[14] -
                    currentPlayer->getCurrentTank()->getHeadMatrix()[2] * 60 -
                    currentPlayer->getCurrentTank()->getHeadMatrix()[10] *
                            24000);
    glVertex3f(
            currentPlayer->getCurrentTank()->getHeadMatrix()[12] -
                    currentPlayer->getCurrentTank()->getHeadMatrix()[0] * 60,
            currentPlayer->getCurrentTank()->getHeadMatrix()[13] + 10000,
            currentPlayer->getCurrentTank()->getHeadMatrix()[14] -
                    currentPlayer->getCurrentTank()->getHeadMatrix()[2] * 60);
    glEnd();

    // Draw Lines of Sight of World Camera
    if (!playerCam && !chaseCamActive) {
        glBegin(GL_LINE_LOOP);
        glColor4f(1, 1, 1, 0.8);
        glVertex3f(
                worldCam->getMatrix()[12] - worldCam->getMatrix()[13] * 0.38,
                10000,
                worldCam->getMatrix()[14] + worldCam->getMatrix()[13] -
                        15000 / width);
        glVertex3f(
                worldCam->getMatrix()[12] - worldCam->getMatrix()[13] * 0.38,
                10000,
                worldCam->getMatrix()[14] - worldCam->getMatrix()[13] +
                        15000 / width);
        glVertex3f(
                worldCam->getMatrix()[12] + worldCam->getMatrix()[13] * 0.82,
                10000,
                worldCam->getMatrix()[14] - worldCam->getMatrix()[13] +
                        15000 / width - 1350);
        glVertex3f(
                worldCam->getMatrix()[12] + worldCam->getMatrix()[13] * 0.82,
                10000,
                worldCam->getMatrix()[14] + worldCam->getMatrix()[13] -
                        15000 / width + 1350);
        glEnd();
    }

    // Draw Projectile
    if (projectile) {
        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glVertex3f(projectile->getPos()[0] - 200,
                   10000,
                   projectile->getPos()[2] - 200);
        glVertex3f(projectile->getPos()[0] - 200,
                   10000,
                   projectile->getPos()[2] + 200);
        glVertex3f(projectile->getPos()[0] + 200,
                   10000,
                   projectile->getPos()[2] + 200);
        glVertex3f(projectile->getPos()[0] + 200,
                   10000,
                   projectile->getPos()[2] - 200);
        glEnd();
    }

    // Draw Projectile Land Mark
    glBegin(GL_LINES);
    glColor3f(player_factory->collectPlayerColor(currentPlayerIndex)[0],
              player_factory->collectPlayerColor(currentPlayerIndex)[1],
              player_factory->collectPlayerColor(currentPlayerIndex)[2]);
    glVertex3f(
            currentPlayer->getCurrentTank()->getProjectileLandPos()[0] - 900,
            10000,
            currentPlayer->getCurrentTank()->getProjectileLandPos()[1] - 900);
    glVertex3f(
            currentPlayer->getCurrentTank()->getProjectileLandPos()[0] + 900,
            10000,
            currentPlayer->getCurrentTank()->getProjectileLandPos()[1] + 900);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(player_factory->collectPlayerColor(currentPlayerIndex)[0],
              player_factory->collectPlayerColor(currentPlayerIndex)[1],
              player_factory->collectPlayerColor(currentPlayerIndex)[2]);
    glVertex3f(
            currentPlayer->getCurrentTank()->getProjectileLandPos()[0] + 900,
            10000,
            currentPlayer->getCurrentTank()->getProjectileLandPos()[1] - 900);
    glVertex3f(
            currentPlayer->getCurrentTank()->getProjectileLandPos()[0] - 900,
            10000,
            currentPlayer->getCurrentTank()->getProjectileLandPos()[1] + 900);
    glEnd();

    int winWidth = glutGet(GLUT_WINDOW_WIDTH);
    int winHeight = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, winWidth, winHeight);
    gluPerspective(
            60.0,
            static_cast<float>(winWidth) / static_cast<float>(winHeight),
            1.0,
            1000000.0);

    glMatrixMode(GL_MODELVIEW);
    glScissor(0, 0, winWidth, winHeight);
    glLoadIdentity();
}

void GameState::playBackgroundSounds() {
    if (strcmp(this->global_settings->getHill_Girth(), "Rock") == 0) {
        sfxRandom = rand() % 3000;  // set the frequency of the wave sound
        if (sfxRandom == 0)
            playSFX(WAVE1);
        else if (sfxRandom == 1)
            playSFX(WAVE2);
        else if (sfxRandom == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        sfxRandom = (rand() * 9) % 5000;
        if (sfxRandom == 0)
            playSFX(SEAGULLS1);
        else if (sfxRandom == 1)
            playSFX(SEAGULLS2);
        else if (sfxRandom == 2)
            playSFX(SEAGULLS3);
        else if (sfxRandom == 3)
            playSFX(SEAGULLS4);
        else if (sfxRandom == 4)
            playSFX(SEAGULLS5);

        playMusic(GAMESTATE_ROCK);
    } else if (strcmp(this->global_settings->getHill_Girth(), "Snow") == 0) {
        sfxRandom = rand() % 3000;  // set the frequency of the wave sound
        if (sfxRandom == 0)
            playSFX(WAVE1);
        else if (sfxRandom == 1)
            playSFX(WAVE2);
        else if (sfxRandom == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        sfxRandom = (rand() * 9) % 5000;
        if (sfxRandom == 0)
            playSFX(SEAGULLS1);
        else if (sfxRandom == 1)
            playSFX(SEAGULLS2);
        else if (sfxRandom == 2)
            playSFX(SEAGULLS3);
        else if (sfxRandom == 3)
            playSFX(SEAGULLS4);
        else if (sfxRandom == 4)
            playSFX(SEAGULLS5);

        playMusic(GAMESTATE_SNOW);
    } else if (strcmp(this->global_settings->getHill_Girth(), "Ice") == 0) {
        sfxRandom = rand() % 3000;  // set the frequency of the wave sound
        if (sfxRandom == 0)
            playSFX(WAVE1);
        else if (sfxRandom == 1)
            playSFX(WAVE2);
        else if (sfxRandom == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);

        playMusic(GAMESTATE_ICE);
    } else if (strcmp(this->global_settings->getHill_Girth(), "Desert") == 0) {
        sfxRandom = rand() % 3000;  // set the frequency of the wave sound
        if (sfxRandom == 0)
            playSFX(WAVE1);
        else if (sfxRandom == 1)
            playSFX(WAVE2);
        else if (sfxRandom == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);
        sfxRandom = (rand() * 9) % 5000;
        if (sfxRandom == 0)
            playSFX(SEAGULLS1);
        else if (sfxRandom == 1)
            playSFX(SEAGULLS2);
        else if (sfxRandom == 2)
            playSFX(SEAGULLS3);
        else if (sfxRandom == 3)
            playSFX(SEAGULLS4);
        else if (sfxRandom == 4)
            playSFX(SEAGULLS5);

        if (atoi(this->global_settings->getHill_Height()) < 2 &&
            atoi(this->global_settings->getHillyness()) > 4) {
            if (!startMusicPlayed) {
                playMusic(GAMESTATE_BEACH_START);
                startMusicPlayed = true;
            } else {
                playMusic(GAMESTATE_BEACH_LOOP);
            }
        } else {
            playMusic(GAMESTATE_DESERT);
        }
    } else if (strcmp(this->global_settings->getHill_Girth(), "Mars") == 0) {
        sfxRandom = rand() % 3000;  // set the frequency of the wave sound
        if (sfxRandom == 0)
            playSFX(WAVE1);
        else if (sfxRandom == 1)
            playSFX(WAVE2);
        else if (sfxRandom == 2)
            playSFX(WAVE3);
        else
            playSFX(WAVE1);

        playMusic(GAMESTATE_MARS);
    } else if (strcmp(this->global_settings->getHill_Girth(), "Lava") == 0) {
        playMusic(GAMESTATE_LAVA);
    }
}

void GameState::drawHelp() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(width * 0.25, height * 0.1, width * 0.5, height * 0.8);
    gluPerspective(
            60.0,
            (static_cast<float>(width) / (1.5 * static_cast<float>(height))),
            1,
            199999999);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScissor(width * 0.25, height * 0.1, width * 0.5, height * 0.8);
    glClearColor(0.75, 0.75, 0.75, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int distance = static_cast<int>(glutGet(GLUT_WINDOW_HEIGHT) / 4 *
                                    tan(1.04719755));
    gluLookAt(0, 0, distance, 0, 0, 0, 0.0f, 1.0f, 0.0f);

    manual->draw();

    int winWidth = glutGet(GLUT_WINDOW_WIDTH);
    int winHeight = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, winWidth, winHeight);
    gluPerspective(
            60.0,
            static_cast<float>(winWidth) / static_cast<float>(winHeight),
            1.0,
            1000000.0);

    glMatrixMode(GL_MODELVIEW);
    glScissor(0, 0, winWidth, winHeight);
    glLoadIdentity();
}

void GameState::handlePlayerControlUpdates() {
    if (this->keyMonitor[1]) {
        playSFX(TANK_CONTROL2);
        if (strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
            this->currentPlayer->updateBalsticMatrix();
            this->currentPlayer->setUpYawVectors();
        }
        this->player_factory->getPlayer(currentPlayerIndex)
                ->getCurrentTank()
                ->rotateHead(-0.1 - 0.4 * this->keyMonitor[1] / 50);
        if (strcmp(currentPlayer->getCurrentTank()->getName(), "Rhinoxx") ==
                    0 ||
            strcmp(currentPlayer->getCurrentTank()->getName(), "HeavyD") ==
                    0) {
            currentPlayer->getCurrentTank()->rotateWheel(-.1);
        }
        this->cameraX =
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[12] +
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[8] *
                        1000;
        this->cameraZ =
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[14] +
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[10] *
                        1000;
    } else if (this->keyMonitor[3]) {
        playSFX(TANK_CONTROL2);
        if (strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
            this->currentPlayer->updateBalsticMatrix();
            this->currentPlayer->setUpYawVectors();
        }
        this->player_factory->getPlayer(currentPlayerIndex)
                ->getCurrentTank()
                ->rotateHead(0.1 + 0.4 * this->keyMonitor[3] / 50);
        if (strcmp(currentPlayer->getCurrentTank()->getName(), "Rhinoxx") ==
                    0 ||
            strcmp(currentPlayer->getCurrentTank()->getName(), "HeavyD") ==
                    0) {
            currentPlayer->getCurrentTank()->rotateWheel(.1);
        }
        this->cameraX =
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[12] +
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[8] *
                        1000;
        this->cameraZ =
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[14] +
                this->currentPlayer->getCurrentTank()->getHeadMatrix()[10] *
                        1000;
    } else {
        if (strcmp(currentPlayer->getPlayer_Type(), "CPU") != 0) {
            Mix_HaltChannel(3);
        }
    }

    if (this->keyMonitor[2]) {
        if ((this->player_factory->getPlayer(currentPlayerIndex)
                     ->getCurrentTank()
                     ->getTurretDegrees() +
             0.1 + 0.4 * this->keyMonitor[2] / 50) < 90) {
            playSFX(TANK_CONTROL1);
            this->player_factory->getPlayer(currentPlayerIndex)
                    ->getCurrentTank()
                    ->rotateTurret(0.1 + 0.4 * this->keyMonitor[2] / 50);
        } else {
            Mix_HaltChannel(2);
            if (Mix_Playing(4) == 0) playSFX(TANK_STUCK);
        }
    } else if (this->keyMonitor[4]) {
        if ((this->player_factory->getPlayer(currentPlayerIndex)
                     ->getCurrentTank()
                     ->getTurretDegrees() -
             0.1 - 0.4 * this->keyMonitor[4] / 50) >= 0) {
            playSFX(TANK_CONTROL1);
            this->player_factory->getPlayer(currentPlayerIndex)
                    ->getCurrentTank()
                    ->rotateTurret(-0.1 - 0.4 * this->keyMonitor[4] / 50);
        } else {
            Mix_HaltChannel(2);
            if (Mix_Playing(4) == 0) playSFX(TANK_STUCK);
        }
    } else {
        if (strcmp(currentPlayer->getPlayer_Type(), "CPU") != 0) {
            Mix_HaltChannel(2);
        }
    }

    if (this->keyMonitor['-'])
        this->player_factory->getPlayer(currentPlayerIndex)
                ->getCurrentTank()
                ->adjustPower(-0.01 - 0.01 * this->keyMonitor['-']);
    if (this->keyMonitor['='])
        this->player_factory->getPlayer(currentPlayerIndex)
                ->getCurrentTank()
                ->adjustPower(0.01 + 0.01 * this->keyMonitor['=']);
}

void GameState::destroyProjectile() {
    this->projectile_fired = false;
    this->chaseCamActive = false;
    delete this->projectile;
    this->projectile = nullptr;
}

bool GameState::getProjectileFired() { return this->projectile_fired; }

void GameState::updateWorldCam() {
    if (this->keyMonitor['w'])
        this->worldCam->moveCam(20 + 20 * this->keyMonitor['w'], 0, 0);
    if (this->keyMonitor['a'])
        this->worldCam->moveCam(0, 0, -20 - 20 * this->keyMonitor['a']);
    if (this->keyMonitor['s'])
        this->worldCam->moveCam(-20 - 20 * this->keyMonitor['s'], 0, 0);
    if (this->keyMonitor['d'])
        this->worldCam->moveCam(0, 0, 20 + 20 * this->keyMonitor['d']);
    if (this->keyMonitor['r'])
        this->worldCam->moveCam(0, -20 - 20 * this->keyMonitor['r'], 0);
    if (this->keyMonitor['f'])
        this->worldCam->moveCam(0, 20 + 20 * this->keyMonitor['f'], 0);
}

void GameState::createSpecialEffect() {
    GLfloat powerRatio = player_factory->getPlayer(currentPlayerIndex)
                                 ->getCurrentTank()
                                 ->getCurrentPower() /
                         10.0;
    player_factory->getPlayer(currentPlayerIndex)
            ->getCurrentTank()
            ->setPreviousPower(static_cast<int>(powerRatio * 1000));
    player_factory->getPlayer(currentPlayerIndex)
            ->getCurrentTank()
            ->setPreviousAngle(
                    static_cast<int>(
                            this->player_factory->getPlayer(currentPlayerIndex)
                                    ->getCurrentTank()
                                    ->getTurretDegrees()) +
                    1);
    player_factory->getPlayer(currentPlayerIndex)
            ->getCurrentTank()
            ->setProjectileLandPos(projectile->getPos()[0],
                                   projectile->getPos()[2]);
    this->specialEffectX = this->projectile->getPos()[0];
    this->specialEffectY = this->projectile->getPos()[1];
    this->specialEffectZ = this->projectile->getPos()[2];
    // Teleport weapon effect handling
    if (strcmp(projectile->getWeapon()->getImageFileName(),
               "WeaponTeleport.raw") == 0) {
        int size = this->global_settings->getCurrentTerrain()->getActualSize();
        // Make sure the projectile has not landed on the water
        if (0 < projectile->getPos()[0] && projectile->getPos()[0] < size &&
            0 < projectile->getPos()[2] && projectile->getPos()[2] < size) {
            int newX = static_cast<int>(projectile->getPos()[0] / 100.0) * 100;
            int newY = static_cast<int>(projectile->getPos()[1] / 100.0) * 100;
            int newZ = static_cast<int>(projectile->getPos()[2] / 100.0) * 100;
            currentPlayer->getCurrentTank()->setTankPos(newX, newY, newZ);
            const GLfloat* bodyMatrix =
                    currentPlayer->getCurrentTank()->getBodyMatrix();
            GLfloat newHeight =
                    this->global_settings->getCurrentTerrain()->getHeightAt(
                            bodyMatrix[14], bodyMatrix[12]);
            int scale = static_cast<int>(
                    global_settings->getCurrentTerrain()->getScale());
            Normal n = this->global_settings->getCurrentTerrain()
                               ->getTriangleNormal(bodyMatrix[12] / scale,
                                                   bodyMatrix[14] / scale);
            currentPlayer->getCurrentTank()->orientTank(&n);
            currentPlayer->getCurrentTank()->setTankPos(
                    bodyMatrix[12], newHeight, bodyMatrix[14]);
        }
    }
    projectile->getWeapon()->playExplosionSFX();
    this->radiusOfCurrentExplosion =
            this->projectile->getWeapon()->getRadius();
    this->specialEffectType = EXPLOSION;

    if (this->specialEffectType == EXPLOSION) {
        this->specialEffectsCount =
                (SPECIAL_EFFECT_TIME_LIMIT / TIME_DIVISORS);
        this->special_effects = new SpecialEffect*[this->specialEffectsCount];
        for (int x = 0; x < this->specialEffectsCount; x++) {
            this->special_effects[x] =
                    new Explosion(this->specialEffectX,
                                  this->specialEffectY,
                                  this->specialEffectZ,
                                  static_cast<int>(radiusOfCurrentExplosion));
            if (this->projectile->getWeapon() != nullptr) {
                this->special_effects[x]->setColors1(
                        this->projectile->getWeapon()->getExplosionColor1());
                this->special_effects[x]->setColors2(
                        this->projectile->getWeapon()->getExplosionColor2());
                this->special_effects[x]->setColors3(
                        this->projectile->getWeapon()->getExplosionColor3());
                this->special_effects[x]->setColors4(
                        this->projectile->getWeapon()->getExplosionColor4());
            }
        }
    }
    this->gameSubState = SPECIAL_EFFECT;
}

void GameState::handleProjectileState() {
    bool collisionOccured = false;
    if (this->projectile) {
        if (this->projectile->getPos()[1] <=
            this->global_settings->getCurrentTerrain()->getHeightAt(
                    this->projectile->getPos()[2],
                    this->projectile->getPos()[0])) {
            collisionOccured = true;
        } else {
            for (int i = 0; i < this->numberOfPlayers; i++) {
                if (this->player_factory->getPlayer(i)
                            ->getCurrentTank()
                            ->checkCollision(this->projectile->getPos()[0],
                                             this->projectile->getPos()[1],
                                             this->projectile->getPos()[2])) {
                    collisionOccured = true;
                }
            }
        }
        if (collisionOccured) {
            /* RESET AI VARIABLES	*/
            this->timer = 0;
            /* DONE RESETING AI VARIABLES */
            this->setPositionOfLastProjectile(this->projectile->getPos()[0],
                                              this->projectile->getPos()[1],
                                              this->projectile->getPos()[2]);
            // Make sure chase cam does not shake till impact
            this->projectile->getChaseCam()->setShakeCam(0);
            this->createSpecialEffect();
        } else {
            this->timer = this->timer + .02f;
            GLfloat scalar = this->projectile->getInitialPositionScalar();
            GLfloat* physicsMatrix =
                    this->currentPlayer->getCurrentTank()->getTurretMatrix();
            GLfloat Xo = this->projectile->getXo();
            GLfloat Yo = this->projectile->getYo();
            GLfloat Zo = this->projectile->getZo();
            ;
            GLfloat Vox = this->projectile->getVox();
            GLfloat Voy = this->projectile->getVoy();
            GLfloat Voz = this->projectile->getVoz();
            GLfloat g = this->gravity;  // Note: gravity is negative

            GLfloat X = Vox * this->timer + Xo;
            GLfloat Y = 0.5f * g * pow(static_cast<double>(this->timer), 2.0) +
                        Voy * this->timer + Yo;
            GLfloat Z = Voz * this->timer + Zo;
            this->projectile->update(X, Y, Z);
        }
    } else {
        GLfloat tankAttributePower =
                player_factory->getPlayer(this->currentPlayerIndex)
                        ->getCurrentTank()
                        ->getPower();
        GLfloat powerBar = player_factory->getPlayer(this->currentPlayerIndex)
                                   ->getCurrentTank()
                                   ->getCurrentPower();
        const GLfloat* turretMatrix =
                player_factory->getPlayer(this->currentPlayerIndex)
                        ->getCurrentTank()
                        ->getTurretMatrix();
        GLfloat matrix[16];
        for (int i = 0; i < 16; i++) {
            matrix[i] = turretMatrix[i];
        }
        matrix[12] = turretMatrix[12] + 1000 * turretMatrix[4];
        matrix[13] = turretMatrix[13] + 1000 * turretMatrix[5];
        matrix[13] = turretMatrix[14] + 1000 * turretMatrix[6];
        this->projectile =
                new Projectile(this,
                               matrix,
                               tankAttributePower * powerBar * BALISTIC_SCALAR,
                               this->projectileModels);
        if (currentPlayer->getLoadedWeapon() != nullptr) {
            projectile->setWeapon(currentPlayer->getLoadedWeapon());
        } else {
            projectile->setWeapon(projectile->getDefaultWeapon());
        }
        this->specialEffectType = EXPLOSION;
        this->createSpecialEffect();
    }
}

void GameState::currentPlayerFire() {
    if (strcmp(currentPlayer->getPlayer_Type(), "HUMAN") == 0) {
        // just in case
        if (this->projectile) {
            this->destroyProjectile();
        }
        this->gameSubState = PROJECTILE;
        this->constructProjectile();
        if (this->projectile) {
            if (currentPlayer->getLoadedWeapon() != nullptr) {
                projectile->setWeapon(currentPlayer->getLoadedWeapon());
            } else {
                projectile->setWeapon(projectile->getDefaultWeapon());
            }
        }
    } else if (strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
        if (this->projectile) {
            this->destroyProjectile();
        }
        this->gameSubState = PROJECTILE;
        this->constructProjectile();
        if (currentPlayer->getLoadedWeapon() != nullptr) {
            if (this->projectile) {
                projectile->setWeapon(currentPlayer->getLoadedWeapon());
                this->chaseCamActive = true;
            }
        } else {
            if (this->projectile) {
                projectile->setWeapon(projectile->getDefaultWeapon());
                this->chaseCamActive = true;
            }
        }
    }
    if (this->projectile) {
        projectile->getWeapon()->playFireSFX();
    }
}

void GameState::constructProjectile() {
    GLfloat tankAttributePower =
            player_factory->getPlayer(this->currentPlayerIndex)
                    ->getCurrentTank()
                    ->getPower();
    GLfloat powerBar = player_factory->getPlayer(this->currentPlayerIndex)
                               ->getCurrentTank()
                               ->getCurrentPower();

    const GLfloat* turretMatrix =
            this->currentPlayer->getCurrentTank()->getTurretMatrix();
    GLfloat scalar = 700.0f;
    // YOU DON'T HAVE PROJECTILE SO SCALAR NEEDS TO BE MODIFIED IN TWO PLACES
    GLfloat pos[3] = {turretMatrix[12] - scalar * turretMatrix[8],
                      turretMatrix[13] - scalar * turretMatrix[9],
                      turretMatrix[14] - scalar * turretMatrix[10]};

    if (pos[1] < this->global_settings->getCurrentTerrain()->getHeightAt(
                         pos[2], pos[0])) {
        this->projectile = nullptr;
    } else {
        this->projectile = new Projectile(
                this,
                player_factory->getPlayer(this->currentPlayerIndex)
                        ->getCurrentTank()
                        ->getTurretMatrix(),
                tankAttributePower * powerBar * BALISTIC_SCALAR,
                this->projectileModels);
    }
}

void GameState::handleSpecialEffectState() {
    specialEffectTimer++;
    if (this->specialEffectTimer < SPECIAL_EFFECT_TIME_LIMIT) {
        if (this->specialEffectTimer == 1) {
            if (strcmp(projectile->getWeapon()->getImageFileName(),
                       "WeaponRevive.raw") != 0) {
                this->global_settings->getCurrentTerrain()->makeCrater(
                        this->projectile->getPos()[0],
                        this->projectile->getPos()[2],
                        this->radiusOfCurrentExplosion);
            }
            int TOTAL_PLAYERS = this->global_settings->getPlayer_Count();
            for (int p = 0; p < TOTAL_PLAYERS; p++) {
                if (this->player_factory->getPlayer(p)->getCurrentTank() !=
                    nullptr) {
                    const GLfloat* bodyMatrix =
                            this->player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->getBodyMatrix();
                    Vertex v0(bodyMatrix[12], bodyMatrix[13], bodyMatrix[14]);
                    Vertex v1(this->projectile->getPos()[0],
                              this->projectile->getPos()[1],
                              this->projectile->getPos()[2]);
                    GLfloat distance = calcDistanceBetweenVertices(&v0, &v1);
                    GLfloat scale = this->global_settings->getCurrentTerrain()
                                            ->getScale();
                    if (distance < this->radiusOfCurrentExplosion * scale) {
                        // PLACEHOLDER BELOW, pass the tank into the weapon and
                        // call dealDamage (or whatever) from there
                        this->projectile->getWeapon()->causeEffectToTank(
                                distance,
                                this->player_factory->getPlayer(p)
                                        ->getCurrentTank());
                        if (this->player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->getDurationFloat() > 0) {
                            Normal* n = new Normal(0, 1, 0);

                            this->player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->orientTank(n);
                            this->player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->setTankPos(
                                            this->player_factory->getPlayer(p)
                                                    ->getCurrentTank()
                                                    ->getBodyMatrix()[12],
                                            this->player_factory->getPlayer(p)
                                                    ->getCurrentTank()
                                                    ->getBodyMatrix()[13],
                                            this->player_factory->getPlayer(p)
                                                    ->getCurrentTank()
                                                    ->getBodyMatrix()[14]);
                            delete n;
                        } else {
                            int size = static_cast<int>(
                                    global_settings->getCurrentTerrain()
                                            ->getActualSize());
                            int scale = static_cast<int>(
                                    global_settings->getCurrentTerrain()
                                            ->getScale());
                            GLfloat newHeight =
                                    this->global_settings->getCurrentTerrain()
                                            ->getHeightAt(bodyMatrix[14],
                                                          bodyMatrix[12]);

                            Normal n =
                                    this->global_settings->getCurrentTerrain()
                                            ->getTriangleNormal(
                                                    bodyMatrix[12] / scale,
                                                    bodyMatrix[14] / scale);

                            player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->orientTank(&n);
                            this->player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->setTankPos(bodyMatrix[12],
                                                 newHeight,
                                                 bodyMatrix[14]);
                            this->player_factory->getPlayer(p)
                                    ->getCurrentTank()
                                    ->checkFallingDamage();
                        }
                    }
                }
            }
        }
        this->projectile->getChaseCam()->updateFactor();
        int shakeItBaby = rand() % 30;
        this->projectile->getChaseCam()->setShakeCam(20 + shakeItBaby);
    } else {
        if (this->specialEffectType == EXPLOSION) {
            for (int x = 0; x < this->specialEffectsCount; x++) {
                if (this->special_effects[x]) {
                    delete this->special_effects[x];
                }
            }

            delete this->special_effects;
        }
        this->projectile->getChaseCam()->setShakeCam(0);
        this->projectile->getChaseCam()->resetFactor();
        this->destroyProjectile();
        this->specialEffectX = 0;
        this->specialEffectY = 0;
        this->specialEffectZ = 0;
        this->specialEffectTimer = 0;
        this->specialEffectsCount = 0;
        this->radiusOfCurrentExplosion = 0;
        this->useTurn();
        this->gameSubState = PASS_TIME;
        int invenIndex = 0;
        this->inventory->handleInventory(currentPlayer, invenIndex);
    }
}

void GameState::handleKeyboardInput(int key, bool key_status) {
    if (strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
        this->handleNonInventoryKeyboard(key, key_status);
    }
    if (strcmp(currentPlayer->getPlayer_Type(), "HUMAN") ==
        0) {  // TEMP TEST FOR CPU PLAYERS REMOVE CPU'S DON'T USE KEYBOARDS
        if (this->gameSubState == INVENTORY) {
            this->handleInventoryKeyboard(key, key_status);
        } else if (this->gameSubState != INVENTORY) {
            this->handleNonInventoryKeyboard(key, key_status);
        }
    }
}

void GameState::handleInventoryKeyboard(int key, bool key_status) {
    if (key_status) {
        if (key == 27 || key == 'i') {
            Mix_VolumeMusic(prevMusicVolume * 3);
            gameSubState = PLAYER_CONTROL;
        } else if (key == 13) {
            Mix_VolumeMusic(prevMusicVolume * 3);
            int index = inventory->getSelectedIndex();
            // UN/LOAD A WEAPON
            if (index < PLAYER_MAX_WEAPONS &&
                currentPlayer->getCurrentWeapons()[index] != nullptr) {
                // UNLOADING
                if (currentPlayer->getLoadedWeapon() != nullptr &&
                    currentPlayer->getLoadedWeapon()->getUNIQUEIDENTIFIER() ==
                            currentPlayer->getCurrentWeapons()[index]
                                    ->getUNIQUEIDENTIFIER()) {
                    playSFX(WEAPON_UNLOAD);
                    delete this->selectedWeaponImg;
                    delete this->selectedWeaponRemain;
                    currentPlayer->setLoadedWeapon(nullptr);
                    selectedWeaponImg = nullptr;
                    selectedWeaponRemain = nullptr;
                }
                // LOADING
                else {
                    playSFX(WEAPON_LOAD);
                    if (selectedWeaponImg != nullptr) {
                        delete this->selectedWeaponImg;
                        delete this->selectedWeaponRemain;
                    }
                    currentPlayer->setLoadedWeapon(
                            currentPlayer->getCurrentWeapons()[index]);
                    this->selectedWeaponImg = new ImageObject(
                            weaponSlot->getXpos() * 1.01,
                            weaponSlot->getYpos() * 1.01,
                            2,
                            weaponSlot->getWidth() * 0.9,
                            weaponSlot->getHeight() * 0.9,
                            0,
                            256,
                            256,
                            const_cast<char*>(currentPlayer->getLoadedWeapon()
                                                      ->getImageFileName()));
                    char* remain = "";
                    char add[10];
                    sprintf(add,
                            "x %d",
                            currentPlayer->getLoadedWeapon()->getRemaining());
                    remain = add;
                    this->selectedWeaponRemain =
                            new TextObject(remain,
                                           0,
                                           0,
                                           3,
                                           GLUT_BITMAP_TIMES_ROMAN_24,
                                           0.6,
                                           0.2,
                                           0.4);
                }
            }
            // USE AN ITEM
            else if (PLAYER_MAX_WEAPONS <= index &&
                     index < PLAYER_MAX_WEAPONS + PLAYER_MAX_ITEMS &&
                     currentPlayer->getCurrentItems()[index -
                                                      PLAYER_MAX_WEAPONS] !=
                             nullptr) {
                // If causeEffectToTank(...) returns true, that means player
                // has used an item which costs 1 turn
                if (currentPlayer
                            ->getCurrentItems()[index - PLAYER_MAX_WEAPONS]
                            ->causeEffectToTank(
                                    currentPlayer->getCurrentTank())) {
                    useTurn();
                }
                currentPlayer->getCurrentItems()[index - PLAYER_MAX_WEAPONS]
                        ->playUseSFX();
                inventory->handleInventory(currentPlayer, index);
                while (Mix_Playing(0));
                inventory->setupInventory(currentPlayer);
                Mix_HaltChannel(0);
            }
            gameSubState = PLAYER_CONTROL;
        } else {
            inventory->keyHandler(key);
        }
    }
}

void GameState::handleNonInventoryKeyboard(int key, bool key_status) {
    if ((key == 'c') && (key_status))
        this->toggleCamera();
    else if ((key == ' ') && (key_status) &&
             (this->gameSubState == PLAYER_CONTROL)) {
        this->currentPlayerFire();
    } else if ((key == 5) && (key_status) &&
               (this->gameSubState == PLAYER_CONTROL)) {
        prevMusicVolume = Mix_VolumeMusic(-1);
        prevMusicVolume = Mix_VolumeMusic(prevMusicVolume / 3);
        playSFX(MANUAL);
        this->gameSubState = HELP;
        needHelp = false;
    } else if ((key == 5) && (key_status) && (this->gameSubState == HELP)) {
        Mix_VolumeMusic(prevMusicVolume * 3);
        this->gameSubState = PLAYER_CONTROL;
        needHelp = false;
    } else if ((key == 6) && (key_status)) {
        Mix_HaltMusic();
        Mix_HaltChannel(-1);
        *currentGameState = MAIN_MENU;
    } else if ((key == 'i') && (key_status) &&
               (this->gameSubState == PLAYER_CONTROL)) {
        if (strcmp(currentPlayer->getPlayer_Type(), "HUMAN") == 0 &&
            currentPlayer->getCurrentTank()->getDurationPadlock() == 0) {
            Mix_HaltChannel(2);
            Mix_HaltChannel(3);
            keyMonitor[1] = 0;
            keyMonitor[2] = 0;
            keyMonitor[3] = 0;
            keyMonitor[4] = 0;
            prevMusicVolume = Mix_VolumeMusic(
                    -1);  // -1 returns the current volume. Other numbers will
                          // change the volume, and it returns the volume
                          // before changed
            prevMusicVolume = Mix_VolumeMusic(prevMusicVolume / 3);
            playSFX(INVENTORY_ACCESS);
            this->gameSubState = INVENTORY;
        } else {
            // playSFX(INVENTORY_INVALID);
        }
    } else if (key == 'b' && !key_status) {
        this->drawHitBox = !this->drawHitBox;
    } else if (key == 'p' && !key_status) {
        if (strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
            if (this->currentPlayer->getDrawDebugLinesandPlanes())
                this->currentPlayer->setDrawDebugLinesandPlanes(false);
            else
                this->currentPlayer->setDrawDebugLinesandPlanes(true);
        }
    } else {
        if (key_status) {
            needHelp = key > 6 && key != 'w' && key != 'a' && key != 's' &&
                       key != 'd' && key != 'r' && key != 'f' && key != 'c' &&
                       key != '-' && key != '=';
            this->keyMonitor[key] += 1;
        } else {
            this->keyMonitor[key] = 0;
        }
    }
}

void GameState::handlePassTime() {
    if ((player_factory->getPlayer(currentPlayerIndex)->getCurrentWait() <=
         0) &&
        (player_factory->getPlayer(currentPlayerIndex)
                 ->getCurrentTank()
                 ->isAlive())) {
        gameSubState = PLAYER_CONTROL;
        this->currentPlayer = player_factory->getPlayer(currentPlayerIndex);
    } else {
        do {
            player_factory->getPlayer(currentPlayerIndex)
                    ->setCurrentWait(
                            player_factory->getPlayer(currentPlayerIndex)
                                    ->getCurrentWait() -
                            1);
            if (!player_factory->getPlayer(currentPlayerIndex)
                         ->getCurrentTank()
                         ->isAlive()) {
                player_factory->getPlayer(currentPlayerIndex)
                        ->setCurrentWait(-1);
            }
            currentPlayerIndex++;
            if (currentPlayerIndex ==
                this->global_settings->getPlayer_Count()) {
                currentPlayerIndex = 0;
            }
            this->currentPlayer =
                    player_factory->getPlayer(currentPlayerIndex);
        } while (player_factory->getPlayer(currentPlayerIndex)
                         ->getCurrentWait() != 0);
    }

    // STATUS EFFECT (ACID)
    if (currentPlayer->getCurrentTank()->getDurationAcid() > 0) {
        playSFX(EFFECT_ACID);
        currentPlayer->getCurrentTank()->dealDamage(
                currentPlayer->getCurrentTank()->getBaseArmor() * 100 * 10 /
                100);
        if (currentPlayer->getCurrentTank()->getHP() <= 0) {
            useTurn();  // skip turn if acid killed him at the beginning of the
                        // turn
        }
        // playSFX(ACID_EFFECT);
    }
    // STATUS EFFECT (PARALYZED)
    if (currentPlayer->getCurrentTank()->getDurationParalyze() > 0) {
        useTurn();
    } else {
        if (strcmp(currentPlayer->getPlayer_Type(), "HUMAN") == 0) {
            inventory->setupInventory(currentPlayer);
            Mix_HaltChannel(0);
            if (selectedWeaponImg != nullptr) {
                delete this->selectedWeaponImg;
                delete this->selectedWeaponRemain;
            }
            if (currentPlayer->getLoadedWeapon() != nullptr) {
                this->selectedWeaponImg = new ImageObject(
                        weaponSlot->getXpos() * 1.01,
                        weaponSlot->getYpos() * 1.01,
                        2,
                        weaponSlot->getWidth() * 0.9,
                        weaponSlot->getHeight() * 0.9,
                        0,
                        256,
                        256,
                        const_cast<char*>(currentPlayer->getLoadedWeapon()
                                                  ->getImageFileName()));
                char* remain = "";
                char add[10];
                sprintf(add,
                        "x %d",
                        currentPlayer->getLoadedWeapon()->getRemaining());
                remain = add;
                this->selectedWeaponRemain =
                        new TextObject(remain,
                                       weaponSlot->getXpos() * 1.1,
                                       weaponSlot->getYpos() * 1.3,
                                       3,
                                       GLUT_BITMAP_TIMES_ROMAN_24,
                                       0.6,
                                       0.2,
                                       0.4);
            } else {
                selectedWeaponImg = nullptr;
                selectedWeaponRemain = nullptr;
            }
        }
    }
}

void GameState::resetTables(int index) {
    if (index == -1) {
        for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
            for (int j = 0; j < this->player_factory->getNumberofPlayers();
                 j++) {
                this->tank_reachable[i][j] = true;
                this->distance_to_target[i][j] = 1E+37;  // MAX FLOAT
            }
        }
    } else if (index < this->player_factory->getNumberofPlayers() &&
               index >= 0) {
        for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
            this->tank_reachable[index][i] = true;
            this->distance_to_target[index][i] = 1E+37;  // MAX FLOAT
        }
    }
}

void GameState::printTables() {
    for (int i = 0; i < this->player_factory->getNumberofPlayers(); i++) {
        for (int j = 0; j < this->player_factory->getNumberofPlayers(); j++) {
            cout << " || " << this->tankList[i][j] << " | "
                 << this->tank_reachable[i][j] << " | "
                 << this->distance_to_target[i][j] << " || ";
        }
        cout << endl;
    }
}

void GameState::controlAI() {
    // this->printTables();
    this->currentPlayer->aiMainLogisticFunction();
}

void GameState::nearestEnemy() {
    if (this->currentPlayer != nullptr &&
        strcmp(currentPlayer->getPlayer_Type(), "CPU") == 0) {
        /*	RECALULATE ALL DISTANCE	*/
        const GLfloat* tankMatrix =
                this->currentPlayer->getCurrentTank()->getBodyMatrix();
        for (int i = 0; i < this->numberOfPlayers; i++) {
            if (this->tankList[this->currentPlayerIndex][i] !=
                        this->currentPlayer->getCurrentTank() &&
                this->tankList[i] != nullptr) {
                const GLfloat* targetMatrix =
                        this->tankList[this->currentPlayerIndex][i]
                                ->getBodyMatrix();
                GLfloat distance =
                        sqrt((tankMatrix[12] - targetMatrix[12]) *
                                     (tankMatrix[12] - targetMatrix[12]) +
                             (tankMatrix[13] - targetMatrix[13]) *
                                     (tankMatrix[13] - targetMatrix[13]) +
                             (tankMatrix[14] - targetMatrix[14]) *
                                     (tankMatrix[14] - targetMatrix[14]));
                this->distance_to_target[this->currentPlayerIndex][i] =
                        distance;
            }
        }

        /*	FIND THE MINIMUM DISTANCE/ REACHABLE TARGET	*/
        int minimum_reachable_tank_index = -1;
        GLfloat minimum_distance = 1E+37;
        for (int i = 0; i < this->numberOfPlayers; i++) {
            if (this->tankList[this->currentPlayerIndex][i] !=
                        this->currentPlayer->getCurrentTank() &&
                this->tankList[this->currentPlayerIndex][i] != nullptr) {
                if (this->distance_to_target[this->currentPlayerIndex][i] <
                            minimum_distance &&
                    this->tank_reachable[this->currentPlayerIndex][i] &&
                    this->tankList[this->currentPlayerIndex][i]->isAlive() &&
                    this->tankList[this->currentPlayerIndex][i]
                                    ->getDurationCloak() == 0 &&
                    (player_factory->getPlayer(currentPlayerIndex)
                                     ->getTeamLabel() !=
                             player_factory->getPlayer(i)->getTeamLabel() ||
                     player_factory->getPlayer(currentPlayerIndex)
                                     ->getTeamLabel() == '-')) {
                    minimum_reachable_tank_index = i;
                    minimum_distance =
                            this->distance_to_target[this->currentPlayerIndex]
                                                    [i];
                }
            }
        }
        /*	CHECK IF TANK FOUND	*/
        if (minimum_reachable_tank_index == -1) {
            this->currentPlayer->setTarget(nullptr);
        } else {
            this->currentPlayer->setTarget(
                    this->tankList[this->currentPlayerIndex]
                                  [minimum_reachable_tank_index]);
            this->tank_reachable[this->currentPlayerIndex]
                                [minimum_reachable_tank_index] = false;
        }
    }
}

GlobalSettings* GameState::getGlobalSettings() {
    return this->global_settings;
}
GLfloat GameState::getGravity() { return this->gravity; }
PlayerFactory* GameState::getPlayerFactory() { return this->player_factory; }
GLfloat GameState::getBalisticScalar() { return this->BALISTIC_SCALAR; }
Vertex GameState::getPositionOfLastProjectile() {
    return this->position_of_last_projectile;
}
void GameState::setPositionOfLastProjectile(GLfloat X, GLfloat Y, GLfloat Z) {
    this->position_of_last_projectile.coordX = X;
    this->position_of_last_projectile.coordY = Y;
    this->position_of_last_projectile.coordZ = Z;
}