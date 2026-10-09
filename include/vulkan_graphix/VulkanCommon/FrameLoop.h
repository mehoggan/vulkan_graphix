#ifndef VULKAN_GRAPHIX_VULKANCOMMON_FRAMELOOP_H
#define VULKAN_GRAPHIX_VULKANCOMMON_FRAMELOOP_H

// The acquire -> record -> submit -> present loop over a TutorialBase's
// swapchain, with a few frames in flight: per frame a command buffer, an
// image-available semaphore, and a fence; per swapchain image a
// framebuffer and a rendering-finished semaphore. begin() hands back a
// command buffer already inside the render pass, end() submits and
// presents it.
//
// The swapchain images go through explicit barriers (UNDEFINED ->
// COLOR_ATTACHMENT_OPTIMAL before the pass, -> PRESENT_SRC_KHR after it,
// with a queue-family transfer when the graphics and present queues
// differ), so the render pass is one made with RenderPassDescription's
// default color layouts. Create it again after the swapchain changes
// (TutorialBase::onWindowSizeChanged()).

#include <cstdint>
#include <functional>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Tutorial/TutorialBase.h"

namespace vulkan_graphix::VulkanCommon {

class FrameLoop {
public:
  enum class FrameResult { ok, out_of_date, failed };

  struct Frame {
    VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
    std::uint32_t m_image_index = 0;
  };

  FrameLoop() = default;
  ~FrameLoop();

  FrameLoop(const FrameLoop&) = delete;
  FrameLoop& operator=(const FrameLoop&) = delete;

  // depth_view, when not null, is every framebuffer's second attachment.
  bool create(
      const TutorialBase& base,
      VkRenderPass render_pass,
      VkImageView depth_view = VK_NULL_HANDLE,
      std::uint32_t frames_in_flight = 3);
  void destroy();

  // Waits for this frame's previous use to finish, acquires a swapchain
  // image, and begins recording: command buffer, barrier, render pass
  // (cleared to clear_values, one per attachment), and a viewport and
  // scissor covering the whole image.
  FrameResult begin(const std::vector<VkClearValue>& clear_values, Frame* out);
  // Ends the render pass and the command buffer, submits, and presents.
  FrameResult end(const Frame& frame);

  // begin(), record(command_buffer), end() - a swapchain gone out of date
  // is rebuilt through base.onWindowSizeChanged() (which destroys and
  // recreates this loop). False when anything fails.
  bool draw(
      TutorialBase& base,
      const std::vector<VkClearValue>& clear_values,
      const std::function<void(VkCommandBuffer)>& record);

  VkExtent2D extent() const;

private:
  struct InFlight {
    VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
    VkSemaphore m_image_available = VK_NULL_HANDLE;
    VkFence m_fence = VK_NULL_HANDLE;
  };

  void recordSwapchainBarrier(
      VkCommandBuffer command_buffer, VkImage image, bool to_draw) const;

  VkDevice m_device = VK_NULL_HANDLE;
  VkQueue m_graphics_queue = VK_NULL_HANDLE;
  VkQueue m_present_queue = VK_NULL_HANDLE;
  std::uint32_t m_graphics_family = 0;
  std::uint32_t m_present_family = 0;
  VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
  VkExtent2D m_extent = {0, 0};
  std::vector<VkImage> m_images;
  VkRenderPass m_render_pass = VK_NULL_HANDLE;
  VkCommandPool m_command_pool = VK_NULL_HANDLE;
  std::vector<InFlight> m_in_flight;
  std::vector<VkFramebuffer> m_framebuffers;
  std::vector<VkSemaphore> m_rendering_finished;
  std::size_t m_next = 0;
};

}  // namespace vulkan_graphix::VulkanCommon

#endif  // VULKAN_GRAPHIX_VULKANCOMMON_FRAMELOOP_H
