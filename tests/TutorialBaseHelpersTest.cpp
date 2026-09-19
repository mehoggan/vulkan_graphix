// Exercises TutorialBase's pure swap-chain-parameter-selection helpers
// directly - no Vulkan device or X11 window needed, since these take an
// already-fetched VkSurfaceCapabilitiesKHR/vector<VkSurfaceFormatKHR>/
// vector<VkPresentModeKHR> and compute a value from it, with no Vulkan
// calls of their own. They're `protected` on TutorialBase, so this file
// exposes them via a trivial `using`-declaring subclass rather than
// changing TutorialBase.h - TutorialBase.h grants no special test access,
// unlike Logging.h's FRIEND_TEST, because plain inheritance is enough
// here: protected members are already accessible to any derived class,
// and a `using Base::member;` in that subclass republishes them as
// public from outside it.

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "vulkan_graphix/TutorialBase.h"

namespace {

class TestableTutorialBase : public vulkan_graphix::TutorialBase {
public:
    using TutorialBase::checkExtensionAvailability;
    using TutorialBase::getSwapChainExtent;
    using TutorialBase::getSwapChainFormat;
    using TutorialBase::getSwapChainNumImages;
    using TutorialBase::getSwapChainPresentMode;
    using TutorialBase::getSwapChainTransform;
    using TutorialBase::getSwapChainUsageFlags;

    bool draw() override { return true; }
    bool childOnWindowSizeChanged() override { return true; }
    void childClear() override {}
};

}  // namespace

TEST(TutorialBaseHelpersTest, GetSwapChainNumImagesAddsTwoForBuffering) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.minImageCount = 2;
    capabilities.maxImageCount = 0;  // 0 == no upper bound

    EXPECT_EQ(tutorial.getSwapChainNumImages(capabilities), 4u);
}

TEST(TutorialBaseHelpersTest, GetSwapChainNumImagesClampsToTheDeviceMaximum) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.minImageCount = 2;
    capabilities.maxImageCount = 3;

    EXPECT_EQ(tutorial.getSwapChainNumImages(capabilities), 3u);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainFormatDefaultsToRgba8WhenFormatIsUndefined) {
    TestableTutorialBase tutorial;
    std::vector<VkSurfaceFormatKHR> formats = {
            {VK_FORMAT_UNDEFINED, VK_COLORSPACE_SRGB_NONLINEAR_KHR}};

    VkSurfaceFormatKHR result = tutorial.getSwapChainFormat(formats);

    EXPECT_EQ(result.format, VK_FORMAT_R8G8B8A8_UNORM);
    EXPECT_EQ(result.colorSpace, VK_COLORSPACE_SRGB_NONLINEAR_KHR);
}

TEST(TutorialBaseHelpersTest, GetSwapChainFormatPrefersRgba8WhenListed) {
    TestableTutorialBase tutorial;
    std::vector<VkSurfaceFormatKHR> formats = {
            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR}};

    VkSurfaceFormatKHR result = tutorial.getSwapChainFormat(formats);

    EXPECT_EQ(result.format, VK_FORMAT_R8G8B8A8_UNORM);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainFormatFallsBackToTheFirstListedFormat) {
    TestableTutorialBase tutorial;
    std::vector<VkSurfaceFormatKHR> formats = {
            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_A2B10G10R10_UNORM_PACK32,
             VK_COLORSPACE_SRGB_NONLINEAR_KHR}};

    VkSurfaceFormatKHR result = tutorial.getSwapChainFormat(formats);

    EXPECT_EQ(result.format, VK_FORMAT_B8G8R8A8_UNORM);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainExtentUsesTheWindowSizeWhenCurrentExtentIsSpecial) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.currentExtent = {static_cast<std::uint32_t>(-1),
                                  static_cast<std::uint32_t>(-1)};
    capabilities.minImageExtent = {100, 100};
    capabilities.maxImageExtent = {1000, 1000};

    VkExtent2D extent = tutorial.getSwapChainExtent(capabilities);

    EXPECT_EQ(extent.width, 640u);
    EXPECT_EQ(extent.height, 480u);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainExtentClampsTheDefaultToTheDeviceMinimum) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.currentExtent = {static_cast<std::uint32_t>(-1),
                                  static_cast<std::uint32_t>(-1)};
    capabilities.minImageExtent = {800, 600};
    capabilities.maxImageExtent = {1920, 1080};

    VkExtent2D extent = tutorial.getSwapChainExtent(capabilities);

    EXPECT_EQ(extent.width, 800u);
    EXPECT_EQ(extent.height, 600u);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainExtentClampsTheDefaultToTheDeviceMaximum) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.currentExtent = {static_cast<std::uint32_t>(-1),
                                  static_cast<std::uint32_t>(-1)};
    capabilities.minImageExtent = {0, 0};
    capabilities.maxImageExtent = {320, 240};

    VkExtent2D extent = tutorial.getSwapChainExtent(capabilities);

    EXPECT_EQ(extent.width, 320u);
    EXPECT_EQ(extent.height, 240u);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainExtentUsesTheCurrentExtentWhenNotSpecial) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.currentExtent = {1280, 720};

    VkExtent2D extent = tutorial.getSwapChainExtent(capabilities);

    EXPECT_EQ(extent.width, 1280u);
    EXPECT_EQ(extent.height, 720u);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainUsageFlagsReturnsColorAttachmentWhenSupported) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    EXPECT_EQ(tutorial.getSwapChainUsageFlags(capabilities),
              static_cast<VkImageUsageFlags>(
                      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT));
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainUsageFlagsReturnsAllOnesWhenColorAttachmentUnsupported) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.supportedUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    EXPECT_EQ(tutorial.getSwapChainUsageFlags(capabilities),
              static_cast<VkImageUsageFlags>(-1));
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainTransformPrefersIdentityWhenSupported) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR |
                                       VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR;
    capabilities.currentTransform = VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR;

    EXPECT_EQ(tutorial.getSwapChainTransform(capabilities),
              VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainTransformFallsBackToCurrentTransformWhenIdentityUnsupported) {
    TestableTutorialBase tutorial;
    VkSurfaceCapabilitiesKHR capabilities{};
    capabilities.supportedTransforms = VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR;
    capabilities.currentTransform = VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR;

    EXPECT_EQ(tutorial.getSwapChainTransform(capabilities),
              VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR);
}

