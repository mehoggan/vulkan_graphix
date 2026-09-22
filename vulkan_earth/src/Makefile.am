bin_PROGRAMS = vulkan_earth_runner vk_skybox_pilot_runner

# Vendored legacy OpenGL/GLUT game, ported off MSVC/Win32 onto X11 via
# GLUT rather than held to this project's own -Werror/-Wall/-pedantic
# policy (see the top-level CLAUDE.md) - it predates this codebase and
# isn't ours to bring up to that bar in one pass.
vulkan_earth_runner_SOURCES = \
	ChaseCam.cpp \
	ControlItem.cpp \
	ControlItemButton.cpp \
	ControlItemCheckBox.cpp \
	ControlItemGrid.cpp \
	ControlItemSelectionBox.cpp \
	ControlItemSliderbar.cpp \
	ControlItemTextField.cpp \
	Explosion.cpp \
	GameState.cpp \
	GlobalSettings.cpp \
	ImageObject.cpp \
	Inventory.cpp \
	Item.cpp \
	ItemAntiAcid.cpp \
	ItemBigRepair.cpp \
	ItemCloak.cpp \
	ItemDoubleAction.cpp \
	ItemExtraBattery.cpp \
	ItemFloat.cpp \
	ItemShield.cpp \
	ItemSmallRepair.cpp \
	ItemTest.cpp \
	LoadingScreen.cpp \
	MainMenu.cpp \
	MainMenuButton.cpp \
	Normal.cpp \
	Particle.cpp \
	ParticleAcid.cpp \
	ParticleFloat.cpp \
	ParticleGenerator.cpp \
	ParticleSmoke.cpp \
	Player.cpp \
	PlayerCPU.cpp \
	PlayerFactory.cpp \
	PlayerHuman.cpp \
	Projectile.cpp \
	ReadyMenu.cpp \
	vulkan_earth.cpp \
	Shader.cpp \
	ShopMenu.cpp \
	SkyboxFactory.cpp \
	sound.cpp \
	SpecialEffect.cpp \
	SubMenu.cpp \
	SubMenuEconomics.cpp \
	SubMenuHardware.cpp \
	SubMenuLandscape.cpp \
	SubMenuPhysics.cpp \
	SubMenuPlayOptions.cpp \
	SubMenuSelectTanks.cpp \
	SubMenuSound.cpp \
	SubMenuTest.cpp \
	SubMenuWeapons.cpp \
	Tank.cpp \
	TankA.cpp \
	TankB.cpp \
	TankC.cpp \
	TankD.cpp \
	TankE.cpp \
	TankF.cpp \
	TankG.cpp \
	TankH.cpp \
	TerrainMaker.cpp \
	TexCoord.cpp \
	TextObject.cpp \
	VBOQualifer.cpp \
	VBOShaderLibrary.cpp \
	Vector.cpp \
	Vertex.cpp \
	Water.cpp \
	Weapon.cpp \
	WeaponAcid.cpp \
	WeaponAtom.cpp \
	WeaponBFB.cpp \
	WeaponDefault.cpp \
	WeaponEMP.cpp \
	WeaponMFB.cpp \
	WeaponNuke.cpp \
	WeaponPadlock.cpp \
	WeaponRevive.cpp \
	WeaponTeleport.cpp \
	WeaponTest.cpp \
	WeaponThor.cpp \
	WorldCam.cpp

vulkan_earth_runner_CPPFLAGS = \
	$(VULKAN_EARTH_GL_CFLAGS) $(VULKAN_EARTH_GLEW_CFLAGS) $(VULKAN_EARTH_SDL_CFLAGS) \
	$(VULKAN_EARTH_XRANDR_CFLAGS)
vulkan_earth_runner_LDADD = \
	$(VULKAN_EARTH_GL_LIBS) $(VULKAN_EARTH_GLEW_LIBS) $(VULKAN_EARTH_SDL_LIBS) \
	$(VULKAN_EARTH_XRANDR_LIBS) $(X11_LIBS)
vulkan_earth_runner_LDFLAGS = -pthread

# Standalone Vulkan-migration pilot (see VulkanSkyboxPilot.h) - additive
# only, doesn't touch or get linked into vulkan_earth_runner above. Built
# against this repo's own tutorial framework (lib/libvulkan_graphix.la),
# not any of the vulkan_earth_runner GL/GLEW/SDL/Xrandr dependencies.
vk_skybox_pilot_runner_SOURCES = \
	VulkanSkyboxPilot.cpp \
	vk_skybox_pilot_main.cpp
vk_skybox_pilot_runner_CPPFLAGS = -Werror -Wall -pedantic \
	-I$(abs_top_srcdir)/include $(GLM_CFLAGS)
vk_skybox_pilot_runner_LDADD = \
	$(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
vk_skybox_pilot_runner_LDFLAGS = -pthread

# shader.07.vert.spv/shader.07.frag.spv are Tutorial07's compiled shaders,
# reused byte-for-byte (see VulkanSkyboxPilot::createPipeline()) - copied
# here rather than referenced in place since Tools::getBinaryFileContents()
# resolves shader filenames relative to the running executable's own
# directory, same convention bin/Makefile.am uses for every tutorial.
vk_skybox_pilot_vert = $(top_srcdir)/resources/07/Data/shader.vert.07.spv
vk_skybox_pilot_frag = $(top_srcdir)/resources/07/Data/shader.frag.07.spv

all-local: $(vk_skybox_pilot_vert) $(vk_skybox_pilot_frag)
	cp $(vk_skybox_pilot_vert) $(builddir)/shader.07.vert.spv
	cp $(vk_skybox_pilot_frag) $(builddir)/shader.07.frag.spv

CLEANFILES = shader.07.vert.spv shader.07.frag.spv
