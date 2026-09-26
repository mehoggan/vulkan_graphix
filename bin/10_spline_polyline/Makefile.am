bin_PROGRAMS = tutorial10_runner

tutorial10_runner_SOURCES = ./main.cpp
tutorial10_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial10_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial10_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/10/Data/shader.vert.10_line.spv \
    $(resources)/10/Data/shader.frag.10_line.spv
	cp $(resources)/10/Data/shader.vert.10_line.spv $(builddir)/shader.10_line.vert.spv
	cp $(resources)/10/Data/shader.frag.10_line.spv $(builddir)/shader.10_line.frag.spv

CLEANFILES = shader.10_line.vert.spv \
    shader.10_line.frag.spv
