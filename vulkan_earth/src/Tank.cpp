#include "Tank.h"
#include <iostream>
#include "Normal.h"
#include "ParticleGenerator.h"
#include "VBOShaderLibrary.h"
#include "Vector.h"
#include "MacroCrtdbg.h"

using namespace std;

Tank::Tank() {
    hit_box_height = 200;
    hit_box_length = 400;
    hit_box_width = 300;
    tank_alive = true;
    smoke_gen = nullptr;
    acid_gen = nullptr;
    float_gen = nullptr;
}
Tank::~Tank() {
    if (smoke_gen) {
        smoke_gen->killGenerator();
        delete smoke_gen;
    }
    if (acid_gen) {
        acid_gen->killGenerator();
        delete acid_gen;
    }
    if (float_gen) {
        float_gen->killGenerator();
        delete float_gen;
    }
}

void Tank::printTurretMatrix() {
    cout << " Turret Matrix " << endl;
    cout << "|" << turret_matrix[0] << " " << turret_matrix[4] << " "
         << turret_matrix[8] << " " << turret_matrix[12] << "|" << endl;
    cout << "|" << turret_matrix[1] << " " << turret_matrix[5] << " "
         << turret_matrix[9] << " " << turret_matrix[13] << "|" << endl;
    cout << "|" << turret_matrix[2] << " " << turret_matrix[6] << " "
         << turret_matrix[10] << " " << turret_matrix[14] << "|" << endl;
    cout << "|" << turret_matrix[3] << " " << turret_matrix[7] << " "
         << turret_matrix[11] << " " << turret_matrix[15] << "|" << endl;
}

void Tank::printHeadMatrix() {
    cout << " Head Matrix " << endl;
    cout << "|" << head_matrix[0] << " " << head_matrix[4] << " "
         << head_matrix[8] << " " << head_matrix[12] << "|" << endl;
    cout << "|" << head_matrix[1] << " " << head_matrix[5] << " "
         << head_matrix[9] << " " << head_matrix[13] << "|" << endl;
    cout << "|" << head_matrix[2] << " " << head_matrix[6] << " "
         << head_matrix[10] << " " << head_matrix[14] << "|" << endl;
    cout << "|" << head_matrix[3] << " " << head_matrix[7] << " "
         << head_matrix[11] << " " << head_matrix[15] << "|" << endl;
}

void Tank::printBodyMatrix() {
    cout << " Body Matrix " << endl;
    cout << "|" << body_matrix[0] << " " << body_matrix[4] << " "
         << body_matrix[8] << " " << body_matrix[12] << "|" << endl;
    cout << "|" << body_matrix[1] << " " << body_matrix[5] << " "
         << body_matrix[9] << " " << body_matrix[13] << "|" << endl;
    cout << "|" << body_matrix[2] << " " << body_matrix[6] << " "
         << body_matrix[10] << " " << body_matrix[14] << "|" << endl;
    cout << "|" << body_matrix[3] << " " << body_matrix[7] << " "
         << body_matrix[11] << " " << body_matrix[15] << "|" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
         << endl;
}

void Tank::setTankPos(GLfloat x, GLfloat y, GLfloat z) {
    // Keep Track of Tanks Position for Falling Damage
    previous_height = current_height;
    current_height = y;

    body_matrix[12] = x + body_offset[0];
    body_matrix[13] = y + body_offset[1];
    body_matrix[14] = z + body_offset[2];
    head_matrix[12] = body_matrix[12] + head_offset[0] * body_matrix[0] +
                      head_offset[1] * body_matrix[4] +
                      head_offset[2] * body_matrix[8];
    head_matrix[13] = body_matrix[13] + head_offset[0] * body_matrix[1] +
                      head_offset[1] * body_matrix[5] +
                      head_offset[2] * body_matrix[9];
    head_matrix[14] = body_matrix[14] + head_offset[0] * body_matrix[2] +
                      head_offset[1] * body_matrix[6] +
                      head_offset[2] * body_matrix[10];

    turret_matrix[12] = head_matrix[12] + turret_offset[0] * head_matrix[0] +
                        turret_offset[1] * head_matrix[4] +
                        turret_offset[2] * head_matrix[8];
    turret_matrix[13] = head_matrix[13] + turret_offset[0] * head_matrix[1] +
                        turret_offset[1] * head_matrix[5] +
                        turret_offset[2] * head_matrix[9];
    turret_matrix[14] = head_matrix[14] + turret_offset[0] * head_matrix[2] +
                        turret_offset[1] * head_matrix[6] +
                        turret_offset[2] * head_matrix[10];
    updateHitBox();
    if (smoke_gen)
        smoke_gen->update(body_matrix[12] + body_offset[0] + head_offset[0],
                          body_matrix[13] + body_offset[1] + head_offset[1],
                          body_matrix[14] + body_offset[2] + head_offset[2]);
}

