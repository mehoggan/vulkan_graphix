bin_PROGRAMS = tutorial08_runner

tutorial08_runner_SOURCES = ./main.cpp
tutorial08_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial08_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial08_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/08/Data/shader.vert.08.spv \
    $(resources)/08/Data/shader.frag.08.spv
	cp $(resources)/08/Data/shader.vert.08.spv $(builddir)/shader.08.vert.spv
	cp $(resources)/08/Data/shader.frag.08.spv $(builddir)/shader.08.frag.spv

CLEANFILES = shader.08.vert.spv \
    shader.08.frag.spv
