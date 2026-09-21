#include "Player.h"
#include "GameState.h"
#include "GlobalSettings.h"
#include "PlayerFactory.h"
#include "Tank.h"
#include "TerrainMaker.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

Player::Player() {
    state_of_AI = NEED_NEW_TARGET;
    prev_state_of_AI = NEED_NEW_TARGET;
    sub_state_of_AI = NOTHING;
    bool draw_debug_linesand_planes = false;
    maxPitchAngle = 89.0f;
    previous_distance_off_from_target = 0;
    degrees_rotated = 0;
    first_acquired_power = 0;
    first_acquired_pitch = 0;
}

Player::~Player() = default;

void Player::setTarget(Tank* target) {
    this->target = target;
    updateBalsticMatrix();
}

void Player::setGameState(GameState* game_state) {
    this->game_state = game_state;
}
Tank* Player::getTarget() { return this->target; }
Vertex Player::getEnemyPosition() { return enemyPosition; }
GLfloat* Player::getBalisticMatrix() { return balisticMatrix; }

/*	SAVE FOR LATER NEED IN CALCULATING PHYSICS	*/
/*	STATES = {	NEED_NEW_TARGET, FIND_TARGET,					*/
/*				HOMING_IN_ON_TARGET,ALL_TARGETS_NOT_REACHABLE,	*/
/*				ROTATING_LEFT, ROTATING_RIGHT };				*/
void Player::aiMainLogisticFunction() {
    /*	THIS IS THE BRAINS	*/
    if (prev_state_of_AI == SHOT_LAST_ROUND) {
        previous_projectile_landing_spot =
                this->game_state->getPositionOfLastProjectile();
        previous_distance_off_from_target = distance_off_from_target;
        distance_off_from_target =
                sqrt(pow((previous_projectile_landing_spot.coordX -
                          enemyPosition.coordX),
                         2) +
                     pow((previous_projectile_landing_spot.coordY -
                          enemyPosition.coordY),
                         2) +
                     pow((previous_projectile_landing_spot.coordZ -
                          enemyPosition.coordZ),
                         2));
    }

    if (state_of_AI == NEED_NEW_TARGET || this->target == nullptr) {
        /*	Game State Will Set Your Target For You	*/
        /*	You Need To Check Though That Your Tank	*/
        /*	Was Set									*/
        restoreTurretTo0Degrees();
        updateBalsticMatrix();
        this->game_state->nearestEnemy();
        if (this->target) {
            setEnemyPosition();
            setUpYawVectors();
            state_of_AI = FIND_TARGET;
        }
    }
    if (state_of_AI == FIND_TARGET) {
        playSFX(TANK_CONTROL2);
        updateBalsticMatrix();
        setUpYawVectors();
        yaw_angle = this->game_state->calcAngleBetweenVectors(enemy_path,
                                                              projectile_path);
        _rangle = this->game_state->calcAngleBetweenVectors(enemy_path,
                                                            ortho_left);
        _langle = this->game_state->calcAngleBetweenVectors(enemy_path,
                                                            ortho_right);
        if (degrees_rotated > 540) {
            state_of_AI = NEED_NEW_TARGET;
            degrees_rotated = 0;
        }
        if (yaw_angle > 5) {
            if (minimumYawAngle(_rangle, _langle) == 'r') {
                yawRight(0.4f);
            } else if (minimumYawAngle(_rangle, _langle) == 'l') {
                yawLeft(0.4f);
            }
            degrees_rotated += 0.4;
        } else if (yaw_angle <= 5 && yaw_angle > 1) {
            if (minimumYawAngle(_rangle, _langle) == 'r') {
                yawRight(0.2f);
            } else if (minimumYawAngle(_rangle, _langle) == 'l') {
                yawLeft(0.2f);
            }
            degrees_rotated += 0.2;
        } else if (yaw_angle <= 1 && yaw_angle > .1) {
            if (minimumYawAngle(_rangle, _langle) == 'r') {
                yawRight(0.09f);
            } else if (minimumYawAngle(_rangle, _langle) == 'l') {
                yawLeft(0.09f);
            }
            degrees_rotated += 0.09;
        } else {
            state_of_AI = HOMING_IN_ON_TARGET;
            Mix_HaltChannel(3);
        }
    }
    if (state_of_AI == HOMING_IN_ON_TARGET) {
        playSFX(TANK_CONTROL1);
        setUpPitchVectors();
        updateBalsticMatrix();
        setUpYawVectors();
        bool physics = false;
        pitch_angle = this->game_state->calcAngleBetweenVectors(enemy_path,
                                                                pitch_vector);
        if (pitch_angle < maxPitchAngle && !physics) {
            if (getCurrentTank()->getCurrentPower() >= .4 && !physics) {
                physics = calculateProjectilePhysics(600, 100, 600);
                getCurrentTank()->adjustPower(-.09990238);
            } else if (getCurrentTank()->getCurrentPower() <= .5) {
                getCurrentTank()->adjustPower(9.5);
                pitchUp(2.0f);
            }
        }
        if (pitch_angle > maxPitchAngle) {
            state_of_AI = TARGET_NOT_REACHABLE;
            Mix_HaltChannel(2);
            Mix_HaltChannel(3);
        }
        if (!physics) {
            yaw_angle = this->game_state->calcAngleBetweenVectors(
                    enemy_path, projectile_path);
            if (yaw_angle > .01) {
                state_of_AI = FIND_TARGET;
                Mix_HaltChannel(2);
            }
        }
        if (physics) {
            this->game_state->currentPlayerFire();
            degrees_rotated = 0;
            prev_state_of_AI = SHOT_LAST_ROUND;
            state_of_AI = WALKING_IN;
            first_acquired_power = getCurrentTank()->getCurrentPower();
            first_acquired_pitch = pitch_angle;
        }
    }
    if (state_of_AI == WALKING_IN) {
        /*const GLfloat* velMatrix = getCurrentTank()->getTurretMatrix();
        GLfloat tankAttributePower = first_acquired_power;
        GLfloat powerBar = getCurrentTank()->getCurrentPower();
        GLfloat scalar = this->game_state->getBalisticScalar();
        GLfloat power = tankAttributePower*powerBar*scalar;
        GLfloat mag = sqrt	(
                                pow((power*velMatrix[8]),2) +
                                pow((power*velMatrix[9]),2)	+
                                pow((power*velMatrix[10]),2)
                            );
        GLfloat ang = first_acquired_pitch;*/
        degrees_rotated = 0;
        this->game_state->currentPlayerFire();
        prev_state_of_AI = SHOT_LAST_ROUND;
        state_of_AI = WALKING_IN;
    }
    if (state_of_AI == TARGET_NOT_REACHABLE) {
        state_of_AI = NEED_NEW_TARGET;
    }
}

