if HAVE_GTEST
TESTS = vertex_types_test math_test
check_PROGRAMS = vertex_types_test math_test

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

if COVERAGE_ENABLED
vertex_types_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
vertex_types_test_LDFLAGS += $(COVERAGE_LDFLAGS)
math_test_CPPFLAGS += $(COVERAGE_CXXFLAGS)
math_test_LDFLAGS += $(COVERAGE_LDFLAGS)
endif
endif

CLEANFILES = *.o
