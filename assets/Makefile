
TARGETS:=geometry/hextile.gen.vbo textures/tiles.gen.png textures/emblems.gen.png
CFLAGS:= -lm
MKDIR:=mkdir -p
IMGCONVERT:=convert
ifndef EXTASSETSDIR
    $(error Need external assets. Download Skyport-assets and set EXTASSETSDIR in Makefile.)
endif

TILESRC:=$(wildcard ../$(EXTASSETSDIR)/tiles/*-tile.png)
EMBLEMSRC:=$(wildcard ../$(EXTASSETSDIR)/billboards/*-billboard.png)

ifndef TILESRC
    $(error External assets not found.)
endif

all: $(TARGETS)

textures/tiles.gen.png: $(TILESRC)
	$(MKDIR) $(@D)
	$(IMGCONVERT) $? -append $@

textures/emblems.gen.png: $(EMBLEMSRC)
	$(MKDIR) $(@D)
	$(IMGCONVERT) $? -append $@

geometry/hextile.gen.vbo: bin/hextilegen Makefile
	$(MKDIR) $(@D)
	$< $@

bin/hextilegen: generators/hextilegen.c Makefile
	$(MKDIR) $(@D)
	$(CC) $(CFLAGS) $< -o $@

.PHONY:
clean:
	$(RM) bin/* $(shell find . -name "*.gen.*")
