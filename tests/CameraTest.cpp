// Exercises the cameras libvulkan_graphix shares between the tutorials
// (OrbitCamera) and vulkan_earth (WorldCamera, ChaseCamera), and the shake
// and view matrix their Camera base gives all three. Pure math, no Vulkan
// device or X11 window, so this runs unconditionally (no DISPLAY check).

#include <array>
#include <cmath>
#include <cstdint>

#include <gtest/gtest.h>
#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/ChaseCamera.h"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/WorldCamera.h"

namespace {

namespace vg = vulkan_graphix;
namespace math = vulkan_graphix::Math;

constexpr float c_epsilon = 1e-3f;

void expectNear(
    const math::Vec3<float>& actual, const math::Vec3<float>& expected) {
  EXPECT_NEAR(actual.x, expected.x, c_epsilon);
  EXPECT_NEAR(actual.y, expected.y, c_epsilon);
  EXPECT_NEAR(actual.z, expected.z, c_epsilon);
}

void expectNear(
    const math::Mat4<float>& actual, const math::Mat4<float>& expected) {
  for (std::int32_t col = 0; col < 4; ++col) {
    for (std::int32_t row = 0; row < 4; ++row) {
      EXPECT_NEAR(actual[col][row], expected[col][row], c_epsilon);
    }
  }
}

// vulkan_earth's original WorldCam/ChaseCam::updateShakeCam(), verbatim,
// as the reference Camera::updateShake() must reproduce.
void originalUpdateShake(std::array<std::int32_t, 3>& shake) {
  if (shake[0] != 0) {
    shake[0] = shake[0] / 1.015281239159713;
    if (shake[0] % 3 == 0) {
      shake[0] *= -1;
    }
  }
  if (shake[1] != 0) {
    shake[1] = shake[1] / 1.015281239159713;
    if (shake[1] % 3 == 1) {
      shake[1] *= -1;
    }
  }
  if (shake[2] != 0) {
    shake[2] = shake[2] / 1.015281239159713;
    if (shake[2] % 3 == 2) {
      shake[2] *= -1;
    }
  }
}

}  // namespace

TEST(CameraTest, OrbitCameraDefaultsToFrontOnView) {
  const vg::OrbitCamera camera;
  expectNear(camera.target(), math::Vec3<float>(0.0f));
  expectNear(camera.eye(),
      9.0f * math::Vec3<float>(0.0f, std::sin(0.45f), std::cos(0.45f)));
  expectNear(camera.up(), math::Vec3<float>(0.0f, 1.0f, 0.0f));
  expectNear(
      camera.view(), glm::lookAt(camera.eye(), camera.target(), camera.up()));
}

TEST(CameraTest, OrbitCameraDragRotatesAndScrollZoomsWithinLimits) {
  vg::OrbitCamera camera(0.0f, 0.0f, 2.0f);
  camera.onMouseMove(50, 0);  // not dragging yet - ignored
  expectNear(camera.eye(), math::Vec3<float>(0.0f, 0.0f, 2.0f));

  camera.onMouseButton(1, true, 0, 0);
  camera.onMouseMove(100, 0);  // yaw += 1 radian
  camera.onMouseButton(1, false, 100, 0);
  expectNear(camera.eye(),
      math::Vec3<float>(2.0f * std::sin(1.0f), 0.0f, 2.0f * std::cos(1.0f)));

  for (std::int32_t i = 0; i < 10; ++i) {
    camera.onMouseButton(4, true, 0, 0);  // zoom in, clamped at 1.5
  }
  EXPECT_NEAR(glm::length(camera.eye()), 1.5f, c_epsilon);
}

TEST(CameraTest, ShakeMatchesTheGamesOriginalDecay) {
  vg::OrbitCamera camera;
  const math::Mat4<float> steady = camera.view();

  camera.setShake(37);
  std::array<std::int32_t, 3> expected{37, 37, 37};
  expectNear(camera.shakeOffset(), math::Vec3<float>(37.0f));
  expectNear(camera.view(),
      glm::lookAt(camera.eye() + math::Vec3<float>(37.0f),
          camera.target() + math::Vec3<float>(37.0f),
          camera.up()));

  for (std::int32_t frame = 0; frame < 400; ++frame) {
    camera.updateShake();
    originalUpdateShake(expected);
    ASSERT_EQ(camera.shakeOffset(),
        math::Vec3<float>(static_cast<float>(expected[0]),
            static_cast<float>(expected[1]),
            static_cast<float>(expected[2])))
        << "frame " << frame;
  }
  // It always decays to rest.
  expectNear(camera.shakeOffset(), math::Vec3<float>(0.0f));
  expectNear(camera.view(), steady);

  camera.setShake(20);
  camera.setShake(0);
  expectNear(camera.shakeOffset(), math::Vec3<float>(0.0f));
}

TEST(CameraTest, WorldCameraLooksDownTiltedTowardPlusX) {
  vg::WorldCamera camera(10.0f, 20000.0f, 300.0f);
  const float tilt = glm::radians(10.0f);
  expectNear(camera.eye(), math::Vec3<float>(10.0f, 20000.0f, 300.0f));
  expectNear(camera.target() - camera.eye(),
      math::Vec3<float>(std::sin(tilt), -std::cos(tilt), 0.0f));
  expectNear(
      camera.up(), math::Vec3<float>(std::cos(tilt), std::sin(tilt), 0.0f));

  camera.move(20.0f, -40.0f, 5.0f);
  expectNear(camera.eye(), math::Vec3<float>(30.0f, 19960.0f, 305.0f));
  expectNear(camera.target() - camera.eye(),
      math::Vec3<float>(std::sin(tilt), -std::cos(tilt), 0.0f));
}

TEST(CameraTest, ChaseCameraFollowsItsTargetsLiveState) {
  float position[3] = {100.0f, 50.0f, -20.0f};
  float direction[3] = {3.0f, 0.0f, 4.0f};  // length 5
  vg::ChaseCamera camera(position, direction);

  expectNear(camera.eye(), math::Vec3<float>(100.0f - 300.0f, 51.0f, -420.0f));
  // The target's y follows the direction's x - the original's quirk.
  expectNear(camera.target(), math::Vec3<float>(220.0f, 170.0f, 140.0f));

  camera.riseOverhead();
  expectNear(camera.eye(), math::Vec3<float>(100.0f, 4050.0f, -420.0f));

  camera.resetHeight();
  expectNear(camera.eye(), math::Vec3<float>(-200.0f, 150.0f, -420.0f));

  // It reads the followed object's arrays every call.
  position[0] = 0.0f;
  expectNear(camera.eye(), math::Vec3<float>(-300.0f, 150.0f, -420.0f));
}
