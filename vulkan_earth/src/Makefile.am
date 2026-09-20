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
