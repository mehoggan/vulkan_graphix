bin_PROGRAMS = tutorial22_runner

tutorial22_runner_SOURCES = ./main.cpp
tutorial22_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial22_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial22_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/22/Data/shader.vert.22_3d.spv \
    $(resources)/22/Data/shader.frag.22_3d.spv \
    $(resources)/22/Data/shader.vert.22_panel.spv \
    $(resources)/22/Data/shader.frag.22_panel.spv \
    $(resources)/16/Data/Hellfire_Body.ogl \
    $(resources)/16/Data/Hellfire_Head.ogl \
    $(resources)/16/Data/Hellfire_Turret.ogl \
    $(resources)/16/Data/TestImage.raw
	cp $(resources)/22/Data/shader.vert.22_3d.spv $(builddir)/shader.22_3d.vert.spv
	cp $(resources)/22/Data/shader.frag.22_3d.spv $(builddir)/shader.22_3d.frag.spv
	cp $(resources)/22/Data/shader.vert.22_panel.spv $(builddir)/shader.22_panel.vert.spv
	cp $(resources)/22/Data/shader.frag.22_panel.spv $(builddir)/shader.22_panel.frag.spv
	cp $(resources)/16/Data/Hellfire_Body.ogl $(builddir)/Hellfire_Body.ogl
	cp $(resources)/16/Data/Hellfire_Head.ogl $(builddir)/Hellfire_Head.ogl
	cp $(resources)/16/Data/Hellfire_Turret.ogl $(builddir)/Hellfire_Turret.ogl
	cp $(resources)/16/Data/TestImage.raw $(builddir)/TestImage.raw

CLEANFILES = shader.22_3d.vert.spv \
    shader.22_3d.frag.spv \
    shader.22_panel.vert.spv \
    shader.22_panel.frag.spv \
    Hellfire_Body.ogl \
    Hellfire_Head.ogl \
    Hellfire_Turret.ogl \
    TestImage.raw
