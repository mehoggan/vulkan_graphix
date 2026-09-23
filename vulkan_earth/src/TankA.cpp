#include "TankA.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "MacroCrtdbg.h"

const char* tank_a_name = "Rhinoxx";
const int tank_a_hp = 1000;
const int tank_a_power = 5;
const int tank_a_armor = 7;
const int tank_a_speed = 40;

TankA::TankA() = default;
TankA::TankA(GLfloat x, GLfloat y, GLfloat z) {
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

    for (int i = 0; i < 3; i++) {
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
    /*	CODE NEEDED TO USE SHADERS AND VBOS	*/
    if (!(VBOShaderLibrary::initGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::areVbOsSupported())) {
        exit(1);
    }

    vbo_shader_turret->getVBOPointerFunctions();
    vbo_shader_body->getVBOPointerFunctions();
    vbo_shader_head->getVBOPointerFunctions();

    vbo_shader_turret->loadClientData("./Rhinoxx_Tank/Rhinoxx_Turret.ogl");
    vbo_shader_body->loadClientData("./Rhinoxx_Tank/Rhinoxx_Body.ogl");
    vbo_shader_head->loadClientData("./Rhinoxx_Tank/Rhinoxx_Head.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");

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
int TankA::getBaseHP() { return tank_a_hp; }
int TankA::getBasePower() { return tank_a_power; }
int TankA::getBaseArmor() { return tank_a_armor; }
int TankA::getBaseSpeed() { return tank_a_speed; }
std::string TankA::getName() { return tank_a_name; }

void TankA::drawTankHitBox() {
    // Vertex tankPos(headMatrix[12],headMatrix[13],headMatrix[14]);
    // Vector right(-headMatrix[0],-headMatrix[1],-headMatrix[2]);
    // Vector up(headMatrix[4],headMatrix[5],headMatrix[6]);
    // Vector at(-headMatrix[8],-headMatrix[9],-headMatrix[10]);

    // Vector left(headMatrix[0],headMatrix[1],headMatrix[2]);
    // Vector down(-headMatrix[4],-headMatrix[5],-headMatrix[6]);
    // Vector back(headMatrix[8],headMatrix[9],headMatrix[10]);
    glBegin(GL_QUADS);
    // top
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glEnd();

    glBegin(GL_QUADS);
    // right
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * down.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * down.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * down.compo_z);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * down.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * down.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * down.compo_z);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glEnd();

    glBegin(GL_QUADS);
    // left
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * down.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * down.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * down.compo_z);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * down.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * down.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * down.compo_z);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glEnd();

    glBegin(GL_QUADS);
    // front
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * down.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * down.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * down.compo_z);
    glColor4f(0.0, 0.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * down.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * down.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * down.compo_z);
    glEnd();

    glBegin(GL_QUADS);
    // back
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * up.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * up.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * up.compo_z);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * left.compo_x +
                       hit_box_height / 2.0 * down.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * left.compo_y +
                       hit_box_height / 2.0 * down.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * left.compo_z +
                       hit_box_height / 2.0 * down.compo_z);
    glColor4f(0.0, 0.0, 0.0, .75);
    glVertex3f(tank_pos.coord_x + hit_box_length / 2.0 * back.compo_x +
                       hit_box_width / 2.0 * right.compo_x +
                       hit_box_height / 2.0 * down.compo_x,
               tank_pos.coord_y + hit_box_length / 2.0 * back.compo_y +
                       hit_box_width / 2.0 * right.compo_y +
                       hit_box_height / 2.0 * down.compo_y,
               tank_pos.coord_z + hit_box_length / 2.0 * back.compo_z +
                       hit_box_width / 2.0 * right.compo_z +
                       hit_box_height / 2.0 * down.compo_z);
    glEnd();
}

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
