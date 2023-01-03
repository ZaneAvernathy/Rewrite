
# Ensure that our environment is set up correctly.

# Ensure that dkA is installed.
ifeq ($(strip $(DEVKITARM)),)
  $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_tools

ifeq ($(shell python -c "import sys; print(int(sys.version_info[0] > 2))"),1)
  export PYTHON3 := python
else
  export PYTHON3 := python3
endif

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
export TOOLSDIR := $(ROOT)/TOOLS

export EADIR   := $(ROOT)/../Tools/EventAssembler
export CLIBDIR := $(ROOT)/../Tools/CLib

# Tools

export TABLE := $(PYTHON3) $(TOOLSDIR)/convert_table.py

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

# General tool recipes

# This has a fixed output path/extension.
# Might change this later.
%.tsv.event: %.tsv
	@$(NOTIFY_PROCESS)
	@$(TABLE) $<

# Make is deleting tables after building, so...
.PRECIOUS: %.tsv.event

# Cleaning stuff

ifneq (,$(findstring clean,$(MAKECMDGOALS)))

  TABLEFILES := $(shell find -type f -name '*.tsv')

  EVENT_TABLES_GENERATED := $(TABLEFILES:.tsv=.tsv.event)

endif

clean::
	@$(RM) $(EVENT_TABLES_GENERATED)

veryclean:: clean
	@$(RM) -rf $(CACHEDIR)
