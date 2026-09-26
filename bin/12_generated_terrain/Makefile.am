bin_PROGRAMS = tutorial12_runner

tutorial12_runner_SOURCES = ./main.cpp
tutorial12_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial12_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial12_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/12/Data/shader.vert.12.spv \
    $(resources)/12/Data/shader.frag.12.spv \
    $(resources)/12/Data/Rocky.raw
	cp $(resources)/12/Data/shader.vert.12.spv $(builddir)/shader.12.vert.spv
	cp $(resources)/12/Data/shader.frag.12.spv $(builddir)/shader.12.frag.spv
	cp $(resources)/12/Data/Rocky.raw $(builddir)/Rocky.raw

CLEANFILES = shader.12.vert.spv \
    shader.12.frag.spv \
    Rocky.raw