char Player::minimumYawAngle(GLfloat right_degrees, GLfloat left_degrees) {
    if (right_degrees < left_degrees) {
        return 'r';
    } else if (right_degrees >= left_degrees) {
        return 'l';
    } else {
        return 'e';
    }
}

void Player::updateBalsticMatrix() {
    for (int x = 0; x < 16; x++) {
        balisticMatrix[x] = getCurrentTank()->getTurretMatrix()[x];
    }
    balisticMatrix[1] = 0;
    balisticMatrix[4] = 0;
    balisticMatrix[6] = 0;
    balisticMatrix[9] = 0;
}

void Player::setEnemyPosition() {
    updateBalsticMatrix();
    enemyPosition.coordX = this->target->getBodyMatrix()[12];
    enemyPosition.coordY = this->target->getBodyMatrix()[13];
    enemyPosition.coordZ = this->target->getBodyMatrix()[14];
}

void Player::setUpYawVectors() {
    /* THIS IS KEY FOR ALL CALCULATIONS TO LINE UP TANK WITH THE PROJECTILE
     * PATH */
    updateBalsticMatrix();
    const GLfloat* matrix = getBalisticMatrix();
    Vertex v = getEnemyPosition();

    // Forumlate Perpendicular Vector
    /********************************************************************************************************************/
    /*	Gram-Schmidt Orthogonalization
     */
    /*	w2 = v2 - ((w1*v2)/||w1||^2)*w1
     */
    /*	* -> dot product
     */
    /*	w1 = v1, and {v1,v2} are elements of non-orthoginal basis (must be
     * linearly independent)						*/
    /*	w2 is orthogonal to w1
     */
    /********************************************************************************************************************/
    Vector v1(matrix[8], 0, matrix[10]);
    Vector w1(v1.compoX, v1.compoY, v1.compoZ);
    // Test for linear independence
    Vector v2(0, 0, 0);
    if (v1.compoY == 0 && v1.compoZ == 0) {
        v2.compoX = 0;
        v2.compoY = 0;
        v2.compoZ = 1;
    } else {
        v2.compoX = -v1.compoX;
        v2.compoY = v1.compoY;
        v2.compoZ = v1.compoZ;
    }
    GLfloat magnitude_w1 =
            sqrt((w1.compoX * w1.compoX) + (w1.compoY * w1.compoY) +
                 (w1.compoZ * w1.compoZ));
    GLfloat scalar = (w1.compoX * v2.compoX + w1.compoY * v2.compoY +
                      w1.compoZ * v2.compoZ) /
                     (pow(static_cast<double>(magnitude_w1), 2.0));
    w1.compoX *= scalar;
    w1.compoY *= scalar;
    w1.compoZ *= scalar;
    Vector w2((v2.compoX - w1.compoX),
              (v2.compoY - w1.compoY),
              (v2.compoZ - w1.compoZ));
    if ((w1.compoX <= 0 && w1.compoZ <= 0) ||
        (w1.compoX >= 0 && w1.compoZ >= 0)) {
        w2.compoX *= -1;
        w2.compoZ *= -1;
    }
    /********************************************************************************************************************/
    /*	End Gram-Schmidt Orthogonalization
     */
    /*	w2 = v2 - ((w1*v2)/||w1||^2)*w1
     */
    /*	* -> dot product
     */
    /*	w1 = v1, and {v1,v2} are elements of non-orthoginal basis (must be
     * linearly independent)						*/
    /*	w2 is orthogonal to w1
     */
    /********************************************************************************************************************/

    // Get Vertices
    Vertex my_position(matrix[12], matrix[13], matrix[14]);
    Vertex enemy_position(v.coordX, v.coordY, v.coordZ);
    Vertex projectile_endmark((matrix[12] - 10000 * matrix[8]),
                              (matrix[13]),
                              (matrix[14] - 10000 * matrix[10]));
    Vertex left_mark((matrix[12] + 1000 * matrix[0]),
                     (matrix[13]),
                     (matrix[14] + 1000 * matrix[2]));
    Vertex right_mark((matrix[12] - 1000 * matrix[0]),
                      (matrix[13]),
                      (matrix[14] - 1000 * matrix[2]));
    Vertex left_ortho((matrix[12] + 1000 * w2.compoX),
                      (matrix[13]),
                      (matrix[14] + 1000 * w2.compoZ));
    Vertex right_ortho((matrix[12] - 1000 * w2.compoX),
                       (matrix[13]),
                       (matrix[14] - 1000 * w2.compoZ));

    // Produce Member Vectors
    ortho_left.compoX = (left_ortho.coordX - my_position.coordX);
    ortho_left.compoY = (left_ortho.coordY - my_position.coordY);
    ortho_left.compoZ = (left_ortho.coordZ - my_position.coordZ);

    ortho_right.compoX = (right_ortho.coordX - my_position.coordX);
    ortho_right.compoY = (right_ortho.coordY - my_position.coordY);
    ortho_right.compoZ = (right_ortho.coordZ - my_position.coordZ);

    enemy_path.compoX = (enemy_position.coordX - my_position.coordX);
    enemy_path.compoY = (my_position.coordY - my_position.coordY);
    enemy_path.compoZ = (enemy_position.coordZ - my_position.coordZ);

    projectile_path.compoX = (projectile_endmark.coordX - my_position.coordX);
    projectile_path.compoY = (projectile_endmark.coordY - my_position.coordY);
    projectile_path.compoZ = (projectile_endmark.coordZ - my_position.coordZ);

    left_vector.compoX = (left_mark.coordX - my_position.coordX);
    left_vector.compoY = (left_mark.coordY - my_position.coordY);
    left_vector.compoZ = (left_mark.coordZ - my_position.coordZ);

    right_vector.compoX = (right_mark.coordX - my_position.coordX);
    right_vector.compoY = (right_mark.coordY - my_position.coordY);
    right_vector.compoZ = (right_mark.coordZ - my_position.coordZ);
}