void Tank::resetTurret() {
    turret_matrix[12] = head_matrix[12] + turret_offset[0] * head_matrix[0] +
                        turret_offset[1] * head_matrix[4] +
                        turret_offset[2] * head_matrix[8];
    turret_matrix[13] = head_matrix[13] + turret_offset[0] * head_matrix[1] +
                        turret_offset[1] * head_matrix[5] +
                        turret_offset[2] * head_matrix[9];
    turret_matrix[14] = head_matrix[14] + turret_offset[0] * head_matrix[2] +
                        turret_offset[1] * head_matrix[6] +
                        turret_offset[2] * head_matrix[10];
}

void Tank::normalizeVector(Vector* v) {
    GLfloat mag = sqrt(v->compo_x * v->compo_x + v->compo_y * v->compo_y +
                       v->compo_z * v->compo_z);
    if (mag != 0) {
        v->compo_x /= mag;
        v->compo_y /= mag;
        v->compo_z /= mag;
    }
}

GLfloat Tank::calcAngleBetweenVectors(Vector one, Vector two) {
    normalizeVector(&one);
    normalizeVector(&two);
    errno = 0;
    GLfloat pi_value = 3.141592653f;
    GLfloat u[3] = {one.compo_x, one.compo_y, one.compo_z};
    GLfloat v[3] = {two.compo_x, two.compo_y, two.compo_z};
    GLfloat angle =
            acos(u[0] * v[0] + u[1] * v[1] + u[2] * v[2]) * (180.0 / pi_value);
    if (errno) {
        return .01;
    }
    return angle;
}

void Tank::orientTank(Normal* n) {
    rotate_degrees = 0;
    alignment_vector.compo_x = n->compo_x;
    alignment_vector.compo_y = n->compo_y;
    alignment_vector.compo_z = n->compo_z;
    Vector tanks_up(body_matrix[4], body_matrix[5], body_matrix[6]);
    Vector perp(alignment_vector.compo_x,
                alignment_vector.compo_y,
                alignment_vector.compo_z);
    GLfloat angle = calcAngleBetweenVectors(perp, tanks_up);

    GLfloat u[3] = {perp.compo_x, perp.compo_y, perp.compo_z};
    GLfloat v[3] = {tanks_up.compo_x, tanks_up.compo_y, tanks_up.compo_z};

    rotate_about.compo_x = u[1] * v[2] - v[1] * u[2];
    rotate_about.compo_y = u[2] * v[0] - u[0] * v[2];
    rotate_about.compo_z = u[0] * v[1] - v[0] * u[1];

    GLfloat mag = static_cast<GLfloat>(
            sqrt(pow(static_cast<double>(rotate_about.compo_x), 2.0) +
                 pow(static_cast<double>(rotate_about.compo_y), 2.0) +
                 pow(static_cast<double>(rotate_about.compo_z), 2.0)));
    if (mag != 0) {
        rotate_about.compo_x /= mag;
        rotate_about.compo_y /= mag;
        rotate_about.compo_z /= mag;
        glPushMatrix();
        glLoadMatrixf(body_matrix);
        glRotatef(angle,
                  rotate_about.compo_z,
                  rotate_about.compo_y,
                  rotate_about.compo_x);
        glGetFloatv(GL_MODELVIEW_MATRIX, body_matrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, head_matrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, turret_matrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, wheel_matrix);
        glPopMatrix();
        turret_degrees = 0;
    }

    updateHitBox();
}

