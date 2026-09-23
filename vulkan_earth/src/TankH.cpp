#include "TankH.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "MacroCrtdbg.h"

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

    turret_offset[0] = 0;
    turret_offset[1] = 0;
    turret_offset[2] = -35;
    head_offset[0] = 0;
    head_offset[1] = 65;
    head_offset[2] = 10;
    body_offset[0] = 0;
    body_offset[1] = 80;
    body_offset[2] = 0;

    for (int i = 0; i < 3; i++) {
        body_scale[i] = 70;
        head_scale[i] = 80;
        turret_scale[i] = 70;
    }

    power = tank_h_power;
    armor = tank_h_armor;
    speed = tank_h_speed;
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

    vbo_shader_turret->loadClientData("./Predator/Predator_Turret.ogl");
    vbo_shader_body->loadClientData("./Predator/Predator_Body.ogl");
    vbo_shader_head->loadClientData("./Predator/Predator_Head.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");

    vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    projectile_land_pos[0] = 9999999;
    projectile_land_pos[1] = 9999999;
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
