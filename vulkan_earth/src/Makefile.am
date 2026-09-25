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

vulkan_earth_runner_CPPFLAGS = \
	$(VULKAN_EARTH_GL_CFLAGS) $(VULKAN_EARTH_GLEW_CFLAGS) $(VULKAN_EARTH_SDL_CFLAGS) \
	$(VULKAN_EARTH_XRANDR_CFLAGS)
vulkan_earth_runner_LDADD = \
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
# own (unmodified vendored code, see the comment above). For an
# out-of-tree build ($(srcdir) != $(builddir), e.g. a separate build/
# directory configured via ../configure - see the top-level CLAUDE.md)
# this copies resources/vulkan_earth/Data/'s contents into $(builddir)
# next to the built binary, the same role each Vulkan tutorial's own
# all-local: rule in bin/Makefile.am plays for itself. For an in-tree
# build the assets already sit right there (this subdir's own
# $(srcdir)), so both rules are guarded to do nothing - critically,
# never rm -rf anything here when $(srcdir) == $(builddir), since that
# would delete the vendored game's own original asset files instead of
# a copy of them.
all-local:
	@if test "$(srcdir)" != "$(builddir)"; then \
		mkdir -p $(builddir); \
		for entry in $(top_srcdir)/resources/vulkan_earth/Data/*; do \
			cp -r "$$entry" $(builddir)/; \
		done; \
	fi

clean-local:
	@if test "$(srcdir)" != "$(builddir)"; then \
		for entry in $(top_srcdir)/resources/vulkan_earth/Data/*; do \
			rm -rf $(builddir)/`basename "$$entry"`; \
		done; \
	fi