void Tank::rotateHead(GLfloat degrees) {
    rotate_degrees += degrees;
    if (rotate_degrees > 360) {
        rotate_degrees -= 360;
    } else if (rotate_degrees < 0) {
        rotate_degrees += 360;
    }

    GLfloat angle = calcAngleBetweenVectors(
            Vector(head_matrix[4], head_matrix[5], head_matrix[6]),
            Vector(turret_matrix[4], turret_matrix[5], turret_matrix[6]));

    glPushMatrix();
    glLoadMatrixf(head_matrix);
    glRotatef(degrees, 0, 1, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, head_matrix);
    glPopMatrix();

    glPushMatrix();
    glLoadMatrixf(turret_matrix);
    glRotatef(-angle, 1, 0, 0);
    glTranslatef(-turret_offset[0], -turret_offset[1], -turret_offset[2]);
    glRotatef(degrees, 0, 1, 0);
    glTranslatef(turret_offset[0], turret_offset[1], turret_offset[2]);
    glRotatef(angle, 1, 0, 0);
    /*cout << "63. ";
    printTurretMatrix();
    printHeadMatrix();
    printBodyMatrix();*/
    glGetFloatv(GL_MODELVIEW_MATRIX, turret_matrix);
    /*cout << "64. ";
    printTurretMatrix();
    printHeadMatrix();
    printBodyMatrix();*/
    glPopMatrix();
    updateHitBox();
}

void Tank::rotateTurret(GLfloat degrees) {
    if ((turret_degrees + degrees <= 90) && (turret_degrees + degrees >= 0)) {
        turret_degrees += degrees;
        glPushMatrix();
        glLoadMatrixf(turret_matrix);
        glRotatef(degrees, 1, 0, 0);
        // cout << "65. ";
        // printTurretMatrix();
        // printHeadMatrix();
        // printBodyMatrix();
        glGetFloatv(GL_MODELVIEW_MATRIX, turret_matrix);
        // cout << "66. ";
        // printTurretMatrix();
        // printHeadMatrix();
        // printBodyMatrix();
        glPopMatrix();
    }
}

