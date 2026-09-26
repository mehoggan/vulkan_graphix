bin_PROGRAMS = tutorial07_runner

tutorial07_runner_SOURCES = ./main.cpp
tutorial07_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial07_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial07_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/07/Data/shader.vert.07.spv \
    $(resources)/07/Data/shader.frag.07.spv \
    $(resources)/07/Data/texture.07.png
	cp $(resources)/07/Data/shader.vert.07.spv $(builddir)/shader.07.vert.spv
	cp $(resources)/07/Data/shader.frag.07.spv $(builddir)/shader.07.frag.spv
	cp $(resources)/07/Data/texture.07.png $(builddir)/texture.07.png

CLEANFILES = shader.07.vert.spv \
    shader.07.frag.spv \
    texture.07.png
