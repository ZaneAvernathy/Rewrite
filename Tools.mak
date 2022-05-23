
# Ensure that our environment is set up correctly.

# Ensure that dkA is installed.
ifeq ($(strip $(DEVKITARM)),)
  $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_tools

# One day I'll set up a Linux VM to test this.

ifeq ($(OS),Windows_NT)
  export EXE    := .exe
  export COMPAT := 
else
  export EXE    := 
  export COMPAT := wine
endif

# Folders

# This requires that this `Tools.mak` file is in our project root.
export ROOT := $(realpath .)

export DESTDIR  := $(ROOT)/BIN
export SRCDIR   := $(ROOT)/SRC
export CACHEDIR := $(ROOT)/.CACHE

export EADIR   := $(ROOT)/../Tools/EventAssembler
export CLIBDIR := $(ROOT)/../Tools/CLib

# Tools

export EADEP := $(EADIR)/Tools/ea-dep$(EXE)
export LYN   := $(EADIR)/Tools/lyn$(EXE)

# These must be run with $(COMPAT) on non-Windows but are listed
# here without it because we do some `cd` shenanigans elsewhere.
export EA_NL := $(EADIR)/Core.exe
export EA_CC := $(EADIR)/ColorzCore.exe

export NOTIFY_PROCESS = @echo "$(notdir $<) => $(notdir $@)"

# Special directories

$(DESTDIR):
	@mkdir -p $(DESTDIR)

$(CACHEDIR):
	@mkdir -p $(CACHEDIR)

veryclean:: clean
	@$(RM) $(CACHEDIR)/*
	@$(RM) -d $(CACHEDIR)
