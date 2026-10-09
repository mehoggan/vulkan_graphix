if HAVE_GTEST
TESTS = vertex_types_test math_test tools_test game_logic_test camera_test geometry_test \
		operating_system_test logging_test logger_helpers_test \
		tutorial_base_helpers_test tutorial_base_fault_injection_test \
		tutorial_base_fault_injection_swapchain_test \
		tutorial_base_fault_injection_device_level_test \
		tutorial03_fault_injection_test \
		tutorial04_fault_injection_test tutorial05_fault_injection_test \
		tutorial06_fault_injection_test tutorial07_fault_injection_test \
		tutorial08_fault_injection_test tutorial09_fault_injection_test \
		tutorial10_fault_injection_test \
		tutorial01_integration_test \
		tutorial02_integration_test \
		tutorial03_integration_test \
		tutorial04_integration_test \
		tutorial05_integration_test \
		tutorial06_integration_test \
		tutorial07_integration_test \
		tutorial08_integration_test \
		tutorial09_integration_test \
		tutorial10_integration_test \
		vulkan_common_integration_test
check_PROGRAMS = $(TESTS)

vertex_types_test_SOURCES = ./VertexTypesTest.cpp
vertex_types_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
vertex_types_test_LDADD = $(GTEST_LIBS)
vertex_types_test_LDFLAGS = -pthread

math_test_SOURCES = ./MathTest.cpp
math_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
math_test_LDADD = $(GTEST_LIBS)
math_test_LDFLAGS = -pthread

# Exercises vulkan_graphix::Tools's file-IO/math helpers directly (see
# ToolsTest.cpp's header comment) - needs libvulkan_graphix.la (Tools.cpp
# carries the STB_IMAGE_IMPLEMENTATION and the Vulkan/X11 symbols the
# library as a whole depends on), but no live device or window, unlike the
# integration tests below.
tools_test_SOURCES = ./ToolsTest.cpp
tools_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS) \
		$(VULKAN_CFLAGS)
tools_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tools_test_LDFLAGS = -pthread

# Exercises the gameplay math shared with vulkan_earth (TerrainGenerator's
# world queries/craters, Ballistics, TankOrientation) - see
# GameLogicTest.cpp's header comment. Pure math, no device or window.
game_logic_test_SOURCES = ./GameLogicTest.cpp
game_logic_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
game_logic_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
game_logic_test_LDFLAGS = -pthread

camera_test_SOURCES = ./CameraTest.cpp
camera_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
camera_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
camera_test_LDFLAGS = -pthread

geometry_test_SOURCES = ./GeometryTest.cpp
geometry_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
geometry_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
geometry_test_LDFLAGS = -pthread

# Drives os::Window::renderingLoop() with synthetic X11 events (see
# OperatingSystemTest.cpp's header comment) - needs a live DISPLAY, like the
# integration tests below, but not a Vulkan device.
operating_system_test_SOURCES = ./OperatingSystemTest.cpp
operating_system_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS) \
		$(X11_CFLAGS)
operating_system_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
operating_system_test_LDFLAGS = -pthread

# Exercises vulkan_graphix::Logging directly (see LoggingTest.cpp's header
# comment) - pure std::filesystem/ostream code, no live device or window.
logging_test_SOURCES = ./LoggingTest.cpp
logging_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
logging_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
logging_test_LDFLAGS = -pthread

# Exercises LoggerHelpers.h's stream operators directly (see
# LoggerHelpersTest.cpp's header comment) - pure stringstream formatting,
# no X11/library link needed, so LoggerHelpers.cpp is compiled
# straight into this binary rather than linking libvulkan_graphix.la.
logger_helpers_test_SOURCES = ./LoggerHelpersTest.cpp \
		../lib/LoggerHelpers.cpp
logger_helpers_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(VULKAN_CFLAGS)
logger_helpers_test_LDADD = $(GTEST_LIBS) $(VULKAN_LIBS)
logger_helpers_test_LDFLAGS = -pthread

