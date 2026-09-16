#include "vulkan_graphix/OrbitCamera.h"

#include <algorithm>
#include <cmath>

namespace vulkan_graphix {

OrbitCamera::OrbitCamera() : OrbitCamera(0.0f, 0.45f, 9.0f) {}

OrbitCamera::OrbitCamera(float initial_yaw_radians,
                         float initial_pitch_radians,
                         float initial_distance)
        : m_yaw(initial_yaw_radians)
        , m_pitch(initial_pitch_radians)
        , m_distance(initial_distance)
        , m_dragging(false)
        , m_last_x(0)
        , m_last_y(0)
        , m_target(0.0f, 0.0f, 0.0f) {}

void OrbitCamera::onMouseButton(int button,
                                bool pressed,
                                int pos_x,
                                int pos_y) {
    constexpr int c_left_button = 1;
    constexpr int c_scroll_up = 4;
    constexpr int c_scroll_down = 5;
    constexpr float c_zoom_step = 0.5f;
    constexpr float c_min_distance = 1.5f;
    constexpr float c_max_distance = 40.0f;

    if (button == c_left_button) {
        m_dragging = pressed;
        m_last_x = pos_x;
        m_last_y = pos_y;
    } else if (pressed && button == c_scroll_up) {
        m_distance = std::max(c_min_distance, m_distance - c_zoom_step);
    } else if (pressed && button == c_scroll_down) {
        m_distance = std::min(c_max_distance, m_distance + c_zoom_step);
    }
}

void OrbitCamera::onMouseMove(int pos_x, int pos_y) {
    if (!m_dragging) {
        return;
    }

    constexpr float c_sensitivity = 0.01f;
    // ~89 degrees in radians - stops just short of the poles to avoid
    // gimbal lock, while still comfortably allowing the camera below the
    // y=0 terrain plane to see its underside.
    constexpr float c_max_pitch = 1.553343f;

    int const delta_x = pos_x - m_last_x;
    int const delta_y = pos_y - m_last_y;
    m_last_x = pos_x;
    m_last_y = pos_y;

    m_yaw += static_cast<float>(delta_x) * c_sensitivity;
    m_pitch += static_cast<float>(delta_y) * c_sensitivity;
    m_pitch = std::clamp(m_pitch, -c_max_pitch, c_max_pitch);
}

Math::Vec3<float> OrbitCamera::eye() const {
    return m_target +
           m_distance * Math::Vec3<float>(std::cos(m_pitch) * std::sin(m_yaw),
                                          std::sin(m_pitch),
                                          std::cos(m_pitch) * std::cos(m_yaw));
}

Math::Vec3<float> const& OrbitCamera::target() const { return m_target; }

}  // namespace vulkan_graphix
