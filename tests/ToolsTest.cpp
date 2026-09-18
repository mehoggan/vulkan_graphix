// Exercises vulkan_graphix::Tools's pure/file-IO helpers directly - no
// Vulkan device or X11 window needed, unlike the TutorialNNIntegrationTest
// binaries, so this runs unconditionally (no DISPLAY check).

#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "vulkan_graphix/Tools.h"

namespace {

constexpr float c_epsilon = 1e-4f;

}  // namespace

TEST(ToolsTest, GetBinaryFileContentsReadsAnExistingFile) {
    const std::string path = "tools_test_binary_fixture.bin";
    {
        std::ofstream out_stream(path, std::ios::binary);
        out_stream.write("\x01\x02\x03\x04\x05", 5);
    }

    std::vector<char> contents =
            vulkan_graphix::Tools::getBinaryFileContents(path);

    ASSERT_EQ(contents.size(), 5u);
    EXPECT_EQ(contents[0], 0x01);
    EXPECT_EQ(contents[4], 0x05);

    std::remove(path.c_str());
}

TEST(ToolsTest, GetBinaryFileContentsReturnsEmptyForAMissingFile) {
    std::vector<char> contents = vulkan_graphix::Tools::getBinaryFileContents(
            "this_file_does_not_exist_anywhere.bin");

    EXPECT_TRUE(contents.empty());
}

TEST(ToolsTest, GetImageDataLoadsARealPngAndReportsItsDimensions) {
    int width = 0, height = 0, components = 0, data_size = 0;
    std::vector<char> data = vulkan_graphix::Tools::getImageData(
            "texture.06.png", 4, &width, &height, &components, &data_size);

    ASSERT_FALSE(data.empty());
    EXPECT_GT(width, 0);
    EXPECT_GT(height, 0);
    EXPECT_GT(components, 0);
    EXPECT_EQ(data_size, width * height * 4);
    EXPECT_EQ(static_cast<int>(data.size()), data_size);
}

TEST(ToolsTest, GetImageDataReturnsEmptyForAMissingFile) {
    int width = 0, height = 0;
    std::vector<char> data = vulkan_graphix::Tools::getImageData(
            "this_texture_does_not_exist.png",
            4,
            &width,
            &height,
            nullptr,
            nullptr);

    EXPECT_TRUE(data.empty());
}

TEST(ToolsTest, GetImageDataReturnsEmptyForCorruptImageData) {
    const std::string path = "tools_test_corrupt_fixture.png";
    {
        std::ofstream out_stream(path, std::ios::binary);
        out_stream.write("not a real png file", 20);
    }

    std::vector<char> data = vulkan_graphix::Tools::getImageData(
            path, 4, nullptr, nullptr, nullptr, nullptr);

    EXPECT_TRUE(data.empty());

    std::remove(path.c_str());
}

TEST(ToolsTest, GetPerspectiveProjectionMatrixMatchesTheStandardFormula) {
    const float aspect_ratio = 16.0f / 9.0f;
    const float field_of_view = 60.0f;
    const float near_clip = 0.1f;
    const float far_clip = 100.0f;

    auto result = vulkan_graphix::Tools::getPerspectiveProjectionMatrix(
            aspect_ratio, field_of_view, near_clip, far_clip);

    const float expected_fov_value =
            1.0f / std::tan(field_of_view * 0.5f *
                            0.01745329251994329576923690768489f);

    EXPECT_NEAR(result[0][0], expected_fov_value / aspect_ratio, c_epsilon);
    EXPECT_NEAR(result[1][1], -expected_fov_value, c_epsilon);
    EXPECT_NEAR(result[2][2], far_clip / (near_clip - far_clip), c_epsilon);
    EXPECT_NEAR(result[2][3], -1.0f, c_epsilon);
    EXPECT_NEAR(result[3][2],
                (near_clip * far_clip) / (near_clip - far_clip),
                c_epsilon);
}

TEST(ToolsTest, GetOrthographicProjectionMatrixMatchesTheStandardFormula) {
    const float left = -1.0f, right = 1.0f;
    const float top_plane = -1.0f, bottom = 1.0f;
    const float near_plane = 0.1f, far_plane = 100.0f;

    auto result = vulkan_graphix::Tools::getOrthographicProjectionMatrix(
            left, right, top_plane, bottom, near_plane, far_plane);

    EXPECT_NEAR(result[0][0], 2.0f / (right - left), c_epsilon);
    EXPECT_NEAR(result[1][1], 2.0f / (bottom - top_plane), c_epsilon);
    EXPECT_NEAR(result[2][2], 1.0f / (near_plane - far_plane), c_epsilon);
    EXPECT_NEAR(result[3][0], -(right + left) / (right - left), c_epsilon);
    EXPECT_NEAR(result[3][1],
                -(bottom + top_plane) / (bottom - top_plane),
                c_epsilon);
    EXPECT_NEAR(
            result[3][2], near_plane / (near_plane - far_plane), c_epsilon);
    EXPECT_NEAR(result[3][3], 1.0f, c_epsilon);
}

namespace {

void fakeDeleter(VkDevice, VkBuffer, VkAllocationCallbacks const*) {}

}  // namespace

TEST(ToolsTest, AutoDeleterDefaultConstructionIsInert) {
    vulkan_graphix::Tools::AutoDeleter<VkBuffer, decltype(&fakeDeleter)>
            deleter;
    EXPECT_TRUE(!deleter);
    EXPECT_EQ(deleter.get(), static_cast<VkBuffer>(VK_NULL_HANDLE));
}

TEST(ToolsTest, AutoDeleterMoveTransfersOwnership) {
    VkBuffer fake_buffer = reinterpret_cast<VkBuffer>(0x1);
    VkDevice fake_device = reinterpret_cast<VkDevice>(0x2);
    vulkan_graphix::Tools::AutoDeleter<VkBuffer, decltype(&fakeDeleter)>
            original(fake_buffer, &fakeDeleter, fake_device);
    EXPECT_FALSE(!original);

    vulkan_graphix::Tools::AutoDeleter<VkBuffer, decltype(&fakeDeleter)> moved(
            std::move(original));

    EXPECT_FALSE(!moved);
    EXPECT_EQ(moved.get(), fake_buffer);
    EXPECT_TRUE(!original);  // NOLINT(bugprone-use-after-move) - checking
                             // the moved-from state is the point here.
}