# Exercises TutorialBase's pure swap-chain-parameter-selection helpers (see
# TutorialBaseHelpersTest.cpp's header comment) - no live device or window,
# just data in, data out - but still needs libvulkan_graphix.la for
# TutorialBase's own (non-inline) methods/vtable.
tutorial_base_helpers_test_SOURCES = ./TutorialBaseHelpersTest.cpp
tutorial_base_helpers_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial_base_helpers_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial_base_helpers_test_LDFLAGS = -pthread

# Fault-injects TutorialBase's Vulkan-call failure branches by swapping the
# mutable vulkan_graphix::vkSomething function-pointer globals (see
# TutorialBaseFaultInjectionTest.cpp's header comment) - needs a live
# device/window like the integration tests, since the real setup sequence
# runs for real up to the point under test.
tutorial_base_fault_injection_test_SOURCES = \
		./TutorialBaseFaultInjectionTest.cpp
tutorial_base_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial_base_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial_base_fault_injection_test_LDFLAGS = -pthread

# Split from tutorial_base_fault_injection_test into their own binaries -
# see TutorialBaseFaultInjectionCommon.h's header comment for why: each of
# these two is the one test in its group that ends up with a real, live
# VkDevice, and process-global vkGetDeviceProcAddr-loaded pointers aren't
# actually valid across different VkDevice objects, so more than one such
# test per process is a real (observed) crash risk.
tutorial_base_fault_injection_swapchain_test_SOURCES = \
		./TutorialBaseFaultInjectionSwapChainTest.cpp
tutorial_base_fault_injection_swapchain_test_CPPFLAGS = \
		-Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial_base_fault_injection_swapchain_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial_base_fault_injection_swapchain_test_LDFLAGS = -pthread

tutorial_base_fault_injection_device_level_test_SOURCES = \
		./TutorialBaseFaultInjectionDeviceLevelTest.cpp
tutorial_base_fault_injection_device_level_test_CPPFLAGS = \
		-Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial_base_fault_injection_device_level_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial_base_fault_injection_device_level_test_LDFLAGS = -pthread

# Fault-injects Tutorial04's own create*() failure branches (see
# Tutorial04FaultInjectionTest.cpp's header comment) - one real device per
# process, like the TutorialBaseFaultInjection* binaries above.
tutorial04_fault_injection_test_SOURCES = \
		./Tutorial04FaultInjectionTest.cpp \
		../bin/04_vertex_attributes/Tutorial04.cpp
