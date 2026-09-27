bin_PROGRAMS = tutorial13_runner

tutorial13_runner_SOURCES = ./main.cpp
tutorial13_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial13_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial13_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/13/Data/shader.vert.13.spv \
    $(resources)/13/Data/shader.frag.13.spv \
    $(resources)/13/Data/projectileDefault.ogl \
    $(resources)/13/Data/projectileDefault.raw
	cp $(resources)/13/Data/shader.vert.13.spv $(builddir)/shader.13.vert.spv
	cp $(resources)/13/Data/shader.frag.13.spv $(builddir)/shader.13.frag.spv
	cp $(resources)/13/Data/projectileDefault.ogl $(builddir)/projectileDefault.ogl
	cp $(resources)/13/Data/projectileDefault.raw $(builddir)/projectileDefault.raw

CLEANFILES = shader.13.vert.spv \
    shader.13.frag.spv \
    projectileDefault.ogl \
    projectileDefault.raw
