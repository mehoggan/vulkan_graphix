bin_PROGRAMS = tutorial01_runner

tutorial01_runner_SOURCES = ./main.cpp
tutorial01_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial01_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial01_runner_LDFLAGS = -pthread
