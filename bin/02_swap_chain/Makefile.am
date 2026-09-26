bin_PROGRAMS = tutorial02_runner

tutorial02_runner_SOURCES = ./main.cpp
tutorial02_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial02_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial02_runner_LDFLAGS = -pthread
