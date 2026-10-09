#ifndef VULKAN_EARTH_SUBMENULANDSCAPE_H
#define VULKAN_EARTH_SUBMENULANDSCAPE_H

#include <cstdint>
#include <string>
#include "vulkan_earth/SubMenu.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;
class ControlItemSliderbar;
class TerrainMaker;

const std::int32_t num_control_items_lnd = 4;
const std::int32_t preview_button = 3;

class SubMenuLandscape : public SubMenu {
public:
  SubMenuLandscape();
  SubMenuLandscape(
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
      float new_percent_border);
  ~SubMenuLandscape() override;
  std::int32_t getUNIQUEIDENTIFIER() override;
  void setUNIQUEIDENTIFIER(std::int32_t id) override;
  float getXPos() override;
  void setXPos(float new_xpos) override;
  float getYPos() override;
  void setYPos(float new_ypos) override;
  float getZPos() override;
  void setZPos(float new_zpos) override;
  float getRed() override;
  void setRed(float red) override;
  float getGreen() override;
  void setGreen(float green) override;
  float getBlue() override;
  void setBlue(float blue) override;
  std::int32_t getWidth() override;
  void setWdith(std::int32_t new_width) override;
  std::int32_t getHeight() override;
  void setHeight(std::int32_t new_height) override;
  std::string getCaption() override;
  void setCaption(const std::string& new_caption) override;
  float getPerecentBorder() override;
  void setPercentBorder(float percent) override;
  void draw(vulkan_graphix::Render::RenderContext& context) override;
  std::string collectData() override;
  void subMenuMouseTest(
      std::int32_t x, std::int32_t y, std::int32_t button_down) override;
  void updateMouse(std::int32_t x, std::int32_t y) override;
  TerrainMaker* m_tm;  // PUBLIC BECAUSE I AM TOO LAZY TO UPDATE ENTIRE
                       // INTERFACE FOR ONE GET FUNCTION
  ControlItem*
      m_sub_menu_button[num_control_items_lnd];  // BOTH THESE ITEMS NEED
                                                 // GETTERS AND SETTERS
                                                 // WHICH MEANS UPDATE TO
                                                 // INTERFACE
private:
  std::int32_t m_uniqueidentifier;
  std::int32_t m_old_mouse_x, m_old_mouse_y;
  float m_cam_x, m_cam_y, m_cam_z;
  float m_x_pos;
  float m_y_pos;
  float m_z_pos;
  float m_color[4];
  std::int32_t m_width;
  std::int32_t m_height;
  std::string m_caption;
  float m_percent_border;
  TextObject* m_label;
  ControlItem* m_button_pressed;
  std::int32_t m_numberpressed;
  vulkan_graphix::Render::UiMesh m_frame_mesh;
  vulkan_graphix::Render::UiMesh m_border_mesh;
};

#endif  // VULKAN_EARTH_SUBMENULANDSCAPE_H