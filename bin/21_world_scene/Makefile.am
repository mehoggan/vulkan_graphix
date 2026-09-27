bin_PROGRAMS = tutorial21_runner

tutorial21_runner_SOURCES = ./main.cpp
tutorial21_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial21_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial21_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/21/Data/shader.vert.21_terrain.spv \
    $(resources)/21/Data/shader.frag.21_terrain.spv \
    $(resources)/21/Data/shader.vert.21_object.spv \
    $(resources)/21/Data/shader.frag.21_object.spv \
    $(resources)/11/Data/SkyBox.jpg \
    $(resources)/12/Data/Rocky.raw \
    $(resources)/16/Data/Hellfire_Body.ogl \
    $(resources)/16/Data/Hellfire_Head.ogl \
    $(resources)/16/Data/Hellfire_Turret.ogl \
    $(resources)/16/Data/TestImage.raw
	cp $(resources)/21/Data/shader.vert.21_terrain.spv $(builddir)/shader.21_terrain.vert.spv
	cp $(resources)/21/Data/shader.frag.21_terrain.spv $(builddir)/shader.21_terrain.frag.spv
	cp $(resources)/21/Data/shader.vert.21_object.spv $(builddir)/shader.21_object.vert.spv
	cp $(resources)/21/Data/shader.frag.21_object.spv $(builddir)/shader.21_object.frag.spv
	cp $(resources)/11/Data/SkyBox.jpg $(builddir)/SkyBox.jpg
	cp $(resources)/12/Data/Rocky.raw $(builddir)/Rocky.raw
	cp $(resources)/16/Data/Hellfire_Body.ogl $(builddir)/Hellfire_Body.ogl
	cp $(resources)/16/Data/Hellfire_Head.ogl $(builddir)/Hellfire_Head.ogl
	cp $(resources)/16/Data/Hellfire_Turret.ogl $(builddir)/Hellfire_Turret.ogl
	cp $(resources)/16/Data/TestImage.raw $(builddir)/TestImage.raw

CLEANFILES = shader.21_terrain.vert.spv \
    shader.21_terrain.frag.spv \
    shader.21_object.vert.spv \
    shader.21_object.frag.spv \
    SkyBox.jpg \
    Rocky.raw \
    Hellfire_Body.ogl \
    Hellfire_Head.ogl \
    Hellfire_Turret.ogl \
    TestImage.raw
