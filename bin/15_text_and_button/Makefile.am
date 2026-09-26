bin_PROGRAMS = tutorial15_runner

tutorial15_runner_SOURCES = ./main.cpp
tutorial15_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial15_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial15_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/15/Data/shader.vert.15.spv \
    $(resources)/15/Data/shader.frag.15.spv
	cp $(resources)/15/Data/shader.vert.15.spv $(builddir)/shader.15.vert.spv
	cp $(resources)/15/Data/shader.frag.15.spv $(builddir)/shader.15.frag.spv

CLEANFILES = shader.15.vert.spv \
    shader.15.frag.spv
