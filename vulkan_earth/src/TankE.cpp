#include "vulkan_earth/TankE.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/MacroCrtdbg.h"

const char* tank_e_name = "Eggroid";
const std::int32_t tank_e_hp = 1000;
const std::int32_t tank_e_power = 4;
const std::int32_t tank_e_armor = 3;
const std::int32_t tank_e_speed = 80;

TankE::TankE() = default;
TankE::TankE(float /*x*/, float /*y*/, float /*z*/) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    m_turret_offset[0] = 0;
    m_turret_offset[1] = 0;
    m_turret_offset[2] = 0;
    m_head_offset[0] = 0;
    m_head_offset[1] = 100;
    m_head_offset[2] = 0;
    m_body_offset[0] = 0;
    m_body_offset[1] = 80;
    m_body_offset[2] = 0;

    m_hit_box_height = 200;
    m_hit_box_length = 300;
    m_hit_box_width = 300;

    for (std::int32_t i = 0; i < 3; i++) {
        m_body_scale[i] = 40;
        m_head_scale[i] = 40;
        m_turret_scale[i] = 40;
        m_wheel_scale[i] = 40;
    }

    m_power = tank_e_power;
    m_armor = tank_e_armor;
    m_speed = tank_e_speed;
    m_current_power = 10;
    m_previous_power = 1000;
    m_previous_angle = 1;
    m_hp = m_armor * 100;

    m_vbo_shader_head = new VBOShaderLibrary();
    m_vbo_shader_body = new VBOShaderLibrary();
    m_vbo_shader_turret = new VBOShaderLibrary();
    // vbo_shader_wheel=new VBOShaderLibrary();

    m_vbo_shader_turret->loadClientData("./Eggroid/Eggroid_Turret.ogl");
    m_vbo_shader_body->loadClientData("./Eggroid/Eggroid_Body.ogl");
    m_vbo_shader_head->loadClientData("./Eggroid/Eggroid_Head.ogl");
    // vbo_shader_wheel->loadClientData("./Eggroid/Eggroid_Wheel.ogl");

    m_vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    m_vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    m_vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);
    // vbo_shader_wheel->LoadTexture("TestImage.raw",1024,1024);

    m_projectile_land_pos[0] = 9999999;
    m_projectile_land_pos[1] = 9999999;
}
TankE::~TankE() {
    delete m_vbo_shader_head;
    delete m_vbo_shader_body;
    delete m_vbo_shader_turret;
    // delete vbo_shader_wheel;
}

// GETTERS
std::int32_t TankE::getBaseHP() { return tank_e_hp; }
std::int32_t TankE::getBasePower() { return tank_e_power; }
std::int32_t TankE::getBaseArmor() { return tank_e_armor; }
std::int32_t TankE::getBaseSpeed() { return tank_e_speed; }
std::string TankE::getName() { return tank_e_name; }
