#include "TankB.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "MacroCrtdbg.h"

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

    turret_offset[0] = 0;
    turret_offset[1] = 0.0001;
    turret_offset[2] = -50.0001;
    head_offset[0] = 0;
    head_offset[1] = 70;
    head_offset[2] = 0;
    body_offset[0] = 0;
    body_offset[1] = 65;
    body_offset[2] = 0;

    for (int i = 0; i < 3; i++) {
        body_scale[i] = 50;
        head_scale[i] = 50;
        turret_scale[i] = 50;
        wheel_scale[i] = 50;
    }

    power = tank_b_power;
    armor = tank_b_armor;
    speed = tank_b_speed;
    current_power = 10;
    previous_power = 1000;
    previous_angle = 1;
    hp = armor * 100;

    vbo_shader_head = new VBOShaderLibrary();
    vbo_shader_body = new VBOShaderLibrary();
    vbo_shader_turret = new VBOShaderLibrary();

    /*	CODE NEEDED TO USE SHADERS AND VBOS	*/
    if (!(VBOShaderLibrary::initGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::areVbOsSupported())) {
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

    vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    projectile_land_pos[0] = 9999999;
    projectile_land_pos[1] = 9999999;
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
