// Shared TestableTutorialBase/bring-up helpers for the
// TutorialBaseFaultInjection*Test.cpp files - see
// TutorialBaseFaultInjectionTest.cpp's header comment for the technique
// these all share (swapping a mutable vulkan_graphix::vkSomething function
// pointer for a fake, calling the one step under test, restoring it).
//
// Split into three binaries, not one, because of a real cross-test hazard:
// vkGetDeviceProcAddr-loaded pointers (vkDeviceWaitIdle, vkDestroyDevice,
// ...) are process-global and not actually tied to the specific VkDevice
// they were resolved against - the Vulkan spec only guarantees a
// device-level pointer is valid for calls against *that* device. A test
// that creates a real VkDevice and then destructs it can leave those
// globals populated; a *later* test's own (possibly different, possibly
// never-fully-populated) VkDevice then reuses them via ~TutorialBase(),
// which is undefined behavior and was observed to segfault in practice
// (confirmed via gdb backtrace landing in TutorialBase::~TutorialBase()).
// Keeping at most one real-VkDevice-creating test per process avoids it,
// the same way the TutorialNNIntegrationTest binaries avoid the analogous,
// already-documented dlopen()/dlclose()-cycling hazard by being one
// process per tutorial.

#ifndef VULKAN_GRAPHIX_TESTS_TUTORIALBASEFAULTINJECTIONCOMMON_H
#define VULKAN_GRAPHIX_TESTS_TUTORIALBASEFAULTINJECTIONCOMMON_H

#include <dlfcn.h>

#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/TutorialBase.h"

namespace vulkan_graphix::test {

// See TutorialBaseHelpersTest.cpp for why a `using`-declaring subclass
// (not a TutorialBase.h change) is the right way to reach `protected`
// members from a test.
class TestableTutorialBase : public vulkan_graphix::TutorialBase {
public:
    using TutorialBase::checkPhysicalDeviceProperties;
    using TutorialBase::createDevice;
    using TutorialBase::createInstance;
    using TutorialBase::createPresentationSurface;
    using TutorialBase::createSwapChain;
    using TutorialBase::createSwapChainImageViews;
    using TutorialBase::getDeviceQueue;
    using TutorialBase::loadDeviceLevelEntryPoints;
    using TutorialBase::loadExportedEntryPoints;
    using TutorialBase::loadGlobalLevelEntryPoints;
    using TutorialBase::loadInstanceLevelEntryPoints;
    using TutorialBase::m_vulkan_common_parameters;
    using TutorialBase::m_vulkan_library_handle;
    using TutorialBase::m_window_parameters;

    bool draw() override { return true; }
    bool childOnWindowSizeChanged() override { return true; }
    void childClear() override {}

    // ~TutorialBase() dlclose()s m_vulkan_library_handle unconditionally
    // once it's set. Each of these binaries shares one dlopen() of
    // libvulkan.so.1 (via sharedVulkanLibraryHandle() below) across every
    // TEST() in it rather than each one calling the real
    // loadVulkanLibrary() - repeated dlopen()/dlclose() cycling of the
    // Vulkan ICD within a single process is a known source of intermittent
    // heap corruption in the Mesa/Intel driver's shader-cache code (see
    // the integration tests' own "one dlopen, one dlclose" rationale in
    // Makefile.am). Clearing the handle before destruction turns that
    // dlclose() into a no-op, leaving the shared handle open for the next
    // TEST().
    ~TestableTutorialBase() override { m_vulkan_library_handle = nullptr; }
};

// dlopen()s libvulkan.so.1 exactly once for the whole test binary and
// never explicitly closes it (same effect as a real process's single
// dlopen that just lives until exit) - see TestableTutorialBase's dtor.
inline void* sharedVulkanLibraryHandle() {
    static void* const handle = dlopen("libvulkan.so.1", RTLD_NOW);
    return handle;
}

// Brings a tutorial up through loadExportedEntryPoints() for real - the
// prefix every fault test needs, sharing sharedVulkanLibraryHandle()
// instead of each test calling the real loadVulkanLibrary().
inline bool bringUpThroughExportedEntryPoints(TestableTutorialBase& tutorial) {
    tutorial.m_vulkan_library_handle = sharedVulkanLibraryHandle();
    return (tutorial.m_vulkan_library_handle != nullptr) &&
           tutorial.loadExportedEntryPoints();
}

// ... through createInstance() for real.
inline bool bringUpThroughInstance(TestableTutorialBase& tutorial) {
    return bringUpThroughExportedEntryPoints(tutorial) &&
           tutorial.loadGlobalLevelEntryPoints() && tutorial.createInstance();
}

// ... through createPresentationSurface() - needs a real window.
inline bool bringUpThroughSurface(
        TestableTutorialBase& tutorial,
        vulkan_graphix::os::WindowParameters params) {
    tutorial.m_window_parameters = params;
    return bringUpThroughInstance(tutorial) &&
           tutorial.loadInstanceLevelEntryPoints() &&
           tutorial.createPresentationSurface();
}

// ... through createDevice().
inline bool bringUpThroughDevice(TestableTutorialBase& tutorial,
                                 vulkan_graphix::os::WindowParameters params) {
    return bringUpThroughSurface(tutorial, params) && tutorial.createDevice();
}

// ... through getDeviceQueue() - everything createSwapChain() needs.
inline bool bringUpThroughDeviceQueue(
        TestableTutorialBase& tutorial,
        vulkan_graphix::os::WindowParameters params) {
    return bringUpThroughDevice(tutorial, params) &&
           tutorial.loadDeviceLevelEntryPoints() && tutorial.getDeviceQueue();
}

}  // namespace vulkan_graphix::test

#endif  // VULKAN_GRAPHIX_TESTS_TUTORIALBASEFAULTINJECTIONCOMMON_H
