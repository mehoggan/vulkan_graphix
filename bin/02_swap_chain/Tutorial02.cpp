#include "Tutorial02.h"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <limits>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

VulkanTutorial02Parameters::VulkanTutorial02Parameters()
        : m_vk_instance(VK_NULL_HANDLE)
        , m_vk_physical_device(VK_NULL_HANDLE)
        , m_vk_device(VK_NULL_HANDLE)
        , m_graphics_vk_queue(VK_NULL_HANDLE)
        , m_present_vk_queue(VK_NULL_HANDLE)
        , m_graphics_queue_family_index(0)
        , m_present_queue_family_index(0)
        , m_presentation_vk_surface_khr(VK_NULL_HANDLE)
        , m_vk_swapchain_khr(VK_NULL_HANDLE)
        , m_present_queue_vk_command_buffers(0)
        , m_present_queue_vk_command_pool(VK_NULL_HANDLE)
        , m_image_available_vk_semaphore(VK_NULL_HANDLE)
        , m_rendering_finished_semaphores({})
        , m_vk_fence(VK_NULL_HANDLE)
        , m_vk_debug_utils_messenger(VK_NULL_HANDLE) {}

const VkInstance& VulkanTutorial02Parameters::getVkInstance() const {
    return m_vk_instance;
}
VkInstance& VulkanTutorial02Parameters::getVkInstance() {
    return m_vk_instance;
}
void VulkanTutorial02Parameters::setVkInstance(const VkInstance& vk_instance) {
    m_vk_instance = vk_instance;
}

const VkPhysicalDevice& VulkanTutorial02Parameters::getVkPhysicalDevice()
        const {
    return m_vk_physical_device;
}
void VulkanTutorial02Parameters::setVkPhysicalDevice(
        const VkPhysicalDevice& vk_physical_device) {
    m_vk_physical_device = vk_physical_device;
}

const VkDevice& VulkanTutorial02Parameters::getVkDevice() const {
    return m_vk_device;
}
VkDevice& VulkanTutorial02Parameters::getVkDevice() { return m_vk_device; }
void VulkanTutorial02Parameters::setVkDevice(const VkDevice& vk_device) {
    m_vk_device = vk_device;
}

const VkQueue& VulkanTutorial02Parameters::getGraphicsVkQueue() const {
    return m_graphics_vk_queue;
}
VkQueue& VulkanTutorial02Parameters::getGraphicsVkQueue() {
    return m_graphics_vk_queue;
}
void VulkanTutorial02Parameters::setGraphicsVkQueue(
        const VkQueue& graphics_vk_queue) {
    m_graphics_vk_queue = graphics_vk_queue;
}

const VkQueue& VulkanTutorial02Parameters::getPresentVkQueue() const {
    return m_present_vk_queue;
}
VkQueue& VulkanTutorial02Parameters::getPresentVkQueue() {
    return m_present_vk_queue;
}
void VulkanTutorial02Parameters::setPresentVkQueue(
        const VkQueue& present_vk_queue) {
    m_present_vk_queue = present_vk_queue;
}

std::uint32_t VulkanTutorial02Parameters::getGraphicsQueueFamilyIndex() const {
    return m_graphics_queue_family_index;
}
void VulkanTutorial02Parameters::setGraphicsQueueFamilyIndex(
        const std::uint32_t graphics_queue_family_index) {
    m_graphics_queue_family_index = graphics_queue_family_index;
}

std::uint32_t VulkanTutorial02Parameters::getPresentQueueFamilyIndex() const {
    return m_present_queue_family_index;
}
void VulkanTutorial02Parameters::setPresentQueueFamilyIndex(
        const std::uint32_t graphics_queue_family_index) {
    m_present_queue_family_index = graphics_queue_family_index;
}

const VkSurfaceKHR& VulkanTutorial02Parameters::getPresentVkSurfaceKHR()
        const {
    return m_presentation_vk_surface_khr;
}
VkSurfaceKHR& VulkanTutorial02Parameters::getPresentVkSurfaceKHR() {
    return m_presentation_vk_surface_khr;
}
void VulkanTutorial02Parameters::setPresentVkSurfaceKHR(
        const VkSurfaceKHR& presentation_vk_surface_khr) {
    m_presentation_vk_surface_khr = presentation_vk_surface_khr;
}

