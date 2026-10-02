#include "vulkan_earth/TankA.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/MacroCrtdbg.h"

const char* tank_a_name = "Rhinoxx";
const std::int32_t tank_a_hp = 1000;
const std::int32_t tank_a_power = 5;
const std::int32_t tank_a_armor = 7;
const std::int32_t tank_a_speed = 40;

TankA::TankA() = default;
TankA::TankA(float x, float y, float z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turret_offset[0] = 0;
    turret_offset[1] = 40;
    turret_offset[2] = 70;
    head_offset[0] = 0;
    head_offset[1] = 0;
    head_offset[2] = 0;
    body_offset[0] = 0;
    body_offset[1] = 70;
    body_offset[2] = 0;

    for (std::int32_t i = 0; i < 3; i++) {
        body_scale[i] = 80;
        head_scale[i] = 80;
        turret_scale[i] = 80;
        wheel_scale[i] = 80;
    }

    power = tank_a_power;
    armor = tank_a_armor;
    speed = tank_a_speed;
    current_power = 10;
    previous_power = 1000;
    previous_angle = 1;
    hp = armor * 100;

    vbo_shader_head = new VBOShaderLibrary();
    vbo_shader_body = new VBOShaderLibrary();
    vbo_shader_turret = new VBOShaderLibrary();

    vbo_shader_turret->loadClientData("./Rhinoxx_Tank/Rhinoxx_Turret.ogl");
    vbo_shader_body->loadClientData("./Rhinoxx_Tank/Rhinoxx_Body.ogl");
    vbo_shader_head->loadClientData("./Rhinoxx_Tank/Rhinoxx_Head.ogl");

    vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    projectile_land_pos[0] = 9999999;
    projectile_land_pos[1] = 9999999;
}
TankA::~TankA() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
std::int32_t TankA::getBaseHP() { return tank_a_hp; }
std::int32_t TankA::getBasePower() { return tank_a_power; }
std::int32_t TankA::getBaseArmor() { return tank_a_armor; }
std::int32_t TankA::getBaseSpeed() { return tank_a_speed; }
std::string TankA::getName() { return tank_a_name; }

void TankA::updateHitBox() {
    tank_pos.coord_x = head_matrix[12];
    tank_pos.coord_y = head_matrix[13];
    tank_pos.coord_z = head_matrix[14];
    right.compo_x = -head_matrix[0];
    right.compo_y = -head_matrix[1];
    right.compo_z = -head_matrix[2];
    left.compo_x = head_matrix[0];
    left.compo_y = head_matrix[1];
    left.compo_z = head_matrix[2];
    up.compo_x = head_matrix[4];
    up.compo_y = head_matrix[5];
    up.compo_z = head_matrix[6];
    down.compo_x = -head_matrix[4];
    down.compo_y = -head_matrix[5];
    down.compo_z = -head_matrix[6];
    at.compo_x = -head_matrix[8];
    at.compo_y = -head_matrix[9];
    at.compo_z = -head_matrix[10];
    back.compo_x = head_matrix[8];
    back.compo_y = head_matrix[9];
    back.compo_z = head_matrix[10];
}
