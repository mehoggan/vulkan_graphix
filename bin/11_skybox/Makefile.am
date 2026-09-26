bin_PROGRAMS = tutorial11_runner

tutorial11_runner_SOURCES = ./main.cpp
tutorial11_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial11_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial11_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/11/Data/shader.vert.11.spv \
    $(resources)/11/Data/shader.frag.11.spv \
    $(resources)/11/Data/SkyBox.jpg
	cp $(resources)/11/Data/shader.vert.11.spv $(builddir)/shader.11.vert.spv
	cp $(resources)/11/Data/shader.frag.11.spv $(builddir)/shader.11.frag.spv
	cp $(resources)/11/Data/SkyBox.jpg $(builddir)/SkyBox.jpg

CLEANFILES = shader.11.vert.spv \
    shader.11.frag.spv \
    SkyBox.jpg