bool Tank::checkCollision(GLfloat x, GLfloat y, GLfloat z) {
    // First check distance from tank, then check each face of hit box
    if (sqrt(pow((x - tank_pos.coord_x), 2) + pow((y - tank_pos.coord_y), 2) +
             pow((z - tank_pos.coord_z), 2)) < 50000000) {
        // top
        GLfloat plane_x = tank_pos.coord_x +
                          hit_box_length / 2.0 * at.compo_x +
                          hit_box_width / 2.0 * right.compo_x +
                          hit_box_height / 2.0 * up.compo_x;
        GLfloat plane_y = tank_pos.coord_y +
                          hit_box_length / 2.0 * at.compo_y +
                          hit_box_width / 2.0 * right.compo_y +
                          hit_box_height / 2.0 * up.compo_y;
        GLfloat plane_z = tank_pos.coord_z +
                          hit_box_length / 2.0 * at.compo_z +
                          hit_box_width / 2.0 * right.compo_z +
                          hit_box_height / 2.0 * up.compo_z;
        GLfloat d = up.compo_x * plane_x + up.compo_y * plane_y +
                    up.compo_z * plane_z;
        if (up.compo_x * x + up.compo_y * y + up.compo_z * z - d <= 0) {
            // right
            plane_x = tank_pos.coord_x + hit_box_length / 2.0 * at.compo_x +
                      hit_box_width / 2.0 * right.compo_x +
                      hit_box_height / 2.0 * up.compo_x;
            plane_y = tank_pos.coord_y + hit_box_length / 2.0 * at.compo_y +
                      hit_box_width / 2.0 * right.compo_y +
                      hit_box_height / 2.0 * up.compo_y;
            plane_z = tank_pos.coord_z + hit_box_length / 2.0 * at.compo_z +
                      hit_box_width / 2.0 * right.compo_z +
                      hit_box_height / 2.0 * up.compo_z;
            d = right.compo_x * plane_x + right.compo_y * plane_y +
                right.compo_z * plane_z;
            if (right.compo_x * x + right.compo_y * y + right.compo_z * z -
                        d <=
                0) {
                // left
                plane_x = tank_pos.coord_x +
                          hit_box_length / 2.0 * at.compo_x +
                          hit_box_width / 2.0 * left.compo_x +
                          hit_box_height / 2.0 * up.compo_x;
                plane_y = tank_pos.coord_y +
                          hit_box_length / 2.0 * at.compo_y +
                          hit_box_width / 2.0 * left.compo_y +
                          hit_box_height / 2.0 * up.compo_y;
                plane_z = tank_pos.coord_z +
                          hit_box_length / 2.0 * at.compo_z +
                          hit_box_width / 2.0 * left.compo_z +
                          hit_box_height / 2.0 * up.compo_z;
                d = left.compo_x * plane_x + left.compo_y * plane_y +
                    left.compo_z * plane_z;
                if (left.compo_x * x + left.compo_y * y + left.compo_z * z -
                            d <=
                    0) {
                    // front
                    plane_x = tank_pos.coord_x +
                              hit_box_length / 2.0 * at.compo_x +
                              hit_box_width / 2.0 * left.compo_x +
                              hit_box_height / 2.0 * up.compo_x;
                    plane_y = tank_pos.coord_y +
                              hit_box_length / 2.0 * at.compo_y +
                              hit_box_width / 2.0 * left.compo_y +
                              hit_box_height / 2.0 * up.compo_y;
                    plane_z = tank_pos.coord_z +
                              hit_box_length / 2.0 * at.compo_z +
                              hit_box_width / 2.0 * left.compo_z +
                              hit_box_height / 2.0 * up.compo_z;
                    d = at.compo_x * plane_x + at.compo_y * plane_y +
                        at.compo_z * plane_z;
                    if (at.compo_x * x + at.compo_y * y + at.compo_z * z - d <=
                        0) {
                        // back
                        plane_x = tank_pos.coord_x +
                                  hit_box_length / 2.0 * back.compo_x +
                                  hit_box_width / 2.0 * left.compo_x +
                                  hit_box_height / 2.0 * up.compo_x;
                        plane_y = tank_pos.coord_y +
                                  hit_box_length / 2.0 * back.compo_y +
                                  hit_box_width / 2.0 * left.compo_y +
                                  hit_box_height / 2.0 * up.compo_y;
                        plane_z = tank_pos.coord_z +
                                  hit_box_length / 2.0 * back.compo_z +
                                  hit_box_width / 2.0 * left.compo_z +
                                  hit_box_height / 2.0 * up.compo_z;
                        d = back.compo_x * plane_x + back.compo_y * plane_y +
                            back.compo_z * plane_z;
                        if (back.compo_x * x + back.compo_y * y +
                                    back.compo_z * z - d <=
                            0) {
                            cout << "COLLISION WITH TANK DETECTED" << endl;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void Tank::dealDamage(int damage) {
    if (damage < 0) cout << "ERROR: Negative damage";
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        tank_alive = false;
        initDuration();
        smoke_gen = new ParticleGenerator(10, 5, 1, 100, 0);
    }
}

void Tank::checkFallingDamage() {
    if (previous_height != current_height) {
        dealDamage((previous_height - current_height) / 10);
    }
}

bool Tank::isAlive() { return tank_alive; }

void Tank::tankRevive() {
    if (smoke_gen) {
        smoke_gen = nullptr;
    }
    tank_alive = true;
}

void Tank::adjustPower(GLfloat amount) {
    if (((current_power + amount) <= 10.0) &&
        ((current_power + amount) >= 0.0)) {
        current_power += amount;
    }
}

void Tank::initDuration() {
    duration_acid = 0;
    duration_shield = 0;
    duration_emp = 0;
    duration_float = 0;
    duration_double_action = 0;
    duration_padlock = 0;
    duration_cloak = 0;
    duration_paralyze = 0;
    delete acid_gen;
    delete float_gen;
}

void Tank::setBodyColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    body_color[0] = r;
    body_color[1] = g;
    body_color[2] = b;
    body_color[3] = a;
}
void Tank::setBodyScale(float x, float y, float z) {
    body_scale[0] = x;
    body_scale[1] = y;
    body_scale[2] = z;
}

void Tank::setHeadColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    head_color[0] = r;
    head_color[1] = g;
    head_color[2] = b;
    head_color[3] = a;
}
void Tank::setHeadScale(float x, float y, float z) {
    head_scale[0] = x;
    head_scale[1] = y;
    head_scale[2] = z;
}

void Tank::setTurretColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    turret_color[0] = r;
    turret_color[1] = g;
    turret_color[2] = b;
    turret_color[3] = a;
}
void Tank::setTurretScale(float x, float y, float z) {
    turret_scale[0] = x;
    turret_scale[1] = y;
    turret_scale[2] = z;
}

void Tank::setWheelColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    wheel_color[0] = r;
    wheel_color[1] = g;
    wheel_color[2] = b;
    wheel_color[3] = a;
}
void Tank::setWheelScale(float x, float y, float z) {
    wheel_scale[0] = x;
    wheel_scale[1] = y;
    wheel_scale[2] = z;
}
void Tank::setHP(int new_hp) { hp = new_hp; }
void Tank::setPower(int p) { power = p; }
void Tank::setArmor(int a) { armor = a; }
void Tank::setSpeed(int d) { speed = d; }

// Other functions
void Tank::fire() {}

void Tank::updateHitBox() {
    /*Vertex tankPos(bodyMatrix[12],bodyMatrix[13],bodyMatrix[14]);
    Vector right(-bodyMatrix[0],-bodyMatrix[1],-bodyMatrix[2]);
    Vector up(bodyMatrix[4],bodyMatrix[5],bodyMatrix[6]);
    Vector at(-bodyMatrix[8],-bodyMatrix[9],-bodyMatrix[10]);

    Vector left(bodyMatrix[0],bodyMatrix[1],bodyMatrix[2]);
    Vector down(-bodyMatrix[4],-bodyMatrix[5],-bodyMatrix[6]);
    Vector back(bodyMatrix[8],bodyMatrix[9],bodyMatrix[10]);*/
    tank_pos.coord_x = body_matrix[12];
    tank_pos.coord_y = body_matrix[13];
    tank_pos.coord_z = body_matrix[14];
    right.compo_x = -body_matrix[0];
    right.compo_y = -body_matrix[1];
    right.compo_z = -body_matrix[2];
    left.compo_x = body_matrix[0];
    left.compo_y = body_matrix[1];
    left.compo_z = body_matrix[2];
    up.compo_x = body_matrix[4];
    up.compo_y = body_matrix[5];
    up.compo_z = body_matrix[6];
    down.compo_x = -body_matrix[4];
    down.compo_y = -body_matrix[5];
    down.compo_z = -body_matrix[6];
    at.compo_x = -body_matrix[8];
    at.compo_y = -body_matrix[9];
    at.compo_z = -body_matrix[10];
    back.compo_x = body_matrix[8];
    back.compo_y = body_matrix[9];
    back.compo_z = body_matrix[10];
}

void Tank::keyHandler() {}

// Virtuals
int Tank::getBaseHP() { return 0; }
int Tank::getBasePower() { return 0; }
int Tank::getBaseArmor() { return 0; }
int Tank::getBaseSpeed() { return 0; }
std::string Tank::getName() { return "Huh?"; }
Normal Tank::getAlignmentVector() { return alignment_vector; }
Normal Tank::getRotateAbout() { return rotate_about; }
GLfloat Tank::getTurretDegrees() { return turret_degrees; }

void Tank::draw() {
    glPushMatrix();
    glMultMatrixf(turret_matrix);
    glScalef(turret_scale[0], turret_scale[1], turret_scale[2]);
    // Draw tank turret
    vbo_shader_turret->drawClientData();
    glPopMatrix();
    glPushMatrix();
    glMultMatrixf(head_matrix);
    glScalef(head_scale[0], head_scale[1], head_scale[2]);
    // Draw tank head
    vbo_shader_head->drawClientData();
    glPopMatrix();
    glPushMatrix();
    glMultMatrixf(body_matrix);
    // float effect must be same orientation as the tank
    if (float_gen) {
        float_gen->update(0, -body_offset[1], 0);
        float_gen->draw();
    }
    // Draw tank body
    glScalef(body_scale[0], body_scale[1], body_scale[2]);
    vbo_shader_body->drawClientData();
    glPopMatrix();
    glPushMatrix();
    glMultMatrixf(body_matrix);
    // shield
    if (duration_shield > 0) {
        glColor4f(0, 0, 1, 0.2);
        glutSolidSphere(300, 20, 20);
    }
    glPopMatrix();

    // update and draw particles
    if (smoke_gen) {
        smoke_gen->update(body_matrix[12] + body_offset[0] + head_offset[0],
                          body_matrix[13] + body_offset[1] + head_offset[1],
                          body_matrix[14] + body_offset[2] + head_offset[2]);
        smoke_gen->draw();
    }
    if (acid_gen) {
        acid_gen->update(body_matrix[12], body_matrix[13], body_matrix[14]);
        acid_gen->draw();
    }
}

void Tank::drawTankHitBox() {
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

GLfloat Tank::getCurrentPower() { return current_power; }
int Tank::getPreviousPower() { return previous_power; }
int Tank::getPreviousAngle() { return previous_angle; }
GLfloat* Tank::getProjectileLandPos() { return projectile_land_pos; }
void Tank::setPreviousPower(int new_previous_power) {
    previous_power = new_previous_power;
}
void Tank::setPreviousAngle(int new_previous_angle) {
    previous_angle = new_previous_angle;
}
void Tank::setProjectileLandPos(GLfloat x, GLfloat y) {
    projectile_land_pos[0] = x;
    projectile_land_pos[1] = y;
}

// remove later probably
void Tank::rotateWheel(GLfloat degrees) {
    if (!(wheel_degrees + degrees > 90) || (wheel_degrees - degrees < 0)) {
        wheel_degrees += degrees;
        glPushMatrix();
        glLoadMatrixf(wheel_matrix);
        glRotatef(degrees, wheel_matrix[4], wheel_matrix[5], wheel_matrix[6]);
        glGetFloatv(GL_MODELVIEW_MATRIX, wheel_matrix);
        glPopMatrix();
    }
}

void Tank::changeHeadTexture(int current_player_index) {
    if (current_player_index == 0)
        vbo_shader_head->swapTexture("player1Head.raw", 1024, 1024);
    else if (current_player_index == 1)
        vbo_shader_head->swapTexture("player2Head.raw", 1024, 1024);
    else if (current_player_index == 2)
        vbo_shader_head->swapTexture("player3Head.raw", 1024, 1024);
    else if (current_player_index == 3)
        vbo_shader_head->swapTexture("player4Head.raw", 1024, 1024);
    else if (current_player_index == 4)
        vbo_shader_head->swapTexture("player5Head.raw", 1024, 1024);
    else if (current_player_index == 5)
        vbo_shader_head->swapTexture("player6Head.raw", 1024, 1024);
    else if (current_player_index == 6)
        vbo_shader_head->swapTexture("player7Head.raw", 1024, 1024);
    else if (current_player_index == 7)
        vbo_shader_head->swapTexture("player8Head.raw", 1024, 1024);
    else if (current_player_index == 8)
        vbo_shader_head->swapTexture("player9Head.raw", 1024, 1024);
    else if (current_player_index == 9)
        vbo_shader_head->swapTexture("player10Head.raw", 1024, 1024);
    else {
        printf("\nERROR <Tank::changeHeadTexture(int)>: Wrong parameter "
               "passing.\n");
    }
}

int Tank::getCurrentHeight() { return current_height; }

void Tank::setCurrentHeight(int curr_height) { current_height = curr_height; }

int Tank::getPreviousHeight() { return previous_height; }

void Tank::setPreviousHeight(int prev_height) {
    previous_height = prev_height;
}

// Getters for durations
int Tank::getDurationAcid() { return duration_acid; }
int Tank::getDurationShield() { return duration_shield; }
int Tank::getDurationEMP() { return duration_emp; }
int Tank::getDurationFloat() { return duration_float; }
int Tank::getDurationDoubleAction() { return duration_double_action; }
int Tank::getDurationPadlock() { return duration_padlock; }
int Tank::getDurationCloak() { return duration_cloak; }
int Tank::getDurationParalyze() { return duration_paralyze; }
// Setters for durations
void Tank::setDurationAcid(int value) {
    if (value != 0) {
        delete acid_gen;
        acid_gen = new ParticleGenerator(10, 5, 2, 100, 1);
    } else {
        if (acid_gen) {
            acid_gen->killGenerator();
            acid_gen = nullptr;
        }
    }
    duration_acid = value;
}
void Tank::setDurationShield(int value) { duration_shield = value; }
void Tank::setDurationEMP(int value) { duration_emp = value; }
void Tank::setDurationFloat(int value) {
    if (value != 0) {
        delete float_gen;
        float_gen = new ParticleGenerator(10, 5, 2, 100, 2);
    } else {
        if (float_gen) {
            float_gen->killGenerator();
            float_gen = nullptr;
        }
    }
    duration_float = value;
}
void Tank::setDurationDoubleAction(int value) {
    duration_double_action = value;
}
void Tank::setDurationPadlock(int value) { duration_padlock = value; }
void Tank::setDurationCloak(int value) { duration_cloak = value; }
void Tank::setDurationParalyze(int value) { duration_paralyze = value; }
void Tank::setDurationAllPassTurn() {
    // acid
    if (duration_acid > 0) {
        duration_acid--;
    }
    if ((acid_gen) && (duration_acid == 0)) {
        acid_gen->killGenerator();
        acid_gen = nullptr;
    }

    // float
    if (duration_float > 0) {
        duration_float--;
    }
    if ((float_gen) && (duration_float == 0)) {
        float_gen->killGenerator();
        float_gen = nullptr;
    }
    if (duration_emp > 0) duration_emp--;
    if (duration_double_action > 0) duration_double_action--;
    if (duration_padlock > 0) duration_padlock--;
    if (duration_cloak > 0) duration_cloak--;
    if (duration_paralyze > 0) duration_paralyze--;
}
void Tank::initBody() {
    body_right[0] = body_matrix[0] = 0;
    body_right[1] = body_matrix[1] = 0;
    body_right[2] = body_matrix[2] = 1;
    body_matrix[3] = 0;
    body_up[0] = body_matrix[4] = 0;
    body_up[1] = body_matrix[5] = 1;
    body_up[2] = body_matrix[6] = 0;
    body_matrix[7] = 0;
    body_at[0] = body_matrix[8] = 1;
    body_at[1] = body_matrix[9] = 0;
    body_at[2] = body_matrix[10] = 0;
    body_matrix[11] = 0;
    body_matrix[12] = 0;
    body_matrix[13] = 0;
    body_matrix[14] = 0;
    body_matrix[15] = 1;

    body_color[0] = 0.50f;
    body_color[1] = 0.50f;
    body_color[2] = 0.50f;
    body_color[3] = 1.0f;
    body_scale[0] = 70;
    body_scale[1] = 70;
    body_scale[2] = 70;
    previous_height = current_height = body_matrix[13];
}
void Tank::initHead() {
    head_right[0] = head_matrix[0] = 0;
    head_right[1] = head_matrix[1] = 0;
    head_right[2] = head_matrix[2] = 1;
    head_matrix[3] = 0;
    head_up[0] = head_matrix[4] = 0;
    head_up[1] = head_matrix[5] = 1;
    head_up[2] = head_matrix[6] = 0;
    head_matrix[7] = 0;
    head_at[0] = head_matrix[8] = 1;
    head_at[1] = head_matrix[9] = 0;
    head_at[2] = head_matrix[10] = 0;
    head_matrix[11] = 0;
    head_matrix[12] = 0;
    head_matrix[13] = 0;
    head_matrix[14] = 0;
    head_matrix[15] = 1;
    head_color[0] = 0.50f;
    head_color[1] = 0.50f;
    head_color[2] = 0.50f;
    head_color[3] = 1.0f;
    head_scale[0] = 70;
    head_scale[1] = 70;
    head_scale[2] = 70;
}
void Tank::initTurret() {
    turret_right[0] = turret_matrix[0] = 0;
    turret_right[1] = turret_matrix[1] = 0;
    turret_right[2] = turret_matrix[2] = 1;
    turret_matrix[3] = 0;
    turret_up[0] = turret_matrix[4] = 0;
    turret_up[1] = turret_matrix[5] = 1;
    turret_up[2] = turret_matrix[6] = 0;
    turret_matrix[7] = 0;
    turret_at[0] = turret_matrix[8] = 1;
    turret_at[1] = turret_matrix[9] = 0;
    turret_at[2] = turret_matrix[10] = 0;
    turret_matrix[11] = 0;
    turret_matrix[12] = 0;
    turret_matrix[13] = 0;
    turret_matrix[14] = 0;
    turret_matrix[15] = 1;
    turret_color[0] = 0.50f;
    turret_color[1] = 0.50f;
    turret_color[2] = 0.50f;
    turret_color[3] = 1.0f;
    turret_scale[0] = 70;
    turret_scale[1] = 70;
    turret_scale[2] = 70;

    turret_offset[0] = 0;
    turret_offset[1] = 0;
    turret_offset[2] = 0;

    turret_degrees = 0;
    wheel_degrees = 0;
}
void Tank::initWheel() {
    wheel_right[0] = wheel_matrix[0] = 0;
    wheel_right[1] = wheel_matrix[1] = 0;
    wheel_right[2] = wheel_matrix[2] = 1;
    wheel_matrix[3] = 0;
    wheel_up[0] = wheel_matrix[4] = 0;
    wheel_up[1] = wheel_matrix[5] = 1;
    wheel_up[2] = wheel_matrix[6] = 0;
    wheel_matrix[7] = 0;
    wheel_at[0] = wheel_matrix[8] = 1;
    wheel_at[1] = wheel_matrix[9] = 0;
    wheel_at[2] = wheel_matrix[10] = 0;
    wheel_matrix[11] = 0;
    wheel_matrix[12] = 0;
    wheel_matrix[13] = 0;
    wheel_matrix[14] = 0;
    wheel_matrix[15] = 1;
    wheel_color[0] = 0.50f;
    wheel_color[1] = 0.50f;
    wheel_color[2] = 0.50f;
    wheel_color[3] = 1.0f;
    wheel_scale[0] = 70;
    wheel_scale[1] = 70;
    wheel_scale[2] = 70;
}

// GETTERS
const GLfloat* Tank::getBodyMatrix() { return &body_matrix[0]; }
const GLfloat* Tank::getBodyColor() { return &body_color[0]; }
float* Tank::getBodyScale() { return &body_scale[0]; }

const GLfloat* Tank::getHeadMatrix() { return &head_matrix[0]; }
const GLfloat* Tank::getHeadColor() { return &head_color[0]; }
float* Tank::getHeadScale() { return &head_scale[0]; }

GLfloat* Tank::getTurretMatrix() { return &turret_matrix[0]; }
const GLfloat* Tank::getTurretColor() { return &turret_color[0]; }
float* Tank::getTurretScale() { return &turret_scale[0]; }

const GLfloat* Tank::getWheelMatrix() { return &wheel_matrix[0]; }
const GLfloat* Tank::getWheelColor() { return &wheel_color[0]; }
float* Tank::getWheelScale() { return &wheel_scale[0]; }

int Tank::getHP() { return hp; }
int Tank::getPower() { return power; }
int Tank::getArmor() { return armor; }
int Tank::getSpeed() { return speed; }
Tank* Tank::getTankPointer() { return this; }