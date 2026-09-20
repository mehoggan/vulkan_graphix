#include "TankB.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"

const char* tank_b_name = "Hellfire";
const int tank_b_hp = 1000;
const int tank_b_power = 7;
const int tank_b_armor = 5;
const int tank_b_speed = 50;

TankB::TankB() = default;
TankB::TankB(GLfloat x, GLfloat y, GLfloat z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turretOffset[0] = 0;
    turretOffset[1] = 0.0001;
    turretOffset[2] = -50.0001;
    headOffset[0] = 0;
    headOffset[1] = 70;
    headOffset[2] = 0;
    bodyOffset[0] = 0;
    bodyOffset[1] = 65;
    bodyOffset[2] = 0;

    for (int i = 0; i < 3; i++) {
        bodyScale[i] = 50;
        headScale[i] = 50;
        turretScale[i] = 50;
        wheelScale[i] = 50;
    }

    power = tank_b_power;
    armor = tank_b_armor;
    speed = tank_b_speed;
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

    vbo_shader_turret->loadClientData("./Hellfire/Hellfire_Turret.ogl");
    vbo_shader_body->loadClientData("./Hellfire/Hellfire_Body.ogl");
    vbo_shader_head->loadClientData("./Hellfire/Hellfire_Head.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");

    vbo_shader_turret->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->LoadTexture("TestImage.raw", 1024, 1024);

    projectileLandPos[0] = 9999999;
    projectileLandPos[1] = 9999999;
}
TankB::~TankB() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
int TankB::getBaseHP() { return tank_b_hp; }
int TankB::getBasePower() { return tank_b_power; }
int TankB::getBaseArmor() { return tank_b_armor; }
int TankB::getBaseSpeed() { return tank_b_speed; }
std::string TankB::getName() { return tank_b_name; }
