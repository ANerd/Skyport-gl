CPPC:=g++
CC:=gcc
AR:=ar
MKDIR:=mkdir -p
RM:=rm -f
ECHO:=echo
LD:=g++
EXTASSETSDIR:=../Skyport-assets/png
export EXTASSETSDIR

ENGINEDIR:=../ANEngine
ENGINEINC:=$(ENGINEDIR)/include
ENGINELIB:=$(ENGINEDIR)/bin/libanengine.a

ASSETSDIR:=assets

INCFLAGS:= -I$(ENGINEINC) $(shell pkg-config --cflags-only-I sdl SDL_ttf libpng gl json)
CPPFLAGS:= -I$(ENGINEINC) -DASSETSDIR="$(EXTASSETSDIR)" -c -Wall -pthread -std=c++11 -ggdb $(shell pkg-config --cflags sdl SDL_ttf libpng gl json)
LDFLAGS:= -pthread 
LIBFLAGS:= $(shell pkg-config --libs sdl SDL_ttf libpng glew gl json)
ARFLAGS:= rcs
MMFLAGS:= $(INCFLAGS) -std=c++11
MMCFLAGS:= $(INCFLAGS) -std=c99
BINDIR:=bin
CPPSRCFILES:=$(shell find -mindepth 0 -maxdepth 3 -name "*.cpp")
CSRCFILES:=textlib/textlib.c
OBJFILES:=$(patsubst %.cpp, $(BINDIR)/%.o, $(CPPSRCFILES)) $(patsubst %.c, $(BINDIR)/%.o, $(CSRCFILES))
DEPS:=$(OBJFILES:.o=.d)
TARGET:=skyport-gl

CFLAGS := -c $(INCFLAGS) -std=c99 $(shell pkg-config --cflags sdl SDL_ttf libpng)

.PHONY: all
all: $(TARGET) assets
	@$(ECHO) "Build successfull!"

-include $(DEPS)

.PHONY: assets
assets: 
	@$(MAKE) -C $(ASSETSDIR)

.PHONY: force
force:
$(ENGINELIB): force
	@$(MAKE) -C $(ENGINEDIR)

$(TARGET): $(OBJFILES) $(ENGINELIB)
	@$(ECHO) " (LD) " $@
	@$(LD) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

$(BINDIR)/%.o: %.cpp Makefile 
	@$(MKDIR) $(@D)
	@$(ECHO) " (CPPC) " $@
	@$(CPPC) $(CPPFLAGS) -o $@ $<
	@$(CPPC) -MM -MT $@ $(MMFLAGS) $< > $(BINDIR)/$*.d

$(BINDIR)/%.o: %.c Makefile
	@$(MKDIR) $(@D)
	@$(ECHO) " (CC) " $@
	@$(CC) $(CFLAGS) -o $@ $<
	@$(CC) -MM -MT $@ $(MMCFLAGS) $< > $(BINDIR)/$*.d

.PHONY: clean

clean:
	$(RM) $(TARGET) $(shell find $(BINDIR)/ -name "*.o" -o -name "*.d")
	@$(MAKE) -C $(ASSETSDIR) clean
