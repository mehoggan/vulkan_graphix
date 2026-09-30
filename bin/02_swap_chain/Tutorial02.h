#ifndef VULKAN_GRAPHIX_TUTORIAL02_H
#define VULKAN_GRAPHIX_TUTORIAL02_H

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/LoggedClass.hpp"
#include "vulkan_graphix/OperatingSystem.h"

namespace vulkan_graphix {

// ************************************************************ //
// VulkanTutorial02Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
class VulkanTutorial02Parameters {
public:
    VulkanTutorial02Parameters();

    const VkInstance& getVkInstance() const;
    VkInstance& getVkInstance();
    void setVkInstance(const VkInstance& vk_instance);

    const VkPhysicalDevice& getVkPhysicalDevice() const;
    void setVkPhysicalDevice(const VkPhysicalDevice& vk_physical_device);

    const VkDevice& getVkDevice() const;
    VkDevice& getVkDevice();
    void setVkDevice(const VkDevice& vk_device);

    const VkQueue& getGraphicsVkQueue() const;
    VkQueue& getGraphicsVkQueue();
    void setGraphicsVkQueue(const VkQueue& graphics_vk_queue);

    const VkQueue& getPresentVkQueue() const;
    VkQueue& getPresentVkQueue();
    void setPresentVkQueue(const VkQueue& present_vk_queue);

    std::uint32_t getGraphicsQueueFamilyIndex() const;
    void setGraphicsQueueFamilyIndex(
            const std::uint32_t graphics_queue_family_index);

    std::uint32_t getPresentQueueFamilyIndex() const;
    void setPresentQueueFamilyIndex(
            const std::uint32_t graphics_queue_family_index);

    const VkSurfaceKHR& getPresentVkSurfaceKHR() const;
    VkSurfaceKHR& getPresentVkSurfaceKHR();
    void setPresentVkSurfaceKHR(
            const VkSurfaceKHR& presentation_vk_surface_khr);

    const VkSwapchainKHR& getVkSwapchainKHR() const;
    VkSwapchainKHR& getVkSwapchainKHR();
    void setVkSwapchainKHR(const VkSwapchainKHR& vk_swapchain_khr);

    const std::vector<VkCommandBuffer>& getPresentQueueVkCommandBuffers()
            const;
    std::vector<VkCommandBuffer>& getPresentQueueVkCommandBuffers();
    void setPresentQueueVkCommandBuffers(
            const std::vector<VkCommandBuffer>&
                    present_queue_vk_command_buffers);

    const VkCommandPool& getPresentQueueVkCommandPool() const;
    VkCommandPool& getPresentQueueVkCommandPool();
    void setPresentQueueVkCommandPool(
            const VkCommandPool& present_queue_vk_command_pool);

    const VkSemaphore& getImageAvailableVkSemaphore() const;
    VkSemaphore& getImageAvailableVkSemaphore();
    void setImageAvailableVkSemaphore(
            const VkSemaphore& image_available_vk_semaphore);

    // One per swapchain image, indexed by acquired image index rather than
    // a single semaphore reused every frame. See the comment in
    // Tutorial02::createCommandBuffers() for why a single semaphore isn't
    // safe here.
    const std::vector<VkSemaphore>& getRenderingFinishedSemaphores() const;
    std::vector<VkSemaphore>& getRenderingFinishedSemaphores();
    void setRenderingFinishedSemaphores(
            const std::vector<VkSemaphore>& rendering_finished_semaphores);

    const VkFence& getVkFence() const;
    VkFence& getVkFence();
    void setVkFence(const VkFence& vk_fence);

    const VkDebugUtilsMessengerEXT& getVkDebugUtilsMessenger() const;
    VkDebugUtilsMessengerEXT& getVkDebugUtilsMessenger();
    void setVkDebugUtilsMessenger(
            const VkDebugUtilsMessengerEXT& vk_debug_utils_messenger);

private:
    VkInstance m_vk_instance;
    VkPhysicalDevice m_vk_physical_device;
    VkDevice m_vk_device;
    VkQueue m_graphics_vk_queue;
    VkQueue m_present_vk_queue;
    std::uint32_t m_graphics_queue_family_index;
    std::uint32_t m_present_queue_family_index;
    VkSurfaceKHR m_presentation_vk_surface_khr;
    VkSwapchainKHR m_vk_swapchain_khr;
    std::vector<VkCommandBuffer> m_present_queue_vk_command_buffers;
    VkCommandPool m_present_queue_vk_command_pool;
    VkSemaphore m_image_available_vk_semaphore;
    std::vector<VkSemaphore> m_rendering_finished_semaphores;
    VkFence m_vk_fence;
    VkDebugUtilsMessengerEXT m_vk_debug_utils_messenger;
};

// ************************************************************ //
// Tutorial02                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial02 : public os::ProjectBase, public LoggedClass<Tutorial02> {
public:
    Tutorial02();
    ~Tutorial02() override;

    bool prepareVulkan(os::WindowParameters parameters);
    bool createSwapChain();
    bool onWindowSizeChanged() override;
    bool createCommandBuffers();
    bool draw() override;

private:
    bool loadVulkanLibrary();
    bool loadExportedEntryPoints();
    bool loadGlobalLevelEntryPoints();
    bool createInstance();
    bool loadInstanceLevelEntryPoints();
    bool createPresentationSurface();
    bool createDevice();
    bool checkPhysicalDeviceProperties(
            VkPhysicalDevice physical_device,
            std::uint32_t& graphics_queue_family_index,
            std::uint32_t& present_queue_family_index);
    bool loadDeviceLevelEntryPoints();
    bool getDeviceQueue();
    bool createSemaphores();
    bool recordCommandBuffers();
    void clear();

    bool checkValidationLayerSupport() const;
    bool setupDebugMessenger();
    bool destroyDebugMessenger();

    bool checkExtensionAvailability(
            const char* extension_name,
            const std::vector<VkExtensionProperties>& available_extensions);
    std::uint32_t getSwapChainNumImages(
            VkSurfaceCapabilitiesKHR& surface_capabilities);
    VkSurfaceFormatKHR getSwapChainFormat(
            std::vector<VkSurfaceFormatKHR>& surface_formats);
    VkExtent2D getSwapChainExtent(
            VkSurfaceCapabilitiesKHR& surface_capabilities);
    VkImageUsageFlags getSwapChainUsageFlags(
            VkSurfaceCapabilitiesKHR& surface_capabilities);
    VkSurfaceTransformFlagBitsKHR getSwapChainTransform(
            VkSurfaceCapabilitiesKHR& surface_capabilities);
    VkPresentModeKHR getSwapChainPresentMode(
            std::vector<VkPresentModeKHR>& present_modes);

    friend std::ostream& operator<<(
            std::ostream& out, const std::vector<VkLayerProperties>& vect);

    os::LibraryHandle m_vulkan_library;
    os::WindowParameters m_window_parameters;
    VulkanTutorial02Parameters m_vulkan_tutorial02_parameters;
    std::atomic<bool> m_enable_vulkan_debug;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL02_H
