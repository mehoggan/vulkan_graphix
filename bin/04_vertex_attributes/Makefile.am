bin_PROGRAMS = tutorial04_runner

tutorial04_runner_SOURCES = ./main.cpp
tutorial04_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial04_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial04_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/04/Data/shader.vert.04.spv \
    $(resources)/04/Data/shader.frag.04.spv
	cp $(resources)/04/Data/shader.vert.04.spv $(builddir)/shader.04.vert.spv
	cp $(resources)/04/Data/shader.frag.04.spv $(builddir)/shader.04.frag.spv

CLEANFILES = shader.04.vert.spv \
    shader.04.frag.spv
