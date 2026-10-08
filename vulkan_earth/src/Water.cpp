#include "vulkan_earth/Water.h"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "math.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
//
using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

Water::Water() = default;

Water::Water(std::int32_t new_scale, std::int32_t new_size) {
  srand(time(nullptr));
  m_timer = 0.0;
  m_scale = new_scale;
  m_size = new_size;
  m_total_vertices = m_size * m_size;
  m_tri_strip_buffer_size = (m_size - 1) * (m_size - 1) * 6;
  initData();
  prepTerrain();
  // GL_LINEAR filtering, GL_REPEAT wrapping. (The original also loaded
  // bumpMap.raw as a normal map; its water shader never used it.)
  m_color_texture = render::Renderer::instance().loadRawTexture(
      "Water.raw", 1024, 1024, VK_SAMPLER_ADDRESS_MODE_REPEAT);
  prepareData();
}

Water::~Water() {
  if (m_surfaceheight != nullptr) {
    for (std::int32_t i = 0; i < m_size; i++) {
      delete m_surfaceheight[i];
    }
    delete m_surfaceheight;
  }
}

std::int32_t Water::getScale() { return m_scale; }
std::int32_t Water::getActualSize() { return (m_size) * (m_scale); }

void Water::initData() {
  m_vertices.resize(m_tri_strip_buffer_size);
  m_normals.resize(m_tri_strip_buffer_size);
  m_tex_coord.resize(m_tri_strip_buffer_size);
}

void Water::draw(render::RenderContext& context) {
  if (m_mesh) {
    context.drawMesh(*m_mesh,
        vulkan_earth::pipelines().m_water,
        m_color_texture.get(),
        math::Mat4<float>(1.0f),
        math::Vec4<float>(m_timer, 0.0f, 0.0f, 0.0f));
  }
  m_timer += 0.002 * 3.14159265;
  if (m_timer >= 2 * 3.14159265) m_timer = 0.0;
}

void Water::calcAverageofSixNormals(vulkan_graphix::Math::Vec3<float>* v_0,
    float x1,
    float y1,
    float z1,
    float x2,
    float y2,
    float z2,
    float x3,
    float y3,
    float z3,
    float x4,
    float y4,
    float z4,
    float x5,
    float y5,
    float z5,
    float x6,
    float y6,
    float z6,
    vulkan_graphix::Math::Vec3<float>* n) {
  float u_1_x = x1 - v_0->x;
  float u_1_y = y1 - v_0->y;
  float u_1_z = z1 - v_0->z;
  float u_2_x = x2 - v_0->x;
  float u_2_y = y2 - v_0->y;
  float u_2_z = z2 - v_0->z;
  float u_3_x = x3 - v_0->x;
  float u_3_y = y3 - v_0->y;
  float u_3_z = z3 - v_0->z;
  float u_4_x = x4 - v_0->x;
  float u_4_y = y4 - v_0->y;
  float u_4_z = z4 - v_0->z;
  float u_5_x = x5 - v_0->x;
  float u_5_y = y5 - v_0->y;
  float u_5_z = z5 - v_0->z;
  float u_6_x = x6 - v_0->x;
  float u_6_y = y6 - v_0->y;
  float u_6_z = z6 - v_0->z;
  n->x += u_6_y * u_1_z - u_1_y * u_6_z;
  n->y += u_6_z * u_1_x - u_6_x * u_1_z;
  n->z += u_6_x * u_1_y - u_1_x * u_6_y;
  n->x += u_1_y * u_2_z - u_2_y * u_1_z;
  n->y += u_1_z * u_2_x - u_1_x * u_2_z;
  n->z += u_1_x * u_2_y - u_2_x * u_1_y;
  n->x += u_2_y * u_3_z - u_3_y * u_2_z;
  n->y += u_2_z * u_3_x - u_2_x * u_3_z;
  n->z += u_2_x * u_3_y - u_3_x * u_2_y;
  n->x += u_3_y * u_4_z - u_4_y * u_3_z;
  n->y += u_3_z * u_4_x - u_3_x * u_4_z;
  n->z += u_3_x * u_4_y - u_4_x * u_3_y;
  n->x += u_4_y * u_5_z - u_5_y * u_4_z;
  n->y += u_4_z * u_5_x - u_4_x * u_5_z;
  n->z += u_4_x * u_5_y - u_5_x * u_4_y;
  n->x += u_5_y * u_6_z - u_6_y * u_5_z;
  n->y += u_5_z * u_6_x - u_5_x * u_6_z;
  n->z += u_5_x * u_6_y - u_6_x * u_5_y;
  n->x /= 6;
  n->y /= 6;
  n->z /= 6;
  float magnitude = sqrt((n->x * n->x) + (n->y * n->y) + (n->z * n->z));
  n->x /= magnitude;
  n->y /= magnitude;
  n->z /= magnitude;
}

