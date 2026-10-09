// Drives VulkanCommon's renderer building blocks - ResourceContext (buffers,
// textures, depth image, render pass, pipeline, descriptors), the
// descriptor writes, and FrameLoop - through a live Vulkan device and X11
// window: a minimal TutorialBase application built only from them draws a
// textured, depth-tested quad, survives a swapchain rebuild, and tears
// down. Uses Tutorial07's compiled shaders ({vec4 position, vec2 texcoord}
// vertices; binding 0 = sampler, binding 1 = a mat4 uniform). Skipped
// without a DISPLAY, like the tutorials' own integration tests.

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_graphix/VulkanCommon/Descriptors.h"
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/Pipeline.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"
#include "vulkan_graphix/VulkanFunctions.h"

#include "IntegrationTestCommon.h"

namespace {

namespace vg = vulkan_graphix;
namespace vc = vulkan_graphix::VulkanCommon;

struct QuadVertex {
  vg::Math::Vec4<float> m_position;
  vg::Math::Vec2<float> m_texcoord;
};

class BuildingBlocksApp : public vg::TutorialBase {
public:
  ~BuildingBlocksApp() override { childClear(); }

  bool createResources() {
    if (!m_resources.initialize(*this)) {
      return false;
    }
    // A 2x2 checkerboard texture, an identity transform.
    const std::vector<char> pixels = {
        -1, 0, 0, -1, 0, -1, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1};
    if (!m_resources.createTexture(
            2, 2, pixels, VK_SAMPLER_ADDRESS_MODE_REPEAT, m_texture) ||
        !m_resources.createUniformBuffer(
            sizeof(vg::Math::Mat4<float>), m_uniform_buffer) ||
        !m_resources.writeBuffer(
            m_uniform_buffer, vg::Math::Mat4<float>(1.0f))) {
      return false;
    }

    const std::vector<vc::DescriptorBinding> bindings = {
        {0,
         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
         VK_SHADER_STAGE_FRAGMENT_BIT},
        {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}};
    VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
    VkDescriptorPool pool = VK_NULL_HANDLE;
    if (!m_resources.createDescriptorSetLayout(bindings, &set_layout) ||
        !m_resources.createDescriptorPool(bindings, 1, &pool) ||
        !vc::allocateDescriptorSet(
            getVkDevice(), pool, set_layout, &m_descriptor_set)) {
      return false;
    }
    vc::writeImageDescriptor(getVkDevice(), m_descriptor_set, 0, m_texture);
    vc::writeUniformBufferDescriptor(
        getVkDevice(), m_descriptor_set, 1, m_uniform_buffer);

    VkRenderPass render_pass = VK_NULL_HANDLE;
    if (!m_resources.createDepthImage(
            getSwapchainParameters().getVkExtent2d(),
            VK_FORMAT_D32_SFLOAT,
            m_depth_image) ||
        !m_resources.createRenderPass(
            {.m_color_format = getSwapchainParameters().getVkFormat(),
             .m_depth_format = VK_FORMAT_D32_SFLOAT},
            &render_pass) ||
        !m_resources.createPipelineLayout(
            {set_layout}, {}, &m_pipeline_layout) ||
        !m_resources.createGraphicsPipeline(
            {.m_vertex_shader = "shader.07.vert.spv",
             .m_fragment_shader = "shader.07.frag.spv",
             .m_vertex_stride = sizeof(QuadVertex),
             .m_vertex_attributes =
                 {{0,
                   0,
                   VK_FORMAT_R32G32B32A32_SFLOAT,
                   offsetof(QuadVertex, m_position)},
                  {1,
                   0,
                   VK_FORMAT_R32G32_SFLOAT,
                   offsetof(QuadVertex, m_texcoord)}},
             .m_depth = vc::DepthState{},
             .m_blend = vc::alphaBlend(),
             .m_layout = m_pipeline_layout,
             .m_render_pass = render_pass},
            &m_pipeline)) {
      return false;
    }

    const std::vector<QuadVertex> vertices = {
        {vg::Math::Vec4<float>(-0.5f, -0.5f, 0.5f, 1.0f),
         vg::Math::Vec2<float>(0.0f, 0.0f)},
        {vg::Math::Vec4<float>(0.5f, -0.5f, 0.5f, 1.0f),
         vg::Math::Vec2<float>(1.0f, 0.0f)},
        {vg::Math::Vec4<float>(0.5f, 0.5f, 0.5f, 1.0f),
         vg::Math::Vec2<float>(1.0f, 1.0f)},
        {vg::Math::Vec4<float>(-0.5f, 0.5f, 0.5f, 1.0f),
         vg::Math::Vec2<float>(0.0f, 1.0f)}};
    const std::vector<std::uint32_t> indices = {0, 1, 2, 0, 2, 3};
    return m_resources.createDeviceLocalBuffer(
               vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_vertex_buffer) &&
        m_resources.createDeviceLocalBuffer(
            indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_index_buffer) &&
        m_frames.create(*this, render_pass, m_depth_image.getVkImageView());
  }

