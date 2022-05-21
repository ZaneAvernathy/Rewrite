
# Ensure that our environment is set up correctly.

# Ensure that dkA is installed.
ifeq ($(strip $(DEVKITARM)),)
  $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_tools

# One day I'll set up a Linux VM to test this.

ifeq ($(OS),Windows_NT)
  EXE    := .exe
  COMPAT := 
else
  EXE    := 
  COMPAT := wine
endif

# Folders

# This requires that this `Tools.mak` file is in our project root.
ROOT := $(realpath .)

DESTDIR  := $(ROOT)/BIN
SRCDIR   := $(ROOT)/SRC
CACHEDIR := $(ROOT)/.CACHE

EADIR   := $(ROOT)/../Tools/EventAssembler
CLIBDIR := $(ROOT)/../Tools/CLib

# Tools

EADEP := $(EADIR)/Tools/ea-dep$(EXE)
LYN   := $(EADIR)/Tools/lyn$(EXE)

# These must be run with $(COMPAT) on non-Windows but are listed
# here without it because we do some `cd` shenanigans elsewhere.
EA_NL := $(EADIR)/Core.exe
EA_CC := $(EADIR)/ColorzCore.exe

NOTIFY_PROCESS = @echo "$(notdir $<) => $(notdir $@)"

# Special directories

$(DESTDIR):
	@mkdir -p $(DESTDIR)

$(CACHEDIR):
	@mkdir -p $(CACHEDIR)

veryclean:: clean
	@$(RM) $(CACHEDIR)/*
	@$(RM) -d $(CACHEDIR)
