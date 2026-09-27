#ifndef VULKAN_GRAPHIX_TUTORIALBASE_H
#define VULKAN_GRAPHIX_TUTORIALBASE_H

#include <atomic>
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/LoggedClass.hpp"
#include "vulkan_graphix/OperatingSystem.h"

namespace vulkan_graphix {

// ************************************************************ //
// QueueParameters                                              //
//                                                              //
// Vulkan Queue's parameters container class                    //
// ************************************************************ //
class QueueParameters {
public:
    QueueParameters();

    const VkQueue& getVkQueue() const;
    VkQueue& getVkQueue();
    void setVkQueue(const VkQueue& vk_queue);

    std::uint32_t getFamilyIndex() const;
    void setFamilyIndex(const std::uint32_t family_index);

private:
    VkQueue m_vk_queue;
    std::uint32_t m_family_index;
};

// ************************************************************ //
// ImageParameters                                              //
//                                                              //
// Vulkan Image's parameters container class                    //
// ************************************************************ //
class ImageParameters {
public:
    ImageParameters();

    const VkImage& getVkImage() const;
    void setVkImage(const VkImage& other);

    const VkImageView& getVkImageView() const;
    VkImageView& getVkImageView();
    void setVkImageView(const VkImageView& other);

    const VkSampler& getVkSampler() const;
    void setVkSampler(const VkSampler& other);

    const VkDeviceMemory& getVkDeviceMemory() const;
    void setVkDeviceMemory(const VkDeviceMemory& other);

private:
    VkImage m_vk_image;
    VkImageView m_vk_image_view;
    VkSampler m_vk_sampler;
    VkDeviceMemory m_vk_device_memory;
};

// ************************************************************ //
// BufferParameters                                             //
//                                                              //
// Vulkan Buffer's parameters container class                   //
// ************************************************************ //
class BufferParameters {
public:
    BufferParameters();

    const VkBuffer& getVkBuffer() const;
    void setVkBuffer(const VkBuffer& other);

    const VkDeviceMemory& getVkDeviceMemory() const;
    void setVkDeviceMemory(const VkDeviceMemory& other);

    std::uint32_t getSize() const;
    void setSize(const std::uint32_t size);

private:
    VkBuffer m_vk_buffer;
    VkDeviceMemory m_vk_device_memory;
    std::uint32_t m_size;
};

// ************************************************************ //
// DescriptorParameters                                         //
//                                                              //
// Container class for descriptor related resources             //
// ************************************************************ //
class DescriptorSetParameters {
public:
    DescriptorSetParameters();

    const VkDescriptorPool& getVkDescriptorPool() const;
    void setVkDescriptorPool(const VkDescriptorPool& other);

    const VkDescriptorSetLayout& getVkDescriptorSetLayout() const;
    void setVkDescriptorSetLayout(const VkDescriptorSetLayout& other);

    const VkDescriptorSet& getVkDescriptorSet() const;
    void setVkDescriptorSet(const VkDescriptorSet& other);

private:
    VkDescriptorPool m_vk_descriptor_pool;
    VkDescriptorSetLayout m_vk_descriptor_set_layout;
    VkDescriptorSet m_vk_descriptor_set;
};

// ************************************************************ //
// RenderingResourceParameters                                  //
//                                                              //
// Container class for the resources used during a single frame //
// ************************************************************ //
class RenderingResourceParameters {
public:
    RenderingResourceParameters();

    const VkFramebuffer& getVkFramebuffer() const;
    VkFramebuffer& getVkFramebuffer();
    void setVkFramebuffer(const VkFramebuffer& vk_framebuffer);

    const VkCommandBuffer& getVkCommandBuffer() const;
    VkCommandBuffer& getVkCommandBuffer();
    void setVkCommandBuffer(const VkCommandBuffer& vk_command_buffer);

    // Note: no "finished rendering" semaphore here on purpose. That
    // semaphore is signaled by a submit and waited on by a swapchain
    // present, so it must be indexed by the acquired swapchain image, not
    // by frame-in-flight/rendering-resource slot; each tutorial that needs
    // one keeps its own std::vector<VkSemaphore> sized to the swapchain's
    // image count instead. See the comment in e.g. Tutorial04::
    // createSemaphores() for the full rationale.
    const VkSemaphore& getImageAvailableVkSemaphore() const;
    VkSemaphore& getImageAvailableVkSemaphore();
    void setImageAvailableVkSemaphore(const VkSemaphore& vk_semaphore);

