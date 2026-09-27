bin_PROGRAMS = tutorial09_runner

tutorial09_runner_SOURCES = ./main.cpp
tutorial09_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial09_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial09_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/09/Data/shader.vert.09.spv \
    $(resources)/09/Data/shader.frag.09.spv \
    $(resources)/09/Data/texture.09.png
	cp $(resources)/09/Data/shader.vert.09.spv $(builddir)/shader.09.vert.spv
	cp $(resources)/09/Data/shader.frag.09.spv $(builddir)/shader.09.frag.spv
	cp $(resources)/09/Data/texture.09.png $(builddir)/texture.09.png

CLEANFILES = shader.09.vert.spv \
    shader.09.frag.spv \
    texture.09.png
