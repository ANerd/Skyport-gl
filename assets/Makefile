
TARGETS:=geometry/hexborder.gen.vbo geometry/hextile.gen.vbo geometry/skybox.gen.vbo textures/tiles.gen.png textures/emblems.gen.png textures/sky.gen.png textures/figure.gen.png textures/laser.gen.png
CFLAGS:= -lm -ggdb
MKDIR:=mkdir -p
CP:=cp
IMGCONVERT:=convert
ifndef EXTASSETSDIR
    $(error Need external assets. Download Skyport-assets and set EXTASSETSDIR in Makefile.)
endif

TILESRC:=$(sort $(wildcard ../$(EXTASSETSDIR)/tiles/*-tile.png))
EMBLEMSRC:=$(sort $(wildcard ../$(EXTASSETSDIR)/billboards/*-billboard.png))

ifndef TILESRC
    $(error External assets not found.)
endif

all: $(TARGETS)

textures/sky.gen.png: ../$(EXTASSETSDIR)/skybox/skybox.png
	$(CP) $< $@
textures/figure.gen.png: ../$(EXTASSETSDIR)/char/Jern-kjempen.png
	$(CP) $< $@

textures/laser.gen.png: ../$(EXTASSETSDIR)/projectiles/laser.png Makefile
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
