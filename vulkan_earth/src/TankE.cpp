#include "TankE.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "MacroCrtdbg.h"

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

    turret_offset[0] = 0;
    turret_offset[1] = 0;
    turret_offset[2] = 0;
    head_offset[0] = 0;
    head_offset[1] = 100;
    head_offset[2] = 0;
    body_offset[0] = 0;
    body_offset[1] = 80;
    body_offset[2] = 0;

    hit_box_height = 200;
    hit_box_length = 300;
    hit_box_width = 300;

    for (int i = 0; i < 3; i++) {
        body_scale[i] = 40;
        head_scale[i] = 40;
        turret_scale[i] = 40;
        wheel_scale[i] = 40;
    }

    power = tank_e_power;
    armor = tank_e_armor;
    speed = tank_e_speed;
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

    vbo_shader_turret->loadClientData("./Eggroid/Eggroid_Turret.ogl");
    vbo_shader_body->loadClientData("./Eggroid/Eggroid_Body.ogl");
    vbo_shader_head->loadClientData("./Eggroid/Eggroid_Head.ogl");
    // vbo_shader_wheel->loadClientData("./Eggroid/Eggroid_Wheel.ogl");

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
