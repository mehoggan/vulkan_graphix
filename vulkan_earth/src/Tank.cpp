#include "Tank.h"
#include <iostream>
#include "Normal.h"
#include "ParticleGenerator.h"
#include "VBOShaderLibrary.h"
#include "Vector.h"
#include "macro_crtdbg.h"

using namespace std;

Tank::Tank() {
    hitBoxHeight = 200;
    hitBoxLength = 400;
    hitBoxWidth = 300;
    Vector right(0, 0, 0);
    Vector up(0, 0, 0);
    Vector at(0, 0, 0);
    Vector left(0, 0, 0);
    Vector down(0, 0, 0);
    Vector back(0, 0, 0);
    tankAlive = true;
    smokeGen = nullptr;
    acidGen = nullptr;
    floatGen = nullptr;
}
Tank::~Tank() {
    if (smokeGen) {
        smokeGen->killGenerator();
        delete smokeGen;
    }
    if (acidGen) {
        acidGen->killGenerator();
        delete acidGen;
    }
    if (floatGen) {
        floatGen->killGenerator();
        delete floatGen;
    }
}

void Tank::printTurretMatrix() {
    cout << " Turret Matrix " << endl;
    cout << "|" << turretMatrix[0] << " " << turretMatrix[4] << " "
         << turretMatrix[8] << " " << turretMatrix[12] << "|" << endl;
    cout << "|" << turretMatrix[1] << " " << turretMatrix[5] << " "
         << turretMatrix[9] << " " << turretMatrix[13] << "|" << endl;
    cout << "|" << turretMatrix[2] << " " << turretMatrix[6] << " "
         << turretMatrix[10] << " " << turretMatrix[14] << "|" << endl;
    cout << "|" << turretMatrix[3] << " " << turretMatrix[7] << " "
         << turretMatrix[11] << " " << turretMatrix[15] << "|" << endl;
}

void Tank::printHeadMatrix() {
    cout << " Head Matrix " << endl;
    cout << "|" << headMatrix[0] << " " << headMatrix[4] << " "
         << headMatrix[8] << " " << headMatrix[12] << "|" << endl;
    cout << "|" << headMatrix[1] << " " << headMatrix[5] << " "
         << headMatrix[9] << " " << headMatrix[13] << "|" << endl;
    cout << "|" << headMatrix[2] << " " << headMatrix[6] << " "
         << headMatrix[10] << " " << headMatrix[14] << "|" << endl;
    cout << "|" << headMatrix[3] << " " << headMatrix[7] << " "
         << headMatrix[11] << " " << headMatrix[15] << "|" << endl;
}

void Tank::printBodyMatrix() {
    cout << " Body Matrix " << endl;
    cout << "|" << bodyMatrix[0] << " " << bodyMatrix[4] << " "
         << bodyMatrix[8] << " " << bodyMatrix[12] << "|" << endl;
    cout << "|" << bodyMatrix[1] << " " << bodyMatrix[5] << " "
         << bodyMatrix[9] << " " << bodyMatrix[13] << "|" << endl;
    cout << "|" << bodyMatrix[2] << " " << bodyMatrix[6] << " "
         << bodyMatrix[10] << " " << bodyMatrix[14] << "|" << endl;
    cout << "|" << bodyMatrix[3] << " " << bodyMatrix[7] << " "
         << bodyMatrix[11] << " " << bodyMatrix[15] << "|" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
         << endl;
}

