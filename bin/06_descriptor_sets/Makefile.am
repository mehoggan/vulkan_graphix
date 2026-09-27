bin_PROGRAMS = tutorial06_runner

tutorial06_runner_SOURCES = ./main.cpp
tutorial06_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial06_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial06_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/06/Data/shader.vert.06.spv \
    $(resources)/06/Data/shader.frag.06.spv \
    $(resources)/06/Data/texture.06.png
	cp $(resources)/06/Data/shader.vert.06.spv $(builddir)/shader.06.vert.spv
	cp $(resources)/06/Data/shader.frag.06.spv $(builddir)/shader.06.frag.spv
	cp $(resources)/06/Data/texture.06.png $(builddir)/texture.06.png

CLEANFILES = shader.06.vert.spv \
    shader.06.frag.spv \
    texture.06.png