const VkSwapchainKHR& VulkanTutorial02Parameters::getVkSwapchainKHR() const {
    return m_vk_swapchain_khr;
}
VkSwapchainKHR& VulkanTutorial02Parameters::getVkSwapchainKHR() {
    return m_vk_swapchain_khr;
}
void VulkanTutorial02Parameters::setVkSwapchainKHR(
        const VkSwapchainKHR& vk_swapchain_khr) {
    m_vk_swapchain_khr = vk_swapchain_khr;
}

const std::vector<VkCommandBuffer>&
VulkanTutorial02Parameters::getPresentQueueVkCommandBuffers() const {
    return m_present_queue_vk_command_buffers;
}
std::vector<VkCommandBuffer>&
VulkanTutorial02Parameters::getPresentQueueVkCommandBuffers() {
    return m_present_queue_vk_command_buffers;
}
void VulkanTutorial02Parameters::setPresentQueueVkCommandBuffers(
        const std::vector<VkCommandBuffer>& present_queue_vk_command_buffers) {
    std::copy(present_queue_vk_command_buffers.begin(),
              present_queue_vk_command_buffers.end(),
              std::back_inserter(m_present_queue_vk_command_buffers));
}

const VkCommandPool& VulkanTutorial02Parameters::getPresentQueueVkCommandPool()
        const {
    return m_present_queue_vk_command_pool;
}
VkCommandPool& VulkanTutorial02Parameters::getPresentQueueVkCommandPool() {
    return m_present_queue_vk_command_pool;
}
void VulkanTutorial02Parameters::setPresentQueueVkCommandPool(
        const VkCommandPool& present_queue_vk_command_pool) {
    m_present_queue_vk_command_pool = present_queue_vk_command_pool;
}

const VkSemaphore& VulkanTutorial02Parameters::getImageAvailableVkSemaphore()
        const {
    return m_image_available_vk_semaphore;
}
VkSemaphore& VulkanTutorial02Parameters::getImageAvailableVkSemaphore() {
    return m_image_available_vk_semaphore;
}
void VulkanTutorial02Parameters::setImageAvailableVkSemaphore(
        const VkSemaphore& image_available_vk_semaphore) {
    m_image_available_vk_semaphore = image_available_vk_semaphore;
}

const std::vector<VkSemaphore>&
VulkanTutorial02Parameters::getRenderingFinishedSemaphores() const {
    return m_rendering_finished_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial02Parameters::getRenderingFinishedSemaphores() {
    return m_rendering_finished_semaphores;
}
void VulkanTutorial02Parameters::setRenderingFinishedSemaphores(
        const std::vector<VkSemaphore>& rendering_finished_semaphores) {
    m_rendering_finished_semaphores = rendering_finished_semaphores;
}

const VkFence& VulkanTutorial02Parameters::getVkFence() const {
    return m_vk_fence;
}
VkFence& VulkanTutorial02Parameters::getVkFence() { return m_vk_fence; }
void VulkanTutorial02Parameters::setVkFence(const VkFence& vk_fence) {
    m_vk_fence = vk_fence;
}

const VkDebugUtilsMessengerEXT&
VulkanTutorial02Parameters::getVkDebugUtilsMessenger() const {
    return m_vk_debug_utils_messenger;
}
VkDebugUtilsMessengerEXT&
VulkanTutorial02Parameters::getVkDebugUtilsMessenger() {
    return m_vk_debug_utils_messenger;
}
void VulkanTutorial02Parameters::setVkDebugUtilsMessenger(
        const VkDebugUtilsMessengerEXT& vk_debug_utils_messenger) {
    m_vk_debug_utils_messenger = vk_debug_utils_messenger;
}

Tutorial02::Tutorial02()
        : LoggedClass<Tutorial02>(*this), m_vulkan_library() {}

Tutorial02::~Tutorial02() {
    clear();

    if (m_vulkan_tutorial02_parameters.getVkDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vulkan_tutorial02_parameters.getVkDevice());

        if (m_vulkan_tutorial02_parameters.getVkDebugUtilsMessenger() !=
            VK_NULL_HANDLE) {
            if (!destroyDebugMessenger()) {
                Logging::error(
                        LOG_TAG,
                        "Failed to destroy VkDebugUtilsMessengerExt!!!");
            }
        }

        if (m_vulkan_tutorial02_parameters.getImageAvailableVkSemaphore() !=
            VK_NULL_HANDLE) {
            vkDestroySemaphore(m_vulkan_tutorial02_parameters.getVkDevice(),
                               m_vulkan_tutorial02_parameters
                                       .getImageAvailableVkSemaphore(),
                               nullptr);
        }
        std::vector<VkSemaphore>& rendering_finished_semaphores =
                m_vulkan_tutorial02_parameters
                        .getRenderingFinishedSemaphores();
        for (std::size_t i = 0; i < rendering_finished_semaphores.size();
             ++i) {
            if (rendering_finished_semaphores[i] != VK_NULL_HANDLE) {
                vkDestroySemaphore(
                        m_vulkan_tutorial02_parameters.getVkDevice(),
                        rendering_finished_semaphores[i],
                        nullptr);
            }
        }
        rendering_finished_semaphores.clear();

        if (m_vulkan_tutorial02_parameters.getVkFence() != VK_NULL_HANDLE) {
            vkDestroyFence(m_vulkan_tutorial02_parameters.getVkDevice(),
                           m_vulkan_tutorial02_parameters.getVkFence(),
                           nullptr);
        }
        if (m_vulkan_tutorial02_parameters.getVkSwapchainKHR() !=
            VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(
                    m_vulkan_tutorial02_parameters.getVkDevice(),
                    m_vulkan_tutorial02_parameters.getVkSwapchainKHR(),
                    nullptr);
        }
        vkDestroyDevice(m_vulkan_tutorial02_parameters.getVkDevice(), nullptr);
    }

    if (m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR() !=
        VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(
                m_vulkan_tutorial02_parameters.getVkInstance(),
                m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR(),
                nullptr);
    }

    if (m_vulkan_tutorial02_parameters.getVkInstance() != VK_NULL_HANDLE) {
        vkDestroyInstance(m_vulkan_tutorial02_parameters.getVkInstance(),
                          nullptr);
    }

    if (m_vulkan_library) {
        dlclose(m_vulkan_library);
    }
}

