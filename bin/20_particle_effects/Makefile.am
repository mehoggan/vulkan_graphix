bin_PROGRAMS = tutorial20_runner

tutorial20_runner_SOURCES = ./main.cpp
tutorial20_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial20_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial20_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/20/Data/shader.vert.20.spv \
    $(resources)/20/Data/shader.frag.20.spv
	cp $(resources)/20/Data/shader.vert.20.spv $(builddir)/shader.20.vert.spv
	cp $(resources)/20/Data/shader.frag.20.spv $(builddir)/shader.20.frag.spv

CLEANFILES = shader.20.vert.spv \
    shader.20.frag.spv
