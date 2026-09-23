lib_LTLIBRARIES = libvulkan_graphix.la

libvulkan_graphix_la_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GLM_CFLAGS)

libvulkan_graphix_la_SOURCES = ./LoggerHelpers.cpp \
															./Logging.cpp \
															./OperatingSystem.cpp \
															./OrbitCamera.cpp \
															./TerrainGenerator.cpp \
															./Tools.cpp \
															./Tutorial01.cpp \
															./Tutorial02.cpp \
															./Tutorial03.cpp \
															./Tutorial04.cpp \
															./Tutorial05.cpp \
															./Tutorial06.cpp \
															./Tutorial07.cpp \
															./Tutorial08.cpp \
															./Tutorial09.cpp \
															./Tutorial10.cpp \
															./Tutorial11.cpp \
															./Tutorial12.cpp \
															./TutorialBase.cpp \
															./VulkanFunctions.cpp

libvulkan_graphix_la_LIBADD = -lboost_log -lboost_system -lboost_thread \
														$(VULKAN_LIBS) $(X11_LIBS)

libvulkan_graphix_la_LDFLAGS = -pthread

if COVERAGE_ENABLED
libvulkan_graphix_la_CPPFLAGS += $(COVERAGE_CXXFLAGS)
libvulkan_graphix_la_LDFLAGS += $(COVERAGE_LDFLAGS)
endif
