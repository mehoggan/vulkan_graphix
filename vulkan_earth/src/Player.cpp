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
    state_of_ai = NEED_NEW_TARGET;
    prev_state_of_ai = NEED_NEW_TARGET;
    sub_state_of_ai = NOTHING;
    draw_debug_linesand_planes = false;
    max_pitch_angle = 89.0f;
    previous_distance_off_from_target = 0;
    degrees_rotated = 0;
    first_acquired_power = 0;
    first_acquired_pitch = 0;
}

Player::~Player() = default;

void Player::setTarget(Tank* new_target) {
    target = new_target;
    updateBalsticMatrix();
}

void Player::setGameState(GameState* new_game_state) {
    game_state = new_game_state;
}
Tank* Player::getTarget() { return target; }
Vertex Player::getEnemyPosition() { return enemy_position; }
GLfloat* Player::getBalisticMatrix() { return balistic_matrix; }

/*	SAVE FOR LATER NEED IN CALCULATING PHYSICS	*/
/*	STATES = {	NEED_NEW_TARGET, FIND_TARGET,					*/
/*				HOMING_IN_ON_TARGET,ALL_TARGETS_NOT_REACHABLE,	*/
/*				ROTATING_LEFT, ROTATING_RIGHT };				*/
void Player::aiMainLogisticFunction() {
    /*	THIS IS THE BRAINS	*/
    if (prev_state_of_ai == SHOT_LAST_ROUND) {
        previous_projectile_landing_spot =
                game_state->getPositionOfLastProjectile();
        previous_distance_off_from_target = distance_off_from_target;
        distance_off_from_target =
                sqrt(pow((previous_projectile_landing_spot.coord_x -
                          enemy_position.coord_x),
                         2) +
                     pow((previous_projectile_landing_spot.coord_y -
                          enemy_position.coord_y),
                         2) +
                     pow((previous_projectile_landing_spot.coord_z -
                          enemy_position.coord_z),
                         2));
    }

    if (state_of_ai == NEED_NEW_TARGET || target == nullptr) {
        /*	Game State Will Set Your Target For You	*/
        /*	You Need To Check Though That Your Tank	*/
        /*	Was Set									*/
        restoreTurretTo0Degrees();
        updateBalsticMatrix();
        game_state->nearestEnemy();
        if (target) {
            setEnemyPosition();
            setUpYawVectors();
            state_of_ai = FIND_TARGET;
        }
    }
    if (state_of_ai == FIND_TARGET) {
        playSFX(TANK_CONTROL2);
        updateBalsticMatrix();
        setUpYawVectors();
        yaw_angle = game_state->calcAngleBetweenVectors(enemy_path,
                                                        projectile_path);
        rangle = game_state->calcAngleBetweenVectors(enemy_path, ortho_left);
        langle = game_state->calcAngleBetweenVectors(enemy_path, ortho_right);
        if (degrees_rotated > 540) {
            state_of_ai = NEED_NEW_TARGET;
            degrees_rotated = 0;
        }
        if (yaw_angle > 5) {
            if (minimumYawAngle(rangle, langle) == 'r') {
                yawRight(0.4f);
            } else if (minimumYawAngle(rangle, langle) == 'l') {
                yawLeft(0.4f);
            }
            degrees_rotated += 0.4;
        } else if (yaw_angle <= 5 && yaw_angle > 1) {
            if (minimumYawAngle(rangle, langle) == 'r') {
                yawRight(0.2f);
            } else if (minimumYawAngle(rangle, langle) == 'l') {
                yawLeft(0.2f);
            }
            degrees_rotated += 0.2;
        } else if (yaw_angle <= 1 && yaw_angle > .1) {
            if (minimumYawAngle(rangle, langle) == 'r') {
                yawRight(0.09f);
            } else if (minimumYawAngle(rangle, langle) == 'l') {
                yawLeft(0.09f);
            }
            degrees_rotated += 0.09;
        } else {
            state_of_ai = HOMING_IN_ON_TARGET;
            Mix_HaltChannel(3);
        }
    }
    if (state_of_ai == HOMING_IN_ON_TARGET) {
        playSFX(TANK_CONTROL1);
        setUpPitchVectors();
        updateBalsticMatrix();
        setUpYawVectors();
        bool physics = false;
        pitch_angle =
                game_state->calcAngleBetweenVectors(enemy_path, pitch_vector);
        if (pitch_angle < max_pitch_angle && !physics) {
            if (getCurrentTank()->getCurrentPower() >= .4 && !physics) {
                physics = calculateProjectilePhysics(600, 100, 600);
                getCurrentTank()->adjustPower(-.09990238);
            } else if (getCurrentTank()->getCurrentPower() <= .5) {
                getCurrentTank()->adjustPower(9.5);
                pitchUp(2.0f);
            }
        }
        if (pitch_angle > max_pitch_angle) {
            state_of_ai = TARGET_NOT_REACHABLE;
            Mix_HaltChannel(2);
            Mix_HaltChannel(3);
        }
        if (!physics) {
            yaw_angle = game_state->calcAngleBetweenVectors(enemy_path,
                                                            projectile_path);
            if (yaw_angle > .01) {
                state_of_ai = FIND_TARGET;
                Mix_HaltChannel(2);
            }
        }
        if (physics) {
            game_state->currentPlayerFire();
            degrees_rotated = 0;
            prev_state_of_ai = SHOT_LAST_ROUND;
            state_of_ai = WALKING_IN;
            first_acquired_power = getCurrentTank()->getCurrentPower();
            first_acquired_pitch = pitch_angle;
        }
    }
    if (state_of_ai == WALKING_IN) {
        /*const GLfloat* velMatrix = getCurrentTank()->getTurretMatrix();
        GLfloat tankAttributePower = first_acquired_power;
        GLfloat powerBar = getCurrentTank()->getCurrentPower();
        GLfloat scalar = game_state->getBalisticScalar();
        GLfloat power = tankAttributePower*powerBar*scalar;
        GLfloat mag = sqrt	(
                                pow((power*velMatrix[8]),2) +
                                pow((power*velMatrix[9]),2)	+
                                pow((power*velMatrix[10]),2)
                            );
        GLfloat ang = first_acquired_pitch;*/
        degrees_rotated = 0;
        game_state->currentPlayerFire();
        prev_state_of_ai = SHOT_LAST_ROUND;
        state_of_ai = WALKING_IN;
    }
    if (state_of_ai == TARGET_NOT_REACHABLE) {
        state_of_ai = NEED_NEW_TARGET;
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
        balistic_matrix[x] = getCurrentTank()->getTurretMatrix()[x];
    }
    balistic_matrix[1] = 0;
    balistic_matrix[4] = 0;
    balistic_matrix[6] = 0;
    balistic_matrix[9] = 0;
}

