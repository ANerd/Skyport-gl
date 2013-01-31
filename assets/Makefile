
TARGETS:=geometry/hextile.gen.vbo
CFLAGS:= -lm
MKDIR:=mkdir -p

all: $(TARGETS)

geometry/hextile.gen.vbo: bin/hextilegen Makefile
	$< $@

bin/hextilegen: generators/hextilegen.c Makefile
	$(MKDIR) $(@D)
	$(CC) $(CFLAGS) $< -o $@