    const VkFence& getVkFence() const;
    VkFence& getVkFence();
    void setVkFence(const VkFence& vk_fence);

private:
    VkFramebuffer m_vk_framebuffer;
    VkCommandBuffer m_vk_command_buffer;
    VkSemaphore m_image_available_vk_semaphore;
    VkFence m_vk_fence;
};

// ************************************************************ //
// SwapChainParameters                                          //
//                                                              //
// Vulkan SwapChain's parameters container class                //
// ************************************************************ //
class SwapChainParameters {
public:
    SwapChainParameters();

    const VkSwapchainKHR& getVkSwapchainKhr() const;
    VkSwapchainKHR& getVkSwapchainKhr();
    void setVkSwapchainKhr(const VkSwapchainKHR& other);

    const VkFormat& getVkFormat() const;
    void setVkFormat(VkFormat& other);

    const std::vector<ImageParameters>& getImageParameters() const;
    std::vector<ImageParameters>& getImageParameters();
    void setImageParameters(const std::vector<ImageParameters>& other);

    const VkExtent2D& getVkExtent2d() const;
    void setVkExtent2d(VkExtent2D& other);

private:
    VkSwapchainKHR m_vk_swapchain_khr;
    VkFormat m_vk_format;
    std::vector<ImageParameters> m_image_parameters;
    VkExtent2D m_vk_extent_2d;
};

// ************************************************************ //
// TutorialBaseParameters                                       //
//                                                              //
// General Vulkan parameters' container class                   //
// ************************************************************ //
class TutorialBaseParameters {
public:
    TutorialBaseParameters();

    const VkInstance& getVkInstance() const;
    VkInstance& getVkInstance();
    void setVkInstance(const VkInstance& other);

    const VkPhysicalDevice& getVkPhysicalDevice() const;
    VkPhysicalDevice& getVkPhysicalDevice();
    void setVkPhysicalDevice(const VkPhysicalDevice& other);

    const VkDevice& getVkDevice() const;
    VkDevice& getVkDevice();
    void setVkDevice(const VkDevice& other);

    const QueueParameters& getGraphicsQueueParameters() const;
    QueueParameters& getGraphicsQueueParameters();
    void setGraphicsQueueParameters(const QueueParameters& other);

    const QueueParameters& getPresentQueueParameters() const;
    QueueParameters& getPresentQueueParameters();
    void setPresentQueueParameters(const QueueParameters& other);

    const VkSurfaceKHR& getVkSurfaceKhr() const;
    VkSurfaceKHR& getVkSurfaceKhr();
    void setVkSurfaceKhr(const VkSurfaceKHR& other);

    const SwapChainParameters& getSwapchainParameters() const;
    SwapChainParameters& getSwapchainParameters();
    void setSwapchainParameters(const SwapChainParameters& other);

    const VkDebugUtilsMessengerEXT& getVkDebugUtilsMessenger() const;
    VkDebugUtilsMessengerEXT& getVkDebugUtilsMessenger();
    void setVkDebugUtilsMessenger(
            const VkDebugUtilsMessengerEXT& vk_debug_utils_messenger);

private:
    VkInstance m_vk_instance;
    VkPhysicalDevice m_vk_physical_device;
    VkDevice m_vk_device;
    QueueParameters m_graphics_queue_parameters;
    QueueParameters m_present_queue_parameters;
    VkSurfaceKHR m_vk_surface_khr;
    SwapChainParameters m_swapchain_parameters;
    VkDebugUtilsMessengerEXT m_vk_debug_utils_messenger;
};

// ************************************************************ //
// TutorialBase                                                 //
//                                                              //
// Base class for Vulkan more advanced tutorial classes         //
// ************************************************************ //
class TutorialBase : public os::ProjectBase, public LoggedClass<TutorialBase> {
public:
    TutorialBase();
    ~TutorialBase() override;

    bool prepareVulkan(os::WindowParameters parameters);
    bool onWindowSizeChanged() override;

    const VkPhysicalDevice& getVkPhysicalDevice() const;
    VkPhysicalDevice& getVkPhysicalDevice();
    const VkDevice& getVkDevice() const;
    VkDevice& getVkDevice();

    const QueueParameters& getGraphicsQueueParameters() const;
    const QueueParameters& getPresentQueueParameters() const;

    const SwapChainParameters& getSwapchainParameters() const;

protected:
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
    bool createSwapChain();
    bool createSwapChainImageViews();

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

    bool checkValidationLayerSupport() const;
    bool setupDebugMessenger();
    bool destroyDebugMessenger();

    virtual bool childOnWindowSizeChanged() = 0;
    virtual void childClear() = 0;

    os::LibraryHandle m_vulkan_library_handle;
    os::WindowParameters m_window_parameters;
    TutorialBaseParameters m_vulkan_common_parameters;
    std::atomic<bool> m_enable_vk_debug;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIALBASE_H
