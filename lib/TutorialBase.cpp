#include "vulkan_graphix/TutorialBase.h"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <cstdint>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

// ************************************************************ //
// QueueParameters                                              //
//                                                              //
// Vulkan Queue's parameters container class                    //
// ************************************************************ //
QueueParameters::QueueParameters()
        : m_vk_queue(VK_NULL_HANDLE), m_family_index(0) {}

const VkQueue& QueueParameters::getVkQueue() const { return m_vk_queue; }

VkQueue& QueueParameters::getVkQueue() { return m_vk_queue; }
void QueueParameters::setVkQueue(const VkQueue& vk_queue) {
    m_vk_queue = vk_queue;
}

std::uint32_t QueueParameters::getFamilyIndex() const {
    return m_family_index;
}
void QueueParameters::setFamilyIndex(const std::uint32_t family_index) {
    m_family_index = family_index;
}

// ************************************************************ //
// ImageParameters                                              //
//                                                              //
// Vulkan Image's parameters container class                    //
// ************************************************************ //
ImageParameters::ImageParameters()
        : m_vk_image(VK_NULL_HANDLE)
        , m_vk_image_view(VK_NULL_HANDLE)
        , m_vk_sampler(VK_NULL_HANDLE)
        , m_vk_device_memory(VK_NULL_HANDLE) {}

const VkImage& ImageParameters::getVkImage() const { return m_vk_image; }
void ImageParameters::setVkImage(const VkImage& other) { m_vk_image = other; }

const VkImageView& ImageParameters::getVkImageView() const {
    return m_vk_image_view;
}

VkImageView& ImageParameters::getVkImageView() { return m_vk_image_view; }
void ImageParameters::setVkImageView(const VkImageView& other) {
    m_vk_image_view = other;
}

const VkSampler& ImageParameters::getVkSampler() const { return m_vk_sampler; }
void ImageParameters::setVkSampler(const VkSampler& other) {
    m_vk_sampler = other;
}

const VkDeviceMemory& ImageParameters::getVkDeviceMemory() const {
    return m_vk_device_memory;
}
void ImageParameters::setVkDeviceMemory(const VkDeviceMemory& other) {
    m_vk_device_memory = other;
}

// ************************************************************ //
// BufferParameters                                             //
//                                                              //
// Vulkan Buffer's parameters container class                   //
// ************************************************************ //
BufferParameters::BufferParameters()
        : m_vk_buffer(VK_NULL_HANDLE)
        , m_vk_device_memory(VK_NULL_HANDLE)
        , m_size(0) {}

const VkBuffer& BufferParameters::getVkBuffer() const { return m_vk_buffer; }
void BufferParameters::setVkBuffer(const VkBuffer& other) {
    m_vk_buffer = other;
}

const VkDeviceMemory& BufferParameters::getVkDeviceMemory() const {
    return m_vk_device_memory;
}
void BufferParameters::setVkDeviceMemory(const VkDeviceMemory& other) {
    m_vk_device_memory = other;
}

std::uint32_t BufferParameters::getSize() const { return m_size; }
void BufferParameters::setSize(const std::uint32_t size) { m_size = size; }

// ************************************************************ //
// DescriptorParameters                                         //
//                                                              //
// Container class for descriptor related resources             //
// ************************************************************ //
DescriptorSetParameters::DescriptorSetParameters()
        : m_vk_descriptor_pool(VK_NULL_HANDLE)
        , m_vk_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_descriptor_set(VK_NULL_HANDLE) {}

const VkDescriptorPool& DescriptorSetParameters::getVkDescriptorPool() const {
    return m_vk_descriptor_pool;
}
void DescriptorSetParameters::setVkDescriptorPool(
        const VkDescriptorPool& other) {
    m_vk_descriptor_pool = other;
}

const VkDescriptorSetLayout&
DescriptorSetParameters::getVkDescriptorSetLayout() const {
    return m_vk_descriptor_set_layout;
}
void DescriptorSetParameters::setVkDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_descriptor_set_layout = other;
}

const VkDescriptorSet& DescriptorSetParameters::getVkDescriptorSet() const {
    return m_vk_descriptor_set;
}
void DescriptorSetParameters::setVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_descriptor_set = other;
}

// ************************************************************ //
// RenderingResourceParameters                                  //
//                                                              //
// Container class for the resources used during a single frame //
// ************************************************************ //
RenderingResourceParameters::RenderingResourceParameters()
        : m_vk_framebuffer(VK_NULL_HANDLE)
        , m_vk_command_buffer(VK_NULL_HANDLE)
        , m_image_available_vk_semaphore(VK_NULL_HANDLE)
        , m_vk_fence(VK_NULL_HANDLE) {}

const VkFramebuffer& RenderingResourceParameters::getVkFramebuffer() const {
    return m_vk_framebuffer;
}
VkFramebuffer& RenderingResourceParameters::getVkFramebuffer() {
    return m_vk_framebuffer;
}
void RenderingResourceParameters::setVkFramebuffer(
        const VkFramebuffer& vk_framebuffer) {
    m_vk_framebuffer = vk_framebuffer;
}

const VkCommandBuffer& RenderingResourceParameters::getVkCommandBuffer()
        const {
    return m_vk_command_buffer;
}
VkCommandBuffer& RenderingResourceParameters::getVkCommandBuffer() {
    return m_vk_command_buffer;
}
void RenderingResourceParameters::setVkCommandBuffer(
        const VkCommandBuffer& vk_command_buffer) {
    m_vk_command_buffer = vk_command_buffer;
}

const VkSemaphore& RenderingResourceParameters::getImageAvailableVkSemaphore()
        const {
    return m_image_available_vk_semaphore;
}
VkSemaphore& RenderingResourceParameters::getImageAvailableVkSemaphore() {
    return m_image_available_vk_semaphore;
}
void RenderingResourceParameters::setImageAvailableVkSemaphore(
        const VkSemaphore& vk_semaphore) {
    m_image_available_vk_semaphore = vk_semaphore;
}