void Player::setEnemyPosition() {
    updateBalsticMatrix();
    enemy_position.coord_x = target->getBodyMatrix()[12];
    enemy_position.coord_y = target->getBodyMatrix()[13];
    enemy_position.coord_z = target->getBodyMatrix()[14];
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
    Vector w1(v1.compo_x, v1.compo_y, v1.compo_z);
    // Test for linear independence
    Vector v2(0, 0, 0);
    if (v1.compo_y == 0 && v1.compo_z == 0) {
        v2.compo_x = 0;
        v2.compo_y = 0;
        v2.compo_z = 1;
    } else {
        v2.compo_x = -v1.compo_x;
        v2.compo_y = v1.compo_y;
        v2.compo_z = v1.compo_z;
    }
    GLfloat magnitude_w1 =
            sqrt((w1.compo_x * w1.compo_x) + (w1.compo_y * w1.compo_y) +
                 (w1.compo_z * w1.compo_z));
    GLfloat scalar = (w1.compo_x * v2.compo_x + w1.compo_y * v2.compo_y +
                      w1.compo_z * v2.compo_z) /
                     (pow(static_cast<double>(magnitude_w1), 2.0));
    w1.compo_x *= scalar;
    w1.compo_y *= scalar;
    w1.compo_z *= scalar;
    Vector w2((v2.compo_x - w1.compo_x),
              (v2.compo_y - w1.compo_y),
              (v2.compo_z - w1.compo_z));
    if ((w1.compo_x <= 0 && w1.compo_z <= 0) ||
        (w1.compo_x >= 0 && w1.compo_z >= 0)) {
        w2.compo_x *= -1;
        w2.compo_z *= -1;
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
    Vertex enemy_vertex(v.coord_x, v.coord_y, v.coord_z);
    Vertex projectile_endmark((matrix[12] - 10000 * matrix[8]),
                              (matrix[13]),
                              (matrix[14] - 10000 * matrix[10]));
    Vertex left_mark((matrix[12] + 1000 * matrix[0]),
                     (matrix[13]),
                     (matrix[14] + 1000 * matrix[2]));
    Vertex right_mark((matrix[12] - 1000 * matrix[0]),
                      (matrix[13]),
                      (matrix[14] - 1000 * matrix[2]));
    Vertex left_ortho((matrix[12] + 1000 * w2.compo_x),
                      (matrix[13]),
                      (matrix[14] + 1000 * w2.compo_z));
    Vertex right_ortho((matrix[12] - 1000 * w2.compo_x),
                       (matrix[13]),
                       (matrix[14] - 1000 * w2.compo_z));

    // Produce Member Vectors
    ortho_left.compo_x = (left_ortho.coord_x - my_position.coord_x);
    ortho_left.compo_y = (left_ortho.coord_y - my_position.coord_y);
    ortho_left.compo_z = (left_ortho.coord_z - my_position.coord_z);

    ortho_right.compo_x = (right_ortho.coord_x - my_position.coord_x);
    ortho_right.compo_y = (right_ortho.coord_y - my_position.coord_y);
    ortho_right.compo_z = (right_ortho.coord_z - my_position.coord_z);

    enemy_path.compo_x = (enemy_vertex.coord_x - my_position.coord_x);
    enemy_path.compo_y = (enemy_vertex.coord_y - my_position.coord_y);
    enemy_path.compo_z = (enemy_vertex.coord_z - my_position.coord_z);

    projectile_path.compo_x =
            (projectile_endmark.coord_x - my_position.coord_x);
    projectile_path.compo_y =
            (projectile_endmark.coord_y - my_position.coord_y);
    projectile_path.compo_z =
            (projectile_endmark.coord_z - my_position.coord_z);

    left_vector.compo_x = (left_mark.coord_x - my_position.coord_x);
    left_vector.compo_y = (left_mark.coord_y - my_position.coord_y);
    left_vector.compo_z = (left_mark.coord_z - my_position.coord_z);

    right_vector.compo_x = (right_mark.coord_x - my_position.coord_x);
    right_vector.compo_y = (right_mark.coord_y - my_position.coord_y);
    right_vector.compo_z = (right_mark.coord_z - my_position.coord_z);
}

void Player::setUpPitchVectors() {
    const GLfloat* matrix = getBalisticMatrix();
    GLfloat* turret_matrix = getCurrentTank()->getTurretMatrix();
    pitch_vector.compo_x = ((turret_matrix[12] - 10000 * turret_matrix[8]) -
                            turret_matrix[12]);
    pitch_vector.compo_y = ((turret_matrix[13] - 10000 * turret_matrix[9]) -
                            turret_matrix[13]);
    pitch_vector.compo_z = ((turret_matrix[14] - 10000 * turret_matrix[10]) -
                            turret_matrix[14]);
    up_vector.compo_x = ((matrix[12] + 1000 * matrix[4]) - matrix[12]);
    up_vector.compo_y = ((matrix[13] + 1000 * matrix[5]) - matrix[13]);
    up_vector.compo_z = ((matrix[14] + 1000 * matrix[6]) - matrix[14]);
    down_vector.compo_x = ((matrix[12] - 1000 * matrix[4]) - matrix[12]);
    down_vector.compo_y = ((matrix[13] - 1000 * matrix[5]) - matrix[13]);
    down_vector.compo_z = ((matrix[14] - 1000 * matrix[6]) - matrix[14]);
}

bool Player::calculateProjectilePhysics(GLfloat xerr,
                                        GLfloat yerr,
                                        GLfloat zerr) {
    /*	VARIABLES NEEDED BY GAMESTATE.CPP	*/
    GLfloat percent_errory = yerr;
    GLfloat percent_errorxz = xerr;
    GLfloat numerator = game_state->getGlobalSettings()
                                ->getCurrentTerrain()
                                ->getActualSize();
    GLfloat denominator =
            game_state->getGlobalSettings()->getCurrentTerrain()->getScale();
    GLfloat terrain_size = numerator / denominator;
    GLfloat g = game_state->getGravity();  // Note: gravity is negative
    GLfloat tank_attribute_power = getCurrentTank()->getPower();
    GLfloat power_bar = getCurrentTank()->getCurrentPower();
    GLfloat balistic_scalar = game_state->getBalisticScalar();
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
    GLfloat xe = getEnemyPosition().coord_x;
    GLfloat ye = getEnemyPosition().coord_y;
    GLfloat ze = getEnemyPosition().coord_z;
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

        GLfloat terrain_height = game_state->getGlobalSettings()
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
    GLfloat balistic_scalar = game_state->getBalisticScalar();
    GLfloat speed = tank_attribute_power * power_bar * balistic_scalar;
    GLfloat g = game_state->getGravity();  // Note: gravity is negative

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
    GLfloat xe = getEnemyPosition().coord_x;
    GLfloat ye = getEnemyPosition().coord_y;
    GLfloat ze = getEnemyPosition().coord_z;

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

int Player::getAIState() { return state_of_ai; }

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
    Vertex xyzri(
            balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
    Vertex xyzrf(scalar * balistic_matrix[0],
                 scalar * balistic_matrix[1],
                 scalar * balistic_matrix[2]);
    glVertex3f(xyzri.coord_x, xyzri.coord_y, xyzri.coord_z);
    glVertex3f(xyzri.coord_x + xyzrf.coord_x,
               xyzri.coord_y + xyzrf.coord_y,
               xyzri.coord_z + xyzrf.coord_z);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(Green);
    Vertex xyzui(
            balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
    Vertex xyzuf(scalar * balistic_matrix[4],
                 scalar * balistic_matrix[5],
                 scalar * balistic_matrix[6]);
    glVertex3f(xyzui.coord_x, xyzui.coord_y, xyzui.coord_z);
    glVertex3f(xyzui.coord_x + xyzuf.coord_x,
               xyzui.coord_y + xyzuf.coord_y,
               xyzui.coord_z + xyzuf.coord_z);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(Blue);
    Vertex xyzai(
            balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
    Vertex xyzaf(scalar * balistic_matrix[8],
                 scalar * balistic_matrix[9],
                 scalar * balistic_matrix[10]);
    glVertex3f(xyzai.coord_x, xyzai.coord_y, xyzai.coord_z);
    glVertex3f(xyzai.coord_x + xyzaf.coord_x,
               xyzai.coord_y + xyzaf.coord_y,
               xyzai.coord_z + xyzaf.coord_z);
    glEnd();
    /*	END BALISTIC AXES	*/

    if (target) {
        /*	START ENEMY VECTOR AXES	*/
        glBegin(GL_LINES);
        glColor3f(LightSteelBlue);
        Vertex mypos(
                balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
        Vertex enemypos(
                enemy_position.coord_x, mypos.coord_y, enemy_position.coord_z);
        glVertex3f(mypos.coord_x, mypos.coord_y, mypos.coord_z);
        glVertex3f(enemypos.coord_x, enemypos.coord_y, enemypos.coord_z);
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
        Vertex o3(
                balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
        Vertex path_end(o3.coord_x - 100000 * balistic_matrix[8],
                        o3.coord_y,
                        o3.coord_z - 100000 * balistic_matrix[10]);
        glVertex3f(o3.coord_x, o3.coord_y, o3.coord_z);
        glVertex3f(path_end.coord_x, path_end.coord_y, path_end.coord_z);
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

bool Player::getDrawDebugLinesandPlanes() {
    return draw_debug_linesand_planes;
}
void Player::setDrawDebugLinesandPlanes(bool flag) {
    draw_debug_linesand_planes = flag;
}