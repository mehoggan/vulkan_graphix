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
                       GLfloat* turret_matrix,
                       GLfloat speed,
                       VBOShaderLibrary** projectile_models) {
    default_weapon = new WeaponDefault(10);
    this->parent = parent;
    scalar = 500;
    pos[0] = turret_matrix[12] - scalar * turret_matrix[8];
    pos[1] = turret_matrix[13] - scalar * turret_matrix[9];
    pos[2] = turret_matrix[14] - scalar * turret_matrix[10];

    xo = pos[0];
    yo = pos[1];
    zo = pos[2];

    /*	THE COORD SYSTEM WE USE HAS X AND Z INVERSED X = -X and Z = -Z	*/
    v_vec[0] = -turret_matrix[8] * speed;
    v_vec[1] = -turret_matrix[9] * speed;
    v_vec[2] = -turret_matrix[10] * speed;

    chase_cam = new ChaseCam(pos, v_vec);
    weapon = nullptr;
    this->projectile_models = projectile_models;

    if (!(VBOShaderLibrary::initGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::areVbOsSupported())) {
        exit(1);
    }
    projectile_default = new VBOShaderLibrary();
    projectile_default->getVBOPointerFunctions();
    projectile_default->loadClientData("Projectiles/projectileDefault.ogl");
    projectile_default->loadShaders("VertexTank.vs", "FragmentTank.vs");
    projectile_default->loadTexture(
            "Projectiles/projectileDefault.raw", 512, 512);

    rotate = 4;
    timer = 0;
    printed = false;
}

Projectile::~Projectile() {
    delete chase_cam;
    delete projectile_default;
}

// void Projectile::update(GLfloat gravity) {
//	pos[0]+=vVec[0];
//	pos[1]+=vVec[1];
//	pos[2]+=vVec[2];
//	vVec[1]+=gravity;
// }

void Projectile::update(GLfloat x, GLfloat y, GLfloat z) {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

void Projectile::draw() {
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glRotatef(rotate, -1, .3, -.4);
    if (weapon == nullptr) {
        glScalef(60, 60, 60);
        projectile_default->drawClientData();
    } else {
        glScalef(weapon->getScale(), weapon->getScale(), weapon->getScale());
        projectile_models[weapon->getUNIQUEIDENTIFIER()]->drawClientData();
    }
    glPopMatrix();
    rotate += 4;
}

GLfloat* Projectile::getPos() { return pos; }

void Projectile::chaseView() { chase_cam->view(); }

Weapon* Projectile::getWeapon() { return weapon; }
void Projectile::setWeapon(Weapon* wpn) { weapon = wpn; }
Weapon* Projectile::getDefaultWeapon() { return default_weapon; }
int Projectile::getDefaultDamage() { return default_damage; }
int Projectile::getDefaultRadius() { return default_radius; }
ChaseCam* Projectile::getChaseCam() { return chase_cam; }
int Projectile::getRadius() { return default_radius; }
int Projectile::getDamage() { return default_damage; }
GLfloat Projectile::getInitialPositionScalar() { return scalar; }
GLfloat Projectile::getVox() { return v_vec[0]; }
GLfloat Projectile::getVoy() { return v_vec[1]; }
GLfloat Projectile::getVoz() { return v_vec[2]; }
GLfloat Projectile::getXo() { return xo; }
GLfloat Projectile::getYo() { return yo; }
GLfloat Projectile::getZo() { return zo; }