void Tank::setTankPos(GLfloat x, GLfloat y, GLfloat z) {
    // Keep Track of Tanks Position for Falling Damage
    previousHeight = currentHeight;
    currentHeight = y;

    bodyMatrix[12] = x + bodyOffset[0];
    bodyMatrix[13] = y + bodyOffset[1];
    bodyMatrix[14] = z + bodyOffset[2];
    headMatrix[12] = bodyMatrix[12] + headOffset[0] * bodyMatrix[0] +
                     headOffset[1] * bodyMatrix[4] +
                     headOffset[2] * bodyMatrix[8];
    headMatrix[13] = bodyMatrix[13] + headOffset[0] * bodyMatrix[1] +
                     headOffset[1] * bodyMatrix[5] +
                     headOffset[2] * bodyMatrix[9];
    headMatrix[14] = bodyMatrix[14] + headOffset[0] * bodyMatrix[2] +
                     headOffset[1] * bodyMatrix[6] +
                     headOffset[2] * bodyMatrix[10];

    turretMatrix[12] = headMatrix[12] + turretOffset[0] * headMatrix[0] +
                       turretOffset[1] * headMatrix[4] +
                       turretOffset[2] * headMatrix[8];
    turretMatrix[13] = headMatrix[13] + turretOffset[0] * headMatrix[1] +
                       turretOffset[1] * headMatrix[5] +
                       turretOffset[2] * headMatrix[9];
    turretMatrix[14] = headMatrix[14] + turretOffset[0] * headMatrix[2] +
                       turretOffset[1] * headMatrix[6] +
                       turretOffset[2] * headMatrix[10];
    updateHitBox();
    if (smokeGen)
        smokeGen->update(bodyMatrix[12] + bodyOffset[0] + headOffset[0],
                         bodyMatrix[13] + bodyOffset[1] + headOffset[1],
                         bodyMatrix[14] + bodyOffset[2] + headOffset[2]);
}

void Tank::resetTurret() {
    turretMatrix[12] = headMatrix[12] + turretOffset[0] * headMatrix[0] +
                       turretOffset[1] * headMatrix[4] +
                       turretOffset[2] * headMatrix[8];
    turretMatrix[13] = headMatrix[13] + turretOffset[0] * headMatrix[1] +
                       turretOffset[1] * headMatrix[5] +
                       turretOffset[2] * headMatrix[9];
    turretMatrix[14] = headMatrix[14] + turretOffset[0] * headMatrix[2] +
                       turretOffset[1] * headMatrix[6] +
                       turretOffset[2] * headMatrix[10];
}

void Tank::NormalizeVector(Vector* v) {
    GLfloat mag = sqrt(v->compoX * v->compoX + v->compoY * v->compoY +
                       v->compoZ * v->compoZ);
    if (mag != 0) {
        v->compoX /= mag;
        v->compoY /= mag;
        v->compoZ /= mag;
    }
}

GLfloat Tank::calcAngleBetweenVectors(Vector one, Vector two) {
    NormalizeVector(&one);
    NormalizeVector(&two);
    errno = 0;
    GLfloat tt = 3.141592653f;
    GLfloat u[3] = {one.compoX, one.compoY, one.compoZ};
    GLfloat v[3] = {two.compoX, two.compoY, two.compoZ};
    GLfloat angle =
            acos(u[0] * v[0] + u[1] * v[1] + u[2] * v[2]) * (180.0 / tt);
    if (errno) {
        return .01;
    }
    return angle;
}