const VkFence& RenderingResourceParameters::getVkFence() const {
    return m_vk_fence;
}
VkFence& RenderingResourceParameters::getVkFence() { return m_vk_fence; }
void RenderingResourceParameters::setVkFence(const VkFence& vk_fence) {
    m_vk_fence = vk_fence;
}

// ************************************************************ //
// SwapChainParameters                                          //
//                                                              //
// Vulkan SwapChain's parameters container class                //
// ************************************************************ //
SwapChainParameters::SwapChainParameters()
        : m_vk_swapchain_khr(VK_NULL_HANDLE)
        , m_vk_format(VK_FORMAT_UNDEFINED)
        , m_vk_extent_2d() {}

const VkSwapchainKHR& SwapChainParameters::getVkSwapchainKhr() const {
    return m_vk_swapchain_khr;
}

VkSwapchainKHR& SwapChainParameters::getVkSwapchainKhr() {
    return m_vk_swapchain_khr;
}
void SwapChainParameters::setVkSwapchainKhr(const VkSwapchainKHR& other) {
    m_vk_swapchain_khr = other;
}

const VkFormat& SwapChainParameters::getVkFormat() const {
    return m_vk_format;
}
void SwapChainParameters::setVkFormat(VkFormat& other) { m_vk_format = other; }

const std::vector<ImageParameters>& SwapChainParameters::getImageParameters()
        const {
    return m_image_parameters;
}

std::vector<ImageParameters>& SwapChainParameters::getImageParameters() {
    return m_image_parameters;
}
void SwapChainParameters::setImageParameters(
        const std::vector<ImageParameters>& other) {
    m_image_parameters = other;
}

const VkExtent2D& SwapChainParameters::getVkExtent2d() const {
    return m_vk_extent_2d;
}
void SwapChainParameters::setVkExtent2d(VkExtent2D& other) {
    m_vk_extent_2d = other;
}

// ************************************************************ //
// TutorialBaseParameters                                       //
//                                                              //
// General Vulkan parameters' container class                   //
// ************************************************************ //
TutorialBaseParameters::TutorialBaseParameters()
        : m_vk_instance(VK_NULL_HANDLE)
        , m_vk_physical_device(VK_NULL_HANDLE)
        , m_vk_device(VK_NULL_HANDLE)
        , m_vk_surface_khr(VK_NULL_HANDLE) {}

const VkInstance& TutorialBaseParameters::getVkInstance() const {
    return m_vk_instance;
}

VkInstance& TutorialBaseParameters::getVkInstance() { return m_vk_instance; }
void TutorialBaseParameters::setVkInstance(const VkInstance& other) {
    m_vk_instance = other;
}

const VkPhysicalDevice& TutorialBaseParameters::getVkPhysicalDevice() const {
    return m_vk_physical_device;
}

VkPhysicalDevice& TutorialBaseParameters::getVkPhysicalDevice() {
    return m_vk_physical_device;
}
void TutorialBaseParameters::setVkPhysicalDevice(
        const VkPhysicalDevice& other) {
    m_vk_physical_device = other;
}

const VkDevice& TutorialBaseParameters::getVkDevice() const {
    return m_vk_device;
}

VkDevice& TutorialBaseParameters::getVkDevice() { return m_vk_device; }
void TutorialBaseParameters::setVkDevice(const VkDevice& other) {
    m_vk_device = other;
}

const QueueParameters& TutorialBaseParameters::getGraphicsQueueParameters()
        const {
    return m_graphics_queue_parameters;
}

QueueParameters& TutorialBaseParameters::getGraphicsQueueParameters() {
    return m_graphics_queue_parameters;
}
void TutorialBaseParameters::setGraphicsQueueParameters(
        const QueueParameters& other) {
    m_graphics_queue_parameters = other;
}

const QueueParameters& TutorialBaseParameters::getPresentQueueParameters()
        const {
    return m_present_queue_parameters;
}

QueueParameters& TutorialBaseParameters::getPresentQueueParameters() {
    return m_present_queue_parameters;
}
void TutorialBaseParameters::setPresentQueueParameters(
        const QueueParameters& other) {
    m_present_queue_parameters = other;
}

const VkSurfaceKHR& TutorialBaseParameters::getVkSurfaceKhr() const {
    return m_vk_surface_khr;
}

VkSurfaceKHR& TutorialBaseParameters::getVkSurfaceKhr() {
    return m_vk_surface_khr;
}
void TutorialBaseParameters::setVkSurfaceKhr(const VkSurfaceKHR& other) {
    m_vk_surface_khr = other;
}

const SwapChainParameters& TutorialBaseParameters::getSwapchainParameters()
        const {
    return m_swapchain_parameters;
}

SwapChainParameters& TutorialBaseParameters::getSwapchainParameters() {
    return m_swapchain_parameters;
}
void TutorialBaseParameters::setSwapchainParameters(
        const SwapChainParameters& other) {
    m_swapchain_parameters = other;
}

const VkDebugUtilsMessengerEXT&
TutorialBaseParameters::getVkDebugUtilsMessenger() const {
    return m_vk_debug_utils_messenger;
}
VkDebugUtilsMessengerEXT& TutorialBaseParameters::getVkDebugUtilsMessenger() {
    return m_vk_debug_utils_messenger;
}
void TutorialBaseParameters::setVkDebugUtilsMessenger(
        const VkDebugUtilsMessengerEXT& vk_debug_utils_messenger) {
    m_vk_debug_utils_messenger = vk_debug_utils_messenger;
}

TutorialBase::TutorialBase()
        : LoggedClass<TutorialBase>(*this)
        , m_vulkan_library_handle()
        , m_enable_vk_debug(true) {}

