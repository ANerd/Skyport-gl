CC := gcc
BINNAME := testbed
INPUTFILES := testbed.c

INCLUDECFLAGS := `pkg-config --cflags sdl SDL_ttf`
INCLUDELIBFLAGS := -I"." `pkg-config --libs sdl SDL_ttf` -lm
INCLUDEFLAGS := $(INCLUDECFLAGS) $(INCLUDELIBFLAGS)

# Flags in common by all
CFLAGS := -std=c99 -Wall -Wextra -pedantic -pedantic-errors -Wfloat-equal -Wundef -Wshadow \
-Winit-self -Winline -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wmissing-prototypes \
-Wwrite-strings -Wvla -Wswitch-enum -Wconversion -Wformat=2 -Wold-style-definition \
-Wunreachable-code -Wswitch-default -Wstrict-overflow -Warray-bounds

CNFLAGS := $(CFLAGS) -O3 -s

default: Makefile $(BINNAME)
$(BINNAME): $(INPUTFILES) textlib.o
	@echo "(CC) $<"
	@$(CC) $(CNFLAGS) $(INCLUDEFLAGS) $(INPUTFILES) textlib.o -o $(BINNAME)

textlib.o: textlib.c textlib.h
	@echo "(CC) $<"
	@$(CC) $(CNFLAGS) $(INCLUDEFLAGS) -c $< -o $@

run: default
	./$(BINNAME)
.PHONY: run
clean:
	@$(RM) $(BINNAME)
	@$(RM) *.o
	@$(RM) *.plist
