bin_PROGRAMS = tutorial03_runner

tutorial03_runner_SOURCES = ./main.cpp
tutorial03_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial03_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial03_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/03/Data/shader.vert.03.spv \
    $(resources)/03/Data/shader.frag.03.spv
	cp $(resources)/03/Data/shader.vert.03.spv $(builddir)/shader.03.vert.spv
	cp $(resources)/03/Data/shader.frag.03.spv $(builddir)/shader.03.frag.spv

CLEANFILES = shader.03.vert.spv \
    shader.03.frag.spv
