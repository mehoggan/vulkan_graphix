#include "TankE.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"

const char* tank_e_name = "Eggroid";
const int tank_e_hp = 1000;
const int tank_e_power = 4;
const int tank_e_armor = 3;
const int tank_e_speed = 80;

TankE::TankE() = default;
TankE::TankE(GLfloat x, GLfloat y, GLfloat z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turretOffset[0] = 0;
    turretOffset[1] = 0;
    turretOffset[2] = 0;
    headOffset[0] = 0;
    headOffset[1] = 100;
    headOffset[2] = 0;
    bodyOffset[0] = 0;
    bodyOffset[1] = 80;
    bodyOffset[2] = 0;

    hitBoxHeight = 200;
    hitBoxLength = 300;
    hitBoxWidth = 300;

    for (int i = 0; i < 3; i++) {
        bodyScale[i] = 40;
        headScale[i] = 40;
        turretScale[i] = 40;
        wheelScale[i] = 40;
    }

    power = tank_e_power;
    armor = tank_e_armor;
    speed = tank_e_speed;
    currentPower = 10;
    previousPower = 1000;
    previousAngle = 1;
    HP = armor * 100;

    vbo_shader_head = new VBOShaderLibrary();
    vbo_shader_body = new VBOShaderLibrary();
    vbo_shader_turret = new VBOShaderLibrary();
    // vbo_shader_wheel=new VBOShaderLibrary();
    /*	CODE NEEDED TO USE SHADERS AND VBOS	*/
    if (!(VBOShaderLibrary::InitGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::AreVBOsSupported())) {
        exit(1);
    }

    vbo_shader_turret->getVBOPointerFunctions();
    vbo_shader_body->getVBOPointerFunctions();
    vbo_shader_head->getVBOPointerFunctions();
    // vbo_shader_wheel->getVBOPointerFunctions();

    vbo_shader_turret->loadClientData("./Eggroid/Eggroid_Turret.ogl");
    vbo_shader_body->loadClientData("./Eggroid/Eggroid_Body.ogl");
    vbo_shader_head->loadClientData("./Eggroid/Eggroid_Head.ogl");
    // vbo_shader_wheel->loadClientData("./Eggroid/Eggroid_Wheel.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");
    // vbo_shader_wheel->loadShaders("VertexTank.vs","FragmentTank.vs");

    vbo_shader_turret->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->LoadTexture("TestImage.raw", 1024, 1024);
    // vbo_shader_wheel->LoadTexture("TestImage.raw",1024,1024);

    projectileLandPos[0] = 9999999;
    projectileLandPos[1] = 9999999;
}
TankE::~TankE() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
    // delete vbo_shader_wheel;
}

// GETTERS
int TankE::getBaseHP() { return tank_e_hp; }
int TankE::getBasePower() { return tank_e_power; }
int TankE::getBaseArmor() { return tank_e_armor; }
int TankE::getBaseSpeed() { return tank_e_speed; }
std::string TankE::getName() { return tank_e_name; }
