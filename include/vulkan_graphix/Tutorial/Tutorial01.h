#ifndef VULKAN_GRAPHIX_TUTORIAL01_H
#define VULKAN_GRAPHIX_TUTORIAL01_H

#include <atomic>
#include <cstdint>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/LoggedClass.hpp"
#include "vulkan_graphix/OperatingSystem.h"

namespace vulkan_graphix {

// ************************************************************ //
// VulkanTutorial01Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
class VulkanTutorial01Parameters {
public:
    VulkanTutorial01Parameters();

    const VkInstance& getVkInstance() const;
    VkInstance& getVkInstance();
    void setVkInstance(const VkInstance& vk_instance);

    const VkDevice& getVkDevice() const;
    VkDevice& getVkDevice();
    void setVkDevice(const VkDevice& vk_device);

    std::uint32_t getQueueFamilyIndex() const;
    void setQueueFamilyIndex(const std::uint32_t queue_family_index);

    const VkQueue& getVkQueue() const;
    VkQueue& getVkQueue();
    void setVkQueue(const VkQueue& vk_queue);

    const VkDebugUtilsMessengerEXT& getVkDebugUtilsMessenger() const;
    VkDebugUtilsMessengerEXT& getVkDebugUtilsMessenger();
    void setVkDebugUtilsMessenger(
            const VkDebugUtilsMessengerEXT& vk_debug_utils_messenger);

private:
    VkInstance m_vk_instance;
    VkDevice m_vk_device;
    std::uint32_t m_queue_family_index;
    VkQueue m_vk_queue;
    VkDebugUtilsMessengerEXT m_vk_debug_utils_messenger;
};

// ************************************************************ //
// Tutorial01                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial01 : public os::ProjectBase, public LoggedClass<Tutorial01> {
public:
    explicit Tutorial01(bool enable_debug = false);
    ~Tutorial01() override;

    bool onWindowSizeChanged() override;
    bool draw() override;

    bool prepareVulkan();

private:
    bool loadVulkanLibrary();
    bool loadExportedEntryPoints();
    bool loadGlobalLevelEntryPoints();
    bool createInstance();
    bool loadInstanceLevelEntryPoints();
    bool createDevice();
    bool checkPhysicalDeviceProperties(VkPhysicalDevice physical_device,
                                       std::uint32_t& queue_family_index);
    bool loadDeviceLevelEntryPoints();
    bool getDeviceQueue();

    bool checkValidationLayerSupport() const;
    bool setupDebugMessenger();
    bool destroyDebugMessenger();

    friend std::ostream& operator<<(
            std::ostream& out, const std::vector<VkLayerProperties>& vect);

    os::LibraryHandle m_vulkan_library_handle;
    VulkanTutorial01Parameters m_vulkan_tutorial01_parameters;
    std::atomic<bool> m_enable_vulkan_debug;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL01_H
