#include "TankD.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"

const char* tank_d_name = "Panzer";
const int tank_d_hp = 1000;
const int tank_d_power = 9;
const int tank_d_armor = 5;
const int tank_d_speed = 30;

TankD::TankD() = default;
TankD::TankD(GLfloat x, GLfloat y, GLfloat z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turretOffset[0] = 0;
    turretOffset[1] = 10;
    turretOffset[2] = 0.0001;
    headOffset[0] = 0;
    headOffset[1] = 70;
    headOffset[2] = 40;
    bodyOffset[0] = 0;
    bodyOffset[1] = 45;
    bodyOffset[2] = 0;

    for (int i = 0; i < 3; i++) {
        bodyScale[i] = 50;
        headScale[i] = 50;
        turretScale[i] = 50;
        wheelScale[i] = 50;
    }

    power = tank_d_power;
    armor = tank_d_armor;
    speed = tank_d_speed;
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

    vbo_shader_turret->loadClientData("./Panzer/Panzer_Turret.ogl");
    vbo_shader_body->loadClientData("./Panzer/Panzer_Body.ogl");
    vbo_shader_head->loadClientData("./Panzer/Panzer_Head.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");

    vbo_shader_turret->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->LoadTexture("TestImage.raw", 1024, 1024);

    projectileLandPos[0] = 9999999;
    projectileLandPos[1] = 9999999;
}
TankD::~TankD() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
int TankD::getBaseHP() { return tank_d_hp; }
int TankD::getBasePower() { return tank_d_power; }
int TankD::getBaseArmor() { return tank_d_armor; }
int TankD::getBaseSpeed() { return tank_d_speed; }
std::string TankD::getName() { return tank_d_name; }