tutorial04_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include \
		-I$(abs_top_srcdir)/bin/04_vertex_attributes \
		$(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial04_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial04_fault_injection_test_LDFLAGS = -pthread

# Fault-injects Tutorial05/06/07's own create*() failure branches, same
# pattern as tutorial04_fault_injection_test above.
tutorial05_fault_injection_test_SOURCES = \
		./Tutorial05FaultInjectionTest.cpp \
		../bin/05_staging_resources/Tutorial05.cpp
tutorial05_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include \
		-I$(abs_top_srcdir)/bin/05_staging_resources \
		$(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial05_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial05_fault_injection_test_LDFLAGS = -pthread

tutorial06_fault_injection_test_SOURCES = \
		./Tutorial06FaultInjectionTest.cpp \
		../bin/06_descriptor_sets/Tutorial06.cpp
tutorial06_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include \
		-I$(abs_top_srcdir)/bin/06_descriptor_sets \
		$(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial06_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial06_fault_injection_test_LDFLAGS = -pthread

tutorial07_fault_injection_test_SOURCES = \
		./Tutorial07FaultInjectionTest.cpp \
		../bin/07_uniform_buffers/Tutorial07.cpp
tutorial07_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include \
		-I$(abs_top_srcdir)/bin/07_uniform_buffers \
		$(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial07_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial07_fault_injection_test_LDFLAGS = -pthread

# Fault-injects Tutorial03/08/09/10's own create*() failure branches, same
# pattern as tutorial04_fault_injection_test above.
tutorial03_fault_injection_test_SOURCES = \
		./Tutorial03FaultInjectionTest.cpp \
		../bin/03_first_triangle/Tutorial03.cpp
tutorial03_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include \
		-I$(abs_top_srcdir)/bin/03_first_triangle \
		$(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial03_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial03_fault_injection_test_LDFLAGS = -pthread

tutorial08_fault_injection_test_SOURCES = \
		./Tutorial08FaultInjectionTest.cpp \
		../bin/08_phong_sphere/Tutorial08.cpp
tutorial08_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include \
		-I$(abs_top_srcdir)/bin/08_phong_sphere \
		$(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial08_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial08_fault_injection_test_LDFLAGS = -pthread

tutorial09_fault_injection_test_SOURCES = \
		./Tutorial09FaultInjectionTest.cpp \
		../bin/09_tessellated_terrain/Tutorial09.cpp
tutorial09_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include \
		-I$(abs_top_srcdir)/bin/09_tessellated_terrain \
		$(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial09_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial09_fault_injection_test_LDFLAGS = -pthread

tutorial10_fault_injection_test_SOURCES = \
		./Tutorial10FaultInjectionTest.cpp \
		../bin/10_spline_polyline/Tutorial10.cpp
tutorial10_fault_injection_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include \
		-I$(abs_top_srcdir)/bin/10_spline_polyline \
		$(GTEST_CFLAGS) $(GLM_CFLAGS)
tutorial10_fault_injection_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
tutorial10_fault_injection_test_LDFLAGS = -pthread

# Integration tests: drive a real tutorial through a live Vulkan device and
# X11 window (see Tutorial01IntegrationTest.cpp's header comment) - unlike
# the two tests above, these link against the library itself. Each
# tutorial gets its own binary/process rather than sharing one, since
# repeatedly dlopen()/dlclose()-ing the Vulkan ICD within a single
# long-lived process (as one shared binary running nine TEST()s would do)
# triggered intermittent heap corruption inside the Mesa/Intel driver's
# own shader-cache code - not a bug in this project, but the Vulkan ICD
# clearly isn't robust to being reloaded many times in one process, so
# match how every tutorialNN_runner actually runs: one process, one
# dlopen, one dlclose.
integration_test_common_cppflags = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
integration_test_common_ldadd = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		$(VULKAN_LIBS) $(X11_LIBS)
integration_test_common_ldflags = -pthread

tutorial01_integration_test_SOURCES = ./Tutorial01IntegrationTest.cpp \
		../bin/01_device_initialization/Tutorial01.cpp
tutorial01_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/01_device_initialization
tutorial01_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial01_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial02_integration_test_SOURCES = ./Tutorial02IntegrationTest.cpp \
		../bin/02_swap_chain/Tutorial02.cpp
tutorial02_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/02_swap_chain
tutorial02_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial02_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial03_integration_test_SOURCES = ./Tutorial03IntegrationTest.cpp \
		../bin/03_first_triangle/Tutorial03.cpp
tutorial03_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/03_first_triangle
tutorial03_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial03_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial04_integration_test_SOURCES = ./Tutorial04IntegrationTest.cpp \
		../bin/04_vertex_attributes/Tutorial04.cpp
tutorial04_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/04_vertex_attributes
tutorial04_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial04_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial05_integration_test_SOURCES = ./Tutorial05IntegrationTest.cpp \
		../bin/05_staging_resources/Tutorial05.cpp
tutorial05_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/05_staging_resources
tutorial05_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial05_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial06_integration_test_SOURCES = ./Tutorial06IntegrationTest.cpp \
		../bin/06_descriptor_sets/Tutorial06.cpp
tutorial06_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/06_descriptor_sets
tutorial06_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial06_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial07_integration_test_SOURCES = ./Tutorial07IntegrationTest.cpp \
		../bin/07_uniform_buffers/Tutorial07.cpp
tutorial07_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/07_uniform_buffers
tutorial07_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial07_integration_test_LDFLAGS = $(integration_test_common_ldflags)

# The library's VulkanCommon building blocks on a live device (borrows
# Tutorial07's shaders, which all-local already copies here).
vulkan_common_integration_test_SOURCES = ./VulkanCommonIntegrationTest.cpp
vulkan_common_integration_test_CPPFLAGS = $(integration_test_common_cppflags)
vulkan_common_integration_test_LDADD = $(integration_test_common_ldadd)
vulkan_common_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial08_integration_test_SOURCES = ./Tutorial08IntegrationTest.cpp \
		../bin/08_phong_sphere/Tutorial08.cpp
tutorial08_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/08_phong_sphere
tutorial08_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial08_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial09_integration_test_SOURCES = ./Tutorial09IntegrationTest.cpp \
		../bin/09_tessellated_terrain/Tutorial09.cpp
tutorial09_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/09_tessellated_terrain
tutorial09_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial09_integration_test_LDFLAGS = $(integration_test_common_ldflags)

tutorial10_integration_test_SOURCES = ./Tutorial10IntegrationTest.cpp \
		../bin/10_spline_polyline/Tutorial10.cpp
tutorial10_integration_test_CPPFLAGS = $(integration_test_common_cppflags) \
		-I$(abs_top_srcdir)/bin/10_spline_polyline
tutorial10_integration_test_LDADD = $(integration_test_common_ldadd)
tutorial10_integration_test_LDFLAGS = $(integration_test_common_ldflags)

# --- Shader copying for the integration tests (mirrors bin/Makefile.am) ---
integration_tutorial03_vert = $(top_srcdir)/resources/03/Data/shader.vert.03.spv
integration_tutorial03_frag = $(top_srcdir)/resources/03/Data/shader.frag.03.spv
integration_tutorial04_vert = $(top_srcdir)/resources/04/Data/shader.vert.04.spv
integration_tutorial04_frag = $(top_srcdir)/resources/04/Data/shader.frag.04.spv
integration_tutorial05_vert = $(top_srcdir)/resources/05/Data/shader.vert.05.spv
integration_tutorial05_frag = $(top_srcdir)/resources/05/Data/shader.frag.05.spv
integration_tutorial06_vert = $(top_srcdir)/resources/06/Data/shader.vert.06.spv
integration_tutorial06_frag = $(top_srcdir)/resources/06/Data/shader.frag.06.spv
integration_tutorial06_texture = $(top_srcdir)/resources/06/Data/texture.06.png
integration_tutorial07_vert = $(top_srcdir)/resources/07/Data/shader.vert.07.spv
integration_tutorial07_frag = $(top_srcdir)/resources/07/Data/shader.frag.07.spv
integration_tutorial07_texture = $(top_srcdir)/resources/07/Data/texture.07.png
integration_tutorial08_vert = $(top_srcdir)/resources/08/Data/shader.vert.08.spv
integration_tutorial08_frag = $(top_srcdir)/resources/08/Data/shader.frag.08.spv
integration_tutorial09_vert = $(top_srcdir)/resources/09/Data/shader.vert.09.spv
integration_tutorial09_frag = $(top_srcdir)/resources/09/Data/shader.frag.09.spv
integration_tutorial09_texture = $(top_srcdir)/resources/09/Data/texture.09.png
integration_tutorial10_line_vert = $(top_srcdir)/resources/10/Data/shader.vert.10_line.spv
integration_tutorial10_line_frag = $(top_srcdir)/resources/10/Data/shader.frag.10_line.spv

all-local: $(integration_tutorial03_vert) $(integration_tutorial03_frag) \
		$(integration_tutorial04_vert) $(integration_tutorial04_frag) \
		$(integration_tutorial05_vert) $(integration_tutorial05_frag) \
		$(integration_tutorial06_vert) $(integration_tutorial06_frag) \
		$(integration_tutorial06_texture) $(integration_tutorial07_vert) \
		$(integration_tutorial07_frag) $(integration_tutorial07_texture) \
		$(integration_tutorial08_vert) $(integration_tutorial08_frag) \
		$(integration_tutorial09_vert) $(integration_tutorial09_frag) \
		$(integration_tutorial09_texture) $(integration_tutorial10_line_vert) \
		$(integration_tutorial10_line_frag)
	cp $(integration_tutorial03_vert) $(builddir)/shader.03.vert.spv
	cp $(integration_tutorial03_frag) $(builddir)/shader.03.frag.spv
	cp $(integration_tutorial04_vert) $(builddir)/shader.04.vert.spv
	cp $(integration_tutorial04_frag) $(builddir)/shader.04.frag.spv
	cp $(integration_tutorial05_vert) $(builddir)/shader.05.vert.spv
	cp $(integration_tutorial05_frag) $(builddir)/shader.05.frag.spv
	cp $(integration_tutorial06_vert) $(builddir)/shader.06.vert.spv
	cp $(integration_tutorial06_frag) $(builddir)/shader.06.frag.spv
	cp $(integration_tutorial06_texture) $(builddir)/texture.06.png
	cp $(integration_tutorial07_vert) $(builddir)/shader.07.vert.spv
	cp $(integration_tutorial07_frag) $(builddir)/shader.07.frag.spv
	cp $(integration_tutorial07_texture) $(builddir)/texture.07.png
	cp $(integration_tutorial08_vert) $(builddir)/shader.08.vert.spv
	cp $(integration_tutorial08_frag) $(builddir)/shader.08.frag.spv
	cp $(integration_tutorial09_vert) $(builddir)/shader.09.vert.spv
	cp $(integration_tutorial09_frag) $(builddir)/shader.09.frag.spv
	cp $(integration_tutorial09_texture) $(builddir)/texture.09.png
	cp $(integration_tutorial10_line_vert) $(builddir)/shader.10_line.vert.spv
	cp $(integration_tutorial10_line_frag) $(builddir)/shader.10_line.frag.spv

if COVERAGE_ENABLED
vertex_types_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
vertex_types_test_LDFLAGS += $(COVERAGE_LDFLAGS)
math_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
math_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tools_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
game_logic_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
game_logic_test_LDFLAGS += $(COVERAGE_LDFLAGS)
camera_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
camera_test_LDFLAGS += $(COVERAGE_LDFLAGS)
geometry_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
geometry_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tools_test_LDFLAGS += $(COVERAGE_LDFLAGS)
operating_system_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
operating_system_test_LDFLAGS += $(COVERAGE_LDFLAGS)
logging_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
logging_test_LDFLAGS += $(COVERAGE_LDFLAGS)
logger_helpers_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
logger_helpers_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial_base_helpers_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial_base_helpers_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial_base_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial_base_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial_base_fault_injection_swapchain_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial_base_fault_injection_swapchain_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial_base_fault_injection_device_level_test_CPPFLAGS += \
		$(COVERAGE_CXXFLAGS)
tutorial_base_fault_injection_device_level_test_LDFLAGS += \
		$(COVERAGE_LDFLAGS)
tutorial04_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial04_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial05_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial05_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial06_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial06_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial07_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial07_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial03_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial03_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial08_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial08_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial09_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial09_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial10_fault_injection_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial10_fault_injection_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial01_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial01_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial02_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial02_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial03_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial03_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial04_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial04_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial05_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial05_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial06_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial06_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial07_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial07_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
vulkan_common_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
vulkan_common_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial08_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial08_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial09_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial09_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
tutorial10_integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
tutorial10_integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
endif
endif

CLEANFILES = *.o