void Player::setUpPitchVectors() {
    const GLfloat* matrix = getBalisticMatrix();
    GLfloat* turret_matrix = getCurrentTank()->getTurretMatrix();
    pitch_vector.compoX = ((turret_matrix[12] - 10000 * turret_matrix[8]) -
                           turret_matrix[12]);
    pitch_vector.compoY = ((turret_matrix[13] - 10000 * turret_matrix[9]) -
                           turret_matrix[13]);
    pitch_vector.compoZ = ((turret_matrix[14] - 10000 * turret_matrix[10]) -
                           turret_matrix[14]);
    up_vector.compoX = (matrix[12] - matrix[12]);
    up_vector.compoY = ((matrix[13] + 1000 * matrix[5]) - matrix[13]);
    up_vector.compoZ = (matrix[14] - matrix[14]);
    down_vector.compoX = (matrix[12] - matrix[12]);
    down_vector.compoY = ((matrix[13] - 1000 * matrix[5]) - matrix[13]);
    down_vector.compoZ = (matrix[14] - matrix[14]);
}

bool Player::calculateProjectilePhysics(GLfloat xerr,
                                        GLfloat yerr,
                                        GLfloat zerr) {
    /*	VARIABLES NEEDED BY GAMESTATE.CPP	*/
    GLfloat percent_errory = yerr;
    GLfloat percent_errorxz = xerr;
    GLfloat numerator = this->game_state->getGlobalSettings()
                                ->getCurrentTerrain()
                                ->getActualSize();
    GLfloat denominator = this->game_state->getGlobalSettings()
                                  ->getCurrentTerrain()
                                  ->getScale();
    GLfloat terrain_size = numerator / denominator;
    GLfloat g = this->game_state->getGravity();  // Note: gravity is negative
    GLfloat tank_attribute_power = getCurrentTank()->getPower();
    GLfloat power_bar = getCurrentTank()->getCurrentPower();
    GLfloat balistic_scalar = this->game_state->getBalisticScalar();
    GLfloat speed = tank_attribute_power * power_bar * balistic_scalar;

    /****************************************************************************************/
    /*	Physics Calculations First (Formulas)
     */
    /*	Yf = Yo + Voy*time - 1/2*(gravity)*t^2
     */
    /*	Xf = Xo + Vox*time */
    /*	Zf = Zo + Voz*time */
    /****************************************************************************************/
    GLfloat* turret_matrix = getCurrentTank()->getTurretMatrix();
    // MAKE SURE TO UPDATE 200 TO WHAT EVER SCALAR IS IN PROJECTILE.CPP
    GLfloat xo = turret_matrix[12] - 200 * turret_matrix[8];
    GLfloat yo = turret_matrix[13] - 200 * turret_matrix[9];
    GLfloat zo = turret_matrix[14] - 200 * turret_matrix[10];
    GLfloat vox = -turret_matrix[8] * speed;
    GLfloat voy = -turret_matrix[9] * speed;
    GLfloat voz = -turret_matrix[10] * speed;
    GLfloat xf = xo;
    GLfloat yf = yo;
    GLfloat zf = zo;
    GLfloat xe = getEnemyPosition().coordX;
    GLfloat ye = getEnemyPosition().coordY;
    GLfloat ze = getEnemyPosition().coordZ;
    GLfloat t = 0;
    bool on_target = false;
    while (yf > 0) {
        xf = vox * t + xo;
        yf = .5 * g * pow(t, 2.0f) + voy * t + yo;
        zf = voz * t + zo;

        if (abs(xf - xe) <= percent_errorxz &&
            abs(zf - ze) <= percent_errorxz &&
            abs(yf - ye) <= percent_errory) {
            on_target = true;
            break;
        }

        GLfloat terrain_height = this->game_state->getGlobalSettings()
                                         ->getCurrentTerrain()
                                         ->getHeightAt(zf, xf);
        if (terrain_height >= yf) {
            break;
        }

        t = t + .02;
    }
    return on_target;
}