TEST(TutorialBaseHelpersTest, GetSwapChainPresentModePrefersMailbox) {
    TestableTutorialBase tutorial;
    std::vector<VkPresentModeKHR> modes = {VK_PRESENT_MODE_FIFO_KHR,
                                           VK_PRESENT_MODE_IMMEDIATE_KHR,
                                           VK_PRESENT_MODE_MAILBOX_KHR};

    EXPECT_EQ(tutorial.getSwapChainPresentMode(modes),
              VK_PRESENT_MODE_MAILBOX_KHR);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainPresentModeFallsBackToImmediateWhenMailboxUnavailable) {
    TestableTutorialBase tutorial;
    std::vector<VkPresentModeKHR> modes = {VK_PRESENT_MODE_FIFO_KHR,
                                           VK_PRESENT_MODE_IMMEDIATE_KHR};

    EXPECT_EQ(tutorial.getSwapChainPresentMode(modes),
              VK_PRESENT_MODE_IMMEDIATE_KHR);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainPresentModeFallsBackToFifoWhenNeitherPreferredIsAvailable) {
    TestableTutorialBase tutorial;
    std::vector<VkPresentModeKHR> modes = {VK_PRESENT_MODE_FIFO_KHR};

    EXPECT_EQ(tutorial.getSwapChainPresentMode(modes),
              VK_PRESENT_MODE_FIFO_KHR);
}

TEST(TutorialBaseHelpersTest,
     GetSwapChainPresentModeReturnsInvalidWhenFifoIsNotEvenAvailable) {
    TestableTutorialBase tutorial;
    std::vector<VkPresentModeKHR> modes;

    EXPECT_EQ(tutorial.getSwapChainPresentMode(modes),
              static_cast<VkPresentModeKHR>(-1));
}

TEST(TutorialBaseHelpersTest, CheckExtensionAvailabilityFindsAListedName) {
    TestableTutorialBase tutorial;
    std::vector<VkExtensionProperties> extensions(1);
    std::snprintf(extensions[0].extensionName,
                  sizeof(extensions[0].extensionName),
                  "%s",
                  "VK_KHR_swapchain");

    EXPECT_TRUE(tutorial.checkExtensionAvailability("VK_KHR_swapchain",
                                                    extensions));
}

TEST(TutorialBaseHelpersTest,
     CheckExtensionAvailabilityReturnsFalseForAnUnlistedName) {
    TestableTutorialBase tutorial;
    std::vector<VkExtensionProperties> extensions(1);
    std::snprintf(extensions[0].extensionName,
                  sizeof(extensions[0].extensionName),
                  "%s",
                  "VK_KHR_swapchain");

    EXPECT_FALSE(tutorial.checkExtensionAvailability("VK_KHR_does_not_exist",
                                                     extensions));
}
