bin_PROGRAMS = vulkan_earth_runner

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

# The game calls into libvulkan_graphix for everything it shares with the
# Vulkan tutorials (terrain height field/queries/craters, ballistics, tank
# orientation) instead of keeping its own copies - see CLAUDE.md.
vulkan_earth_runner_CPPFLAGS = \
	-I$(top_srcdir)/vulkan_earth/include \
	-I$(abs_top_srcdir)/include $(GLM_CFLAGS) \
	$(VULKAN_EARTH_GL_CFLAGS) $(VULKAN_EARTH_GLEW_CFLAGS) $(VULKAN_EARTH_SDL_CFLAGS) \
	$(VULKAN_EARTH_XRANDR_CFLAGS)
vulkan_earth_runner_LDADD = \
	$(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) \
	$(VULKAN_EARTH_GL_LIBS) $(VULKAN_EARTH_GLEW_LIBS) $(VULKAN_EARTH_SDL_LIBS) \
	$(VULKAN_EARTH_XRANDR_LIBS) $(X11_LIBS)
vulkan_earth_runner_LDFLAGS = -pthread

# All four Vulkan-migration pilots (skybox, terrain, projectile, particle)
# that used to live here as standalone additive binaries have since
# graduated to Tutorial11-14 - see bin/Makefile.am and each tutorial's own
# resources/NN/Data/.

# The game's own textures/meshes/shaders/sound files are duplicated (not
# shared/symlinked) into resources/vulkan_earth/Data/, deliberately kept
# separate from any Vulkan tutorial's own resources/NN/Data/ even where
# the content overlaps (e.g. Hellfire_*.ogl, TestImage.raw) - the two
# sides are free to diverge independently. The game itself still loads
# everything by bare relative filename (e.g. "SkyBox.raw",
# "Projectiles/projectileDefault.ogl") with no path-prefix logic of its
# own (unmodified vendored code, see the comment above), so every asset
# has to sit next to the built binary.
#
# The .raw textures live ONLY in resources/vulkan_earth/Data/ (and its
# Projectiles/ subdirectory); the other asset types still also have their
# vendored originals here in this subdir's $(srcdir).
#
# Out-of-tree build ($(srcdir) != $(builddir), e.g. a separate build/
# directory configured via ../configure - see the top-level CLAUDE.md):
# copy all of resources/vulkan_earth/Data/ into $(builddir), the same role
# each Vulkan tutorial's own all-local: rule plays for itself, and remove
# those copies again on clean.
#
# In-tree build ($(srcdir) == $(builddir)): every non-.raw asset already
# sits right here, so copy in just the .raw textures - and on clean,
# remove just those .raw copies. Never rm -rf whole Data/ entries here,
# since that would delete the vendored game's own original (tracked)
# non-.raw asset files instead of a copy of them.
raw_textures = $(top_srcdir)/resources/vulkan_earth/Data

all-local:
	@if test "$(srcdir)" != "$(builddir)"; then \
		mkdir -p $(builddir); \
		for entry in $(raw_textures)/*; do \
			cp -r "$$entry" $(builddir)/; \
		done; \
	else \
		mkdir -p $(builddir)/Projectiles; \
		cp $(raw_textures)/*.raw $(builddir)/; \
		cp $(raw_textures)/Projectiles/*.raw $(builddir)/Projectiles/; \
	fi

clean-local:
	@if test "$(srcdir)" != "$(builddir)"; then \
		for entry in $(raw_textures)/*; do \
			rm -rf $(builddir)/`basename "$$entry"`; \
		done; \
	else \
		for entry in $(raw_textures)/*.raw; do \
			rm -f $(builddir)/`basename "$$entry"`; \
		done; \
		for entry in $(raw_textures)/Projectiles/*.raw; do \
			rm -f $(builddir)/Projectiles/`basename "$$entry"`; \
		done; \
	fi