  bool draw() override {
    std::vector<VkClearValue> clear_values(2);
    clear_values[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
    clear_values[1].depthStencil = {1.0f, 0};
    return m_frames.draw(
        *this, clear_values, [this](VkCommandBuffer command_buffer) {
          vg::vkCmdBindPipeline(
              command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
          const VkDeviceSize offset = 0;
          vg::vkCmdBindVertexBuffers(
              command_buffer, 0, 1, &m_vertex_buffer.getVkBuffer(), &offset);
          vg::vkCmdBindIndexBuffer(
              command_buffer,
              m_index_buffer.getVkBuffer(),
              0,
              VK_INDEX_TYPE_UINT32);
          vg::vkCmdBindDescriptorSets(
              command_buffer,
              VK_PIPELINE_BIND_POINT_GRAPHICS,
              m_pipeline_layout,
              0,
              1,
              &m_descriptor_set,
              0,
              nullptr);
          vg::vkCmdDrawIndexed(command_buffer, 6, 1, 0, 0, 0);
        });
  }

private:
  bool childOnWindowSizeChanged() override { return createResources(); }

  void childClear() override {
    m_frames.destroy();
    m_resources.releaseAll();
  }

  vc::ResourceContext m_resources;
  vc::FrameLoop m_frames;
  vg::ImageParameters m_texture;
  vg::ImageParameters m_depth_image;
  vg::BufferParameters m_uniform_buffer;
  vg::BufferParameters m_vertex_buffer;
  vg::BufferParameters m_index_buffer;
  VkDescriptorSet m_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
};

}  // namespace

TEST(VulkanCommonIntegrationTest, BuildsDrawsRebuildsAndReleases) {
  if (!vg::test::hasDisplay()) {
    GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                    "and X11 window)";
  }

  vg::os::Window window;
  ASSERT_TRUE(window.create("integration-vulkan-common"));
  auto application = std::make_shared<BuildingBlocksApp>();
  ASSERT_TRUE(application->prepareVulkan(window.getParameters()));
  ASSERT_TRUE(application->createResources());

  for (std::int32_t i = 0; i < vg::test::c_draw_iterations; ++i) {
    EXPECT_TRUE(application->draw());
  }
  // Releases everything, rebuilds the swapchain, and creates it all again.
  EXPECT_TRUE(application->onWindowSizeChanged());
  for (std::int32_t i = 0; i < vg::test::c_draw_iterations; ++i) {
    EXPECT_TRUE(application->draw());
  }
}

TEST(VulkanCommonIntegrationTest, MissingShaderFailsPipelineCreation) {
  if (!vg::test::hasDisplay()) {
    GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                    "and X11 window)";
  }

  vg::os::Window window;
  ASSERT_TRUE(window.create("integration-vulkan-common-missing"));
  auto application = std::make_shared<BuildingBlocksApp>();
  ASSERT_TRUE(application->prepareVulkan(window.getParameters()));

  vc::ResourceContext resources;
  ASSERT_TRUE(resources.initialize(*application));
  VkRenderPass render_pass = VK_NULL_HANDLE;
  ASSERT_TRUE(resources.createRenderPass(
      {.m_color_format = application->getSwapchainParameters().getVkFormat()},
      &render_pass));
  VkPipelineLayout layout = VK_NULL_HANDLE;
  ASSERT_TRUE(resources.createPipelineLayout({}, {}, &layout));
  VkPipeline pipeline = VK_NULL_HANDLE;
  EXPECT_FALSE(resources.createGraphicsPipeline(
      {.m_vertex_shader = "no-such-shader.vert.spv",
       .m_fragment_shader = "no-such-shader.frag.spv",
       .m_layout = layout,
       .m_render_pass = render_pass},
      &pipeline));
  EXPECT_EQ(static_cast<VkPipeline>(VK_NULL_HANDLE), pipeline);
  resources.releaseAll();
}