TutorialBase::~TutorialBase() {
    // vkDeviceWaitIdle/vkDestroyImageView/vkDestroyDevice/vkDestroyInstance
    // are all loaded on demand (loadInstanceLevelEntryPoints()/
    // loadDeviceLevelEntryPoints(), see ListOfFunctions.inl) rather than
    // linked directly, so a handle being non-null doesn't guarantee its
    // matching destroy function was ever resolved: if prepareVulkan() fails
    // partway - e.g. the device was created but loadDeviceLevelEntryPoints()
    // then failed on an earlier function than vkDeviceWaitIdle/
    // vkDestroyDevice - those pointers are still null, and calling through
    // a null function pointer here segfaults. Guard each one explicitly;
    // vkDestroySwapchainKHR/vkDestroySurfaceKHR aren't namespaced the same
    // way (see ListOfFunctions.inl's USE_SWAPCHAIN_EXTENSIONS comment) and
    // always resolve to the real libvulkan.so symbol, so they don't need
    // this guard.
    if (m_vulkan_common_parameters.getVkDevice() != VK_NULL_HANDLE) {
        if (vkDeviceWaitIdle != nullptr) {
            vkDeviceWaitIdle(m_vulkan_common_parameters.getVkDevice());
        }

        if (m_vulkan_common_parameters.getVkDebugUtilsMessenger() !=
            VK_NULL_HANDLE) {
            if (!destroyDebugMessenger()) {
                Logging::error(
                        LOG_TAG,
                        "Failed to destroy VkDebugUtilsMessengerExt!!!");
            }
        }

        if (vkDestroyImageView != nullptr) {
            for (size_t i = 0;
                 i < m_vulkan_common_parameters.getSwapchainParameters()
                             .getImageParameters()
                             .size();
                 ++i) {
                if (m_vulkan_common_parameters.getSwapchainParameters()
                            .getImageParameters()[i]
                            .getVkImageView() != VK_NULL_HANDLE) {
                    vkDestroyImageView(
                            getVkDevice(),
                            m_vulkan_common_parameters.getSwapchainParameters()
                                    .getImageParameters()[i]
                                    .getVkImageView(),
                            nullptr);
                }
            }
        }

        if (m_vulkan_common_parameters.getSwapchainParameters()
                    .getVkSwapchainKhr() != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(
                    m_vulkan_common_parameters.getVkDevice(),
                    m_vulkan_common_parameters.getSwapchainParameters()
                            .getVkSwapchainKhr(),
                    nullptr);
        }
        if (vkDestroyDevice != nullptr) {
            vkDestroyDevice(m_vulkan_common_parameters.getVkDevice(), nullptr);
        }
    }

    if (m_vulkan_common_parameters.getVkSurfaceKhr() != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_vulkan_common_parameters.getVkInstance(),
                            m_vulkan_common_parameters.getVkSurfaceKhr(),
                            nullptr);
    }

    if ((m_vulkan_common_parameters.getVkInstance() != VK_NULL_HANDLE) &&
        (vkDestroyInstance != nullptr)) {
        vkDestroyInstance(m_vulkan_common_parameters.getVkInstance(), nullptr);
    }

    if (m_vulkan_library_handle) {
        dlclose(m_vulkan_library_handle);
    }
}

bool TutorialBase::prepareVulkan(os::WindowParameters parameters) {
    m_window_parameters = parameters;

    Logging::info(LOG_TAG, "loadVulkanLibrary()");
    if (!loadVulkanLibrary()) {
        return false;
    }
    Logging::info(LOG_TAG, "loadExportedEntryPoints()");
    if (!loadExportedEntryPoints()) {
        return false;
    }
    Logging::info(LOG_TAG, "loadGlobalLevelEntryPoints()");
    if (!loadGlobalLevelEntryPoints()) {
        return false;
    }
    Logging::info(LOG_TAG, "createInstance()");
    if (!createInstance()) {
        return false;
    }
    Logging::info(LOG_TAG, "loadInstanceLevelEntryPoints()");
    if (!loadInstanceLevelEntryPoints()) {
        return false;
    }
    Logging::info(LOG_TAG, "createPresentationSurface()");
    if (!createPresentationSurface()) {
        return false;
    }
    Logging::info(LOG_TAG, "createDevice()");
    if (!createDevice()) {
        return false;
    }
    Logging::info(LOG_TAG, "loadDeviceLevelEntryPoints()");
    if (!loadDeviceLevelEntryPoints()) {
        return false;
    }
    Logging::info(LOG_TAG, "getDeviceQueue()");
    if (!getDeviceQueue()) {
        return false;
    }
    Logging::info(LOG_TAG, "createSwapChain()");
    return createSwapChain();
}

bool TutorialBase::onWindowSizeChanged() {
    if (m_vulkan_common_parameters.getVkDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vulkan_common_parameters.getVkDevice());
    }

    childClear();

    if (createSwapChain()) {
        if (m_can_render) {
            return childOnWindowSizeChanged();
        }
        return true;
    }

    return false;
}

const VkPhysicalDevice& TutorialBase::getVkPhysicalDevice() const {
    return m_vulkan_common_parameters.getVkPhysicalDevice();
}

VkPhysicalDevice& TutorialBase::getVkPhysicalDevice() {
    return m_vulkan_common_parameters.getVkPhysicalDevice();
}

const VkDevice& TutorialBase::getVkDevice() const {
    return m_vulkan_common_parameters.getVkDevice();
}

VkDevice& TutorialBase::getVkDevice() {
    return m_vulkan_common_parameters.getVkDevice();
}

const QueueParameters& TutorialBase::getGraphicsQueueParameters() const {
    return m_vulkan_common_parameters.getGraphicsQueueParameters();
}

const QueueParameters& TutorialBase::getPresentQueueParameters() const {
    return m_vulkan_common_parameters.getPresentQueueParameters();
}

const SwapChainParameters& TutorialBase::getSwapchainParameters() const {
    return m_vulkan_common_parameters.getSwapchainParameters();
}

bool TutorialBase::loadVulkanLibrary() {
    m_vulkan_library_handle = dlopen("libvulkan.so.1", RTLD_NOW);

    if (m_vulkan_library_handle == nullptr) {
        Logging::error(LOG_TAG, "Could not load Vulkan library!");
        return false;
    }
    return true;
}

