bin_PROGRAMS = tutorial17_runner

tutorial17_runner_SOURCES = ./main.cpp
tutorial17_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include $(GLM_CFLAGS)
tutorial17_runner_LDADD = \
    $(abs_top_builddir)/lib/libvulkan_graphix.la $(VULKAN_LIBS) $(X11_LIBS)
tutorial17_runner_LDFLAGS = -pthread

# --- Asset Copying Logic ---
# Assets are resolved next to the executable (see
# Tools.cpp's executableDir()), so copy them into this
# tutorial's own build directory.
resources = $(top_srcdir)/resources

all-local: $(resources)/17/Data/shader.vert.17.spv \
    $(resources)/17/Data/shader.frag.17.spv \
    $(resources)/17/Data/ItemAntiAcid.raw \
    $(resources)/17/Data/ItemBigRepair.raw \
    $(resources)/17/Data/ItemCloak.raw \
    $(resources)/17/Data/ItemDoubleAction.raw \
    $(resources)/17/Data/ItemExtraBattery.raw \
    $(resources)/17/Data/ItemFloat.raw \
    $(resources)/17/Data/ItemShield.raw \
    $(resources)/17/Data/ItemSmallRepair.raw
	cp $(resources)/17/Data/shader.vert.17.spv $(builddir)/shader.17.vert.spv
	cp $(resources)/17/Data/shader.frag.17.spv $(builddir)/shader.17.frag.spv
	cp $(resources)/17/Data/ItemAntiAcid.raw $(builddir)/ItemAntiAcid.raw
	cp $(resources)/17/Data/ItemBigRepair.raw $(builddir)/ItemBigRepair.raw
	cp $(resources)/17/Data/ItemCloak.raw $(builddir)/ItemCloak.raw
	cp $(resources)/17/Data/ItemDoubleAction.raw $(builddir)/ItemDoubleAction.raw
	cp $(resources)/17/Data/ItemExtraBattery.raw $(builddir)/ItemExtraBattery.raw
	cp $(resources)/17/Data/ItemFloat.raw $(builddir)/ItemFloat.raw
	cp $(resources)/17/Data/ItemShield.raw $(builddir)/ItemShield.raw
	cp $(resources)/17/Data/ItemSmallRepair.raw $(builddir)/ItemSmallRepair.raw

CLEANFILES = shader.17.vert.spv \
    shader.17.frag.spv \
    ItemAntiAcid.raw \
    ItemBigRepair.raw \
    ItemCloak.raw \
    ItemDoubleAction.raw \
    ItemExtraBattery.raw \
    ItemFloat.raw \
    ItemShield.raw \
    ItemSmallRepair.raw
