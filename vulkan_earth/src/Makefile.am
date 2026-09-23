bin_PROGRAMS = vulkan_earth_runner \
	vk_projectile_pilot_runner vk_particle_pilot_runner

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
	VulkanEarth.cpp \
	Shader.cpp \
	ShopMenu.cpp \
	SkyboxFactory.cpp \
	Sound.cpp \
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

# Standalone Vulkan-migration pilot #3 (see VulkanProjectilePilot.h) -
# additive only, doesn't touch or get linked into vulkan_earth_runner
# above. Built against this repo's own tutorial framework
# (lib/libvulkan_graphix.la), not any of the vulkan_earth_runner
# GL/GLEW/SDL/Xrandr dependencies. Pilots #1 (skybox) and #2 (terrain)
# have since graduated to Tutorial11/12 - see bin/Makefile.am.
vk_projectile_pilot_runner_SOURCES = \
	VulkanProjectilePilot.cpp \
	vk_projectile_pilot_main.cpp
vk_projectile_pilot_runner_CPPFLAGS = -Werror -Wall -pedantic \
	-I$(abs_top_srcdir)/include $(GLM_CFLAGS)
vk_projectile_pilot_runner_LDADD = \
	$(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
vk_projectile_pilot_runner_LDFLAGS = -pthread

# Standalone Vulkan-migration pilot #4 (see VulkanParticlePilot.h) - same
# additive-only setup as vk_skybox_pilot_runner above.
vk_particle_pilot_runner_SOURCES = \
	VulkanParticlePilot.cpp \
	vk_particle_pilot_main.cpp
vk_particle_pilot_runner_CPPFLAGS = -Werror -Wall -pedantic \
	-I$(abs_top_srcdir)/include $(GLM_CFLAGS)
vk_particle_pilot_runner_LDADD = \
	$(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
vk_particle_pilot_runner_LDFLAGS = -pthread

# projectileDefault.ogl/projectileDefault.raw (the real tank-shell mesh and
# its texture, parsed/loaded by VulkanProjectilePilot) live one directory
# down, under Projectiles/ - copied flat since
# Tools::getBinaryFileContents()/getRawImageData() resolve filenames
# relative to the running executable's own directory, same convention
# bin/Makefile.am uses for every tutorial.
vk_projectile_pilot_mesh = $(srcdir)/Projectiles/projectileDefault.ogl
vk_projectile_pilot_texture = $(srcdir)/Projectiles/projectileDefault.raw

# shader.vert.spv/shader.frag.spv are this pilot's own new shaders (see
# VulkanParticlePilot.h) - not reused from any numbered tutorial, since
# none of them do alpha-blended flat-color Phong lighting. Compiled from
# resources/particle_pilot/Data/shader.{vert,frag} via glslc (glslang
# Validator isn't installed in this environment; glslc produces equivalent
# SPIR-V - see compile_shaders.sh for the project's usual glslangValidator
# invocation used for every numbered tutorial's shaders).
vk_particle_pilot_vert = $(top_srcdir)/resources/particle_pilot/Data/shader.vert.spv
vk_particle_pilot_frag = $(top_srcdir)/resources/particle_pilot/Data/shader.frag.spv

all-local: $(vk_projectile_pilot_mesh) $(vk_projectile_pilot_texture) \
		$(vk_particle_pilot_vert) $(vk_particle_pilot_frag)
	cp $(vk_projectile_pilot_mesh) $(builddir)/projectileDefault.ogl
	cp $(vk_projectile_pilot_texture) $(builddir)/projectileDefault.raw
	cp $(vk_particle_pilot_vert) $(builddir)/shader.particle.vert.spv
	cp $(vk_particle_pilot_frag) $(builddir)/shader.particle.frag.spv

CLEANFILES = projectileDefault.ogl projectileDefault.raw \
	shader.particle.vert.spv shader.particle.frag.spv
