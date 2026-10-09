#include "vulkan_graphix/VulkanCommon/FrameLoop.h"

#include <array>
#include <cstdint>

#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix::VulkanCommon {

FrameLoop::~FrameLoop() { destroy(); }

bool FrameLoop::create(
    const TutorialBase& base,
    VkRenderPass render_pass,
    VkImageView depth_view,
    std::uint32_t frames_in_flight) {
  destroy();
  m_device = base.getVkDevice();
  m_graphics_queue = base.getGraphicsQueueParameters().getVkQueue();
  m_graphics_family = base.getGraphicsQueueParameters().getFamilyIndex();
  m_present_queue = base.getPresentQueueParameters().getVkQueue();
  m_present_family = base.getPresentQueueParameters().getFamilyIndex();
  const SwapChainParameters& swapchain = base.getSwapchainParameters();
  m_swapchain = swapchain.getVkSwapchainKhr();
  m_extent = swapchain.getVkExtent2d();
  m_render_pass = render_pass;

  const VkCommandPoolCreateInfo pool_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = m_graphics_family};
  if (vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) !=
      VK_SUCCESS) {
    return false;
  }
  const FrameResourceFactory factory(m_device);
  m_in_flight.resize(frames_in_flight);
  for (InFlight& frame : m_in_flight) {
    if (!factory.allocateCommandBuffers(
            m_command_pool, 1, &frame.m_command_buffer) ||
        !factory.createSemaphore(&frame.m_image_available) ||
        !factory.createFence(true, &frame.m_fence)) {
      return false;
    }
  }

  for (const ImageParameters& image : swapchain.getImageParameters()) {
    m_images.push_back(image.getVkImage());
    std::vector<VkImageView> attachments = {image.getVkImageView()};
    if (depth_view != VK_NULL_HANDLE) {
      attachments.push_back(depth_view);
    }
    const VkFramebufferCreateInfo framebuffer_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = m_render_pass,
        .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = m_extent.width,
        .height = m_extent.height,
        .layers = 1};
    m_framebuffers.push_back(VK_NULL_HANDLE);
    m_rendering_finished.push_back(VK_NULL_HANDLE);
    if (vkCreateFramebuffer(
            m_device, &framebuffer_info, nullptr, &m_framebuffers.back()) !=
            VK_SUCCESS ||
        !factory.createSemaphore(&m_rendering_finished.back())) {
      return false;
    }
  }
  return true;
}

void FrameLoop::destroy() {
  if (m_device == VK_NULL_HANDLE) {
    return;
  }
  vkDeviceWaitIdle(m_device);
  for (VkFramebuffer framebuffer : m_framebuffers) {
    if (framebuffer != VK_NULL_HANDLE) {
      vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }
  }
  for (VkSemaphore semaphore : m_rendering_finished) {
    if (semaphore != VK_NULL_HANDLE) {
      vkDestroySemaphore(m_device, semaphore, nullptr);
    }
  }
  for (const InFlight& frame : m_in_flight) {
    if (frame.m_image_available != VK_NULL_HANDLE) {
      vkDestroySemaphore(m_device, frame.m_image_available, nullptr);
    }
    if (frame.m_fence != VK_NULL_HANDLE) {
      vkDestroyFence(m_device, frame.m_fence, nullptr);
    }
  }
  // Destroying the pool frees its command buffers.
  if (m_command_pool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
  }
  m_framebuffers.clear();
  m_rendering_finished.clear();
  m_in_flight.clear();
  m_images.clear();
  m_command_pool = VK_NULL_HANDLE;
  m_device = VK_NULL_HANDLE;
  m_next = 0;
}

void FrameLoop::recordSwapchainBarrier(
    VkCommandBuffer command_buffer, VkImage image, bool to_draw) const {
  const bool queues_differ = m_present_queue != m_graphics_queue;
  const std::uint32_t present_family =
      queues_differ ? m_present_family : VK_QUEUE_FAMILY_IGNORED;
  const std::uint32_t graphics_family =
      queues_differ ? m_graphics_family : VK_QUEUE_FAMILY_IGNORED;
  const VkImageMemoryBarrier barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = nullptr,
      .srcAccessMask = to_draw
          ? VkAccessFlags{VK_ACCESS_MEMORY_READ_BIT}
          : VkAccessFlags{VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT},
      .dstAccessMask = to_draw
          ? VkAccessFlags{VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT}
          : VkAccessFlags{VK_ACCESS_MEMORY_READ_BIT},
      .oldLayout = to_draw ? VK_IMAGE_LAYOUT_UNDEFINED
                           : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = to_draw ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                           : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      .srcQueueFamilyIndex = to_draw ? present_family : graphics_family,
      .dstQueueFamilyIndex = to_draw ? graphics_family : present_family,
      .image = image,
      .subresourceRange = {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1}};
  vkCmdPipelineBarrier(
      command_buffer,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      to_draw ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
              : VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
      0,
      0,
      nullptr,
      0,
      nullptr,
      1,
      &barrier);
}