void Water::prepTerrain() {
  m_surfaceheight = new std::int32_t*[m_size];
  for (std::int32_t i = 0; i < m_size; i++) {
    m_surfaceheight[i] = new std::int32_t[m_size];
  }
  for (std::int32_t y = 0; y < m_size; y++) {
    for (std::int32_t x = 0; x < m_size; x++) {
      m_surfaceheight[x][y] = -5 * 100;
    }
  }
}

void Water::prepareData() {
  std::int32_t buffersize = m_tri_strip_buffer_size;
  std::int32_t prep_size = m_size;
  std::int32_t prep_scale = m_scale;

  //
  // 				v_k
  //	v_i			v_z
  //		x-----x
  //		|    /|
  //		|  /  |
  //		|/    |
  //		x-----x
  //	v_j			v_y
  //	v_x
  std::int32_t index = 0;
  std::int32_t index_normals = 0;
  std::int32_t index_texture = 0;
  for (std::int32_t i = 0; i < prep_size - 1; i++) {
    for (std::int32_t j = 0; j < prep_size - 1; j++) {
      /************************************************************/
      /*	V_I -- N_I		                            */
      /************************************************************/
      vulkan_graphix::Math::Vec3<float> v_i(
          j * prep_scale, m_surfaceheight[i][j] /*SCALE*/, i * prep_scale);
      m_vertices[index++] = v_i;
      vulkan_graphix::Math::Vec2<float> t_i(
          i / (static_cast<float>(prep_size) - 1),
          (j) / (static_cast<float>(prep_size) - 1));
      m_tex_coord[index_texture++] = t_i;
      vulkan_graphix::Math::Vec3<float> n_i(0, 0, 0);
      if (i == 0 && j == 0) {
        calcAverageofSixNormals(&v_i,
            v_i.x,
            v_i.y,
            v_i.z,
            v_i.x,
            v_i.y,
            v_i.z,
            v_i.x,
            v_i.y,
            v_i.z,
            v_i.x,
            v_i.y,
            v_i.z,
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            &n_i);
      } else if (i == 0) {
        calcAverageofSixNormals(&v_i,
            v_i.x,
            v_i.y,
            v_i.z,
            v_i.x,
            v_i.y,
            v_i.z,
            static_cast<float>(j - 1),
            m_surfaceheight[i][j - 1],
            static_cast<float>(i),
            static_cast<float>(j - 1),
            m_surfaceheight[i + 1][j - 1],
            static_cast<float>(i + 1),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            &n_i);
      } else if (j == 0) {
        calcAverageofSixNormals(&v_i,
            static_cast<float>(j + 1),
            m_surfaceheight[i - 1][j + 1],
            static_cast<float>(i - 1),
            static_cast<float>(j),
            m_surfaceheight[i - 1][j],
            static_cast<float>(i - 1),
            v_i.x,
            v_i.y,
            v_i.z,
            v_i.x,
            v_i.y,
            v_i.z,
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            &n_i);
      } else {
        calcAverageofSixNormals(&v_i,
            static_cast<float>(j + 1),
            m_surfaceheight[i - 1][j + 1],
            static_cast<float>(i - 1),
            static_cast<float>(j),
            m_surfaceheight[i - 1][j],
            static_cast<float>(i - 1),
            static_cast<float>(j - 1),
            m_surfaceheight[i][j - 1],
            static_cast<float>(i),
            static_cast<float>(j - 1),
            m_surfaceheight[i + 1][j - 1],
            static_cast<float>(i + 1),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            &n_i);
      }
      m_normals[index_normals++] = n_i;
      /************************************************************/
      /*	V_J -- N_J		                 	    */
      /************************************************************/
      vulkan_graphix::Math::Vec3<float> v_j(j * prep_scale,
          m_surfaceheight[i + 1][j] /*SCALE*/,
          (i + 1) * prep_scale);
      m_vertices[index++] = v_j;
      vulkan_graphix::Math::Vec2<float> t_j(
          (i + 1) / (static_cast<float>(prep_size) - 1),
          (j) / (static_cast<float>(prep_size) - 1));
      m_tex_coord[index_texture++] = t_j;
      vulkan_graphix::Math::Vec3<float> n_j(0, 0, 0);
      if (i == prep_size - 2 && j == 0) {
        calcAverageofSixNormals(&v_j,
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            v_j.x,
            v_j.y,
            v_j.z,
            v_j.x,
            v_j.y,
            v_j.z,
            v_j.x,
            v_j.y,
            v_j.z,
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            &n_j);
      } else if (j == 0) {
        calcAverageofSixNormals(&v_j,
            static_cast<float>(j) + 1,
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            v_j.x,
            v_j.y,
            v_j.z,
            v_j.x,
            v_j.y,
            v_j.z,
            static_cast<float>(j),
            m_surfaceheight[i + 2][j],
            static_cast<float>(i + 2),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            &n_j);
      } else if (i == prep_size - 2) {
        calcAverageofSixNormals(&v_j,
            static_cast<float>(j) + 1,
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j - 1),
            m_surfaceheight[i + 1][j - 1],
            static_cast<float>(i + 1),
            v_j.x,
            v_j.y,
            v_j.z,
            v_j.x,
            v_j.y,
            v_j.z,
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            &n_j);
      } else {
        calcAverageofSixNormals(&v_j,
            static_cast<float>(j) + 1,
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j - 1),
            m_surfaceheight[i + 1][j - 1],
            static_cast<float>(i + 1),
            static_cast<float>(j - 1),
            m_surfaceheight[i + 2][j - 1],
            static_cast<float>(i + 2),
            static_cast<float>(j),
            m_surfaceheight[i + 2][j],
            static_cast<float>(i + 2),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            &n_j);
      }
      m_normals[index_normals++] = n_j;
      /************************************************************/
      /*	V_K -- N_K					    */
      /************************************************************/
      vulkan_graphix::Math::Vec3<float> v_k((j + 1) * prep_scale,
          m_surfaceheight[i][j + 1] /*SCALE*/,
          (i)*prep_scale);
      m_vertices[index++] = v_k;
      vulkan_graphix::Math::Vec2<float> t_k(
          i / (static_cast<float>(prep_size) - 1),
          (j + 1) / (static_cast<float>(prep_size) - 1));
      m_tex_coord[index_texture++] = t_k;
      vulkan_graphix::Math::Vec3<float> n_k(0, 0, 0);
      if (i == 0 && j == prep_size - 2) {
        calcAverageofSixNormals(&v_k,
            v_k.x,
            v_k.y,
            v_k.z,
            v_k.x,
            v_k.y,
            v_k.z,
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            v_k.x,
            v_k.y,
            v_k.z,
            &n_k);
      } else if (i == 0) {
        calcAverageofSixNormals(&v_k,
            v_i.x,
            v_i.y,
            v_i.z,
            v_i.x,
            v_i.y,
            v_i.z,
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            static_cast<float>(j + 2),
            m_surfaceheight[i][j + 2],
            static_cast<float>(i),
            &n_k);
      } else if (j == prep_size - 2) {
        calcAverageofSixNormals(&v_k,
            v_i.x,
            v_i.y,
            v_i.z,
            static_cast<float>(j + 1),
            m_surfaceheight[i - 1][j + 1],
            static_cast<float>(i - 1),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            v_i.x,
            v_i.y,
            v_i.z,
            &n_k);
      } else {
        calcAverageofSixNormals(&v_k,
            static_cast<float>(j + 2),
            m_surfaceheight[i - 1][j + 2],
            static_cast<float>(i - 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i - 1][j + 1],
            static_cast<float>(i - 1),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            static_cast<float>(j + 2),
            m_surfaceheight[i][j + 2],
            static_cast<float>(i),
            &n_k);
      }
      m_normals[index_normals++] = n_k;
      /************************************************************/
      /*	V_X -- N_X	(SAME AS V_J/N_J)	            */
      /************************************************************/
      vulkan_graphix::Math::Vec3<float> v_x(j * prep_scale,
          m_surfaceheight[i + 1][j] /*SCALE*/,
          (i + 1) * prep_scale);
      m_vertices[index++] = v_x;
      vulkan_graphix::Math::Vec2<float> t_x(
          (i + 1) / (static_cast<float>(prep_size) - 1),
          (j) / (static_cast<float>(prep_size) - 1));
      m_tex_coord[index_texture++] = t_x;
      vulkan_graphix::Math::Vec3<float> n_x(0, 0, 0);
      if (i == prep_size - 2 && j == 0) {
        calcAverageofSixNormals(&v_x,
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            v_x.x,
            v_x.y,
            v_x.z,
            v_x.x,
            v_x.y,
            v_x.z,
            v_x.x,
            v_x.y,
            v_x.z,
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            &n_x);
      } else if (j == 0) {
        calcAverageofSixNormals(&v_x,
            static_cast<float>(j) + 1,
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            v_x.x,
            v_x.y,
            v_x.z,
            v_x.x,
            v_x.y,
            v_x.z,
            static_cast<float>(j),
            m_surfaceheight[i + 2][j],
            static_cast<float>(i + 2),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            &n_x);
      } else if (i == prep_size - 2) {
        calcAverageofSixNormals(&v_x,
            static_cast<float>(j) + 1,
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j - 1),
            m_surfaceheight[i + 1][j - 1],
            static_cast<float>(i + 1),
            v_x.x,
            v_x.y,
            v_x.z,
            v_x.x,
            v_x.y,
            v_x.z,
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            &n_x);
      } else {
        calcAverageofSixNormals(&v_x,
            static_cast<float>(j) + 1,
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j - 1),
            m_surfaceheight[i + 1][j - 1],
            static_cast<float>(i + 1),
            static_cast<float>(j - 1),
            m_surfaceheight[i + 2][j - 1],
            static_cast<float>(i + 2),
            static_cast<float>(j),
            m_surfaceheight[i + 2][j],
            static_cast<float>(i + 2),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            &n_x);
      }
      m_normals[index_normals++] = n_x;

      /************************************************************/
      /*	V_Y -- N_Y					    */
      /************************************************************/
      vulkan_graphix::Math::Vec3<float> v_y((j + 1) * prep_scale,
          m_surfaceheight[i + 1][j + 1] /*SCALE*/,
          (i + 1) * prep_scale);
      m_vertices[index++] = v_y;
      vulkan_graphix::Math::Vec2<float> t_y(
          (i + 1) / (static_cast<float>(prep_size) - 1),
          (j + 1) / (static_cast<float>(prep_size) - 1));
      m_tex_coord[index_texture++] = t_y;
      vulkan_graphix::Math::Vec3<float> n_y(0, 0, 0);
      if (i == prep_size - 2 && j == prep_size - 2) {
        calcAverageofSixNormals(&v_y,
            v_y.x,
            v_y.y,
            v_y.z,
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            v_y.x,
            v_y.y,
            v_y.z,
            v_y.x,
            v_y.y,
            v_y.z,
            v_y.x,
            v_y.y,
            v_y.z,
            &n_y);
      } else if (i == prep_size - 2) {
        calcAverageofSixNormals(&v_y,
            static_cast<float>(j + 2),
            m_surfaceheight[i][j + 2],
            static_cast<float>(i),
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            v_y.x,
            v_y.y,
            v_y.z,
            v_y.x,
            v_y.y,
            v_y.z,
            static_cast<float>(j + 2),
            m_surfaceheight[i + 1][j + 2],
            static_cast<float>(i + 1),
            &n_y);
      } else if (j == prep_size - 2) {
        calcAverageofSixNormals(&v_y,
            v_y.x,
            v_y.y,
            v_y.z,
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j),
            m_surfaceheight[i + 2][j],
            static_cast<float>(i + 2),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 2][j + 1],
            static_cast<float>(i + 2),
            v_y.x,
            v_y.y,
            v_y.z,
            &n_y);
      } else {
        calcAverageofSixNormals(&v_y,
            static_cast<float>(j + 2),
            m_surfaceheight[i][j + 2],
            static_cast<float>(i),
            static_cast<float>(j + 1),
            m_surfaceheight[i][j + 1],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j),
            m_surfaceheight[i + 2][j],
            static_cast<float>(i + 2),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 2][j + 1],
            static_cast<float>(i + 2),
            static_cast<float>(j + 2),
            m_surfaceheight[i + 1][j + 2],
            static_cast<float>(i + 1),
            &n_y);
      }
      m_normals[index_normals++] = n_y;
      /************************************************************/
      /*	V_Z -- N_Z					    */
      /************************************************************/
      vulkan_graphix::Math::Vec3<float> v_z((j + 1) * prep_scale,
          m_surfaceheight[i][j + 1] /*SCALE*/,
          (i)*prep_scale);
      m_vertices[index++] = v_z;
      vulkan_graphix::Math::Vec2<float> t_z(
          i / (static_cast<float>(prep_size) - 1),
          (j + 1) / (static_cast<float>(prep_size) - 1));
      m_tex_coord[index_texture++] = t_z;
      vulkan_graphix::Math::Vec3<float> n_z(0, 0, 0);
      if (i == 0 && j == prep_size - 2) {
        calcAverageofSixNormals(&v_z,
            v_z.x,
            v_z.y,
            v_z.z,
            v_z.x,
            v_z.y,
            v_z.z,
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            v_z.x,
            v_z.y,
            v_z.z,
            &n_z);
      } else if (i == 0) {
        calcAverageofSixNormals(&v_z,
            v_i.x,
            v_i.y,
            v_i.z,
            v_i.x,
            v_i.y,
            v_i.z,
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            static_cast<float>(j + 2),
            m_surfaceheight[i][j + 2],
            static_cast<float>(i),
            &n_z);
      } else if (j == prep_size - 2) {
        calcAverageofSixNormals(&v_z,
            v_i.x,
            v_i.y,
            v_i.z,
            static_cast<float>(j + 1),
            m_surfaceheight[i - 1][j + 1],
            static_cast<float>(i - 1),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            v_i.x,
            v_i.y,
            v_i.z,
            &n_z);
      } else {
        calcAverageofSixNormals(&v_z,
            static_cast<float>(j + 2),
            m_surfaceheight[i - 1][j + 2],
            static_cast<float>(i - 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i - 1][j + 1],
            static_cast<float>(i - 1),
            static_cast<float>(j),
            m_surfaceheight[i][j],
            static_cast<float>(i),
            static_cast<float>(j),
            m_surfaceheight[i + 1][j],
            static_cast<float>(i + 1),
            static_cast<float>(j + 1),
            m_surfaceheight[i + 1][j + 1],
            static_cast<float>(i + 1),
            static_cast<float>(j + 2),
            m_surfaceheight[i][j + 2],
            static_cast<float>(i),
            &n_z);
      }
      m_normals[index_normals++] = n_z;
    }
  }
  render::MeshVertices mesh_vertices(m_vertices.size());
  for (std::size_t i = 0; i < m_vertices.size(); ++i) {
    mesh_vertices[i] = {m_vertices[i],
        m_normals[i],
        math::Vec2<float>(m_tex_coord[i].s, m_tex_coord[i].t)};
  }
  m_mesh = render::Renderer::instance().createMesh(mesh_vertices);
}
