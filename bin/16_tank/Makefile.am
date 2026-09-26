bin_PROGRAMS = tutorial16_runner

tutorial16_runner_SOURCES = ./main.cpp
tutorial16_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial16_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial16_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/16/Data/shader.vert.16.spv \
    $(resources)/16/Data/shader.frag.16.spv \
    $(resources)/16/Data/Hellfire_Body.ogl \
    $(resources)/16/Data/Hellfire_Head.ogl \
    $(resources)/16/Data/Hellfire_Turret.ogl \
    $(resources)/16/Data/TestImage.raw
	cp $(resources)/16/Data/shader.vert.16.spv $(builddir)/shader.16.vert.spv
	cp $(resources)/16/Data/shader.frag.16.spv $(builddir)/shader.16.frag.spv
	cp $(resources)/16/Data/Hellfire_Body.ogl $(builddir)/Hellfire_Body.ogl
	cp $(resources)/16/Data/Hellfire_Head.ogl $(builddir)/Hellfire_Head.ogl
	cp $(resources)/16/Data/Hellfire_Turret.ogl $(builddir)/Hellfire_Turret.ogl
	cp $(resources)/16/Data/TestImage.raw $(builddir)/TestImage.raw

CLEANFILES = shader.16.vert.spv \
    shader.16.frag.spv \
    Hellfire_Body.ogl \
    Hellfire_Head.ogl \
    Hellfire_Turret.ogl \
    TestImage.raw
