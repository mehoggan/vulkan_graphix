bin_PROGRAMS = tutorial14_runner

tutorial14_runner_SOURCES = ./main.cpp
tutorial14_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial14_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial14_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/14/Data/shader.vert.14.spv \
    $(resources)/14/Data/shader.frag.14.spv
	cp $(resources)/14/Data/shader.vert.14.spv $(builddir)/shader.14.vert.spv
	cp $(resources)/14/Data/shader.frag.14.spv $(builddir)/shader.14.frag.spv

CLEANFILES = shader.14.vert.spv \
    shader.14.frag.spv
