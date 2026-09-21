#include "TankG.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"

const char* tank_g_name = "Cubix";
const int tank_g_hp = 1000;
const int tank_g_power = 10;
const int tank_g_armor = 5;
const int tank_g_speed = 75;

TankG::TankG() = default;
TankG::TankG(GLfloat x, GLfloat y, GLfloat z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turret_offset[0] = 0;
    turret_offset[1] = 22;
    turret_offset[2] = 12;
    head_offset[0] = 0;
    head_offset[1] = 0;
    head_offset[2] = 0;
    body_offset[0] = 0;
    body_offset[1] = 120;
    body_offset[2] = 0;

    for (int i = 0; i < 3; i++) {
        body_scale[i] = 100;
        head_scale[i] = 100;
        turret_scale[i] = 100;
        wheel_scale[i] = 100;
    }

    power = tank_g_power;
    armor = tank_g_armor;
    speed = tank_g_speed;
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

    vbo_shader_turret->loadClientData("./Cubix/Cubix_Turret.ogl");
    vbo_shader_body->loadClientData("./Cubix/Cubix_Body.ogl");
    vbo_shader_head->loadClientData("./Cubix/Cubix_Head.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");

    vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    projectile_land_pos[0] = 9999999;
    projectile_land_pos[1] = 9999999;
}
TankG::~TankG() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
int TankG::getBaseHP() { return tank_g_hp; }
int TankG::getBasePower() { return tank_g_power; }
int TankG::getBaseArmor() { return tank_g_armor; }
int TankG::getBaseSpeed() { return tank_g_speed; }
std::string TankG::getName() { return tank_g_name; }

void TankG::drawTankHitBox() {
    /*Vertex tankPos(headMatrix[12],headMatrix[13],headMatrix[14]);
    Vector right(-headMatrix[0],-headMatrix[1],-headMatrix[2]);
    Vector up(headMatrix[4],headMatrix[5],headMatrix[6]);
    Vector at(-headMatrix[8],-headMatrix[9],-headMatrix[10]);

    Vector left(headMatrix[0],headMatrix[1],headMatrix[2]);
    Vector down(-headMatrix[4],-headMatrix[5],-headMatrix[6]);
    Vector back(headMatrix[8],headMatrix[9],headMatrix[10]);*/
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

void TankG::updateHitBox() {
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