void Player::displayProjectilePhysiscs() {
    /*	VARIABLES NEEDED BY GAMESTATE.CPP	*/
    GLfloat tank_attribute_power = getCurrentTank()->getPower();
    GLfloat power_bar = getCurrentTank()->getCurrentPower();
    GLfloat balistic_scalar = this->game_state->getBalisticScalar();
    GLfloat speed = tank_attribute_power * power_bar * balistic_scalar;
    GLfloat g = this->game_state->getGravity();  // Note: gravity is negative

    GLfloat* turret_matrix = getCurrentTank()->getTurretMatrix();
    // MAKE SURE TO UPDATE 200 TO WHAT EVER SCALAR IS IN PROJECTILE.CPP
    GLfloat xo = turret_matrix[12] - 200 * turret_matrix[8];
    GLfloat yo = turret_matrix[13] - 200 * turret_matrix[9];
    GLfloat zo = turret_matrix[14] - 200 * turret_matrix[10];
    GLfloat vox = -turret_matrix[8] * speed;
    GLfloat voy = -turret_matrix[9] * speed;
    GLfloat voz = -turret_matrix[10] * speed;
    GLfloat xf = xo;
    GLfloat yf = yo;
    GLfloat zf = zo;
    GLfloat xe = getEnemyPosition().coordX;
    GLfloat ye = getEnemyPosition().coordY;
    GLfloat ze = getEnemyPosition().coordZ;

    GLfloat t = 0;
    while (yf > 0) {
        xf = vox * t + xo;
        yf = .5 * g * pow(t, 2.0f) + voy * t + yo;
        zf = voz * t + zo;
        t = t + .02;
    }

    cout << "Variables:" << endl;
    cout << " Vox = " << vox << " Voy = " << voy << " Voz = " << voz << endl
         << endl
         << " Xo = " << xo << " Yo = " << yo << " Zo = " << zo << endl
         << endl
         << " Xf = " << xf << " Yf = " << yf << " Zf = " << zf << endl
         << endl
         << " Xe = " << xe << " Ye = " << ye << " Ze = " << ze << endl
         << endl
         << " g = " << g << endl
         << endl;
}

