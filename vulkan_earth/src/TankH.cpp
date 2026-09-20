#include "TankH.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"

const char* tank_h_name = "Predator";
const int tank_h_hp = 1000;
const int tank_h_power = 10;
const int tank_h_armor = 10;
const int tank_h_speed = 100;

TankH::TankH() = default;
TankH::TankH(GLfloat x, GLfloat y, GLfloat z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turretOffset[0] = 0;
    turretOffset[1] = 0;
    turretOffset[2] = -35;
    headOffset[0] = 0;
    headOffset[1] = 65;
    headOffset[2] = 10;
    bodyOffset[0] = 0;
    bodyOffset[1] = 80;
    bodyOffset[2] = 0;

    for (int i = 0; i < 3; i++) {
        bodyScale[i] = 70;
        headScale[i] = 80;
        turretScale[i] = 70;
    }

    power = tank_h_power;
    armor = tank_h_armor;
    speed = tank_h_speed;
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

    vbo_shader_turret->loadClientData("./Predator/Predator_Turret.ogl");
    vbo_shader_body->loadClientData("./Predator/Predator_Body.ogl");
    vbo_shader_head->loadClientData("./Predator/Predator_Head.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");

    vbo_shader_turret->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->LoadTexture("TestImage.raw", 1024, 1024);

    projectileLandPos[0] = 9999999;
    projectileLandPos[1] = 9999999;
}
TankH::~TankH() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
int TankH::getBaseHP() { return tank_h_hp; }
int TankH::getBasePower() { return tank_h_power; }
int TankH::getBaseArmor() { return tank_h_armor; }
int TankH::getBaseSpeed() { return tank_h_speed; }
std::string TankH::getName() { return tank_h_name; }
