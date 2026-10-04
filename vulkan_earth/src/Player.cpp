#include "vulkan_earth/Player.h"
#include <array>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/GameState.h"
#include "vulkan_earth/GlobalSettings.h"
#include "vulkan_earth/OpenGLColors.h"
#include "vulkan_earth/PlayerFactory.h"
#include "vulkan_earth/Projectile.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_graphix/TankOrientation.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);

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
vulkan_graphix::Math::Vec3<float> Player::getEnemyPosition() {
    return enemy_position;
}
float* Player::getBalisticMatrix() { return balistic_matrix; }

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
        distance_off_from_target = sqrt(
                pow((previous_projectile_landing_spot.x - enemy_position.x),
                    2) +
                pow((previous_projectile_landing_spot.y - enemy_position.y),
                    2) +
                pow((previous_projectile_landing_spot.z - enemy_position.z),
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
        yaw_angle = vulkan_graphix::TankOrientation::angleBetweenDegrees(
                enemy_path, projectile_path);
        rangle = vulkan_graphix::TankOrientation::angleBetweenDegrees(
                enemy_path, ortho_left);
        langle = vulkan_graphix::TankOrientation::angleBetweenDegrees(
                enemy_path, ortho_right);
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
        pitch_angle = vulkan_graphix::TankOrientation::angleBetweenDegrees(
                enemy_path, pitch_vector);
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
            yaw_angle = vulkan_graphix::TankOrientation::angleBetweenDegrees(
                    enemy_path, projectile_path);
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
        /*const float* velMatrix = getCurrentTank()->getTurretMatrix();
        float tankAttributePower = first_acquired_power;
        float powerBar = getCurrentTank()->getCurrentPower();
        float scalar = game_state->getBalisticScalar();
        float power = tankAttributePower*powerBar*scalar;
        float mag = sqrt	(
                                pow((power*velMatrix[8]),2) +
                                pow((power*velMatrix[9]),2)	+
                                pow((power*velMatrix[10]),2)
                            );
        float ang = first_acquired_pitch;*/
        degrees_rotated = 0;
        game_state->currentPlayerFire();
        prev_state_of_ai = SHOT_LAST_ROUND;
        state_of_ai = WALKING_IN;
    }
    if (state_of_ai == TARGET_NOT_REACHABLE) {
        state_of_ai = NEED_NEW_TARGET;
    }
}

char Player::minimumYawAngle(float right_degrees, float left_degrees) {
    if (right_degrees < left_degrees) {
        return 'r';
    } else if (right_degrees >= left_degrees) {
        return 'l';
    } else {
        return 'e';
    }
}

void Player::updateBalsticMatrix() {
    for (std::int32_t x = 0; x < 16; x++) {
        balistic_matrix[x] = getCurrentTank()->getTurretMatrix()[x];
    }
    balistic_matrix[1] = 0;
    balistic_matrix[4] = 0;
    balistic_matrix[6] = 0;
    balistic_matrix[9] = 0;
}

void Player::setEnemyPosition() {
    updateBalsticMatrix();
    enemy_position.x = target->getBodyMatrix()[12];
    enemy_position.y = target->getBodyMatrix()[13];
    enemy_position.z = target->getBodyMatrix()[14];
}