int Player::getAIState() { return state_of_AI; }

void Player::restoreTurretTo0Degrees() {
    GLfloat restore_angle = getCurrentTank()->getTurretDegrees();
    getCurrentTank()->rotateTurret(-1 * restore_angle);
}

void Player::yawLeft(GLfloat degrees) {
    getCurrentTank()->rotateHead(-degrees);
}

void Player::yawRight(GLfloat degrees) {
    getCurrentTank()->rotateHead(degrees);
}

void Player::pitchUp(GLfloat degrees) {
    getCurrentTank()->rotateTurret(degrees);
}

void Player::pitchDown(GLfloat degrees) {
    getCurrentTank()->rotateTurret(-degrees);
}

void Player::drawTestLinesandPlanes() {
    GLfloat scalar = 1000;
    /*	START BALISTIC AXES	*/
    glBegin(GL_LINES);
    glColor3f(Red);
    Vertex xyzri(balisticMatrix[12], balisticMatrix[13], balisticMatrix[14]);
    Vertex xyzrf(scalar * balisticMatrix[0],
                 scalar * balisticMatrix[1],
                 scalar * balisticMatrix[2]);
    glVertex3f(xyzri.coordX, xyzri.coordY, xyzri.coordZ);
    glVertex3f(xyzri.coordX + xyzrf.coordX,
               xyzri.coordY + xyzrf.coordY,
               xyzri.coordZ + xyzrf.coordZ);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(Green);
    Vertex xyzui(balisticMatrix[12], balisticMatrix[13], balisticMatrix[14]);
    Vertex xyzuf(scalar * balisticMatrix[4],
                 scalar * balisticMatrix[5],
                 scalar * balisticMatrix[6]);
    glVertex3f(xyzui.coordX, xyzui.coordY, xyzui.coordZ);
    glVertex3f(xyzui.coordX + xyzuf.coordX,
               xyzui.coordY + xyzuf.coordY,
               xyzui.coordZ + xyzuf.coordZ);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(Blue);
    Vertex xyzai(balisticMatrix[12], balisticMatrix[13], balisticMatrix[14]);
    Vertex xyzaf(scalar * balisticMatrix[8],
                 scalar * balisticMatrix[9],
                 scalar * balisticMatrix[10]);
    glVertex3f(xyzai.coordX, xyzai.coordY, xyzai.coordZ);
    glVertex3f(xyzai.coordX + xyzaf.coordX,
               xyzai.coordY + xyzaf.coordY,
               xyzai.coordZ + xyzaf.coordZ);
    glEnd();
    /*	END BALISTIC AXES	*/

    if (this->target) {
        /*	START ENEMY VECTOR AXES	*/
        glBegin(GL_LINES);
        glColor3f(LightSteelBlue);
        Vertex mypos(
                balisticMatrix[12], balisticMatrix[13], balisticMatrix[14]);
        Vertex enemypos(
                enemyPosition.coordX, mypos.coordY, enemyPosition.coordZ);
        glVertex3f(mypos.coordX, mypos.coordY, mypos.coordZ);
        glVertex3f(enemypos.coordX, enemypos.coordY, enemypos.coordZ);
        glEnd();
        /*	END ENEMY VECTOR AXES	*/
        ///*	START PERP VECTORS	*/
        // glBegin(GL_LINES);
        //	glColor3f(MediumSeaGreen);
        //	Vertex
        // o1(balisticMatrix[12],balisticMatrix[13],balisticMatrix[14]);
        //	Vertex f1(	o1.coordX+scalar*ortho_left.compoX,
        //				o1.coordY+scalar*ortho_left.compoY,
        //				o1.coordZ+scalar*ortho_left.compoZ);
        //	glVertex3f(o1.coordX,o1.coordY,o1.coordZ);
        //	glVertex3f(f1.coordX,f1.coordY,f1.coordZ);
        // glEnd();
        // glBegin(GL_LINES);
        //	glColor3f(PaleGreen);
        //	Vertex
        // o2(balisticMatrix[12],balisticMatrix[13],balisticMatrix[14]);
        //	Vertex f2(	o2.coordX+scalar*ortho_right.compoX,
        //				o2.coordY+scalar*ortho_right.compoY,
        //				o2.coordZ+scalar*ortho_right.compoZ);
        //	glVertex3f(o2.coordX,o2.coordY,o2.coordZ);
        //	glVertex3f(f2.coordX,f2.coordY,f2.coordZ);
        // glEnd();
        ///*	END PERP VECTORS	*/
        /*	START PROJECTILE PATH	*/
        glBegin(GL_LINES);
        glColor3f(MediumGoldenrod);
        Vertex o3(balisticMatrix[12], balisticMatrix[13], balisticMatrix[14]);
        Vertex f3(o3.coordX - 100000 * balisticMatrix[8],
                  o3.coordY,
                  o3.coordZ - 100000 * balisticMatrix[10]);
        glVertex3f(o3.coordX, o3.coordY, o3.coordZ);
        glVertex3f(f3.coordX, f3.coordY, f3.coordZ);
        glEnd();
        /*	END	PROJECTILE PATH		*/

        glBegin(GL_QUADS);
        const GLfloat* matrix = getBalisticMatrix();
        glColor4f(Pink, 0.75f);
        glVertex3f(matrix[12] - 1000 * matrix[0],
                   matrix[13],
                   matrix[14] - 1000 * matrix[2]);
        glVertex3f(matrix[12] - 1000 * matrix[0] - 10000 * matrix[8],
                   matrix[13],
                   matrix[14] - 1000 * matrix[2] - 10000 * matrix[10]);
        glVertex3f(matrix[12] + 1000 * matrix[0] - 10000 * matrix[8],
                   matrix[13],
                   matrix[14] + 1000 * matrix[2] - 10000 * matrix[10]);
        glVertex3f(matrix[12] + 1000 * matrix[0],
                   matrix[13],
                   matrix[14] + 1000 * matrix[2]);
        glEnd();

        glBegin(GL_QUADS);
        const GLfloat* t_matrix = getCurrentTank()->getTurretMatrix();
        glColor4f(Red, 0.75f);
        glVertex3f(t_matrix[12] - 1000 * t_matrix[0],
                   t_matrix[13] - 1000 * t_matrix[1],
                   t_matrix[14] - 1000 * t_matrix[2]);
        glVertex3f(t_matrix[12] - 1000 * t_matrix[0] - 10000 * t_matrix[8],
                   t_matrix[13] - 1000 * t_matrix[1] - 10000 * t_matrix[9],
                   t_matrix[14] - 1000 * t_matrix[2] - 10000 * t_matrix[10]);
        glVertex3f(t_matrix[12] + 1000 * t_matrix[0] - 10000 * t_matrix[8],
                   t_matrix[13] + 1000 * t_matrix[1] - 10000 * t_matrix[9],
                   t_matrix[14] + 1000 * t_matrix[2] - 10000 * t_matrix[10]);
        glVertex3f(t_matrix[12] + 1000 * t_matrix[0],
                   t_matrix[13] + 1000 * t_matrix[1],
                   t_matrix[14] + 1000 * t_matrix[2]);
        glEnd();
    }
}

bool Player::getDrawDebugLinesandPlanes() { return drawDebugLinesandPlanes; }
void Player::setDrawDebugLinesandPlanes(bool flag) {
    drawDebugLinesandPlanes = flag;
}