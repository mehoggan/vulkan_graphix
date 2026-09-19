// TutorialBase.cpp's setup sequence (loadVulkanLibrary -> ... ->
// createSwapChain) is mostly `if (vkSomething(...) != VK_SUCCESS) { ...
// return false; }` - branches a real Vulkan device essentially never
// takes, so they're invisible to the integration tests. But every Vulkan
// call in this codebase actually goes through a mutable global function
// pointer (vulkan_graphix::vkCreateInstance, etc. - see VulkanFunctions.h/
// .cpp and ListOfFunctions.inl), populated at runtime via dlsym/
// vkGetInstanceProcAddr/vkGetDeviceProcAddr as part of this very setup
// sequence. That's a ready-made seam: run the real sequence up to some
// point (so the pointer we care about is already loaded with the real
// driver's entry point), swap it for a fake that returns a chosen
// failure VkResult, call the one step under test, assert it fails, then
// restore the real pointer before anything else touches it.
//
// Needs a live Vulkan driver/X11 display like the TutorialNNIntegration
// tests, since the "real sequence up to some point" part is, well, real.
//
// Not every call in the sequence goes through this seam: ListOfFunctions.inl
// only namespaces a function as a mutable vulkan_graphix::vkSomething
// pointer when it's listed under a VK_*_LEVEL_FUNCTION macro, and the WSI
// surface/swapchain entry points (vkCreateXlibSurfaceKHR,
// vkGetPhysicalDeviceSurfaceCapabilitiesKHR, vkCreateSwapchainKHR, etc.)
// are only listed inside `#if defined(USE_SWAPCHAIN_EXTENSIONS)` blocks -
// a macro this project never actually defines. Those calls resolve to the
// real symbols linked straight from libvulkan.so instead (confirmed via
// `nm -D --defined-only libvulkan_graphix.so`: no vulkan_graphix::-prefixed
// symbol exists for them), so createPresentationSurface()'s and
// createSwapChain()'s own Vulkan-call failure branches aren't reachable
// through this technique - only faulting via LD_PRELOAD interposition on
// libvulkan.so itself could reach those, which is out of scope here.
//
// This binary holds every fault test that never ends up with a real,
// live VkDevice by the time its TestableTutorialBase destructs (each one
// either fails before ever creating a device, or is faulting the very
// call that would create one) - see TutorialBaseFaultInjectionCommon.h's
// header comment for why tests that DO end up with a real device each
// need their own separate binary instead of living here too.

#include <cstring>

#include <gtest/gtest.h>

#include "vulkan_graphix/VulkanFunctions.h"

#include "IntegrationTestCommon.h"
#include "TutorialBaseFaultInjectionCommon.h"

using vulkan_graphix::test::bringUpThroughExportedEntryPoints;
using vulkan_graphix::test::bringUpThroughInstance;
using vulkan_graphix::test::bringUpThroughSurface;
using vulkan_graphix::test::TestableTutorialBase;

TEST(TutorialBaseFaultInjectionTest,
     CreateInstanceFailsWhenEnumerateExtensionPropertiesFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver)";
    }
    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughExportedEntryPoints(tutorial));
    ASSERT_TRUE(tutorial.loadGlobalLevelEntryPoints());

    auto real_fn = vulkan_graphix::vkEnumerateInstanceExtensionProperties;
    vulkan_graphix::vkEnumerateInstanceExtensionProperties =
            [](const char*, std::uint32_t*, VkExtensionProperties*)
            -> VkResult { return VK_ERROR_INITIALIZATION_FAILED; };

    EXPECT_FALSE(tutorial.createInstance());

    vulkan_graphix::vkEnumerateInstanceExtensionProperties = real_fn;
}

TEST(TutorialBaseFaultInjectionTest,
     CreateInstanceFailsWhenCreateInstanceFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver)";
    }
    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughExportedEntryPoints(tutorial));
    ASSERT_TRUE(tutorial.loadGlobalLevelEntryPoints());

    auto real_fn = vulkan_graphix::vkCreateInstance;
    vulkan_graphix::vkCreateInstance = [](const VkInstanceCreateInfo*,
                                          const VkAllocationCallbacks*,
                                          VkInstance*) -> VkResult {
        return VK_ERROR_INITIALIZATION_FAILED;
    };

    EXPECT_FALSE(tutorial.createInstance());

    vulkan_graphix::vkCreateInstance = real_fn;
}

TEST(TutorialBaseFaultInjectionTest, CreateDeviceFailsWhenCreateDeviceFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-device"));

    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughSurface(tutorial, window.getParameters()));

    auto real_fn = vulkan_graphix::vkCreateDevice;
    vulkan_graphix::vkCreateDevice = [](VkPhysicalDevice,
                                        const VkDeviceCreateInfo*,
                                        const VkAllocationCallbacks*,
                                        VkDevice*) -> VkResult {
        return VK_ERROR_INITIALIZATION_FAILED;
    };

    EXPECT_FALSE(tutorial.createDevice());

    vulkan_graphix::vkCreateDevice = real_fn;
}

