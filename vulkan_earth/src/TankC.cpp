#include "vulkan_earth/TankC.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/MacroCrtdbg.h"

const char* tank_c_name = "HeavyD";
const std::int32_t tank_c_hp = 1000;
const std::int32_t tank_c_power = 10;
const std::int32_t tank_c_armor = 7;
const std::int32_t tank_c_speed = 5;

TankC::TankC() = default;
TankC::TankC(float /*x*/, float /*y*/, float /*z*/) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turret_offset[0] = 0;
    turret_offset[1] = 70;
    turret_offset[2] = 100.0001;
    head_offset[0] = 0;
    head_offset[1] = 0;
    head_offset[2] = 0;
    body_offset[0] = 0;
    body_offset[1] = 90;
    body_offset[2] = 0;

    for (std::int32_t i = 0; i < 3; i++) {
        body_scale[i] = 80;
        head_scale[i] = 80;
        turret_scale[i] = 80;
        wheel_scale[i] = 80;
    }

    power = tank_c_power;
    armor = tank_c_armor;
    speed = tank_c_speed;
    current_power = 10;
    previous_power = 1000;
    previous_angle = 1;
    hp = armor * 100;

    vbo_shader_head = new VBOShaderLibrary();
    vbo_shader_body = new VBOShaderLibrary();
    vbo_shader_turret = new VBOShaderLibrary();

    vbo_shader_turret->loadClientData("./HeavyD/HeavyD_Turret.ogl");
    vbo_shader_body->loadClientData("./HeavyD/HeavyD_Body.ogl");
    vbo_shader_head->loadClientData("./HeavyD/HeavyD_Head.ogl");

    vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    projectile_land_pos[0] = 9999999;
    projectile_land_pos[1] = 9999999;
}
TankC::~TankC() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
std::int32_t TankC::getBaseHP() { return tank_c_hp; }
std::int32_t TankC::getBasePower() { return tank_c_power; }
std::int32_t TankC::getBaseArmor() { return tank_c_armor; }
std::int32_t TankC::getBaseSpeed() { return tank_c_speed; }
std::string TankC::getName() { return tank_c_name; }

void TankC::updateHitBox() {
    tank_pos.x = head_matrix[12];
    tank_pos.y = head_matrix[13];
    tank_pos.z = head_matrix[14];
    right.x = -head_matrix[0];
    right.y = -head_matrix[1];
    right.z = -head_matrix[2];
    left.x = head_matrix[0];
    left.y = head_matrix[1];
    left.z = head_matrix[2];
    up.x = head_matrix[4];
    up.y = head_matrix[5];
    up.z = head_matrix[6];
    down.x = -head_matrix[4];
    down.y = -head_matrix[5];
    down.z = -head_matrix[6];
    at.x = -head_matrix[8];
    at.y = -head_matrix[9];
    at.z = -head_matrix[10];
    back.x = head_matrix[8];
    back.y = head_matrix[9];
    back.z = head_matrix[10];
}