bin_PROGRAMS = tutorial19_runner

tutorial19_runner_SOURCES = ./main.cpp
tutorial19_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial19_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial19_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/19/Data/shader.vert.19_3d.spv \
    $(resources)/19/Data/shader.frag.19_3d.spv \
    $(resources)/19/Data/shader.vert.19_grid.spv \
    $(resources)/19/Data/shader.frag.19_grid.spv \
    $(resources)/19/Data/projectileDefault.ogl \
    $(resources)/19/Data/projectileDefault.raw \
    $(resources)/19/Data/projectileAcid.ogl \
    $(resources)/19/Data/projectileAcid.raw \
    $(resources)/19/Data/projectileBFB.ogl \
    $(resources)/19/Data/projectileBFB.raw \
    $(resources)/19/Data/WeaponMFB.raw \
    $(resources)/19/Data/WeaponBFB.raw \
    $(resources)/19/Data/WeaponAcid.raw \
    $(resources)/19/Data/WeaponThor.raw \
    $(resources)/19/Data/WeaponEMP.raw \
    $(resources)/19/Data/WeaponPadlock.raw \
    $(resources)/19/Data/WeaponRevive.raw \
    $(resources)/19/Data/WeaponTeleport.raw \
    $(resources)/19/Data/WeaponAtom.raw \
    $(resources)/19/Data/WeaponNuke.raw
	cp $(resources)/19/Data/shader.vert.19_3d.spv $(builddir)/shader.19_3d.vert.spv
	cp $(resources)/19/Data/shader.frag.19_3d.spv $(builddir)/shader.19_3d.frag.spv
	cp $(resources)/19/Data/shader.vert.19_grid.spv $(builddir)/shader.19_grid.vert.spv
	cp $(resources)/19/Data/shader.frag.19_grid.spv $(builddir)/shader.19_grid.frag.spv
	cp $(resources)/19/Data/projectileDefault.ogl $(builddir)/projectileDefault.ogl
	cp $(resources)/19/Data/projectileDefault.raw $(builddir)/projectileDefault.raw
	cp $(resources)/19/Data/projectileAcid.ogl $(builddir)/projectileAcid.ogl
	cp $(resources)/19/Data/projectileAcid.raw $(builddir)/projectileAcid.raw
	cp $(resources)/19/Data/projectileBFB.ogl $(builddir)/projectileBFB.ogl
	cp $(resources)/19/Data/projectileBFB.raw $(builddir)/projectileBFB.raw
	cp $(resources)/19/Data/WeaponMFB.raw $(builddir)/WeaponMFB.raw
	cp $(resources)/19/Data/WeaponBFB.raw $(builddir)/WeaponBFB.raw
	cp $(resources)/19/Data/WeaponAcid.raw $(builddir)/WeaponAcid.raw
	cp $(resources)/19/Data/WeaponThor.raw $(builddir)/WeaponThor.raw
	cp $(resources)/19/Data/WeaponEMP.raw $(builddir)/WeaponEMP.raw
	cp $(resources)/19/Data/WeaponPadlock.raw $(builddir)/WeaponPadlock.raw
	cp $(resources)/19/Data/WeaponRevive.raw $(builddir)/WeaponRevive.raw
	cp $(resources)/19/Data/WeaponTeleport.raw $(builddir)/WeaponTeleport.raw
	cp $(resources)/19/Data/WeaponAtom.raw $(builddir)/WeaponAtom.raw
	cp $(resources)/19/Data/WeaponNuke.raw $(builddir)/WeaponNuke.raw

CLEANFILES = shader.19_3d.vert.spv \
    shader.19_3d.frag.spv \
    shader.19_grid.vert.spv \
    shader.19_grid.frag.spv \
    projectileDefault.ogl \
    projectileDefault.raw \
    projectileAcid.ogl \
    projectileAcid.raw \
    projectileBFB.ogl \
    projectileBFB.raw \
    WeaponMFB.raw \
    WeaponBFB.raw \
    WeaponAcid.raw \
    WeaponThor.raw \
    WeaponEMP.raw \
    WeaponPadlock.raw \
    WeaponRevive.raw \
    WeaponTeleport.raw \
    WeaponAtom.raw \
    WeaponNuke.raw
