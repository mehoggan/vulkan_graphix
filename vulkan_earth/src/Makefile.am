bin_PROGRAMS = vulkan_earth_runner

# Vendored legacy game, originally OpenGL/GLUT on MSVC/Win32, now running
# on Vulkan through libvulkan_graphix's TutorialBase bring-up and its own
# renderer (render/), rather than held to this project's own
# -Werror/-Wall/-pedantic policy (see the top-level CLAUDE.md) - it
# predates this codebase and isn't ours to bring up to that bar in one
# pass.
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
	ParticleGenerator.cpp \
	Player.cpp \
	PlayerCPU.cpp \
	PlayerFactory.cpp \
	PlayerHuman.cpp \
	Projectile.cpp \
	ReadyMenu.cpp \
	VulkanEarth.cpp \
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
	WorldCam.cpp \
	render/Camera.cpp \
	render/Font.cpp \
	render/GlMatrix.cpp \
	render/Mesh.cpp \
	render/Renderer.cpp \
	render/Texture.cpp \
	render/UiBuilders.cpp

# The game calls into libvulkan_graphix for everything it shares with the
# Vulkan tutorials (terrain height field/queries/craters, ballistics, tank
# orientation) instead of keeping its own copies - see CLAUDE.md.
vulkan_earth_runner_CPPFLAGS = \
	-I$(top_srcdir)/vulkan_earth/include \
	-I$(abs_top_srcdir)/include $(GLM_CFLAGS) \
	$(VULKAN_EARTH_SDL_CFLAGS) $(VULKAN_EARTH_XRANDR_CFLAGS)
vulkan_earth_runner_LDADD = \
	$(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) \
	$(VULKAN_EARTH_SDL_LIBS) $(VULKAN_EARTH_XRANDR_LIBS) $(X11_LIBS)
vulkan_earth_runner_LDFLAGS = -pthread

# All four Vulkan-migration pilots (skybox, terrain, projectile, particle)
# that used to live here as standalone additive binaries have since
# graduated to Tutorial11-14 - see bin/Makefile.am and each tutorial's own
# resources/NN/Data/.

# The game's runtime assets (textures, meshes, SPIR-V shaders compiled from
# resources/vulkan_earth/Shaders/, GLUT font atlases, sounds) live only
# in resources/vulkan_earth/Data/, deliberately kept separate from any
# Vulkan tutorial's own resources/NN/Data/ even where the content overlaps
# (e.g. Hellfire_*.ogl, TestImage.raw) - the two sides are free to diverge
# independently. The game itself loads everything by bare relative
# filename (e.g. "SkyBox.raw", "Projectiles/projectileDefault.ogl") with
# no path-prefix logic of its own (unmodified vendored code, see the
# comment above), so all-local: copies that whole tree next to the built
# binary - into $(builddir) for an out-of-tree build (CLAUDE.md's build/),
# and into this subdir itself for an in-tree one, where the copies are
# gitignored.
#
# Both rules work file by file, never on whole directories: in-tree,
# $(builddir) is this source directory, and some asset directories also
# hold tracked files of their own (Predator/'s .ac/.x3d source models), so
# clean-local removes exactly the files resources/vulkan_earth/Data/
# provides, then only the directories that are left empty.
game_assets = $(top_srcdir)/resources/vulkan_earth/Data

all-local:
	@mkdir -p $(builddir)
	@cd $(game_assets) && find . -type f | while read -r asset; do \
		mkdir -p "$(abs_builddir)/`dirname "$$asset"`"; \
		cp "$$asset" "$(abs_builddir)/$$asset"; \
	done

clean-local:
	@cd $(game_assets) && find . -type f | while read -r asset; do \
		rm -f "$(abs_builddir)/$$asset"; \
	done
	@cd $(game_assets) && find . -mindepth 1 -type d | sort -r | \
		while read -r dir; do \
			rmdir "$(abs_builddir)/$$dir" 2>/dev/null || true; \
		done
