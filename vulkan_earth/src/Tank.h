#ifndef TANK_H
#define TANK_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string>

// class Normal;
#include "Normal.h"  //THIS IS FOR DEBUGGING PURPOSES ONLY BAD STYLE
#include "Vector.h"
#include "Vertex.h"

class ParticleGenerator;
class VBOShaderLibrary;

class Tank {
public:
    Tank();
    virtual ~Tank();
    GLfloat calcAngleBetweenVectors(Vector one, Vector two);

    // GETTERS
    Normal getAlignmentVector();
    Normal getRotateAbout();
    void resetTurret();
    void rotateHead(GLfloat degrees);
    void rotateTurret(GLfloat degrees);
    void rotateWheel(GLfloat degrees);
    void adjustPower(GLfloat amount);
    GLfloat getCurrentPower();
    void setPreviousPower(int previous_power);
    void setPreviousAngle(int previous_angle);
    void setProjectileLandPos(GLfloat x, GLfloat y);
    int getPreviousPower();
    int getPreviousAngle();
    GLfloat* getProjectileLandPos();
    void normalizeVector(Vector* v);
    const GLfloat* getBodyMatrix();
    const GLfloat* getBodyColor();
    float* getBodyScale();
    const GLfloat* getHeadMatrix();
    const GLfloat* getHeadColor();
    float* getHeadScale();
    GLfloat* getTurretMatrix();
    const GLfloat* getTurretColor();
    float* getTurretScale();
    GLfloat getTurretDegrees();
    const GLfloat* getWheelMatrix();
    const GLfloat* getWheelColor();
    float* getWheelScale();
    int getHP();
    int getPower();
    int getArmor();
    int getSpeed();
    Tank* getTankPointer();
    virtual int getBaseHP();
    virtual int getBasePower();
    virtual int getBaseArmor();
    virtual int getBaseSpeed();
    virtual std::string getName();

    // SETTERS
    void setTankPos(GLfloat x, GLfloat y, GLfloat z);
    void orientTank(Normal* n);

    void setBodyColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void setBodyScale(float x, float y, float z);

    void setHeadColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void setHeadScale(float x, float y, float z);

    void setTurretColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void setTurretScale(float x, float y, float z);

    void setWheelColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void setWheelScale(float x, float y, float z);

    void setHP(int hp);
    void setPower(int p);
    void setArmor(int a);
    void setSpeed(int d);

    int getCurrentHeight();
    void setCurrentHeight(int curr_height);
    int getPreviousHeight();
    void setPreviousHeight(int prev_height);

    // Other functions
    bool checkCollision(GLfloat x, GLfloat y, GLfloat z);
    void fire();
    void keyHandler();
    void draw();
    virtual void drawTankHitBox();
    virtual void updateHitBox();
    void changeHeadTexture(int current_player_index);
    void dealDamage(int damage);
    void checkFallingDamage();
    bool isAlive();
    void tankRevive();

    void initBody();
    void initHead();
    void initTurret();
    void initWheel();
    void initDuration();

    // Getters for durations
    int getDurationAcid();
    int getDurationShield();
    int getDurationEMP();
    int getDurationFloat();
    int getDurationDoubleAction();
    int getDurationPadlock();
    int getDurationCloak();
    int getDurationParalyze();
    // Setters for durations
    void setDurationAcid(int value);
    void setDurationShield(int value);
    void setDurationEMP(int value);
    void setDurationFloat(int value);
    void setDurationDoubleAction(int value);
    void setDurationPadlock(int value);
    void setDurationCloak(int value);
    void setDurationParalyze(int value);

    void setDurationAllPassTurn();

    void printTurretMatrix();
    void printBodyMatrix();
    void printHeadMatrix();

protected:
    GLfloat body_pos[3];
    GLfloat head_pos[3];
    GLfloat turret_pos[3];
    GLfloat wheel_pos[3];
    GLfloat body_right[3];
    GLfloat head_right[3];
    GLfloat turret_right[3];
    GLfloat wheel_right[3];
    GLfloat body_up[3];
    GLfloat head_up[3];
    GLfloat turret_up[3];
    GLfloat wheel_up[3];
    GLfloat body_at[3];
    GLfloat head_at[3];
    GLfloat turret_at[3];
    GLfloat wheel_at[3];
    GLfloat body_color[4];
    GLfloat head_color[4];
    GLfloat turret_color[4];
    GLfloat wheel_color[4];
    GLfloat body_scale[3];
    GLfloat head_scale[3];
    GLfloat turret_scale[3];
    GLfloat wheel_scale[3];
    GLfloat body_matrix[16];
    GLfloat head_matrix[16];
    GLfloat turret_matrix[16];
    GLfloat wheel_matrix[16];
    GLfloat rotate_degrees;
    GLfloat turret_degrees;
    GLfloat wheel_degrees;
    Normal alignment_vector;
    Normal rotate_about;

    GLfloat turret_offset[3];
    GLfloat head_offset[3];
    GLfloat body_offset[3];

    GLfloat hit_box_length;
    GLfloat hit_box_height;
    GLfloat hit_box_width;

    VBOShaderLibrary* vbo_shader_turret;
    VBOShaderLibrary* vbo_shader_body;
    VBOShaderLibrary* vbo_shader_head;
    VBOShaderLibrary* vbo_shader_wheel;

    bool tank_alive;
    ParticleGenerator* smoke_gen;
    ParticleGenerator* acid_gen;
    ParticleGenerator* float_gen;

    GLfloat current_power;
    int previous_power;
    int previous_angle;
    int previous_height;
    int current_height;

    int hp;
    int power;
    int armor;
    int speed;

    GLfloat projectile_land_pos[2];

    int duration_acid;
    int duration_shield;
    int duration_emp;
    int duration_float;
    int duration_double_action;
    int duration_padlock;
    int duration_cloak;
    int duration_paralyze;

    Vertex tank_pos;
    Vector right;
    Vector up;
    Vector at;
    Vector left;
    Vector down;
    Vector back;
};

#endif