void Tank::orientTank(Normal* n) {
    rotate_degrees = 0;
    alignmentVector.compoX = n->compoX;
    alignmentVector.compoY = n->compoY;
    alignmentVector.compoZ = n->compoZ;
    Vector tanks_up(bodyMatrix[4], bodyMatrix[5], bodyMatrix[6]);
    Vector perp(alignmentVector.compoX,
                alignmentVector.compoY,
                alignmentVector.compoZ);
    GLfloat angle = calcAngleBetweenVectors(perp, tanks_up);

    GLfloat u[3] = {perp.compoX, perp.compoY, perp.compoZ};
    GLfloat v[3] = {tanks_up.compoX, tanks_up.compoY, tanks_up.compoZ};

    rotate_about.compoX = u[1] * v[2] - v[1] * u[2];
    rotate_about.compoY = u[2] * v[0] - u[0] * v[2];
    rotate_about.compoZ = u[0] * v[1] - v[0] * u[1];

    GLfloat mag = static_cast<GLfloat>(
            sqrt(pow(static_cast<double>(rotate_about.compoX), 2.0) +
                 pow(static_cast<double>(rotate_about.compoY), 2.0) +
                 pow(static_cast<double>(rotate_about.compoZ), 2.0)));
    if (mag != 0) {
        rotate_about.compoX /= mag;
        rotate_about.compoY /= mag;
        rotate_about.compoZ /= mag;
        glPushMatrix();
        glLoadMatrixf(bodyMatrix);
        glRotatef(angle,
                  rotate_about.compoZ,
                  rotate_about.compoY,
                  rotate_about.compoX);
        glGetFloatv(GL_MODELVIEW_MATRIX, bodyMatrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, headMatrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, turretMatrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, wheelMatrix);
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
            Vector(headMatrix[4], headMatrix[5], headMatrix[6]),
            Vector(turretMatrix[4], turretMatrix[5], turretMatrix[6]));

    glPushMatrix();
    glLoadMatrixf(headMatrix);
    glRotatef(degrees, 0, 1, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, headMatrix);
    glPopMatrix();

    glPushMatrix();
    glLoadMatrixf(turretMatrix);
    glRotatef(-angle, 1, 0, 0);
    glTranslatef(-turretOffset[0], -turretOffset[1], -turretOffset[2]);
    glRotatef(degrees, 0, 1, 0);
    glTranslatef(turretOffset[0], turretOffset[1], turretOffset[2]);
    glRotatef(angle, 1, 0, 0);
    /*cout << "63. ";
    printTurretMatrix();
    printHeadMatrix();
    printBodyMatrix();*/
    glGetFloatv(GL_MODELVIEW_MATRIX, turretMatrix);
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
        glLoadMatrixf(turretMatrix);
        glRotatef(degrees, 1, 0, 0);
        // cout << "65. ";
        // printTurretMatrix();
        // printHeadMatrix();
        // printBodyMatrix();
        glGetFloatv(GL_MODELVIEW_MATRIX, turretMatrix);
        // cout << "66. ";
        // printTurretMatrix();
        // printHeadMatrix();
        // printBodyMatrix();
        glPopMatrix();
    }
}

