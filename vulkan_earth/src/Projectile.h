#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string>

const int default_damage = 100;
const int default_radius = 5;

class ChaseCam;
class Weapon;
class VBOShaderLibrary;
class GameState;

class Projectile {
public:
    Projectile();
    Projectile(GameState* parent,
               GLfloat* turret_matrix,
               GLfloat speed,
               VBOShaderLibrary** projectile_models);
    ~Projectile();
    void draw();
    // void update(GLfloat gravity);
    void update(GLfloat x, GLfloat y, GLfloat z);
    void chaseView();
    GLfloat* getPos();
    Weapon* getWeapon();
    void setWeapon(Weapon* wpn);
    Weapon* getDefaultWeapon();
    int getDefaultDamage();
    int getDefaultRadius();
    ChaseCam* getChaseCam();
    int getRadius();
    int getDamage();
    GLfloat getVox();
    GLfloat getVoy();
    GLfloat getVoz();
    GLfloat getXo();
    GLfloat getYo();
    GLfloat getZo();
    GLfloat getInitialPositionScalar();

private:
    ChaseCam* chase_cam;
    GLfloat pos[3];
    GLfloat v_vec[3];
    GLfloat speed;
    GLfloat wind;  // implement later
    Weapon* weapon;
    Weapon* default_weapon;
    VBOShaderLibrary* projectile_default;
    VBOShaderLibrary** projectile_models;
    float rotate;
    float y_not;
    GLfloat scalar;
    int timer;
    bool printed;
    GameState* parent;
    GLfloat xo;
    GLfloat yo;
    GLfloat zo;
};

#endif /*	PROJECTILE_H_	*/