TEST(TutorialBaseFaultInjectionTest,
     CreateDeviceFailsWhenEnumeratePhysicalDevicesFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-enum-devices"));

    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughSurface(tutorial, window.getParameters()));

    auto real_fn = vulkan_graphix::vkEnumeratePhysicalDevices;
    vulkan_graphix::vkEnumeratePhysicalDevices =
            [](VkInstance, std::uint32_t*, VkPhysicalDevice*) -> VkResult {
        return VK_ERROR_INITIALIZATION_FAILED;
    };

    EXPECT_FALSE(tutorial.createDevice());

    vulkan_graphix::vkEnumeratePhysicalDevices = real_fn;
}

TEST(TutorialBaseFaultInjectionTest,
     CreateDeviceFailsWhenNoPhysicalDeviceHasTheRequiredExtensions) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-no-suitable-device"));

    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughSurface(tutorial, window.getParameters()));

    // checkPhysicalDeviceProperties() rejects a device whose extension
    // enumeration fails, so faulting this makes every enumerated device
    // look unsuitable - the same "Could not select physical device" path
    // as a real device lacking VK_KHR_swapchain.
    auto real_fn = vulkan_graphix::vkEnumerateDeviceExtensionProperties;
    vulkan_graphix::vkEnumerateDeviceExtensionProperties =
            [](VkPhysicalDevice,
               const char*,
               std::uint32_t*,
               VkExtensionProperties*) -> VkResult {
        return VK_ERROR_INITIALIZATION_FAILED;
    };

    EXPECT_FALSE(tutorial.createDevice());

    vulkan_graphix::vkEnumerateDeviceExtensionProperties = real_fn;
}

TEST(TutorialBaseFaultInjectionTest,
     CheckPhysicalDevicePropertiesFailsWhenEnumerateExtensionsFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-check-device-props"));

    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughSurface(tutorial, window.getParameters()));

    std::uint32_t device_count = 0;
    ASSERT_EQ(vulkan_graphix::vkEnumeratePhysicalDevices(
                      tutorial.m_vulkan_common_parameters.getVkInstance(),
                      &device_count,
                      nullptr),
              VK_SUCCESS);
    ASSERT_GT(device_count, 0u);
    std::vector<VkPhysicalDevice> physical_devices(device_count);
    ASSERT_EQ(vulkan_graphix::vkEnumeratePhysicalDevices(
                      tutorial.m_vulkan_common_parameters.getVkInstance(),
                      &device_count,
                      physical_devices.data()),
              VK_SUCCESS);

    auto real_fn = vulkan_graphix::vkEnumerateDeviceExtensionProperties;
    vulkan_graphix::vkEnumerateDeviceExtensionProperties =
            [](VkPhysicalDevice,
               const char*,
               std::uint32_t*,
               VkExtensionProperties*) -> VkResult {
        return VK_ERROR_INITIALIZATION_FAILED;
    };

    std::uint32_t graphics_index = UINT32_MAX;
    std::uint32_t present_index = UINT32_MAX;
    EXPECT_FALSE(tutorial.checkPhysicalDeviceProperties(
            physical_devices[0], graphics_index, present_index));

    vulkan_graphix::vkEnumerateDeviceExtensionProperties = real_fn;
}

namespace {

PFN_vkGetInstanceProcAddr g_real_get_instance_proc_addr = nullptr;
const char* g_poisoned_instance_function_name = nullptr;

PFN_vkVoidFunction VKAPI_CALL fakeGetInstanceProcAddr(VkInstance instance,
                                                      const char* name) {
    if ((g_poisoned_instance_function_name != nullptr) &&
        (std::strcmp(name, g_poisoned_instance_function_name) == 0)) {
        return nullptr;
    }
    return g_real_get_instance_proc_addr(instance, name);
}

}  // namespace

TEST(TutorialBaseFaultInjectionTest,
     LoadGlobalLevelEntryPointsFailsWhenAFunctionIsMissing) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver)";
    }
    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughExportedEntryPoints(tutorial));

    g_real_get_instance_proc_addr = vulkan_graphix::vkGetInstanceProcAddr;
    g_poisoned_instance_function_name = "vkCreateInstance";
    vulkan_graphix::vkGetInstanceProcAddr = &fakeGetInstanceProcAddr;

    EXPECT_FALSE(tutorial.loadGlobalLevelEntryPoints());

    vulkan_graphix::vkGetInstanceProcAddr = g_real_get_instance_proc_addr;
    g_poisoned_instance_function_name = nullptr;
}

TEST(TutorialBaseFaultInjectionTest,
     LoadInstanceLevelEntryPointsFailsWhenAFunctionIsMissing) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver)";
    }
    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughInstance(tutorial));

    g_real_get_instance_proc_addr = vulkan_graphix::vkGetInstanceProcAddr;
    g_poisoned_instance_function_name = "vkCreateDevice";
    vulkan_graphix::vkGetInstanceProcAddr = &fakeGetInstanceProcAddr;

    EXPECT_FALSE(tutorial.loadInstanceLevelEntryPoints());

    vulkan_graphix::vkGetInstanceProcAddr = g_real_get_instance_proc_addr;
    g_poisoned_instance_function_name = nullptr;
}