bool Tutorial02::prepareVulkan(os::WindowParameters parameters) {
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
    Logging::info(LOG_TAG, "createSemaphores()");
    return createSemaphores();
}

bool Tutorial02::createSwapChain() {
    ProjectBase::m_can_render = false;

    if (m_vulkan_tutorial02_parameters.getVkDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vulkan_tutorial02_parameters.getVkDevice());
    }

    VkSurfaceCapabilitiesKHR surface_capabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                m_vulkan_tutorial02_parameters.getVkPhysicalDevice(),
                m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR(),
                &surface_capabilities) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Could not check presentation surface capabilities!");
        return false;
    }

    std::uint32_t formats_count;
    if ((vkGetPhysicalDeviceSurfaceFormatsKHR(
                 m_vulkan_tutorial02_parameters.getVkPhysicalDevice(),
                 m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR(),
                 &formats_count,
                 nullptr) != VK_SUCCESS) ||
        (formats_count == 0)) {
        Logging::error(LOG_TAG,
                       "Error occurred during presentation surface formats "
                       "enumeration!");
        return false;
    }

    std::vector<VkSurfaceFormatKHR> surface_formats(formats_count);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_vulkan_tutorial02_parameters.getVkPhysicalDevice(),
                m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR(),
                &formats_count,
                surface_formats.data()) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Error occurred during presentation surface formats "
                       "enumeration!");
        return false;
    }

    std::uint32_t present_modes_count;
    if ((vkGetPhysicalDeviceSurfacePresentModesKHR(
                 m_vulkan_tutorial02_parameters.getVkPhysicalDevice(),
                 m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR(),
                 &present_modes_count,
                 nullptr) != VK_SUCCESS) ||
        (present_modes_count == 0)) {
        Logging::error(
                LOG_TAG,
                "Error occurred during presentation surface present modes "
                "enumeration!");
        return false;
    }

    std::vector<VkPresentModeKHR> present_modes(present_modes_count);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_vulkan_tutorial02_parameters.getVkPhysicalDevice(),
                m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR(),
                &present_modes_count,
                present_modes.data()) != VK_SUCCESS) {
        Logging::error(
                LOG_TAG,
                "Error occurred during presentation surface present modes "
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
            m_vulkan_tutorial02_parameters.getVkSwapchainKHR();

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
            .surface = m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR(),
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
                m_vulkan_tutorial02_parameters.getVkDevice(),
                &swap_chain_create_info,
                nullptr,
                &m_vulkan_tutorial02_parameters.getVkSwapchainKHR()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create swap chain!");
        return false;
    }
    if (old_swap_chain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_vulkan_tutorial02_parameters.getVkDevice(),
                              old_swap_chain,
                              nullptr);
    }

    m_can_render = true;

    return true;
}

