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

    turret_offset[0] = 0;
    turret_offset[1] = 20;
    turret_offset[2] = -20.0001;
    head_offset[0] = 0;
    head_offset[1] = 80;
    head_offset[2] = 100;
    body_offset[0] = 0;
    body_offset[1] = 80;
    body_offset[2] = 0;

    for (int i = 0; i < 3; i++) {
        body_scale[i] = 60;
        head_scale[i] = 60;
        turret_scale[i] = 60;
        wheel_scale[i] = 60;
    }

    power = tank_f_power;
    armor = tank_f_armor;
    speed = tank_f_speed;
    current_power = 10;
    previous_power = 1000;
    previous_angle = 1;
    hp = armor * 100;

    vbo_shader_head = new VBOShaderLibrary();
    vbo_shader_body = new VBOShaderLibrary();
    vbo_shader_turret = new VBOShaderLibrary();
    // vbo_shader_wheel=new VBOShaderLibrary();
    /*	CODE NEEDED TO USE SHADERS AND VBOS	*/
    if (!(VBOShaderLibrary::initGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::areVbOsSupported())) {
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

    vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);
    // vbo_shader_wheel->LoadTexture("TestImage.raw",1024,1024);

    projectile_land_pos[0] = 9999999;
    projectile_land_pos[1] = 9999999;
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
