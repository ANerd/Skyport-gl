CC := gcc
BINNAME := testbed
INPUTFILES := testbed.c

INCLUDECFLAGS := `pkg-config --cflags sdl SDL_mixer`
INCLUDELIBFLAGS := -I"." `pkg-config --libs sdl SDL_mixer` -lm
INCLUDEFLAGS := $(INCLUDECFLAGS) $(INCLUDELIBFLAGS)

# Flags in common by all
CFLAGS := -std=c99 -Wall -Wextra -pedantic -pedantic-errors -Wfloat-equal -Wundef -Wshadow \
-Winit-self -Winline -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wmissing-prototypes \
-Wwrite-strings -Wvla -Wswitch-enum -Wconversion -Wformat=2 -Wold-style-definition \
-Wunreachable-code -Wswitch-default -Wstrict-overflow -Warray-bounds

CNFLAGS := $(CFLAGS) -O3 -s -DSNDLIB_SOUND_DIR="\"sounds\"" #-DNO_SNDLIB # this flag disables sound globally.

default: Makefile $(BINNAME)
$(BINNAME): $(INPUTFILES) sndlib.o
	@echo "(CC) $<"
	@$(CC) $(CNFLAGS) $(INCLUDEFLAGS) $(INPUTFILES) sndlib.o -o $(BINNAME)

sndlib.o: sndlib.c sndlib.h
	@echo "(CC) $<"
	@$(CC) $(CNFLAGS) $(INCLUDEFLAGS) -c $< -o $@

run: default
	./$(BINNAME)
.PHONY: run
clean:
	@$(RM) $(BINNAME)
	@$(RM) *.o
	@$(RM) *.plist
