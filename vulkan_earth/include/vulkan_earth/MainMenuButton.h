#ifndef VULKAN_EARTH_MAINMENUBUTTON_H
#define VULKAN_EARTH_MAINMENUBUTTON_H

#include <cstdint>
#include <string>
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;
class SubMenu;

namespace vulkan_graphix::Render {
class RenderContext;
}

class MainMenuButton {
public:
  MainMenuButton();
  MainMenuButton(
      std::int32_t id,
      float new_x_pos,
      float new_y_pos,
      float new_z_pos,
      float red,
      float green,
      float blue,
      std::int32_t new_width,
      std::int32_t new_height,
      const std::string& new_caption,
      SubMenu* new_submenu);
  ~MainMenuButton();
  void draw(vulkan_graphix::Render::RenderContext& context);
  void pressButton();
  void activateSubMenu();
  void deactivateSubMenu();
  void depressButton();
  bool isPressed();
  bool isActive();
  void setLabel(const std::string& c);
  std::int32_t getUNIQUEIDENTIFIER();
  float getXPos();
  float getYPos();
  float getHeight();
  float getWidth();
  float* getColor();
  void setColor(float r, float g, float b);
  SubMenu* getSubMenu();
  void printSelf(std::int32_t i);

private:
  std::int32_t m_uniqueidentifier;
  float m_x_pos;
  float m_y_pos;
  float m_z_pos;
  float m_color[4];
  std::int32_t m_width;
  std::int32_t m_height;
  std::string m_caption;
  TextObject* m_label;
  bool m_pressed;
  bool m_active;
  SubMenu* m_submenu;
  vulkan_graphix::Render::UiMesh m_mesh;
  bool m_built_pressed = false;
  vulkan_graphix::Math::Vec4<float> m_built_color =
      vulkan_graphix::Math::Vec4<float>(-1.0f);
};
#endif  // VULKAN_EARTH_MAINMENUBUTTON_H