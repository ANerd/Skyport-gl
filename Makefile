CPPC:=g++
AR:=ar
MKDIR:=mkdir -p
RM:=rm -f
ECHO:=echo
LD:=g++
ENGINEDIR:=../ANEngine
ENGINEINC:=$(ENGINEDIR)/include
ENGINELIB:=$(ENGINEDIR)/bin
INCFLAGS:= -I$(ENGINEINC) $(shell pkg-config --cflags sdl) $(shell libpng-config --I_opts)
CPPFLAGS:= -c -Wall -pthread -std=c++11 -ggdb $(shell libpng-config --ccopts --cppflags) $(INCFLAGS)
LDFLAGS:= -pthread -L$(ENGINELIB) 
LIBFLAGS:= -lanengine $(shell pkg-config --libs sdl) $(shell libpng-config --libs) -lGL
ARFLAGS:= rcs
MMFLAGS:= $(INCFLAGS) -std=c++11
BINDIR:=bin
CPPSRCFILES:=$(shell find -mindepth 0 -maxdepth 3 -name "*.cpp")
OBJFILES:=$(patsubst %.cpp, $(BINDIR)/%.o, $(CPPSRCFILES))
DEPS:=$(OBJFILES:.o=.d)
TARGET:=skyport-gl

.PHONY: all
all: $(TARGET)
	@$(ECHO) "Build successfull!"

-include $(DEPS)

.PHONY: engine
engine:
	@$(MAKE) -C $(ENGINEDIR)

$(TARGET): $(OBJFILES)
	@$(ECHO) " (LD) " $@
	@$(LD) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

$(BINDIR)/%.o: %.cpp Makefile engine
	@$(MKDIR) $(@D)
	@$(ECHO) " (CC) " $@
	@$(CPPC) $(CPPFLAGS) -o $@ $<
	@$(CPPC) -MM -MT $@ $(MMFLAGS) $< > $(BINDIR)/$*.d

.PHONY: clean

clean:
	$(RM) $(TARGET) $(shell find $(BINDIR)/ -name "*.o" -o -name "*.d")