bool Tutorial02::onWindowSizeChanged() {
    clear();

    if (!createSwapChain()) {
        return false;
    }
    if (!createCommandBuffers()) {
        return false;
    }
    return true;
}

bool Tutorial02::createCommandBuffers() {
    VkCommandPoolCreateInfo cmd_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = m_vulkan_tutorial02_parameters
                                        .getPresentQueueFamilyIndex()};

    if (vkCreateCommandPool(m_vulkan_tutorial02_parameters.getVkDevice(),
                            &cmd_pool_create_info,
                            nullptr,
                            &m_vulkan_tutorial02_parameters
                                     .getPresentQueueVkCommandPool()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create a command pool!");
        return false;
    }

    std::uint32_t image_count = 0;
    if ((vkGetSwapchainImagesKHR(
                 m_vulkan_tutorial02_parameters.getVkDevice(),
                 m_vulkan_tutorial02_parameters.getVkSwapchainKHR(),
                 &image_count,
                 nullptr) != VK_SUCCESS) ||
        (image_count == 0)) {
        Logging::error(LOG_TAG,
                       "Could not get the number of swap chain images!");
        return false;
    }

    m_vulkan_tutorial02_parameters.getPresentQueueVkCommandBuffers().resize(
            image_count);

    // A "rendering finished" semaphore must be indexed by the acquired
    // swapchain image, not reused as a single semaphore every frame: the
    // fence created in createSemaphores() only guarantees the GPU has
    // finished this submit, not that the presentation engine has finished
    // consuming the *previous* signal on this same semaphore object, since
    // present completion isn't tracked by any fence here. See
    // https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
    // Guarded so onWindowSizeChanged() (which calls this again) doesn't
    // leak/recreate these; like every other tutorial, they aren't
    // resize-aware.
    std::vector<VkSemaphore>& rendering_finished_semaphores =
            m_vulkan_tutorial02_parameters.getRenderingFinishedSemaphores();
    if (rendering_finished_semaphores.empty()) {
        VkSemaphoreCreateInfo semaphore_create_info = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0};
        rendering_finished_semaphores.assign(image_count, VK_NULL_HANDLE);
        for (std::uint32_t i = 0; i < image_count; ++i) {
            if (vkCreateSemaphore(m_vulkan_tutorial02_parameters.getVkDevice(),
                                  &semaphore_create_info,
                                  nullptr,
                                  &rendering_finished_semaphores[i]) !=
                VK_SUCCESS) {
                Logging::error(LOG_TAG, "Could not create semaphores!");
                return false;
            }
        }
    }

    VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = m_vulkan_tutorial02_parameters
                                   .getPresentQueueVkCommandPool(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = image_count};
    if (vkAllocateCommandBuffers(m_vulkan_tutorial02_parameters.getVkDevice(),
                                 &cmd_buffer_allocate_info,
                                 m_vulkan_tutorial02_parameters
                                         .getPresentQueueVkCommandBuffers()
                                         .data()) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate command buffers!");
        return false;
    }

    if (!recordCommandBuffers()) {
        Logging::error(LOG_TAG, "Could not record command buffers!");
        return false;
    }
    return true;
}

bool Tutorial02::draw() {
    if (vkWaitForFences(m_vulkan_tutorial02_parameters.getVkDevice(),
                        1,
                        &m_vulkan_tutorial02_parameters.getVkFence(),
                        VK_FALSE,
                        1000000000) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Waiting for fence takes too long!");
        return false;
    }
    vkResetFences(m_vulkan_tutorial02_parameters.getVkDevice(),
                  1,
                  &m_vulkan_tutorial02_parameters.getVkFence());

    std::uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(
            m_vulkan_tutorial02_parameters.getVkDevice(),
            m_vulkan_tutorial02_parameters.getVkSwapchainKHR(),
            UINT64_MAX,
            m_vulkan_tutorial02_parameters.getImageAvailableVkSemaphore(),
            VK_NULL_HANDLE,
            &image_index);
    switch (result) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            return onWindowSizeChanged();
        default:
            Logging::error(
                    LOG_TAG,
                    "Problem occurred during swap chain image acquisition!");
            return false;
    }

    VkSemaphore& rendering_finished_semaphore =
            m_vulkan_tutorial02_parameters
                    .getRenderingFinishedSemaphores()[image_index];

    VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &m_vulkan_tutorial02_parameters
                                        .getImageAvailableVkSemaphore(),
            .pWaitDstStageMask = &wait_dst_stage_mask,
            .commandBufferCount = 1,
            .pCommandBuffers =
                    &m_vulkan_tutorial02_parameters
                             .getPresentQueueVkCommandBuffers()[image_index],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &rendering_finished_semaphore};

    if (vkQueueSubmit(m_vulkan_tutorial02_parameters.getPresentVkQueue(),
                      1,
                      &submit_info,
                      m_vulkan_tutorial02_parameters.getVkFence()) !=
        VK_SUCCESS) {
        return false;
    }

    VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &rendering_finished_semaphore,
            .swapchainCount = 1,
            .pSwapchains = &m_vulkan_tutorial02_parameters.getVkSwapchainKHR(),
            .pImageIndices = &image_index,
            .pResults = nullptr};
    result = vkQueuePresentKHR(
            m_vulkan_tutorial02_parameters.getPresentVkQueue(), &present_info);

    switch (result) {
        case VK_SUCCESS:
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
        case VK_SUBOPTIMAL_KHR:
            return onWindowSizeChanged();
        default:
            Logging::error(LOG_TAG,
                           "Problem occurred during image presentation!");
            return false;
    }

    return true;
}