bool TutorialBase::loadExportedEntryPoints() {
#define LoadProcAddress dlsym

#define VK_EXPORTED_FUNCTION(fun)                                         \
    if (!((fun) = (PFN_##fun)LoadProcAddress(m_vulkan_library_handle,     \
                                             #fun))) {                    \
        Logging::error(                                                   \
                LOG_TAG, "Could not load exported function:", #fun, "!"); \
        return false;                                                     \
    }

#include "vulkan_graphix/ListOfFunctions.inl"

    return true;
}

bool TutorialBase::loadGlobalLevelEntryPoints() {
#define VK_GLOBAL_LEVEL_FUNCTION(fun)                                 \
    if (!((fun) = (PFN_##fun)vkGetInstanceProcAddr(nullptr, #fun))) { \
        Logging::error(LOG_TAG,                                       \
                       "Could not load global level function: ",      \
                       #fun,                                          \
                       "!");                                          \
        return false;                                                 \
    }

#include "vulkan_graphix/ListOfFunctions.inl"

    return true;
}

bool TutorialBase::createInstance() {
    if (!checkValidationLayerSupport()) {
        Logging::error(LOG_TAG,
                       "Failed to create an instance that does not support",
                       "validation layers.");
    }

    std::uint32_t extensions_count = 0;
    if ((vkEnumerateInstanceExtensionProperties(
                 nullptr, &extensions_count, nullptr) != VK_SUCCESS) ||
        (extensions_count == 0)) {
        Logging::error(
                LOG_TAG,
                "Error occurred during instance extensions enumeration!");
        return false;
    }

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    if (vkEnumerateInstanceExtensionProperties(
                nullptr, &extensions_count, available_extensions.data()) !=
        VK_SUCCESS) {
        Logging::error(
                LOG_TAG,
                "Error occurred during instance extensions enumeration!");
        return false;
    }

    std::vector<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME,
                                           VK_KHR_XLIB_SURFACE_EXTENSION_NAME};

    if (m_enable_vk_debug.load()) {
        Logging::info(LOG_TAG,
                      "Adding the",
                      VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                      "extension...");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    for (size_t i = 0; i < extensions.size(); ++i) {
        if (!checkExtensionAvailability(extensions[i], available_extensions)) {
            Logging::error(LOG_TAG,
                           "Could not find instance extension named \"",
                           extensions[i],
                           "\"!");
            return false;
        }
    }

    VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "API without Secrets: Introduction to Vulkan",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Vulkan Tutorial by Intel",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_MAKE_VERSION(1, 0, 0)};

    VkInstanceCreateInfo instance_create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount =
                    static_cast<std::uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()};

    if (vkCreateInstance(&instance_create_info,
                         nullptr,
                         &m_vulkan_common_parameters.getVkInstance()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create Vulkan instance!");
        return false;
    }

    if (!setupDebugMessenger()) {
        Logging::error(LOG_TAG, "Failed to setup debug messenger!!!");
    }

    return true;
}

bool TutorialBase::loadInstanceLevelEntryPoints() {
#define VK_INSTANCE_LEVEL_FUNCTION(fun)                                 \
    if (!((fun) = (PFN_##fun)vkGetInstanceProcAddr(                     \
                  m_vulkan_common_parameters.getVkInstance(), #fun))) { \
        Logging::error(LOG_TAG,                                         \
                       "Could not load instance level function:",       \
                       #fun,                                            \
                       "!");                                            \
        return false;                                                   \
    }

#include "vulkan_graphix/ListOfFunctions.inl"

    return true;
}

bool TutorialBase::createPresentationSurface() {
    VkXlibSurfaceCreateInfoKHR surface_create_info = {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .dpy = m_window_parameters.getDisplayPtr(),
            .window = m_window_parameters.getWindowHandle()};
    if (vkCreateXlibSurfaceKHR(
                m_vulkan_common_parameters.getVkInstance(),
                &surface_create_info,
                nullptr,
                &m_vulkan_common_parameters.getVkSurfaceKhr()) == VK_SUCCESS) {
        return true;
    }

    Logging::error(LOG_TAG, "Could not create presentation surface!");
    return false;
}

bool TutorialBase::createDevice() {
    std::uint32_t num_devices = 0;
    if ((vkEnumeratePhysicalDevices(m_vulkan_common_parameters.getVkInstance(),
                                    &num_devices,
                                    nullptr) != VK_SUCCESS) ||
        (num_devices == 0)) {
        Logging::error(LOG_TAG,
                       "Error occurred during physical devices enumeration!");
        return false;
    }

    std::vector<VkPhysicalDevice> physical_devices(num_devices);
    if (vkEnumeratePhysicalDevices(m_vulkan_common_parameters.getVkInstance(),
                                   &num_devices,
                                   physical_devices.data()) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Error occurred during physical devices enumeration!");
        return false;
    }

    std::uint32_t selected_graphics_queue_family_index = UINT32_MAX;
    std::uint32_t selected_present_queue_family_index = UINT32_MAX;

    for (std::uint32_t i = 0; i < num_devices; ++i) {
        if (checkPhysicalDeviceProperties(
                    physical_devices[i],
                    selected_graphics_queue_family_index,
                    selected_present_queue_family_index)) {
            m_vulkan_common_parameters.setVkPhysicalDevice(
                    physical_devices[i]);
            break;
        }
    }
    if (m_vulkan_common_parameters.getVkPhysicalDevice() == VK_NULL_HANDLE) {
        Logging::error(LOG_TAG,
                       "Could not select physical device based on the chosen "
                       "properties!");
        return false;
    }

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::vector<float> queue_priorities = {1.0f};

    queue_create_infos.push_back(VkDeviceQueueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = selected_graphics_queue_family_index,
            .queueCount = static_cast<std::uint32_t>(queue_priorities.size()),
            .pQueuePriorities = queue_priorities.data()});

    if (selected_graphics_queue_family_index !=
        selected_present_queue_family_index) {
        queue_create_infos.push_back(VkDeviceQueueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = selected_present_queue_family_index,
                .queueCount =
                        static_cast<std::uint32_t>(queue_priorities.size()),
                .pQueuePriorities = queue_priorities.data()});
    }

    std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // Enable wideLines when the device supports it (near-universal - see
    // checkPhysicalDeviceProperties()'s own feature query) so a tutorial
    // that needs a line thicker than the default 1px (e.g. Tutorial10's
    // polyline) can just set the rasterization state's lineWidth, without
    // a device-creation failure on hardware that lacks the feature.
    VkPhysicalDeviceFeatures available_device_features;
    vkGetPhysicalDeviceFeatures(
            m_vulkan_common_parameters.getVkPhysicalDevice(),
            &available_device_features);
    VkPhysicalDeviceFeatures enabled_device_features = {};
    enabled_device_features.wideLines = available_device_features.wideLines;

    VkDeviceCreateInfo device_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount =
                    static_cast<std::uint32_t>(queue_create_infos.size()),
            .pQueueCreateInfos = queue_create_infos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount =
                    static_cast<std::uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = &enabled_device_features};

    if (vkCreateDevice(m_vulkan_common_parameters.getVkPhysicalDevice(),
                       &device_create_info,
                       nullptr,
                       &m_vulkan_common_parameters.getVkDevice()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create Vulkan device!");
        return false;
    }

    m_vulkan_common_parameters.getGraphicsQueueParameters().setFamilyIndex(
            selected_graphics_queue_family_index);
    m_vulkan_common_parameters.getPresentQueueParameters().setFamilyIndex(
            selected_present_queue_family_index);
    return true;
}

bool TutorialBase::checkPhysicalDeviceProperties(
        VkPhysicalDevice physical_device,
        std::uint32_t& selected_graphics_queue_family_index,
        std::uint32_t& selected_present_queue_family_index) {
    std::uint32_t extensions_count = 0;
    if ((vkEnumerateDeviceExtensionProperties(
                 physical_device, nullptr, &extensions_count, nullptr) !=
         VK_SUCCESS) ||
        (extensions_count == 0)) {
        Logging::error(LOG_TAG,
                       "Error occurred during physical device",
                       physical_device,
                       "extensions enumeration!");
        return false;
    }

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    if (vkEnumerateDeviceExtensionProperties(physical_device,
                                             nullptr,
                                             &extensions_count,
                                             available_extensions.data()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Error occurred during physical device",
                       physical_device,
                       "extensions enumeration!");
        return false;
    }

    std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    for (size_t i = 0; i < device_extensions.size(); ++i) {
        if (!checkExtensionAvailability(device_extensions[i],
                                        available_extensions)) {
            Logging::error(LOG_TAG,
                           "Physical device",
                           physical_device,
                           "doesn't support extension named \"",
                           device_extensions[i],
                           "\"!");
            return false;
        }
    }

    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;

    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);

    std::uint32_t major_version =
            VK_VERSION_MAJOR(device_properties.apiVersion);

    if ((major_version < 1) ||
        (device_properties.limits.maxImageDimension2D < 4096)) {
        Logging::error(LOG_TAG,
                       "Physical device",
                       physical_device,
                       "doesn't support required parameters!");
        return false;
    }

    std::uint32_t queue_families_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device, &queue_families_count, nullptr);
    if (queue_families_count == 0) {
        Logging::error(LOG_TAG,
                       "Physical device",
                       physical_device,
                       "doesn't have any queue families!");
        return false;
    }

    std::vector<VkQueueFamilyProperties> queue_family_properties(
            queue_families_count);
    std::vector<VkBool32> queue_present_support(queue_families_count);

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                             &queue_families_count,
                                             queue_family_properties.data());

    std::uint32_t graphics_queue_family_index = UINT32_MAX;
    std::uint32_t present_queue_family_index = UINT32_MAX;

    for (std::uint32_t i = 0; i < queue_families_count; ++i) {
        vkGetPhysicalDeviceSurfaceSupportKHR(
                physical_device,
                i,
                m_vulkan_common_parameters.getVkSurfaceKhr(),
                &queue_present_support[i]);

        if ((queue_family_properties[i].queueCount > 0) &&
            (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            // Select first queue that supports graphics
            if (graphics_queue_family_index == UINT32_MAX) {
                graphics_queue_family_index = i;
            }

            // If there is queue that supports both graphics and present -
            // prefer it
            if (queue_present_support[i]) {
                selected_graphics_queue_family_index = i;
                selected_present_queue_family_index = i;
                return true;
            }
        }
    }

    // We don't have queue that supports both graphics and present so we have
    // to use separate queues
    for (std::uint32_t i = 0; i < queue_families_count; ++i) {
        if (queue_present_support[i]) {
            present_queue_family_index = i;
            break;
        }
    }

    // If this device doesn't support queues with graphics and present
    // capabilities don't use it
    if ((graphics_queue_family_index == UINT32_MAX) ||
        (present_queue_family_index == UINT32_MAX)) {
        Logging::error(LOG_TAG,
                       "Could not find queue families with required",
                       "properties on physical device",
                       physical_device,
                       "!");
        return false;
    }

    selected_graphics_queue_family_index = graphics_queue_family_index;
    selected_present_queue_family_index = present_queue_family_index;
    return true;
}

bool TutorialBase::loadDeviceLevelEntryPoints() {
#define VK_DEVICE_LEVEL_FUNCTION(fun)                                         \
    if (!((fun) = (PFN_##fun)vkGetDeviceProcAddr(                             \
                  m_vulkan_common_parameters.getVkDevice(), #fun))) {         \
        Logging::error(                                                       \
                LOG_TAG, "Could not load device level function:", #fun, "!"); \
        return false;                                                         \
    }

#include "vulkan_graphix/ListOfFunctions.inl"

    return true;
}

bool TutorialBase::getDeviceQueue() {
    vkGetDeviceQueue(m_vulkan_common_parameters.getVkDevice(),
                     m_vulkan_common_parameters.getGraphicsQueueParameters()
                             .getFamilyIndex(),
                     0,
                     &m_vulkan_common_parameters.getGraphicsQueueParameters()
                              .getVkQueue());
    vkGetDeviceQueue(m_vulkan_common_parameters.getVkDevice(),
                     m_vulkan_common_parameters.getPresentQueueParameters()
                             .getFamilyIndex(),
                     0,
                     &m_vulkan_common_parameters.getPresentQueueParameters()
                              .getVkQueue());
    return true;
}

bool TutorialBase::createSwapChain() {
    m_can_render = false;

    if (m_vulkan_common_parameters.getVkDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vulkan_common_parameters.getVkDevice());
    }

    for (size_t i = 0; i < m_vulkan_common_parameters.getSwapchainParameters()
                                   .getImageParameters()
                                   .size();
         ++i) {
        if (m_vulkan_common_parameters.getSwapchainParameters()
                    .getImageParameters()[i]
                    .getVkImageView() != VK_NULL_HANDLE) {
            vkDestroyImageView(
                    getVkDevice(),
                    m_vulkan_common_parameters.getSwapchainParameters()
                            .getImageParameters()[i]
                            .getVkImageView(),
                    nullptr);
            m_vulkan_common_parameters.getSwapchainParameters()
                    .getImageParameters()[i]
                    .setVkImageView(VK_NULL_HANDLE);
        }
    }
    m_vulkan_common_parameters.getSwapchainParameters()
            .getImageParameters()
            .clear();

    VkSurfaceCapabilitiesKHR surface_capabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                m_vulkan_common_parameters.getVkPhysicalDevice(),
                m_vulkan_common_parameters.getVkSurfaceKhr(),
                &surface_capabilities) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Could not check presentation surface capabilities!");
        return false;
    }

    std::uint32_t formats_count;
    if ((vkGetPhysicalDeviceSurfaceFormatsKHR(
                 m_vulkan_common_parameters.getVkPhysicalDevice(),
                 m_vulkan_common_parameters.getVkSurfaceKhr(),
                 &formats_count,
                 nullptr) != VK_SUCCESS) ||
        (formats_count == 0)) {
        Logging::error(LOG_TAG,
                       "Error occurred during presentation surface formats",
                       "enumeration!");
        return false;
    }

    std::vector<VkSurfaceFormatKHR> surface_formats(formats_count);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_vulkan_common_parameters.getVkPhysicalDevice(),
                m_vulkan_common_parameters.getVkSurfaceKhr(),
                &formats_count,
                surface_formats.data()) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Error occurred during presentation surface formats",
                       "enumeration!");
        return false;
    }

    std::uint32_t present_modes_count;
    if ((vkGetPhysicalDeviceSurfacePresentModesKHR(
                 m_vulkan_common_parameters.getVkPhysicalDevice(),
                 m_vulkan_common_parameters.getVkSurfaceKhr(),
                 &present_modes_count,
                 nullptr) != VK_SUCCESS) ||
        (present_modes_count == 0)) {
        Logging::error(
                LOG_TAG,
                "Error occurred during presentation surface present modes",
                "enumeration!");
        return false;
    }

    std::vector<VkPresentModeKHR> present_modes(present_modes_count);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_vulkan_common_parameters.getVkPhysicalDevice(),
                m_vulkan_common_parameters.getVkSurfaceKhr(),
                &present_modes_count,
                present_modes.data()) != VK_SUCCESS) {
        Logging::error(
                LOG_TAG,
                "Error occurred during presentation surface present modes",
                "enumeration!");
        return false;
    }

    std::uint32_t desired_number_of_images =
            getSwapChainNumImages(surface_capabilities);
    VkSurfaceFormatKHR desired_format = getSwapChainFormat(surface_formats);
    VkExtent2D desired_extent = getSwapChainExtent(surface_capabilities);
    VkImageUsageFlags desired_usage =
            getSwapChainUsageFlags(surface_capabilities);
    VkSurfaceTransformFlagBitsKHR desired_transform =
            getSwapChainTransform(surface_capabilities);
    VkPresentModeKHR desired_present_mode =
            getSwapChainPresentMode(present_modes);
    VkSwapchainKHR old_swap_chain =
            m_vulkan_common_parameters.getSwapchainParameters()
                    .getVkSwapchainKhr();

    if (static_cast<int>(desired_usage) == -1) {
        return false;
    }
    if (static_cast<int>(desired_present_mode) == -1) {
        return false;
    }
    if ((desired_extent.width == 0) || (desired_extent.height == 0)) {
        // Current surface size is (0, 0) so we can't create a swap chain and
        // render anything (CanRender == false) But we don't wont to kill the
        // application as this situation may occur i.e. when window gets
        // minimized
        return true;
    }

    VkSwapchainCreateInfoKHR swap_chain_create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = m_vulkan_common_parameters.getVkSurfaceKhr(),
            .minImageCount = desired_number_of_images,
            .imageFormat = desired_format.format,
            .imageColorSpace = desired_format.colorSpace,
            .imageExtent = desired_extent,
            .imageArrayLayers = 1,
            .imageUsage = desired_usage,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = desired_transform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = desired_present_mode,
            .clipped = VK_TRUE,
            .oldSwapchain = old_swap_chain};

    if (vkCreateSwapchainKHR(
                m_vulkan_common_parameters.getVkDevice(),
                &swap_chain_create_info,
                nullptr,
                &m_vulkan_common_parameters.getSwapchainParameters()
                         .getVkSwapchainKhr()) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create swap chain!");
        return false;
    }
    if (old_swap_chain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_vulkan_common_parameters.getVkDevice(),
                              old_swap_chain,
                              nullptr);
    }

    m_vulkan_common_parameters.getSwapchainParameters().setVkFormat(
            desired_format.format);

    std::uint32_t image_count = 0;
    if ((vkGetSwapchainImagesKHR(
                 m_vulkan_common_parameters.getVkDevice(),
                 m_vulkan_common_parameters.getSwapchainParameters()
                         .getVkSwapchainKhr(),
                 &image_count,
                 nullptr) != VK_SUCCESS) ||
        (image_count == 0)) {
        Logging::error(LOG_TAG, "Could not get swap chain images!");
        return false;
    }

    m_vulkan_common_parameters.getSwapchainParameters()
            .getImageParameters()
            .resize(image_count);

    std::vector<VkImage> images(image_count);
    if (vkGetSwapchainImagesKHR(
                m_vulkan_common_parameters.getVkDevice(),
                m_vulkan_common_parameters.getSwapchainParameters()
                        .getVkSwapchainKhr(),
                &image_count,
                images.data()) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not get swap chain images!");
        return false;
    }

    for (size_t i = 0; i < m_vulkan_common_parameters.getSwapchainParameters()
                                   .getImageParameters()
                                   .size();
         ++i) {
        m_vulkan_common_parameters.getSwapchainParameters()
                .getImageParameters()[i]
                .setVkImage(images[i]);
    }
    m_vulkan_common_parameters.getSwapchainParameters().setVkExtent2d(
            desired_extent);

    return createSwapChainImageViews();
}

