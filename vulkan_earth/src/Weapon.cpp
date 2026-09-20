#include "Weapon.h"
#include <string>
#include "ImageObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

using namespace std;

Weapon::Weapon() = default;
Weapon::~Weapon() = default;
/*GETTERS*/
int Weapon::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
float Weapon::getScale() { return this->scale; }
int Weapon::getRemaining() { return remaining; }
std::string Weapon::getImageFileName() { return imageFileName; }
std::string Weapon::getDescription() { return description; }
int Weapon::getPrice() { return price; }
int Weapon::getPackageNum() { return packageNum; }
int Weapon::getMaxStack() { return maxStack; }
float Weapon::getRadius() { return radius; }
int Weapon::getDamage() { return damage; }
GLfloat* Weapon::getExplosionColor1() { return explosionColor1; }
GLfloat* Weapon::getExplosionColor2() { return explosionColor2; }
GLfloat* Weapon::getExplosionColor3() { return explosionColor3; }
GLfloat* Weapon::getExplosionColor4() { return explosionColor4; }
/*SETTERS*/
void Weapon::setScale(float scale) { this->scale = scale; }
void Weapon::setRemaining(int r) { remaining = r; }
void Weapon::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}

void Weapon::playFireSFX() {
    int random = rand() % 3;

    if (random == 0)
        playSFX(TANK_FIRE1);
    else if (random == 1)
        playSFX(TANK_FIRE2);
    else
        playSFX(TANK_FIRE3);
}

void Weapon::playExplosionSFX() {
    int random = rand() % 2;
    Mix_HaltChannel(0);

    if (random == 0)
        playSFX(EXPLOSION1);
    else
        playSFX(EXPLOSION2);
}
