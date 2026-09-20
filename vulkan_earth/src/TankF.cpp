#include "TankF.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"

const char* tank_f_name = "Behemoth";
const int tank_f_hp = 1000;
const int tank_f_power = 8;
const int tank_f_armor = 8;
const int tank_f_speed = 20;

TankF::TankF() = default;
TankF::TankF(GLfloat x, GLfloat y, GLfloat z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turretOffset[0] = 0;
    turretOffset[1] = 20;
    turretOffset[2] = -20.0001;
    headOffset[0] = 0;
    headOffset[1] = 80;
    headOffset[2] = 100;
    bodyOffset[0] = 0;
    bodyOffset[1] = 80;
    bodyOffset[2] = 0;

    for (int i = 0; i < 3; i++) {
        bodyScale[i] = 60;
        headScale[i] = 60;
        turretScale[i] = 60;
        wheelScale[i] = 60;
    }

    power = tank_f_power;
    armor = tank_f_armor;
    speed = tank_f_speed;
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

    vbo_shader_turret->loadClientData("./Behemoth/Behemoth_Turret.ogl");
    vbo_shader_body->loadClientData("./Behemoth/Behemoth_Body.ogl");
    vbo_shader_head->loadClientData("./Behemoth/Behemoth_Head.ogl");
    // vbo_shader_wheel->loadClientData("./Behemoth/Behemoth_Wheel.ogl");

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
TankF::~TankF() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
    // delete vbo_shader_wheel;
}

// GETTERS
int TankF::getBaseHP() { return tank_f_hp; }
int TankF::getBasePower() { return tank_f_power; }
int TankF::getBaseArmor() { return tank_f_armor; }
int TankF::getBaseSpeed() { return tank_f_speed; }
std::string TankF::getName() { return tank_f_name; }
