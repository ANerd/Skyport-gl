CPPC:=g++
CC:=gcc
AR:=ar
MKDIR:=mkdir -p
RM:=rm -f
ECHO:=echo
LD:=g++
ENGINEDIR:=../ANEngine
ENGINEINC:=$(ENGINEDIR)/include
ENGINELIB:=$(ENGINEDIR)/bin/libanengine.a
INCFLAGS:= -I$(ENGINEINC) $(shell pkg-config --cflags-only-I sdl SDL_ttf libpng)
CPPFLAGS:= -I$(ENGINEINC) -c -Wall -pthread -std=c++11 -ggdb $(shell pkg-config --cflags sdl SDL_ttf libpng)
LDFLAGS:= -pthread 
LIBFLAGS:= $(shell pkg-config --libs sdl SDL_ttf libpng) -lGL
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
all: $(TARGET) 
	@$(ECHO) "Build successfull!"

-include $(DEPS)

.PHONY: force
force:
$(ENGINELIB): force
	@$(MAKE) -C $(ENGINEDIR)

$(TARGET): $(OBJFILES) $(ENGINELIB)
	@$(ECHO) " (LD) " $@
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

$(BINDIR)/%.o: %.cpp Makefile 
	@$(MKDIR) $(@D)
	@$(ECHO) " (CC) " $@
	@$(CPPC) $(CPPFLAGS) -o $@ $<
	@$(CPPC) -MM -MT $@ $(MMFLAGS) $< > $(BINDIR)/$*.d

$(BINDIR)/%.o: %.c Makefile
	@$(MKDIR) $(@D)
	@$(ECHO) " (CC) " $@
	$(CC) $(CFLAGS) -o $@ $<
	@$(CC) -MM -MT $@ $(MMCFLAGS) $< > $(BINDIR)/$*.d

.PHONY: clean

clean:
	$(RM) $(TARGET) $(shell find $(BINDIR)/ -name "*.o" -o -name "*.d")
