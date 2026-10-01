#ifndef TANK_H
#define TANK_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string>

// class Normal;
#include "vulkan_earth/Normal.h"  //THIS IS FOR DEBUGGING PURPOSES ONLY BAD STYLE
#include "vulkan_earth/Vector.h"
#include "vulkan_earth/Vertex.h"

class ParticleGenerator;
class VBOShaderLibrary;

class Tank {
public:
    Tank();
    virtual ~Tank();
    float calcAngleBetweenVectors(Vector one, Vector two);

    // GETTERS
    Normal getAlignmentVector();
    Normal getRotateAbout();
    void resetTurret();
    void rotateHead(float degrees);
    void rotateTurret(float degrees);
    void rotateWheel(float degrees);
    void adjustPower(float amount);
    float getCurrentPower();
    void setPreviousPower(int new_previous_power);
    void setPreviousAngle(int new_previous_angle);
    void setProjectileLandPos(float x, float y);
    int getPreviousPower();
    int getPreviousAngle();
    float* getProjectileLandPos();
    const float* getBodyMatrix();
    const float* getBodyColor();
    float* getBodyScale();
    const float* getHeadMatrix();
    const float* getHeadColor();
    float* getHeadScale();
    float* getTurretMatrix();
    const float* getTurretColor();
    float* getTurretScale();
    float getTurretDegrees();
    const float* getWheelMatrix();
    const float* getWheelColor();
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
    void setTankPos(float x, float y, float z);
    void orientTank(Normal* n);

    void setBodyColor(float r, float g, float b, float a);
    void setBodyScale(float x, float y, float z);

    void setHeadColor(float r, float g, float b, float a);
    void setHeadScale(float x, float y, float z);

    void setTurretColor(float r, float g, float b, float a);
    void setTurretScale(float x, float y, float z);

    void setWheelColor(float r, float g, float b, float a);
    void setWheelScale(float x, float y, float z);

    void setHP(int new_hp);
    void setPower(int p);
    void setArmor(int a);
    void setSpeed(int d);

    int getCurrentHeight();
    void setCurrentHeight(int curr_height);
    int getPreviousHeight();
    void setPreviousHeight(int prev_height);

    // Other functions
    bool checkCollision(float x, float y, float z);
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
    float body_pos[3];
    float head_pos[3];
    float turret_pos[3];
    float wheel_pos[3];
    float body_right[3];
    float head_right[3];
    float turret_right[3];
    float wheel_right[3];
    float body_up[3];
    float head_up[3];
    float turret_up[3];
    float wheel_up[3];
    float body_at[3];
    float head_at[3];
    float turret_at[3];
    float wheel_at[3];
    float body_color[4];
    float head_color[4];
    float turret_color[4];
    float wheel_color[4];
    float body_scale[3];
    float head_scale[3];
    float turret_scale[3];
    float wheel_scale[3];
    float body_matrix[16];
    float head_matrix[16];
    float turret_matrix[16];
    float wheel_matrix[16];
    float rotate_degrees;
    float turret_degrees;
    float wheel_degrees;
    Normal alignment_vector;
    Normal rotate_about;

    float turret_offset[3];
    float head_offset[3];
    float body_offset[3];

    float hit_box_length;
    float hit_box_height;
    float hit_box_width;

    VBOShaderLibrary* vbo_shader_turret;
    VBOShaderLibrary* vbo_shader_body;
    VBOShaderLibrary* vbo_shader_head;
    VBOShaderLibrary* vbo_shader_wheel;

    bool tank_alive;
    ParticleGenerator* smoke_gen;
    ParticleGenerator* acid_gen;
    ParticleGenerator* float_gen;

    float current_power;
    int previous_power;
    int previous_angle;
    int previous_height;
    int current_height;

    int hp;
    int power;
    int armor;
    int speed;

    float projectile_land_pos[2];

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