bin_PROGRAMS = tutorial18_runner

tutorial18_runner_SOURCES = ./main.cpp
tutorial18_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial18_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial18_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/18/Data/shader.vert.18_3d.spv \
    $(resources)/18/Data/shader.frag.18_3d.spv \
    $(resources)/18/Data/shader.vert.18_hud.spv \
    $(resources)/18/Data/shader.frag.18_hud.spv \
    $(resources)/16/Data/Hellfire_Body.ogl \
    $(resources)/16/Data/Hellfire_Head.ogl \
    $(resources)/16/Data/Hellfire_Turret.ogl \
    $(resources)/16/Data/TestImage.raw
	cp $(resources)/18/Data/shader.vert.18_3d.spv $(builddir)/shader.18_3d.vert.spv
	cp $(resources)/18/Data/shader.frag.18_3d.spv $(builddir)/shader.18_3d.frag.spv
	cp $(resources)/18/Data/shader.vert.18_hud.spv $(builddir)/shader.18_hud.vert.spv
	cp $(resources)/18/Data/shader.frag.18_hud.spv $(builddir)/shader.18_hud.frag.spv
	cp $(resources)/16/Data/Hellfire_Body.ogl $(builddir)/Hellfire_Body.ogl
	cp $(resources)/16/Data/Hellfire_Head.ogl $(builddir)/Hellfire_Head.ogl
	cp $(resources)/16/Data/Hellfire_Turret.ogl $(builddir)/Hellfire_Turret.ogl
	cp $(resources)/16/Data/TestImage.raw $(builddir)/TestImage.raw

CLEANFILES = shader.18_3d.vert.spv \
    shader.18_3d.frag.spv \
    shader.18_hud.vert.spv \
    shader.18_hud.frag.spv \
    Hellfire_Body.ogl \
    Hellfire_Head.ogl \
    Hellfire_Turret.ogl \
    TestImage.raw
