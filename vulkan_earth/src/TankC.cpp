#include "TankC.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"

const char* tank_c_name = "HeavyD";
const int tank_c_hp = 1000;
const int tank_c_power = 10;
const int tank_c_armor = 7;
const int tank_c_speed = 5;

TankC::TankC() = default;
TankC::TankC(GLfloat x, GLfloat y, GLfloat z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turretOffset[0] = 0;
    turretOffset[1] = 70;
    turretOffset[2] = 100.0001;
    headOffset[0] = 0;
    headOffset[1] = 0;
    headOffset[2] = 0;
    bodyOffset[0] = 0;
    bodyOffset[1] = 90;
    bodyOffset[2] = 0;

    for (int i = 0; i < 3; i++) {
        bodyScale[i] = 80;
        headScale[i] = 80;
        turretScale[i] = 80;
        wheelScale[i] = 80;
    }

    power = tank_c_power;
    armor = tank_c_armor;
    speed = tank_c_speed;
    currentPower = 10;
    previousPower = 1000;
    previousAngle = 1;
    HP = armor * 100;

    vbo_shader_head = new VBOShaderLibrary();
    vbo_shader_body = new VBOShaderLibrary();
    vbo_shader_turret = new VBOShaderLibrary();
    /*	CODE NEEDED TO USE SHADERS AND VBOS	*/
    if (!(VBOShaderLibrary::InitGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::AreVBOsSupported())) {
        exit(1);
    }

    vbo_shader_turret->getVBOPointerFunctions();
    vbo_shader_body->getVBOPointerFunctions();
    vbo_shader_head->getVBOPointerFunctions();

    vbo_shader_turret->loadClientData("./HeavyD/HeavyD_Turret.ogl");
    vbo_shader_body->loadClientData("./HeavyD/HeavyD_Body.ogl");
    vbo_shader_head->loadClientData("./HeavyD/HeavyD_Head.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");

    vbo_shader_turret->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->LoadTexture("TestImage.raw", 1024, 1024);

    projectileLandPos[0] = 9999999;
    projectileLandPos[1] = 9999999;
}
TankC::~TankC() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
int TankC::getBaseHP() { return tank_c_hp; }
int TankC::getBasePower() { return tank_c_power; }
int TankC::getBaseArmor() { return tank_c_armor; }
int TankC::getBaseSpeed() { return tank_c_speed; }
std::string TankC::getName() { return tank_c_name; }

void TankC::drawTankHitBox() {
    /*Vertex tankPos(headMatrix[12],headMatrix[13],headMatrix[14]);
    Vector right(-headMatrix[0],-headMatrix[1],-headMatrix[2]);
    Vector up(headMatrix[4],headMatrix[5],headMatrix[6]);
    Vector at(-headMatrix[8],-headMatrix[9],-headMatrix[10]);

    Vector left(headMatrix[0],headMatrix[1],headMatrix[2]);
    Vector down(-headMatrix[4],-headMatrix[5],-headMatrix[6]);
    Vector back(headMatrix[8],headMatrix[9],headMatrix[10]);*/
    glBegin(GL_QUADS);
    // top
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glEnd();

    glBegin(GL_QUADS);
    // right
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glEnd();

    glBegin(GL_QUADS);
    // left
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glEnd();

    glBegin(GL_QUADS);
    // front
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(0.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glEnd();

    glBegin(GL_QUADS);
    // back
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(0.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glEnd();
}

void TankC::updateHitBox() {
    tankPos.coordX = headMatrix[12];
    tankPos.coordY = headMatrix[13];
    tankPos.coordZ = headMatrix[14];
    right.compoX = -headMatrix[0];
    right.compoY = -headMatrix[1];
    right.compoZ = -headMatrix[2];
    left.compoX = headMatrix[0];
    left.compoY = headMatrix[1];
    left.compoZ = headMatrix[2];
    up.compoX = headMatrix[4];
    up.compoY = headMatrix[5];
    up.compoZ = headMatrix[6];
    down.compoX = -headMatrix[4];
    down.compoY = -headMatrix[5];
    down.compoZ = -headMatrix[6];
    at.compoX = -headMatrix[8];
    at.compoY = -headMatrix[9];
    at.compoZ = -headMatrix[10];
    back.compoX = headMatrix[8];
    back.compoY = headMatrix[9];
    back.compoZ = headMatrix[10];
}