// Split out from TutorialBaseFaultInjectionTest.cpp into its own process:
// this is the other test in the fault-injection group that ends up with a
// real, live VkDevice by the time its TestableTutorialBase destructs - see
// TutorialBaseFaultInjectionCommon.h's header comment for why that means
// it can't share a process with TutorialBaseFaultInjectionSwapChainTest.cpp
// (or with itself run more than once).

#include <cstring>

#include <gtest/gtest.h>

#include "vulkan_graphix/VulkanFunctions.h"

#include "IntegrationTestCommon.h"
#include "TutorialBaseFaultInjectionCommon.h"

using vulkan_graphix::test::bringUpThroughDevice;
using vulkan_graphix::test::TestableTutorialBase;

namespace {

PFN_vkGetDeviceProcAddr g_real_get_device_proc_addr = nullptr;
const char* g_poisoned_device_function_name = nullptr;

PFN_vkVoidFunction VKAPI_CALL fakeGetDeviceProcAddr(VkDevice device,
                                                    const char* name) {
    if ((g_poisoned_device_function_name != nullptr) &&
        (std::strcmp(name, g_poisoned_device_function_name) == 0)) {
        return nullptr;
    }
    return g_real_get_device_proc_addr(device, name);
}

}  // namespace

TEST(TutorialBaseFaultInjectionDeviceLevelTest,
     LoadDeviceLevelEntryPointsFailsWhenAFunctionIsMissing) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-device-level-entry-points"));

    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughDevice(tutorial, window.getParameters()));

    g_real_get_device_proc_addr = vulkan_graphix::vkGetDeviceProcAddr;
    g_poisoned_device_function_name = "vkGetDeviceQueue";
    vulkan_graphix::vkGetDeviceProcAddr = &fakeGetDeviceProcAddr;

    EXPECT_FALSE(tutorial.loadDeviceLevelEntryPoints());

    vulkan_graphix::vkGetDeviceProcAddr = g_real_get_device_proc_addr;
    g_poisoned_device_function_name = nullptr;
}