void Player::setUpYawVectors() {
    /* THIS IS KEY FOR ALL CALCULATIONS TO LINE UP TANK WITH THE PROJECTILE
     * PATH */
    updateBalsticMatrix();
    const float* matrix = getBalisticMatrix();
    vulkan_graphix::Math::Vec3<float> v = getEnemyPosition();

    // Forumlate Perpendicular vulkan_graphix::Math::Vec3<float>
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
    vulkan_graphix::Math::Vec3<float> v1(matrix[8], 0, matrix[10]);
    vulkan_graphix::Math::Vec3<float> w1(v1.x, v1.y, v1.z);
    // Test for linear independence
    vulkan_graphix::Math::Vec3<float> v2(0, 0, 0);
    if (v1.y == 0 && v1.z == 0) {
        v2.x = 0;
        v2.y = 0;
        v2.z = 1;
    } else {
        v2.x = -v1.x;
        v2.y = v1.y;
        v2.z = v1.z;
    }
    float magnitude_w1 = sqrt((w1.x * w1.x) + (w1.y * w1.y) + (w1.z * w1.z));
    float scalar = (w1.x * v2.x + w1.y * v2.y + w1.z * v2.z) /
                   (pow(static_cast<double>(magnitude_w1), 2.0));
    w1.x *= scalar;
    w1.y *= scalar;
    w1.z *= scalar;
    vulkan_graphix::Math::Vec3<float> w2(
            (v2.x - w1.x), (v2.y - w1.y), (v2.z - w1.z));
    if ((w1.x <= 0 && w1.z <= 0) || (w1.x >= 0 && w1.z >= 0)) {
        w2.x *= -1;
        w2.z *= -1;
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
    vulkan_graphix::Math::Vec3<float> my_position(
            matrix[12], matrix[13], matrix[14]);
    vulkan_graphix::Math::Vec3<float> enemy_vertex(v.x, v.y, v.z);
    vulkan_graphix::Math::Vec3<float> projectile_endmark(
            (matrix[12] - 10000 * matrix[8]),
            (matrix[13]),
            (matrix[14] - 10000 * matrix[10]));
    vulkan_graphix::Math::Vec3<float> left_mark(
            (matrix[12] + 1000 * matrix[0]),
            (matrix[13]),
            (matrix[14] + 1000 * matrix[2]));
    vulkan_graphix::Math::Vec3<float> right_mark(
            (matrix[12] - 1000 * matrix[0]),
            (matrix[13]),
            (matrix[14] - 1000 * matrix[2]));
    vulkan_graphix::Math::Vec3<float> left_ortho((matrix[12] + 1000 * w2.x),
                                                 (matrix[13]),
                                                 (matrix[14] + 1000 * w2.z));
    vulkan_graphix::Math::Vec3<float> right_ortho((matrix[12] - 1000 * w2.x),
                                                  (matrix[13]),
                                                  (matrix[14] - 1000 * w2.z));

    // Produce Member Vectors
    ortho_left.x = (left_ortho.x - my_position.x);
    ortho_left.y = (left_ortho.y - my_position.y);
    ortho_left.z = (left_ortho.z - my_position.z);

    ortho_right.x = (right_ortho.x - my_position.x);
    ortho_right.y = (right_ortho.y - my_position.y);
    ortho_right.z = (right_ortho.z - my_position.z);

    enemy_path.x = (enemy_vertex.x - my_position.x);
    enemy_path.y = (enemy_vertex.y - my_position.y);
    enemy_path.z = (enemy_vertex.z - my_position.z);

    projectile_path.x = (projectile_endmark.x - my_position.x);
    projectile_path.y = (projectile_endmark.y - my_position.y);
    projectile_path.z = (projectile_endmark.z - my_position.z);

    left_vector.x = (left_mark.x - my_position.x);
    left_vector.y = (left_mark.y - my_position.y);
    left_vector.z = (left_mark.z - my_position.z);

    right_vector.x = (right_mark.x - my_position.x);
    right_vector.y = (right_mark.y - my_position.y);
    right_vector.z = (right_mark.z - my_position.z);
}

void Player::setUpPitchVectors() {
    const float* matrix = getBalisticMatrix();
    float* turret_matrix = getCurrentTank()->getTurretMatrix();
    pitch_vector.x = ((turret_matrix[12] - 10000 * turret_matrix[8]) -
                      turret_matrix[12]);
    pitch_vector.y = ((turret_matrix[13] - 10000 * turret_matrix[9]) -
                      turret_matrix[13]);
    pitch_vector.z = ((turret_matrix[14] - 10000 * turret_matrix[10]) -
                      turret_matrix[14]);
    up_vector.x = ((matrix[12] + 1000 * matrix[4]) - matrix[12]);
    up_vector.y = ((matrix[13] + 1000 * matrix[5]) - matrix[13]);
    up_vector.z = ((matrix[14] + 1000 * matrix[6]) - matrix[14]);
    down_vector.x = ((matrix[12] - 1000 * matrix[4]) - matrix[12]);
    down_vector.y = ((matrix[13] - 1000 * matrix[5]) - matrix[13]);
    down_vector.z = ((matrix[14] - 1000 * matrix[6]) - matrix[14]);
}

bool Player::calculateProjectilePhysics(float xerr,
                                        float yerr,
                                        float /*zerr*/) {
    /*	VARIABLES NEEDED BY GAMESTATE.CPP	*/
    float percent_errory = yerr;
    float percent_errorxz = xerr;
    float numerator = game_state->getGlobalSettings()
                              ->getCurrentTerrain()
                              ->getActualSize();
    float denominator =
            game_state->getGlobalSettings()->getCurrentTerrain()->getScale();
    float terrain_size = numerator / denominator;
    float g = game_state->getGravity();  // Note: gravity is negative
    float tank_attribute_power = getCurrentTank()->getPower();
    float power_bar = getCurrentTank()->getCurrentPower();
    float balistic_scalar = game_state->getBalisticScalar();
    float speed = tank_attribute_power * power_bar * balistic_scalar;

    /****************************************************************************************/
    /*	Physics Calculations First (Formulas)
     */
    /*	Yf = Yo + Voy*time - 1/2*(gravity)*t^2
     */
    /*	Xf = Xo + Vox*time */
    /*	Zf = Zo + Voz*time */
    /****************************************************************************************/
    // Same launch a real shot gets (Projectile's constructor) - this used to
    // start the simulated shell 200 units out along the barrel while a real
    // one starts at Projectile::c_muzzle_distance (500), despite the old
    // "MAKE SURE TO UPDATE 200" note asking for the two to match.
    vulkan_graphix::Ballistics::Launch const launch =
            vulkan_graphix::Ballistics::launchFromBarrel(
                    glm::make_mat4(getCurrentTank()->getTurretMatrix()),
                    speed,
                    Projectile::c_muzzle_distance);
    float xf = launch.origin.x;
    float yf = launch.origin.y;
    float zf = launch.origin.z;
    float xe = getEnemyPosition().x;
    float ye = getEnemyPosition().y;
    float ze = getEnemyPosition().z;
    float t = 0;
    bool on_target = false;
    while (yf > 0) {
        vulkan_graphix::Math::Vec3<float> const position =
                vulkan_graphix::Ballistics::positionAt(launch, g, t);
        xf = position.x;
        yf = position.y;
        zf = position.z;

        if (abs(xf - xe) <= percent_errorxz &&
            abs(zf - ze) <= percent_errorxz &&
            abs(yf - ye) <= percent_errory) {
            on_target = true;
            break;
        }

        float terrain_height = game_state->getGlobalSettings()
                                       ->getCurrentTerrain()
                                       ->getHeightAt(xf, zf);
        if (terrain_height >= yf) {
            break;
        }

        t = t + .02;
    }
    return on_target;
}

void Player::displayProjectilePhysiscs() {
    /*	VARIABLES NEEDED BY GAMESTATE.CPP	*/
    float tank_attribute_power = getCurrentTank()->getPower();
    float power_bar = getCurrentTank()->getCurrentPower();
    float balistic_scalar = game_state->getBalisticScalar();
    float speed = tank_attribute_power * power_bar * balistic_scalar;
    float g = game_state->getGravity();  // Note: gravity is negative

    // Same launch a real shot gets (Projectile's constructor) - this used to
    // start the simulated shell 200 units out along the barrel while a real
    // one starts at Projectile::c_muzzle_distance (500), despite the old
    // "MAKE SURE TO UPDATE 200" note asking for the two to match.
    vulkan_graphix::Ballistics::Launch const launch =
            vulkan_graphix::Ballistics::launchFromBarrel(
                    glm::make_mat4(getCurrentTank()->getTurretMatrix()),
                    speed,
                    Projectile::c_muzzle_distance);
    float xf = launch.origin.x;
    float yf = launch.origin.y;
    float zf = launch.origin.z;
    float xe = getEnemyPosition().x;
    float ye = getEnemyPosition().y;
    float ze = getEnemyPosition().z;

    float t = 0;
    while (yf > 0) {
        vulkan_graphix::Math::Vec3<float> const position =
                vulkan_graphix::Ballistics::positionAt(launch, g, t);
        xf = position.x;
        yf = position.y;
        zf = position.z;
        t = t + .02;
    }

    cout << "Variables:" << endl;
    cout << " Vox = " << launch.velocity.x << " Voy = " << launch.velocity.y
         << " Voz = " << launch.velocity.z << endl
         << endl
         << " Xo = " << launch.origin.x << " Yo = " << launch.origin.y
         << " Zo = " << launch.origin.z << endl
         << endl
         << " Xf = " << xf << " Yf = " << yf << " Zf = " << zf << endl
         << endl
         << " Xe = " << xe << " Ye = " << ye << " Ze = " << ze << endl
         << endl
         << " g = " << g << endl
         << endl;
}

std::int32_t Player::getAIState() { return state_of_ai; }

void Player::restoreTurretTo0Degrees() {
    float restore_angle = getCurrentTank()->getTurretDegrees();
    getCurrentTank()->rotateTurret(-1 * restore_angle);
}

void Player::yawLeft(float degrees) { getCurrentTank()->rotateHead(-degrees); }

void Player::yawRight(float degrees) { getCurrentTank()->rotateHead(degrees); }

void Player::pitchUp(float degrees) {
    getCurrentTank()->rotateTurret(degrees);
}

void Player::pitchDown(float degrees) {
    getCurrentTank()->rotateTurret(-degrees);
}

void Player::drawTestLinesandPlanes(render::RenderContext& context) {
    float scalar = 1000;
    std::vector<render::UiVertex> lines;
    std::vector<render::UiVertex> quads;
    auto line = [&](math::Vec4<float> const& color,
                    vulkan_graphix::Math::Vec3<float> const& from,
                    vulkan_graphix::Math::Vec3<float> const& end) {
        lines.push_back({from, color, math::Vec2<float>(0.0f)});
        lines.push_back({end, color, math::Vec2<float>(0.0f)});
    };
    auto quad = [&](math::Vec4<float> const& color,
                    std::array<math::Vec3<float>, 4> const& corners) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U}) {
            quads.push_back({corners[corner], color, math::Vec2<float>(0.0f)});
        }
    };
    /*	START BALISTIC AXES	*/
    vulkan_graphix::Math::Vec3<float> xyzri(
            balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
    vulkan_graphix::Math::Vec3<float> xyzrf(scalar * balistic_matrix[0],
                                            scalar * balistic_matrix[1],
                                            scalar * balistic_matrix[2]);
    line(math::Vec4<float>(Red, 1.0f),
         xyzri,
         vulkan_graphix::Math::Vec3<float>(
                 xyzri.x + xyzrf.x, xyzri.y + xyzrf.y, xyzri.z + xyzrf.z));
    vulkan_graphix::Math::Vec3<float> xyzui(
            balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
    vulkan_graphix::Math::Vec3<float> xyzuf(scalar * balistic_matrix[4],
                                            scalar * balistic_matrix[5],
                                            scalar * balistic_matrix[6]);
    line(math::Vec4<float>(Green, 1.0f),
         xyzui,
         vulkan_graphix::Math::Vec3<float>(
                 xyzui.x + xyzuf.x, xyzui.y + xyzuf.y, xyzui.z + xyzuf.z));
    vulkan_graphix::Math::Vec3<float> xyzai(
            balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
    vulkan_graphix::Math::Vec3<float> xyzaf(scalar * balistic_matrix[8],
                                            scalar * balistic_matrix[9],
                                            scalar * balistic_matrix[10]);
    line(math::Vec4<float>(Blue, 1.0f),
         xyzai,
         vulkan_graphix::Math::Vec3<float>(
                 xyzai.x + xyzaf.x, xyzai.y + xyzaf.y, xyzai.z + xyzaf.z));
    /*	END BALISTIC AXES	*/

    if (target) {
        /*	START ENEMY VECTOR AXES	*/
        vulkan_graphix::Math::Vec3<float> mypos(
                balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
        vulkan_graphix::Math::Vec3<float> enemypos(
                enemy_position.x, mypos.y, enemy_position.z);
        line(math::Vec4<float>(LightSteelBlue, 1.0f), mypos, enemypos);
        /*	END ENEMY VECTOR AXES	*/
        ///*	START PERP VECTORS	*/
        // glBegin(GL_LINES);
        //	glColor3f(MediumSeaGreen);
        //	vulkan_graphix::Math::Vec3<float>
        // o1(balisticMatrix[12],balisticMatrix[13],balisticMatrix[14]);
        //	vulkan_graphix::Math::Vec3<float> f1(
        // o1.coordX+scalar*ortho_left.compoX,
        //				o1.coordY+scalar*ortho_left.compoY,
        //				o1.coordZ+scalar*ortho_left.compoZ);
        //	glVertex3f(o1.coordX,o1.coordY,o1.coordZ);
        //	glVertex3f(f1.coordX,f1.coordY,f1.coordZ);
        // glEnd();
        // glBegin(GL_LINES);
        //	glColor3f(PaleGreen);
        //	vulkan_graphix::Math::Vec3<float>
        // o2(balisticMatrix[12],balisticMatrix[13],balisticMatrix[14]);
        //	vulkan_graphix::Math::Vec3<float> f2(
        // o2.coordX+scalar*ortho_right.compoX,
        //				o2.coordY+scalar*ortho_right.compoY,
        //				o2.coordZ+scalar*ortho_right.compoZ);
        //	glVertex3f(o2.coordX,o2.coordY,o2.coordZ);
        //	glVertex3f(f2.coordX,f2.coordY,f2.coordZ);
        // glEnd();
        ///*	END PERP VECTORS	*/
        /*	START PROJECTILE PATH	*/
        vulkan_graphix::Math::Vec3<float> o3(
                balistic_matrix[12], balistic_matrix[13], balistic_matrix[14]);
        vulkan_graphix::Math::Vec3<float> path_end(
                o3.x - 100000 * balistic_matrix[8],
                o3.y,
                o3.z - 100000 * balistic_matrix[10]);
        line(math::Vec4<float>(MediumGoldenrod, 1.0f), o3, path_end);
        /*	END	PROJECTILE PATH		*/

        const float* matrix = getBalisticMatrix();
        quad(math::Vec4<float>(Pink, 0.75f),
             {math::Vec3<float>(matrix[12] - 1000 * matrix[0],
                                matrix[13],
                                matrix[14] - 1000 * matrix[2]),
              math::Vec3<float>(
                      matrix[12] - 1000 * matrix[0] - 10000 * matrix[8],
                      matrix[13],
                      matrix[14] - 1000 * matrix[2] - 10000 * matrix[10]),
              math::Vec3<float>(
                      matrix[12] + 1000 * matrix[0] - 10000 * matrix[8],
                      matrix[13],
                      matrix[14] + 1000 * matrix[2] - 10000 * matrix[10]),
              math::Vec3<float>(matrix[12] + 1000 * matrix[0],
                                matrix[13],
                                matrix[14] + 1000 * matrix[2])});

        const float* t_matrix = getCurrentTank()->getTurretMatrix();
        quad(math::Vec4<float>(Red, 0.75f),
             {math::Vec3<float>(t_matrix[12] - 1000 * t_matrix[0],
                                t_matrix[13] - 1000 * t_matrix[1],
                                t_matrix[14] - 1000 * t_matrix[2]),
              math::Vec3<float>(
                      t_matrix[12] - 1000 * t_matrix[0] - 10000 * t_matrix[8],
                      t_matrix[13] - 1000 * t_matrix[1] - 10000 * t_matrix[9],
                      t_matrix[14] - 1000 * t_matrix[2] -
                              10000 * t_matrix[10]),
              math::Vec3<float>(
                      t_matrix[12] + 1000 * t_matrix[0] - 10000 * t_matrix[8],
                      t_matrix[13] + 1000 * t_matrix[1] - 10000 * t_matrix[9],
                      t_matrix[14] + 1000 * t_matrix[2] -
                              10000 * t_matrix[10]),
              math::Vec3<float>(t_matrix[12] + 1000 * t_matrix[0],
                                t_matrix[13] + 1000 * t_matrix[1],
                                t_matrix[14] + 1000 * t_matrix[2])});
    }
    context.drawTransient(lines, vulkan_earth::pipelines().ui_lines, nullptr);
    context.drawTransient(
            quads, vulkan_earth::pipelines().ui_triangles, nullptr);
}

bool Player::getDrawDebugLinesandPlanes() {
    return draw_debug_linesand_planes;
}
void Player::setDrawDebugLinesandPlanes(bool flag) {
    draw_debug_linesand_planes = flag;
}