bool TutorialBase::createSwapChainImageViews() {
    for (size_t i = 0; i < m_vulkan_common_parameters.getSwapchainParameters()
                                   .getImageParameters()
                                   .size();
         ++i) {
        VkImageViewCreateInfo image_view_create_info = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = m_vulkan_common_parameters.getSwapchainParameters()
                                 .getImageParameters()[i]
                                 .getVkImage(),
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = getSwapchainParameters().getVkFormat(),
                .components =
                        VkComponentMapping{.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .a = VK_COMPONENT_SWIZZLE_IDENTITY},
                .subresourceRange = VkImageSubresourceRange{
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1}};

        if (vkCreateImageView(
                    getVkDevice(),
                    &image_view_create_info,
                    nullptr,
                    &m_vulkan_common_parameters.getSwapchainParameters()
                             .getImageParameters()[i]
                             .getVkImageView()) != VK_SUCCESS) {
            Logging::error(LOG_TAG,
                           "Could not create image view for framebuffer!");
            return false;
        }
    }

    m_can_render = true;

    return true;
}

bool TutorialBase::checkExtensionAvailability(
        const char* extension_name,
        const std::vector<VkExtensionProperties>& available_extensions) {
    for (size_t i = 0; i < available_extensions.size(); ++i) {
        if (strcmp(available_extensions[i].extensionName, extension_name) ==
            0) {
            return true;
        }
    }
    return false;
}

