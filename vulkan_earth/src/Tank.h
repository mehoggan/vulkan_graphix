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
    void setPreviousPower(int previousPower);
    void setPreviousAngle(int previousAngle);
    void setProjectileLandPos(GLfloat x, GLfloat y);
    int getPreviousPower();
    int getPreviousAngle();
    GLfloat* getProjectileLandPos();
    void NormalizeVector(Vector* v);
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
    void changeHeadTexture(int currentPlayerIndex);
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
    GLfloat bodyPos[3];
    GLfloat headPos[3];
    GLfloat turretPos[3];
    GLfloat wheelPos[3];
    GLfloat bodyRight[3];
    GLfloat headRight[3];
    GLfloat turretRight[3];
    GLfloat wheelRight[3];
    GLfloat bodyUp[3];
    GLfloat headUp[3];
    GLfloat turretUp[3];
    GLfloat wheelUp[3];
    GLfloat bodyAt[3];
    GLfloat headAt[3];
    GLfloat turretAt[3];
    GLfloat wheelAt[3];
    GLfloat bodyColor[4];
    GLfloat headColor[4];
    GLfloat turretColor[4];
    GLfloat wheelColor[4];
    GLfloat bodyScale[3];
    GLfloat headScale[3];
    GLfloat turretScale[3];
    GLfloat wheelScale[3];
    GLfloat bodyMatrix[16];
    GLfloat headMatrix[16];
    GLfloat turretMatrix[16];
    GLfloat wheelMatrix[16];
    GLfloat rotate_degrees;
    GLfloat turret_degrees;
    GLfloat wheel_degrees;
    Normal alignmentVector;
    Normal rotate_about;

    GLfloat turretOffset[3];
    GLfloat headOffset[3];
    GLfloat bodyOffset[3];

    GLfloat hitBoxLength;
    GLfloat hitBoxHeight;
    GLfloat hitBoxWidth;

    VBOShaderLibrary* vbo_shader_turret;
    VBOShaderLibrary* vbo_shader_body;
    VBOShaderLibrary* vbo_shader_head;
    VBOShaderLibrary* vbo_shader_wheel;

    bool tankAlive;
    ParticleGenerator* smokeGen;
    ParticleGenerator* acidGen;
    ParticleGenerator* floatGen;

    GLfloat currentPower;
    int previousPower;
    int previousAngle;
    int previousHeight;
    int currentHeight;

    int HP;
    int power;
    int armor;
    int speed;

    GLfloat projectileLandPos[2];

    int durationAcid;
    int durationShield;
    int durationEMP;
    int durationFloat;
    int durationDoubleAction;
    int durationPadlock;
    int durationCloak;
    int durationParalyze;

    Vertex tankPos;
    Vector right;
    Vector up;
    Vector at;
    Vector left;
    Vector down;
    Vector back;
};

#endif