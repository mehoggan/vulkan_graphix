if HAVE_GTEST
TESTS = vertex_types_test math_test integration_test
check_PROGRAMS = vertex_types_test math_test integration_test

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

# Integration tests: drive real tutorials through a live Vulkan device and
# X11 window (see Tutorial01IntegrationTest.cpp's header comment) - unlike
# the two tests above, this links against the library itself. One .cpp per
# tutorial (see IntegrationTestCommon.h) compiled into a single binary.
integration_test_SOURCES = ./IntegrationTestCommon.h \
		./Tutorial01IntegrationTest.cpp \
		./Tutorial02IntegrationTest.cpp \
		./Tutorial03IntegrationTest.cpp \
		./Tutorial04IntegrationTest.cpp \
		./Tutorial05IntegrationTest.cpp
integration_test_CPPFLAGS = -Werror -Wall -pedantic \
		-I$(abs_top_srcdir)/include $(GTEST_CFLAGS) $(GLM_CFLAGS)
integration_test_LDADD = $(GTEST_LIBS) \
		$(abs_top_builddir)/lib/libvulkan_graphix.la \
		-lboost_log -lboost_system -lboost_thread \
		$(VULKAN_LIBS) $(X11_LIBS)
integration_test_LDFLAGS = -pthread

# --- Shader copying for integration_test (mirrors bin/Makefile.am) ---
integration_tutorial03_vert = $(top_srcdir)/resources/03/Data/shader.vert.03.spv
integration_tutorial03_frag = $(top_srcdir)/resources/03/Data/shader.frag.03.spv
integration_tutorial04_vert = $(top_srcdir)/resources/04/Data/shader.vert.04.spv
integration_tutorial04_frag = $(top_srcdir)/resources/04/Data/shader.frag.04.spv
integration_tutorial05_vert = $(top_srcdir)/resources/05/Data/shader.vert.05.spv
integration_tutorial05_frag = $(top_srcdir)/resources/05/Data/shader.frag.05.spv

all-local: $(integration_tutorial03_vert) $(integration_tutorial03_frag) \
		$(integration_tutorial04_vert) $(integration_tutorial04_frag) \
		$(integration_tutorial05_vert) $(integration_tutorial05_frag)
	cp $(integration_tutorial03_vert) $(builddir)/shader.03.vert.spv
	cp $(integration_tutorial03_frag) $(builddir)/shader.03.frag.spv
	cp $(integration_tutorial04_vert) $(builddir)/shader.04.vert.spv
	cp $(integration_tutorial04_frag) $(builddir)/shader.04.frag.spv
	cp $(integration_tutorial05_vert) $(builddir)/shader.05.vert.spv
	cp $(integration_tutorial05_frag) $(builddir)/shader.05.frag.spv

if COVERAGE_ENABLED
vertex_types_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
vertex_types_test_LDFLAGS += $(COVERAGE_LDFLAGS)
math_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
math_test_LDFLAGS += $(COVERAGE_LDFLAGS)
integration_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
integration_test_LDFLAGS += $(COVERAGE_LDFLAGS)
endif
endif

CLEANFILES = *.o
