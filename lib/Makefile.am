lib_LTLIBRARIES = libvulkan_graphix.la

libvulkan_graphix_la_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GLM_CFLAGS)

libvulkan_graphix_la_SOURCES = ./Ballistics.cpp \
														./BitmapFont.cpp \
															./HellfireTank.cpp \
															./LoggerHelpers.cpp \
															./Logging.cpp \
															./OperatingSystem.cpp \
															./OrbitCamera.cpp \
															./TankOrientation.cpp \
															./TerrainGenerator.cpp \
															./Tools.cpp \
															./TutorialBase.cpp \
															./UiGeometry.cpp \
															./VulkanCommon.cpp \
															./VulkanFunctions.cpp

libvulkan_graphix_la_LIBADD = $(VULKAN_LIBS) $(X11_LIBS)

libvulkan_graphix_la_LDFLAGS = -pthread

if COVERAGE_ENABLED
libvulkan_graphix_la_CPPFLAGS += $(COVERAGE_CXXFLAGS)
libvulkan_graphix_la_LDFLAGS += $(COVERAGE_LDFLAGS)
endif
