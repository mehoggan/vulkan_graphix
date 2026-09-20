#include "Projectile.h"
#include <iostream>
#include <sstream>
#include "ChaseCam.h"
#include "GameState.h"
#include "Normal.h"
#include "VBOShaderLibrary.h"
#include "Vector.h"
#include "Weapon.h"
#include "WeaponDefault.h"
#include "macro_crtdbg.h"

using namespace std;

Projectile::Projectile() = default;

Projectile::Projectile(GameState* parent,
                       GLfloat* turretMatrix,
                       GLfloat speed,
                       VBOShaderLibrary** projectileModels) {
    defaultWeapon = new WeaponDefault(10);
    this->parent = parent;
    scalar = 500;
    pos[0] = turretMatrix[12] - scalar * turretMatrix[8];
    pos[1] = turretMatrix[13] - scalar * turretMatrix[9];
    pos[2] = turretMatrix[14] - scalar * turretMatrix[10];

    Xo = pos[0];
    Yo = pos[1];
    Zo = pos[2];

    /*	THE COORD SYSTEM WE USE HAS X AND Z INVERSED X = -X and Z = -Z	*/
    vVec[0] = -turretMatrix[8] * speed;
    vVec[1] = -turretMatrix[9] * speed;
    vVec[2] = -turretMatrix[10] * speed;

    chaseCam = new ChaseCam(pos, vVec);
    weapon = nullptr;
    this->projectileModels = projectileModels;

    if (!(VBOShaderLibrary::InitGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::AreVBOsSupported())) {
        exit(1);
    }
    projectileDefault = new VBOShaderLibrary();
    projectileDefault->getVBOPointerFunctions();
    projectileDefault->loadClientData("Projectiles/projectileDefault.ogl");
    projectileDefault->loadShaders("VertexTank.vs", "FragmentTank.vs");
    projectileDefault->LoadTexture(
            "Projectiles/projectileDefault.raw", 512, 512);

    rotate = 4;
    timer = 0;
    printed = false;
}

Projectile::~Projectile() {
    delete chaseCam;
    delete projectileDefault;
}

// void Projectile::update(GLfloat gravity) {
//	pos[0]+=vVec[0];
//	pos[1]+=vVec[1];
//	pos[2]+=vVec[2];
//	vVec[1]+=gravity;
// }

void Projectile::update(GLfloat X, GLfloat Y, GLfloat Z) {
    pos[0] = X;
    pos[1] = Y;
    pos[2] = Z;
}

void Projectile::draw() {
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glRotatef(rotate, -1, .3, -.4);
    if (weapon == nullptr) {
        glScalef(60, 60, 60);
        projectileDefault->drawClientData();
    } else {
        glScalef(weapon->getScale(), weapon->getScale(), weapon->getScale());
        projectileModels[weapon->getUNIQUEIDENTIFIER()]->drawClientData();
    }
    glPopMatrix();
    rotate += 4;
}

GLfloat* Projectile::getPos() { return pos; }

void Projectile::chaseView() { chaseCam->view(); }

Weapon* Projectile::getWeapon() { return weapon; }
void Projectile::setWeapon(Weapon* wpn) { weapon = wpn; }
Weapon* Projectile::getDefaultWeapon() { return defaultWeapon; }
int Projectile::getDefaultDamage() { return DEFAULT_DAMAGE; }
int Projectile::getDefaultRadius() { return DEFAULT_RADIUS; }
ChaseCam* Projectile::getChaseCam() { return chaseCam; }
int Projectile::getRadius() { return DEFAULT_RADIUS; }
int Projectile::getDamage() { return DEFAULT_DAMAGE; }
GLfloat Projectile::getInitialPositionScalar() { return scalar; }
GLfloat Projectile::getVox() { return vVec[0]; }
GLfloat Projectile::getVoy() { return vVec[1]; }
GLfloat Projectile::getVoz() { return vVec[2]; }
GLfloat Projectile::getXo() { return Xo; }
GLfloat Projectile::getYo() { return Yo; }
GLfloat Projectile::getZo() { return Zo; }