bool Tank::checkCollision(GLfloat x, GLfloat y, GLfloat z) {
    // First check distance from tank, then check each face of hit box
    if (sqrt(pow((x - tankPos.coordX), 2) + pow((y - tankPos.coordY), 2) +
             pow((z - tankPos.coordZ), 2)) < 50000000) {
        // top
        GLfloat plane_x = tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                          hitBoxWidth / 2.0 * right.compoX +
                          hitBoxHeight / 2.0 * up.compoX;
        GLfloat plane_y = tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                          hitBoxWidth / 2.0 * right.compoY +
                          hitBoxHeight / 2.0 * up.compoY;
        GLfloat plane_z = tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                          hitBoxWidth / 2.0 * right.compoZ +
                          hitBoxHeight / 2.0 * up.compoZ;
        GLfloat d = up.compoX * plane_x + up.compoY * plane_y +
                    up.compoZ * plane_z;
        if (up.compoX * x + up.compoY * y + up.compoZ * z - d <= 0) {
            // right
            plane_x = tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                      hitBoxWidth / 2.0 * right.compoX +
                      hitBoxHeight / 2.0 * up.compoX;
            plane_y = tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                      hitBoxWidth / 2.0 * right.compoY +
                      hitBoxHeight / 2.0 * up.compoY;
            plane_z = tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                      hitBoxWidth / 2.0 * right.compoZ +
                      hitBoxHeight / 2.0 * up.compoZ;
            d = right.compoX * plane_x + right.compoY * plane_y +
                right.compoZ * plane_z;
            if (right.compoX * x + right.compoY * y + right.compoZ * z - d <=
                0) {
                // left
                plane_x = tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                          hitBoxWidth / 2.0 * left.compoX +
                          hitBoxHeight / 2.0 * up.compoX;
                plane_y = tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                          hitBoxWidth / 2.0 * left.compoY +
                          hitBoxHeight / 2.0 * up.compoY;
                plane_z = tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                          hitBoxWidth / 2.0 * left.compoZ +
                          hitBoxHeight / 2.0 * up.compoZ;
                d = left.compoX * plane_x + left.compoY * plane_y +
                    left.compoZ * plane_z;
                if (left.compoX * x + left.compoY * y + left.compoZ * z - d <=
                    0) {
                    // front
                    plane_x = tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                              hitBoxWidth / 2.0 * left.compoX +
                              hitBoxHeight / 2.0 * up.compoX;
                    plane_y = tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                              hitBoxWidth / 2.0 * left.compoY +
                              hitBoxHeight / 2.0 * up.compoY;
                    plane_z = tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                              hitBoxWidth / 2.0 * left.compoZ +
                              hitBoxHeight / 2.0 * up.compoZ;
                    d = at.compoX * plane_x + at.compoY * plane_y +
                        at.compoZ * plane_z;
                    if (at.compoX * x + at.compoY * y + at.compoZ * z - d <=
                        0) {
                        // back
                        plane_x = tankPos.coordX +
                                  hitBoxLength / 2.0 * back.compoX +
                                  hitBoxWidth / 2.0 * left.compoX +
                                  hitBoxHeight / 2.0 * up.compoX;
                        plane_y = tankPos.coordY +
                                  hitBoxLength / 2.0 * back.compoY +
                                  hitBoxWidth / 2.0 * left.compoY +
                                  hitBoxHeight / 2.0 * up.compoY;
                        plane_z = tankPos.coordZ +
                                  hitBoxLength / 2.0 * back.compoZ +
                                  hitBoxWidth / 2.0 * left.compoZ +
                                  hitBoxHeight / 2.0 * up.compoZ;
                        d = back.compoX * plane_x + back.compoY * plane_y +
                            back.compoZ * plane_z;
                        if (back.compoX * x + back.compoY * y +
                                    back.compoZ * z - d <=
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
    HP -= damage;
    if (HP <= 0) {
        HP = 0;
        tankAlive = false;
        initDuration();
        smokeGen = new ParticleGenerator(10, 5, 1, 100, 0);
    }
}

void Tank::checkFallingDamage() {
    if (previousHeight != currentHeight) {
        dealDamage((previousHeight - currentHeight) / 10);
    }
}

bool Tank::isAlive() { return tankAlive; }

void Tank::tankRevive() {
    if (smokeGen) {
        smokeGen = nullptr;
    }
    tankAlive = true;
}

void Tank::adjustPower(GLfloat amount) {
    if (((currentPower + amount) <= 10.0) &&
        ((currentPower + amount) >= 0.0)) {
        currentPower += amount;
    }
}

void Tank::initDuration() {
    durationAcid = 0;
    durationShield = 0;
    durationEMP = 0;
    durationFloat = 0;
    durationDoubleAction = 0;
    durationPadlock = 0;
    durationCloak = 0;
    durationParalyze = 0;
    delete acidGen;
    delete floatGen;
}

void Tank::setBodyColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    bodyColor[0] = r;
    bodyColor[1] = g;
    bodyColor[2] = b;
    bodyColor[3] = a;
}
void Tank::setBodyScale(float x, float y, float z) {
    bodyScale[0] = x;
    bodyScale[1] = y;
    bodyScale[2] = z;
}

void Tank::setHeadColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    headColor[0] = r;
    headColor[1] = g;
    headColor[2] = b;
    headColor[3] = a;
}
void Tank::setHeadScale(float x, float y, float z) {
    headScale[0] = x;
    headScale[1] = y;
    headScale[2] = z;
}

void Tank::setTurretColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    turretColor[0] = r;
    turretColor[1] = g;
    turretColor[2] = b;
    turretColor[3] = a;
}
void Tank::setTurretScale(float x, float y, float z) {
    turretScale[0] = x;
    turretScale[1] = y;
    turretScale[2] = z;
}

void Tank::setWheelColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    wheelColor[0] = r;
    wheelColor[1] = g;
    wheelColor[2] = b;
    wheelColor[3] = a;
}
void Tank::setWheelScale(float x, float y, float z) {
    wheelScale[0] = x;
    wheelScale[1] = y;
    wheelScale[2] = z;
}
void Tank::setHP(int hp) { HP = hp; }
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
    tankPos.coordX = bodyMatrix[12];
    tankPos.coordY = bodyMatrix[13];
    tankPos.coordZ = bodyMatrix[14];
    this->right.compoX = -bodyMatrix[0];
    this->right.compoY = -bodyMatrix[1];
    this->right.compoZ = -bodyMatrix[2];
    this->left.compoX = bodyMatrix[0];
    this->left.compoY = bodyMatrix[1];
    this->left.compoZ = bodyMatrix[2];
    this->up.compoX = bodyMatrix[4];
    this->up.compoY = bodyMatrix[5];
    this->up.compoZ = bodyMatrix[6];
    this->down.compoX = -bodyMatrix[4];
    this->down.compoY = -bodyMatrix[5];
    this->down.compoZ = -bodyMatrix[6];
    this->at.compoX = -bodyMatrix[8];
    this->at.compoY = -bodyMatrix[9];
    this->at.compoZ = -bodyMatrix[10];
    this->back.compoX = bodyMatrix[8];
    this->back.compoY = bodyMatrix[9];
    this->back.compoZ = bodyMatrix[10];
}

void Tank::keyHandler() {}

// Virtuals
int Tank::getBaseHP() { return 0; }
int Tank::getBasePower() { return 0; }
int Tank::getBaseArmor() { return 0; }
int Tank::getBaseSpeed() { return 0; }
std::string Tank::getName() { return "Huh?"; }
Normal Tank::getAlignmentVector() { return alignmentVector; }
Normal Tank::getRotateAbout() { return rotate_about; }
GLfloat Tank::getTurretDegrees() { return turret_degrees; }

void Tank::draw() {
    glPushMatrix();
    glMultMatrixf(turretMatrix);
    glScalef(turretScale[0], turretScale[1], turretScale[2]);
    // Draw tank turret
    vbo_shader_turret->drawClientData();
    glPopMatrix();
    glPushMatrix();
    glMultMatrixf(headMatrix);
    glScalef(headScale[0], headScale[1], headScale[2]);
    // Draw tank head
    vbo_shader_head->drawClientData();
    glPopMatrix();
    glPushMatrix();
    glMultMatrixf(bodyMatrix);
    // float effect must be same orientation as the tank
    if (floatGen) {
        floatGen->update(0, -bodyOffset[1], 0);
        floatGen->draw();
    }
    // Draw tank body
    glScalef(bodyScale[0], bodyScale[1], bodyScale[2]);
    vbo_shader_body->drawClientData();
    glPopMatrix();
    glPushMatrix();
    glMultMatrixf(bodyMatrix);
    // shield
    if (durationShield > 0) {
        glColor4f(0, 0, 1, 0.2);
        glutSolidSphere(300, 20, 20);
    }
    glPopMatrix();

    // update and draw particles
    if (smokeGen) {
        smokeGen->update(bodyMatrix[12] + bodyOffset[0] + headOffset[0],
                         bodyMatrix[13] + bodyOffset[1] + headOffset[1],
                         bodyMatrix[14] + bodyOffset[2] + headOffset[2]);
        smokeGen->draw();
    }
    if (acidGen) {
        acidGen->update(bodyMatrix[12], bodyMatrix[13], bodyMatrix[14]);
        acidGen->draw();
    }
}

void Tank::drawTankHitBox() {
    glBegin(GL_QUADS);
    // top
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glEnd();

    glBegin(GL_QUADS);
    // right
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glEnd();

    glBegin(GL_QUADS);
    // left
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(1.0, 1.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glEnd();

    glBegin(GL_QUADS);
    // front
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(0.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * at.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * at.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * at.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glEnd();

    glBegin(GL_QUADS);
    // back
    glColor4f(1.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 1.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * up.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * up.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * up.compoZ);
    glColor4f(0.0, 0.0, 1.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * left.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * left.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * left.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glColor4f(0.0, 0.0, 0.0, .75);
    glVertex3f(tankPos.coordX + hitBoxLength / 2.0 * back.compoX +
                       hitBoxWidth / 2.0 * right.compoX +
                       hitBoxHeight / 2.0 * down.compoX,
               tankPos.coordY + hitBoxLength / 2.0 * back.compoY +
                       hitBoxWidth / 2.0 * right.compoY +
                       hitBoxHeight / 2.0 * down.compoY,
               tankPos.coordZ + hitBoxLength / 2.0 * back.compoZ +
                       hitBoxWidth / 2.0 * right.compoZ +
                       hitBoxHeight / 2.0 * down.compoZ);
    glEnd();
}

GLfloat Tank::getCurrentPower() { return currentPower; }
int Tank::getPreviousPower() { return this->previousPower; }
int Tank::getPreviousAngle() { return this->previousAngle; }
GLfloat* Tank::getProjectileLandPos() { return projectileLandPos; }
void Tank::setPreviousPower(int previous_power) {
    this->previousPower = previous_power;
}
void Tank::setPreviousAngle(int previous_angle) {
    this->previousAngle = previous_angle;
}
void Tank::setProjectileLandPos(GLfloat x, GLfloat y) {
    projectileLandPos[0] = x;
    projectileLandPos[1] = y;
}

// remove later probably
void Tank::rotateWheel(GLfloat degrees) {
    if (!(wheel_degrees + degrees > 90) || (wheel_degrees - degrees < 0)) {
        wheel_degrees += degrees;
        glPushMatrix();
        glLoadMatrixf(wheelMatrix);
        glRotatef(degrees, wheelMatrix[4], wheelMatrix[5], wheelMatrix[6]);
        glGetFloatv(GL_MODELVIEW_MATRIX, wheelMatrix);
        glPopMatrix();
    }
}

void Tank::changeHeadTexture(int current_player_index) {
    if (current_player_index == 0)
        vbo_shader_head->SwapTexture("player1Head.raw", 1024, 1024);
    else if (current_player_index == 1)
        vbo_shader_head->SwapTexture("player2Head.raw", 1024, 1024);
    else if (current_player_index == 2)
        vbo_shader_head->SwapTexture("player3Head.raw", 1024, 1024);
    else if (current_player_index == 3)
        vbo_shader_head->SwapTexture("player4Head.raw", 1024, 1024);
    else if (current_player_index == 4)
        vbo_shader_head->SwapTexture("player5Head.raw", 1024, 1024);
    else if (current_player_index == 5)
        vbo_shader_head->SwapTexture("player6Head.raw", 1024, 1024);
    else if (current_player_index == 6)
        vbo_shader_head->SwapTexture("player7Head.raw", 1024, 1024);
    else if (current_player_index == 7)
        vbo_shader_head->SwapTexture("player8Head.raw", 1024, 1024);
    else if (current_player_index == 8)
        vbo_shader_head->SwapTexture("player9Head.raw", 1024, 1024);
    else if (current_player_index == 9)
        vbo_shader_head->SwapTexture("player10Head.raw", 1024, 1024);
    else {
        printf("\nERROR <Tank::changeHeadTexture(int)>: Wrong parameter "
               "passing.\n");
    }
}

int Tank::getCurrentHeight() { return currentHeight; }

void Tank::setCurrentHeight(int curr_height) { currentHeight = curr_height; }

int Tank::getPreviousHeight() { return previousHeight; }

void Tank::setPreviousHeight(int prev_height) { previousHeight = prev_height; }

// Getters for durations
int Tank::getDurationAcid() { return durationAcid; }
int Tank::getDurationShield() { return durationShield; }
int Tank::getDurationEMP() { return durationEMP; }
int Tank::getDurationFloat() { return durationFloat; }
int Tank::getDurationDoubleAction() { return durationDoubleAction; }
int Tank::getDurationPadlock() { return durationPadlock; }
int Tank::getDurationCloak() { return durationCloak; }
int Tank::getDurationParalyze() { return durationParalyze; }
// Setters for durations
void Tank::setDurationAcid(int value) {
    if (value != 0) {
        delete acidGen;
        acidGen = new ParticleGenerator(10, 5, 2, 100, 1);
    } else {
        if (acidGen) {
            acidGen->killGenerator();
            acidGen = nullptr;
        }
    }
    durationAcid = value;
}
void Tank::setDurationShield(int value) { durationShield = value; }
void Tank::setDurationEMP(int value) { durationEMP = value; }
void Tank::setDurationFloat(int value) {
    if (value != 0) {
        delete floatGen;
        floatGen = new ParticleGenerator(10, 5, 2, 100, 2);
    } else {
        if (floatGen) {
            floatGen->killGenerator();
            floatGen = nullptr;
        }
    }
    durationFloat = value;
}
void Tank::setDurationDoubleAction(int value) { durationDoubleAction = value; }
void Tank::setDurationPadlock(int value) { durationPadlock = value; }
void Tank::setDurationCloak(int value) { durationCloak = value; }
void Tank::setDurationParalyze(int value) { durationParalyze = value; }
void Tank::setDurationAllPassTurn() {
    // acid
    if (durationAcid > 0) {
        durationAcid--;
    }
    if ((acidGen) && (durationAcid == 0)) {
        acidGen->killGenerator();
        acidGen = nullptr;
    }

    // float
    if (durationFloat > 0) {
        durationFloat--;
    }
    if ((floatGen) && (durationFloat == 0)) {
        floatGen->killGenerator();
        floatGen = nullptr;
    }
    if (durationEMP > 0) durationEMP--;
    if (durationDoubleAction > 0) durationDoubleAction--;
    if (durationPadlock > 0) durationPadlock--;
    if (durationCloak > 0) durationCloak--;
    if (durationParalyze > 0) durationParalyze--;
}
void Tank::initBody() {
    bodyRight[0] = bodyMatrix[0] = 0;
    bodyRight[1] = bodyMatrix[1] = 0;
    bodyRight[2] = bodyMatrix[2] = 1;
    bodyMatrix[3] = 0;
    bodyUp[0] = bodyMatrix[4] = 0;
    bodyUp[1] = bodyMatrix[5] = 1;
    bodyUp[2] = bodyMatrix[6] = 0;
    bodyMatrix[7] = 0;
    bodyAt[0] = bodyMatrix[8] = 1;
    bodyAt[1] = bodyMatrix[9] = 0;
    bodyAt[2] = bodyMatrix[10] = 0;
    bodyMatrix[11] = 0;
    bodyMatrix[12] = 0;
    bodyMatrix[13] = 0;
    bodyMatrix[14] = 0;
    bodyMatrix[15] = 1;

    bodyColor[0] = 0.50f;
    bodyColor[1] = 0.50f;
    bodyColor[2] = 0.50f;
    bodyColor[3] = 1.0f;
    bodyScale[0] = 70;
    bodyScale[1] = 70;
    bodyScale[2] = 70;
    previousHeight = currentHeight = bodyMatrix[13];
}
void Tank::initHead() {
    headRight[0] = headMatrix[0] = 0;
    headRight[1] = headMatrix[1] = 0;
    headRight[2] = headMatrix[2] = 1;
    headMatrix[3] = 0;
    headUp[0] = headMatrix[4] = 0;
    headUp[1] = headMatrix[5] = 1;
    headUp[2] = headMatrix[6] = 0;
    headMatrix[7] = 0;
    headAt[0] = headMatrix[8] = 1;
    headAt[1] = headMatrix[9] = 0;
    headAt[2] = headMatrix[10] = 0;
    headMatrix[11] = 0;
    headMatrix[12] = 0;
    headMatrix[13] = 0;
    headMatrix[14] = 0;
    headMatrix[15] = 1;
    headColor[0] = 0.50f;
    headColor[1] = 0.50f;
    headColor[2] = 0.50f;
    headColor[3] = 1.0f;
    headScale[0] = 70;
    headScale[1] = 70;
    headScale[2] = 70;
}
void Tank::initTurret() {
    turretRight[0] = turretMatrix[0] = 0;
    turretRight[1] = turretMatrix[1] = 0;
    turretRight[2] = turretMatrix[2] = 1;
    turretMatrix[3] = 0;
    turretUp[0] = turretMatrix[4] = 0;
    turretUp[1] = turretMatrix[5] = 1;
    turretUp[2] = turretMatrix[6] = 0;
    turretMatrix[7] = 0;
    turretAt[0] = turretMatrix[8] = 1;
    turretAt[1] = turretMatrix[9] = 0;
    turretAt[2] = turretMatrix[10] = 0;
    turretMatrix[11] = 0;
    turretMatrix[12] = 0;
    turretMatrix[13] = 0;
    turretMatrix[14] = 0;
    turretMatrix[15] = 1;
    turretColor[0] = 0.50f;
    turretColor[1] = 0.50f;
    turretColor[2] = 0.50f;
    turretColor[3] = 1.0f;
    turretScale[0] = 70;
    turretScale[1] = 70;
    turretScale[2] = 70;

    turretOffset[0] = 0;
    turretOffset[1] = 0;
    turretOffset[2] = 0;

    turret_degrees = 0;
    wheel_degrees = 0;
}
void Tank::initWheel() {
    wheelRight[0] = wheelMatrix[0] = 0;
    wheelRight[1] = wheelMatrix[1] = 0;
    wheelRight[2] = wheelMatrix[2] = 1;
    wheelMatrix[3] = 0;
    wheelUp[0] = wheelMatrix[4] = 0;
    wheelUp[1] = wheelMatrix[5] = 1;
    wheelUp[2] = wheelMatrix[6] = 0;
    wheelMatrix[7] = 0;
    wheelAt[0] = wheelMatrix[8] = 1;
    wheelAt[1] = wheelMatrix[9] = 0;
    wheelAt[2] = wheelMatrix[10] = 0;
    wheelMatrix[11] = 0;
    wheelMatrix[12] = 0;
    wheelMatrix[13] = 0;
    wheelMatrix[14] = 0;
    wheelMatrix[15] = 1;
    wheelColor[0] = 0.50f;
    wheelColor[1] = 0.50f;
    wheelColor[2] = 0.50f;
    wheelColor[3] = 1.0f;
    wheelScale[0] = 70;
    wheelScale[1] = 70;
    wheelScale[2] = 70;
}

// GETTERS
const GLfloat* Tank::getBodyMatrix() { return &bodyMatrix[0]; }
const GLfloat* Tank::getBodyColor() { return &bodyColor[0]; }
float* Tank::getBodyScale() { return &bodyScale[0]; }

const GLfloat* Tank::getHeadMatrix() { return &headMatrix[0]; }
const GLfloat* Tank::getHeadColor() { return &headColor[0]; }
float* Tank::getHeadScale() { return &headScale[0]; }

GLfloat* Tank::getTurretMatrix() { return &turretMatrix[0]; }
const GLfloat* Tank::getTurretColor() { return &turretColor[0]; }
float* Tank::getTurretScale() { return &turretScale[0]; }

const GLfloat* Tank::getWheelMatrix() { return &wheelMatrix[0]; }
const GLfloat* Tank::getWheelColor() { return &wheelColor[0]; }
float* Tank::getWheelScale() { return &wheelScale[0]; }

int Tank::getHP() { return HP; }
int Tank::getPower() { return power; }
int Tank::getArmor() { return armor; }
int Tank::getSpeed() { return speed; }
Tank* Tank::getTankPointer() { return this; }