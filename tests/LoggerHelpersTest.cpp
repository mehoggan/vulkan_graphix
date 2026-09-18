// Exercises the LoggerHelpers.h stream operators directly - pure
// stringstream formatting over VkLayerProperties/const char* data, no
// live device needed.

#include <cstring>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

#include "vulkan_graphix/LoggerHelpers.h"

TEST(LoggerHelpersTest, StreamsAVectorOfCStringsCommaSeparatedInBrackets) {
    std::vector<const char*> values = {"VK_LAYER_KHRONOS_validation",
                                       "VK_LAYER_MESA_overlay"};
    std::stringstream out_stream;

    out_stream << values;

    EXPECT_EQ(out_stream.str(),
              "[VK_LAYER_KHRONOS_validation, VK_LAYER_MESA_overlay, ]");
}

TEST(LoggerHelpersTest, StreamsAnEmptyVectorOfCStringsAsEmptyBrackets) {
    std::vector<const char*> values;
    std::stringstream out_stream;

    out_stream << values;

    EXPECT_EQ(out_stream.str(), "[]");
}

TEST(LoggerHelpersTest, StreamsAVkLayerPropertiesAsItsLayerName) {
    VkLayerProperties properties{};
    std::strncpy(properties.layerName,
                 "VK_LAYER_KHRONOS_validation",
                 sizeof(properties.layerName) - 1);
    std::stringstream out_stream;

    out_stream << properties;

    EXPECT_EQ(out_stream.str(), "VK_LAYER_KHRONOS_validation");
}

TEST(LoggerHelpersTest,
     StreamsAVectorOfVkLayerPropertiesTabIndentedBetweenEntries) {
    VkLayerProperties first{};
    std::strncpy(first.layerName, "layer-one", sizeof(first.layerName) - 1);
    VkLayerProperties second{};
    std::strncpy(second.layerName, "layer-two", sizeof(second.layerName) - 1);
    std::vector<VkLayerProperties> values = {first, second};
    std::stringstream out_stream;

    out_stream << values;

    EXPECT_EQ(out_stream.str(), "layer-one\n\tlayer-two");
}