bool Tutorial02::loadVulkanLibrary() {
    m_vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);

    if (m_vulkan_library == nullptr) {
        Logging::error(LOG_TAG, "Could not load Vulkan library!");
        return false;
    }
    return true;
}

bool Tutorial02::loadExportedEntryPoints() {
#define LoadProcAddress dlsym

#define VK_EXPORTED_FUNCTION(fun)                                         \
    if (!((fun) = (PFN_##fun)LoadProcAddress(m_vulkan_library, #fun))) {  \
        Logging::error(                                                   \
                LOG_TAG, "Could not load exported function:", #fun, "!"); \
        return false;                                                     \
    }

#include "vulkan_graphix/ListOfFunctions.inl"

    return true;
}

bool Tutorial02::loadGlobalLevelEntryPoints() {
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

bool Tutorial02::createInstance() {
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

    for (size_t i = 0; i < extensions.size(); ++i) {
        if (!checkExtensionAvailability(extensions[i], available_extensions)) {
            Logging::error(LOG_TAG,
                           "Could not find instance extension named \"",
                           extensions[i],
                           "\"!");
            return false;
        }
    }

    std::uint32_t vk_version = VK_MAKE_VERSION(1, 3, 0);
    std::uint32_t engine_version = VK_MAKE_VERSION(1, 0, 0);

    VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Intel Vulkan tutorial",
            .applicationVersion = vk_version,
            .pEngineName = "Vulkan Tutorial by Intel",
            .engineVersion = engine_version,
            .apiVersion = engine_version};

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
                         &m_vulkan_tutorial02_parameters.getVkInstance()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create Vulkan instance!");
        return false;
    }

    if (!setupDebugMessenger()) {
        Logging::error(LOG_TAG, "Failed to setup debug messenger!!!");
    }
    return true;
}

bool Tutorial02::loadInstanceLevelEntryPoints() {
#define VK_INSTANCE_LEVEL_FUNCTION(fun)                                     \
    if (!((fun) = (PFN_##fun)vkGetInstanceProcAddr(                         \
                  m_vulkan_tutorial02_parameters.getVkInstance(), #fun))) { \
        Logging::error(LOG_TAG,                                             \
                       "Could not load instance level function:",           \
                       #fun,                                                \
                       "!");                                                \
        return false;                                                       \
    }

#include "vulkan_graphix/ListOfFunctions.inl"

    return true;
}

bool Tutorial02::createPresentationSurface() {
    VkXlibSurfaceCreateInfoKHR surface_create_info = {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .dpy = m_window_parameters.getDisplayPtr(),
            .window = m_window_parameters.getWindowHandle()};
    if (vkCreateXlibSurfaceKHR(
                m_vulkan_tutorial02_parameters.getVkInstance(),
                &surface_create_info,
                nullptr,
                &m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR()) ==
        VK_SUCCESS) {
        return true;
    }

    Logging::error(LOG_TAG, "Could not create presentation surface!");
    return false;
}

bool Tutorial02::createDevice() {
    std::uint32_t num_devices = 0;
    if ((vkEnumeratePhysicalDevices(
                 m_vulkan_tutorial02_parameters.getVkInstance(),
                 &num_devices,
                 nullptr) != VK_SUCCESS) ||
        (num_devices == 0)) {
        Logging::error(LOG_TAG,
                       "Error occurred during physical devices enumeration!");
        return false;
    }

    std::vector<VkPhysicalDevice> physical_devices(num_devices);
    if (vkEnumeratePhysicalDevices(
                m_vulkan_tutorial02_parameters.getVkInstance(),
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
            m_vulkan_tutorial02_parameters.setVkPhysicalDevice(
                    physical_devices[i]);
            break;
        }
    }
    if (m_vulkan_tutorial02_parameters.getVkPhysicalDevice() ==
        VK_NULL_HANDLE) {
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

    VkDeviceCreateInfo device_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,  // VkDeviceCreateFlags                flags
            .queueCreateInfoCount =
                    static_cast<std::uint32_t>(queue_create_infos.size()),
            .pQueueCreateInfos = queue_create_infos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount =
                    static_cast<std::uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = nullptr};

    if (vkCreateDevice(m_vulkan_tutorial02_parameters.getVkPhysicalDevice(),
                       &device_create_info,
                       nullptr,
                       &m_vulkan_tutorial02_parameters.getVkDevice()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create Vulkan device!");
        return false;
    }

    m_vulkan_tutorial02_parameters.setGraphicsQueueFamilyIndex(
            selected_graphics_queue_family_index);
    m_vulkan_tutorial02_parameters.setPresentQueueFamilyIndex(
            selected_present_queue_family_index);
    return true;
}

bool Tutorial02::checkPhysicalDeviceProperties(
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
                           "Physical device ",
                           physical_device,
                           " doesn't support extension named \"",
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
                       "Physical device ",
                       physical_device,
                       " doesn't support required parameters!");
        return false;
    }

    std::uint32_t queue_families_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device, &queue_families_count, nullptr);
    if (queue_families_count == 0) {
        Logging::error(LOG_TAG,
                       "Physical device ",
                       physical_device,
                       " doesn't have any queue families!");
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
                m_vulkan_tutorial02_parameters.getPresentVkSurfaceKHR(),
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
        Logging::error(
                LOG_TAG,
                "Could not find queue family with required properties on "
                "physical device ",
                physical_device,
                "!");
        return false;
    }

    selected_graphics_queue_family_index = graphics_queue_family_index;
    selected_present_queue_family_index = present_queue_family_index;
    return true;
}

bool Tutorial02::loadDeviceLevelEntryPoints() {
#define VK_DEVICE_LEVEL_FUNCTION(fun)                                     \
    if (!((fun) = (PFN_##fun)vkGetDeviceProcAddr(                         \
                  m_vulkan_tutorial02_parameters.getVkDevice(), #fun))) { \
        Logging::error(LOG_TAG,                                           \
                       "Could not load device level function: ",          \
                       #fun,                                              \
                       "!");                                              \
        return false;                                                     \
    }

#include "vulkan_graphix/ListOfFunctions.inl"

    return true;
}

bool Tutorial02::getDeviceQueue() {
    vkGetDeviceQueue(
            m_vulkan_tutorial02_parameters.getVkDevice(),
            m_vulkan_tutorial02_parameters.getGraphicsQueueFamilyIndex(),
            0,
            &m_vulkan_tutorial02_parameters.getGraphicsVkQueue());
    vkGetDeviceQueue(
            m_vulkan_tutorial02_parameters.getVkDevice(),
            m_vulkan_tutorial02_parameters.getPresentQueueFamilyIndex(),
            0,
            &m_vulkan_tutorial02_parameters.getPresentVkQueue());
    return true;
}

bool Tutorial02::createSemaphores() {
    VkSemaphoreCreateInfo semaphore_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0};

    // The per-swapchain-image "rendering finished" semaphores are created
    // later, in createCommandBuffers(): this runs from prepareVulkan(),
    // before the swapchain (and therefore its image count) exists.
    if (vkCreateSemaphore(m_vulkan_tutorial02_parameters.getVkDevice(),
                          &semaphore_create_info,
                          nullptr,
                          &m_vulkan_tutorial02_parameters
                                   .getImageAvailableVkSemaphore()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create semaphores!");
        return false;
    }

    VkFenceCreateInfo fence_create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    if (vkCreateFence(m_vulkan_tutorial02_parameters.getVkDevice(),
                      &fence_create_info,
                      nullptr,
                      &m_vulkan_tutorial02_parameters.getVkFence()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create a fence!");
        return false;
    }

    return true;
}

bool Tutorial02::recordCommandBuffers() {
    std::uint32_t image_count = static_cast<std::uint32_t>(
            m_vulkan_tutorial02_parameters.getPresentQueueVkCommandBuffers()
                    .size());

    std::vector<VkImage> swap_chain_images(image_count);
    if (vkGetSwapchainImagesKHR(
                m_vulkan_tutorial02_parameters.getVkDevice(),
                m_vulkan_tutorial02_parameters.getVkSwapchainKHR(),
                &image_count,
                swap_chain_images.data()) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not get swap chain images!");
        return false;
    }

    VkCommandBufferBeginInfo cmd_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            .pInheritanceInfo = nullptr};

    VkClearColorValue clear_color = {.float32 = {1.0f, 0.8f, 0.4f, 0.0f}};

    VkImageSubresourceRange image_subresource_range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1};

    for (std::uint32_t i = 0; i < image_count; ++i) {
        VkImageMemoryBarrier barrier_from_present_to_clear = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = swap_chain_images[i],
                .subresourceRange = image_subresource_range};

        VkImageMemoryBarrier barrier_from_clear_to_present = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = swap_chain_images[i],
                .subresourceRange = image_subresource_range};

        vkBeginCommandBuffer(m_vulkan_tutorial02_parameters
                                     .getPresentQueueVkCommandBuffers()[i],
                             &cmd_buffer_begin_info);
        vkCmdPipelineBarrier(m_vulkan_tutorial02_parameters
                                     .getPresentQueueVkCommandBuffers()[i],
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier_from_present_to_clear);

        vkCmdClearColorImage(m_vulkan_tutorial02_parameters
                                     .getPresentQueueVkCommandBuffers()[i],
                             swap_chain_images[i],
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             &clear_color,
                             1,
                             &image_subresource_range);

        vkCmdPipelineBarrier(m_vulkan_tutorial02_parameters
                                     .getPresentQueueVkCommandBuffers()[i],
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier_from_clear_to_present);
        if (vkEndCommandBuffer(
                    m_vulkan_tutorial02_parameters
                            .getPresentQueueVkCommandBuffers()[i]) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not record command buffers!");
            return false;
        }
    }

    return true;
}

void Tutorial02::clear() {
    if (m_vulkan_tutorial02_parameters.getVkDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vulkan_tutorial02_parameters.getVkDevice());

        if ((!m_vulkan_tutorial02_parameters.getPresentQueueVkCommandBuffers()
                      .empty()) &&
            (m_vulkan_tutorial02_parameters
                     .getPresentQueueVkCommandBuffers()[0] !=
             VK_NULL_HANDLE)) {
            vkFreeCommandBuffers(
                    m_vulkan_tutorial02_parameters.getVkDevice(),
                    m_vulkan_tutorial02_parameters
                            .getPresentQueueVkCommandPool(),
                    static_cast<std::uint32_t>(
                            m_vulkan_tutorial02_parameters
                                    .getPresentQueueVkCommandBuffers()
                                    .size()),
                    m_vulkan_tutorial02_parameters
                            .getPresentQueueVkCommandBuffers()
                            .data());
            m_vulkan_tutorial02_parameters.getPresentQueueVkCommandBuffers()
                    .clear();
        }

        if (m_vulkan_tutorial02_parameters.getPresentQueueVkCommandPool() !=
            VK_NULL_HANDLE) {
            vkDestroyCommandPool(m_vulkan_tutorial02_parameters.getVkDevice(),
                                 m_vulkan_tutorial02_parameters
                                         .getPresentQueueVkCommandPool(),
                                 nullptr);
            m_vulkan_tutorial02_parameters.getPresentQueueVkCommandPool() =
                    VK_NULL_HANDLE;
        }
    }
}