std::uint32_t TutorialBase::getSwapChainNumImages(
        VkSurfaceCapabilitiesKHR& surface_capabilities) {
    // Set of images defined in a swap chain may not always be available for
    // application to render to: One may be displayed and one may wait in a
    // queue to be presented If application wants to use more images at the
    // same time it must ask for more images
    std::uint32_t image_count = surface_capabilities.minImageCount + 2;
    if ((surface_capabilities.maxImageCount > 0) &&
        (image_count > surface_capabilities.maxImageCount)) {
        image_count = surface_capabilities.maxImageCount;
    }
    return image_count;
}

VkSurfaceFormatKHR TutorialBase::getSwapChainFormat(
        std::vector<VkSurfaceFormatKHR>& surface_formats) {
    // If the list contains only one entry with undefined format
    // it means that there are no preferred surface formats and any can be
    // chosen
    if ((surface_formats.size() == 1) &&
        (surface_formats[0].format == VK_FORMAT_UNDEFINED)) {
        return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
    }

    // Check if list contains most widely used R8 G8 B8 A8 format
    // with nonlinear color space
    for (VkSurfaceFormatKHR& surface_format : surface_formats) {
        if (surface_format.format == VK_FORMAT_R8G8B8A8_UNORM) {
            return surface_format;
        }
    }

    // Return the first format from the list
    return surface_formats[0];
}

