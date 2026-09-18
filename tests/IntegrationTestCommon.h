#ifndef VULKAN_GRAPHIX_TESTS_INTEGRATIONTESTCOMMON_H
#define VULKAN_GRAPHIX_TESTS_INTEGRATIONTESTCOMMON_H

// Shared helpers for the per-tutorial IntegrationTest .cpp files. Each
// tutorial's own test lives in its own translation unit (TutorialNNInteg
// rationTest.cpp) rather than all in one file, because several
// TutorialNN.h headers define their own struct VertexData directly in
// namespace vulkan_graphix - fine when only one such header is ever
// included per .cpp (as every tutorialNN_main.cpp already does), but an
// ODR violation the moment two of them share a translation unit.

#include <cstdlib>

namespace vulkan_graphix::test {

inline bool hasDisplay() { return std::getenv("DISPLAY") != nullptr; }

inline constexpr int c_draw_iterations = 3;

}  // namespace vulkan_graphix::test

#endif  // VULKAN_GRAPHIX_TESTS_INTEGRATIONTESTCOMMON_H