FrameLoop::FrameResult FrameLoop::begin(
    const std::vector<VkClearValue>& clear_values, Frame* out) {
  const InFlight& frame = m_in_flight[m_next];
  if (vkWaitForFences(m_device, 1, &frame.m_fence, VK_TRUE, UINT64_MAX) !=
      VK_SUCCESS) {
    return FrameResult::failed;
  }
  std::uint32_t image_index = 0;
  const VkResult acquired = ::vkAcquireNextImageKHR(
      m_device,
      m_swapchain,
      UINT64_MAX,
      frame.m_image_available,
      VK_NULL_HANDLE,
      &image_index);
  if (acquired == VK_ERROR_OUT_OF_DATE_KHR) {
    return FrameResult::out_of_date;
  }
  if (acquired != VK_SUCCESS && acquired != VK_SUBOPTIMAL_KHR) {
    return FrameResult::failed;
  }
  vkResetFences(m_device, 1, &frame.m_fence);

  const VkCommandBufferBeginInfo begin_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr};
  if (vkBeginCommandBuffer(frame.m_command_buffer, &begin_info) !=
      VK_SUCCESS) {
    return FrameResult::failed;
  }
  recordSwapchainBarrier(frame.m_command_buffer, m_images[image_index], true);

  const VkRenderPassBeginInfo pass_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = m_render_pass,
      .framebuffer = m_framebuffers[image_index],
      .renderArea = {.offset = {.x = 0, .y = 0}, .extent = m_extent},
      .clearValueCount = static_cast<std::uint32_t>(clear_values.size()),
      .pClearValues = clear_values.data()};
  vkCmdBeginRenderPass(
      frame.m_command_buffer, &pass_info, VK_SUBPASS_CONTENTS_INLINE);

  const VkViewport viewport = {
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(m_extent.width),
      .height = static_cast<float>(m_extent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f};
  const VkRect2D scissor = {.offset = {.x = 0, .y = 0}, .extent = m_extent};
  vkCmdSetViewport(frame.m_command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(frame.m_command_buffer, 0, 1, &scissor);

  out->m_command_buffer = frame.m_command_buffer;
  out->m_image_index = image_index;
  return FrameResult::ok;
}

FrameLoop::FrameResult FrameLoop::end(const Frame& frame) {
  InFlight& in_flight = m_in_flight[m_next];
  m_next = (m_next + 1) % m_in_flight.size();

  vkCmdEndRenderPass(frame.m_command_buffer);
  recordSwapchainBarrier(
      frame.m_command_buffer, m_images[frame.m_image_index], false);
  if (vkEndCommandBuffer(frame.m_command_buffer) != VK_SUCCESS) {
    return FrameResult::failed;
  }

  const VkSemaphore rendering_finished =
      m_rendering_finished[frame.m_image_index];
  const VkPipelineStageFlags wait_stage =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  const VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &in_flight.m_image_available,
      .pWaitDstStageMask = &wait_stage,
      .commandBufferCount = 1,
      .pCommandBuffers = &frame.m_command_buffer,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &rendering_finished};
  if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, in_flight.m_fence) !=
      VK_SUCCESS) {
    return FrameResult::failed;
  }

  const VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &rendering_finished,
      .swapchainCount = 1,
      .pSwapchains = &m_swapchain,
      .pImageIndices = &frame.m_image_index,
      .pResults = nullptr};
  const VkResult presented =
      ::vkQueuePresentKHR(m_present_queue, &present_info);
  if (presented == VK_ERROR_OUT_OF_DATE_KHR ||
      presented == VK_SUBOPTIMAL_KHR) {
    return FrameResult::out_of_date;
  }
  return presented == VK_SUCCESS ? FrameResult::ok : FrameResult::failed;
}

bool FrameLoop::draw(
    TutorialBase& base,
    const std::vector<VkClearValue>& clear_values,
    const std::function<void(VkCommandBuffer)>& record) {
  Frame frame;
  FrameResult result = begin(clear_values, &frame);
  if (result == FrameResult::ok) {
    record(frame.m_command_buffer);
    result = end(frame);
  }
  switch (result) {
    case FrameResult::ok:
      return true;
    case FrameResult::out_of_date:
      // Destroys and recreates this loop - nothing of it is touched after.
      return base.onWindowSizeChanged();
    case FrameResult::failed:
      break;
  }
  return false;
}

VkExtent2D FrameLoop::extent() const { return m_extent; }

}  // namespace vulkan_graphix::VulkanCommon
