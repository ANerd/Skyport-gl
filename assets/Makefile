
TARGETS:=geometry/hexborder.gen.vbo geometry/hextile.gen.vbo geometry/skybox.gen.vbo textures/tiles.gen.png textures/emblems.gen.png textures/sky.gen.png textures/figure.gen.png textures/laser.gen.png textures/mortar.gen.png textures/droid.gen.png textures/meteor.gen.png textures/explosion.gen.png sound/laser.wav sound/wind.wav textures/icons.gen.png
CFLAGS:= -lm -ggdb
MKDIR:=mkdir -p
CP:=cp
IMGCONVERT:=convert
IMGCONVERTFLAGS:= -define png:bit-depth=8
ifndef EXTASSETSDIR
    $(error Need external assets. Download Skyport-assets and set EXTASSETSDIR in Makefile.)
endif

EXTTEXDIR:=$(EXTASSETSDIR)/png
TILESRC:=$(sort $(wildcard ../$(EXTTEXDIR)/tiles/*-tile.png))
EMBLEMSRC:=$(sort $(wildcard ../$(EXTTEXDIR)/billboards/*-billboard.png))


ifndef TILESRC
    $(error External assets not found.)
endif

all: $(TARGETS)

textures/sky.gen.png: ../$(EXTTEXDIR)/skybox/skybox.png
	$(CP) $< $@
textures/figure.gen.png: ../$(EXTTEXDIR)/char/walking.png ../$(EXTTEXDIR)/char/jernkjempen-destroyed.png ../$(EXTTEXDIR)/char/Jern-kjempen.png
	$(MKDIR) $(@D)
	$(IMGCONVERT) $(IMGCONVERTFLAGS) $^ -append $@

textures/icons.gen.png: ../$(EXTTEXDIR)/icons/explosium.png ../$(EXTTEXDIR)/icons/rubidium.png ../$(EXTTEXDIR)/icons/scrap.png
	$(MKDIR) $(@D)
	$(IMGCONVERT) $(IMGCONVERTFLAGS) $^ +append $@

textures/laser.gen.png: ../$(EXTTEXDIR)/projectiles/laser.png Makefile
	$(MKDIR) $(@D)
	$(CP) $< $@

textures/mortar.gen.png: ../$(EXTTEXDIR)/projectiles/mortar.png Makefile
	$(MKDIR) $(@D)
	$(CP) $< $@

textures/droid.gen.png: ../$(EXTTEXDIR)/projectiles/droid.png Makefile
	$(MKDIR) $(@D)
	$(CP) $< $@

textures/explosion.gen.png: ../$(EXTTEXDIR)/misc/explosion-big.png Makefile
	$(MKDIR) $(@D)
	$(CP) $< $@

textures/meteor.gen.png: ../$(EXTTEXDIR)/misc/meteor.png Makefile
	$(MKDIR) $(@D)
	$(CP) $< $@

sound/laser.wav: ../$(EXTASSETSDIR)/snd/8-bit/laser.wav
	$(MKDIR) $(@D)
	$(CP) $< $@

sound/wind.wav: ../$(EXTASSETSDIR)/snd/8-bit/silence.wav
	$(MKDIR) $(@D)
	$(CP) $< $@

textures/tiles.gen.png: $(TILESRC)
	$(MKDIR) $(@D)
	$(IMGCONVERT) $^ -append $@

textures/emblems.gen.png: $(EMBLEMSRC)
	$(MKDIR) $(@D)
	$(IMGCONVERT) $^ -append $@

geometry/hextile.gen.vbo: bin/hextilegen Makefile
	$(MKDIR) $(@D)
	$< $@

geometry/hexborder.gen.vbo: bin/hexbordergen Makefile
	$(MKDIR) $(@D)
	$< $@

geometry/skybox.gen.vbo: bin/skyboxgen Makefile
	$(MKDIR) $(@D)
	$< $@

bin/hextilegen: generators/hextilegen.c Makefile
	$(MKDIR) $(@D)
	$(CC) $(CFLAGS) $< -o $@
bin/skyboxgen: generators/skyboxgen.c Makefile
	$(MKDIR) $(@D)
	$(CC) $(CFLAGS) $< -o $@
bin/hexbordergen: generators/hexbordergen.c Makefile
	$(MKDIR) $(@D)
	$(CC) $(CFLAGS) $< -o $@

.PHONY:
clean:
	$(RM) bin/* $(shell find . -name "*.gen.*")
