bin_PROGRAMS = tutorial05_runner

tutorial05_runner_SOURCES = ./main.cpp
tutorial05_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial05_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial05_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/05/Data/shader.vert.05.spv \
    $(resources)/05/Data/shader.frag.05.spv
	cp $(resources)/05/Data/shader.vert.05.spv $(builddir)/shader.05.vert.spv
	cp $(resources)/05/Data/shader.frag.05.spv $(builddir)/shader.05.frag.spv

CLEANFILES = shader.05.vert.spv \
    shader.05.frag.spv