bool Tutorial02::checkValidationLayerSupport() const {
    static const std::vector<const char*> validation_layers = {
            "VK_LAYER_KHRONOS_validation"};

    std::uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> vk_layer_properties(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count,
                                       vk_layer_properties.data());
    Logging::info(LOG_TAG, "The vk_instance has the following properties:");
    Logging::info(LOG_TAG, vk_layer_properties);

    bool response = true;
    for (const char* layer_name : validation_layers) {
        std::vector<VkLayerProperties>::iterator layer_it = std::find_if(
                vk_layer_properties.begin(),
                vk_layer_properties.end(),
                [&layer_name](const VkLayerProperties& vk_layer_property) {
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
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT /*message_severity*/,
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

    Logging::error(debug_log_tag,
                   "validation layer:",
                   vk_debug_utils_messenger_callback_data_ext->pMessage);

    return VK_FALSE;
}

bool Tutorial02::setupDebugMessenger() {
    bool response = false;
    if (!m_enable_vulkan_debug.load()) {
        response = true;
    } else {
        Logging::info(LOG_TAG, "Setting up Vulkan debugger...");
        VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info_ext =
                {.sType =
                         VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                 .messageSeverity =
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
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
                                m_vulkan_tutorial02_parameters.getVkInstance(),
                                "vkCreateDebugUtilsMessengerEXT"));

        VkResult vk_result = VK_SUCCESS;
        if (func != nullptr) {
            vk_result = func(m_vulkan_tutorial02_parameters.getVkInstance(),
                             &vk_debug_utils_messenger_create_info_ext,
                             nullptr,
                             &m_vulkan_tutorial02_parameters
                                      .getVkDebugUtilsMessenger());
        } else {
            vk_result = VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        response = vk_result == VK_SUCCESS;
    }

    return response;
}

bool Tutorial02::destroyDebugMessenger() {
    bool response = false;
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(
                    m_vulkan_tutorial02_parameters.getVkInstance(),
                    "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        func(m_vulkan_tutorial02_parameters.getVkInstance(),
             m_vulkan_tutorial02_parameters.getVkDebugUtilsMessenger(),
             nullptr);
        response = true;
    }

    return response;
}

bool Tutorial02::checkExtensionAvailability(
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

std::uint32_t Tutorial02::getSwapChainNumImages(
        VkSurfaceCapabilitiesKHR& surface_capabilities) {
    // Set of images defined in a swap chain may not always be available for
    // application to render to: One may be displayed and one may wait in a
    // queue to be presented If application wants to use more images at the
    // same time it must ask for more images
    std::uint32_t image_count = surface_capabilities.minImageCount + 1;
    if ((surface_capabilities.maxImageCount > 0) &&
        (image_count > surface_capabilities.maxImageCount)) {
        image_count = surface_capabilities.maxImageCount;
    }
    return image_count;
}

VkSurfaceFormatKHR Tutorial02::getSwapChainFormat(
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

VkExtent2D Tutorial02::getSwapChainExtent(
        VkSurfaceCapabilitiesKHR& surface_capabilities) {
    // Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within
    // defined confines
    if (surface_capabilities.currentExtent.width ==
        std::numeric_limits<std::uint32_t>::max()) {
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

VkImageUsageFlags Tutorial02::getSwapChainUsageFlags(
        VkSurfaceCapabilitiesKHR& surface_capabilities) {
    // Color attachment flag must always be supported
    // We can define other usage flags but we always need to check if they are
    // supported
    if (surface_capabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
               VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    Logging::error(
            LOG_TAG,
            "VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by "
            "the swap chain!\n",
            "Supported swap chain's image usages include:\n",
            (surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                     ? "    VK_IMAGE_USAGE_TRANSFER_SRC\n"
                     : ""),
            (surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_TRANSFER_DST_BIT
                     ? "    VK_IMAGE_USAGE_TRANSFER_DST\n"
                     : ""),
            (surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_SAMPLED_BIT
                     ? "    VK_IMAGE_USAGE_SAMPLED\n"
                     : ""),
            (surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_STORAGE_BIT
                     ? "    VK_IMAGE_USAGE_STORAGE\n"
                     : ""),
            (surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                     ? "    VK_IMAGE_USAGE_COLOR_ATTACHMENT\n"
                     : ""),
            (surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                     ? "    "
                       "VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT\n"
                     : ""),
            (surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
                     ? "    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT\n"
                     : ""),
            (surface_capabilities.supportedUsageFlags &
                             VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
                     ? "    VK_IMAGE_USAGE_INPUT_ATTACHMENT"
                     : ""));
    return static_cast<VkImageUsageFlags>(-1);
}

VkSurfaceTransformFlagBitsKHR Tutorial02::getSwapChainTransform(
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

VkPresentModeKHR Tutorial02::getSwapChainPresentMode(
        std::vector<VkPresentModeKHR>& present_modes) {
    // FIFO present mode is always available
    // MAILBOX is the lowest latency V-Sync enabled mode (something like
    // triple-buffering) so use it if available
    for (VkPresentModeKHR& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }
    for (VkPresentModeKHR& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_FIFO_KHR) {
            return present_mode;
        }
    }
    Logging::error(LOG_TAG,
                   "FIFO present mode is not supported by the swap chain!");
    return static_cast<VkPresentModeKHR>(-1);
}
}  // namespace vulkan_graphix