VkExtent2D TutorialBase::getSwapChainExtent(
        VkSurfaceCapabilitiesKHR& surface_capabilities) {
    // Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within
    // defined confines
    if (static_cast<std::int32_t>(surface_capabilities.currentExtent.width) ==
        -1) {
        VkExtent2D swap_chain_extent = {.width = 640, .height = 480};
        if (swap_chain_extent.width <
            surface_capabilities.minImageExtent.width) {
            swap_chain_extent.width =
                    surface_capabilities.minImageExtent.width;
        }
        if (swap_chain_extent.height <
            surface_capabilities.minImageExtent.height) {
            swap_chain_extent.height =
                    surface_capabilities.minImageExtent.height;
        }
        if (swap_chain_extent.width >
            surface_capabilities.maxImageExtent.width) {
            swap_chain_extent.width =
                    surface_capabilities.maxImageExtent.width;
        }
        if (swap_chain_extent.height >
            surface_capabilities.maxImageExtent.height) {
            swap_chain_extent.height =
                    surface_capabilities.maxImageExtent.height;
        }
        return swap_chain_extent;
    }

    // Most of the cases we define size of the swap_chain images equal to
    // current window's size
    return surface_capabilities.currentExtent;
}

VkImageUsageFlags TutorialBase::getSwapChainUsageFlags(
        VkSurfaceCapabilitiesKHR& surface_capabilities) {
    // Color attachment flag must always be supported
    // We can define other usage flags but we always need to check if they are
    // supported
    if (surface_capabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    Logging::error(LOG_TAG,
                   "VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT image usage is not "
                   "supported by the swap chain!\n",
                   "Supported swap chain's image usages include:",
                   (surface_capabilities.supportedUsageFlags &
                                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                            ? "\tVK_IMAGE_USAGE_TRANSFER_SRC\n"
                            : ""),
                   (surface_capabilities.supportedUsageFlags &
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT
                            ? "\tVK_IMAGE_USAGE_TRANSFER_DST\n"
                            : ""),
                   (surface_capabilities.supportedUsageFlags &
                                    VK_IMAGE_USAGE_SAMPLED_BIT
                            ? "\tVK_IMAGE_USAGE_SAMPLED\n"
                            : ""),
                   (surface_capabilities.supportedUsageFlags &
                                    VK_IMAGE_USAGE_STORAGE_BIT
                            ? "\tVK_IMAGE_USAGE_STORAGE\n"
                            : ""),
                   (surface_capabilities.supportedUsageFlags &
                                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                            ? "\tVK_IMAGE_USAGE_COLOR_ATTACHMENT\n"
                            : ""),
                   (surface_capabilities.supportedUsageFlags &
                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                            ? "\tVK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT\n"
                            : ""),
                   (surface_capabilities.supportedUsageFlags &
                                    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
                            ? "\tVK_IMAGE_USAGE_TRANSIENT_ATTACHMENT\n"
                            : ""),
                   (surface_capabilities.supportedUsageFlags &
                                    VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
                            ? "\tVK_IMAGE_USAGE_INPUT_ATTACHMENT"
                            : ""));
    return static_cast<VkImageUsageFlags>(-1);
}

VkSurfaceTransformFlagBitsKHR TutorialBase::getSwapChainTransform(
        VkSurfaceCapabilitiesKHR& surface_capabilities) {
    // Sometimes images must be transformed before they are presented (i.e. due
    // to device's orientation being other than default orientation) If the
    // specified transform is other than current transform, presentation engine
    // will transform image during presentation operation; this operation may
    // hit performance on some platforms Here we don't want any transformations
    // to occur so if the identity transform is supported use it otherwise just
    // use the same transform as current transform
    if (surface_capabilities.supportedTransforms &
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        return surface_capabilities.currentTransform;
    }
}

VkPresentModeKHR TutorialBase::getSwapChainPresentMode(
        std::vector<VkPresentModeKHR>& present_modes) {
    // MAILBOX is the lowest latency V-Sync enabled mode (something like
    // triple-buffering) so use it if available
    for (VkPresentModeKHR& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }
    // IMMEDIATE mode allows us to display frames in a V-Sync independent
    // manner so it can introduce screen tearing But this mode is the best for
    // benchmarking purposes if we want to check the real number of FPS
    for (VkPresentModeKHR& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            return present_mode;
        }
    }
    // FIFO present mode is always available
    for (VkPresentModeKHR& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_FIFO_KHR) {
            return present_mode;
        }
    }
    Logging::error(LOG_TAG,
                   "FIFO present mode is not supported by the swap chain!");
    return static_cast<VkPresentModeKHR>(-1);
}

bool TutorialBase::checkValidationLayerSupport() const {
    static const std::vector<const char*> validation_layers = {
            "VK_LAYER_KHRONOS_validation"};

    std::uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> vk_layer_properties(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count,
                                       vk_layer_properties.data());
    Logging::error(LOG_TAG, "The vk_instance has the following properties:");
    Logging::error(LOG_TAG, vk_layer_properties);

    bool response = true;
    for (const char* layer_name : validation_layers) {
        std::vector<VkLayerProperties>::iterator layer_it = std::find_if(
                vk_layer_properties.begin(),
                vk_layer_properties.end(),
                [&layer_name](
                        const VkLayerProperties& vk_layer_property) -> bool {
                    return strcmp(layer_name, vk_layer_property.layerName) ==
                           0;
                });
        if (layer_it == vk_layer_properties.end()) {
            Logging::error(LOG_TAG,
                           "The following layer \"",
                           layer_name,
                           "\""
                           "could not be loaded!!!");
            response = false;
            break;
        }
    }

    return response;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
              VkDebugUtilsMessageTypeFlagsEXT /*message_type*/,
              const VkDebugUtilsMessengerCallbackDataEXT*
                      vk_debug_utils_messenger_callback_data_ext,
              void* /*p_user_data*/) {
    static LogTag debug_log_tag("DebugCallback");
    static std::atomic<bool> log_tag_created(false);
    if (!log_tag_created.load()) {
        Logging::addStdCerrLogger(debug_log_tag);
        log_tag_created.store(true);
    }

    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        Logging::error(debug_log_tag,
                       "validation layer:",
                       vk_debug_utils_messenger_callback_data_ext->pMessage);
    } else if (message_severity &
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        Logging::info(debug_log_tag,
                      "validation layer:",
                      vk_debug_utils_messenger_callback_data_ext->pMessage);
    } else if (message_severity &
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        Logging::warn(debug_log_tag,
                      "validation layer:",
                      vk_debug_utils_messenger_callback_data_ext->pMessage);
    }

    return VK_FALSE;
}

bool TutorialBase::setupDebugMessenger() {
    bool response = false;
    if (!m_enable_vk_debug.load()) {
        response = true;
    } else {
        Logging::error(LOG_TAG, "Setting up Vulkan debugger...");
        VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info_ext =
                {.sType =
                         VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                 .messageSeverity =
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                 .messageType =
                         VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                 .pfnUserCallback = debugCallback};

        PFN_vkCreateDebugUtilsMessengerEXT func =
                reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                        vkGetInstanceProcAddr(
                                m_vulkan_common_parameters.getVkInstance(),
                                "vkCreateDebugUtilsMessengerEXT"));

        VkResult vk_result = VK_SUCCESS;
        if (func != nullptr) {
            vk_result = func(
                    m_vulkan_common_parameters.getVkInstance(),
                    &vk_debug_utils_messenger_create_info_ext,
                    nullptr,
                    &m_vulkan_common_parameters.getVkDebugUtilsMessenger());
        } else {
            vk_result = VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        response = vk_result == VK_SUCCESS;
    }

    return response;
}

bool TutorialBase::destroyDebugMessenger() {
    bool response = false;
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_vulkan_common_parameters.getVkInstance(),
                                  "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        func(m_vulkan_common_parameters.getVkInstance(),
             m_vulkan_common_parameters.getVkDebugUtilsMessenger(),
             nullptr);
        response = true;
    }

    return response;
}

}  // namespace vulkan_graphix
