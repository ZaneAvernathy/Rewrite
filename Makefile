
# Preliminary stuff

SHELL = /bin/sh

.SUFFIXES:
.PHONY: all debug clean veryclean
.DEFAULT_GOAL := all

include Tools.mak

# Main file names
# These are relative to the project root and get properly
# mangled by the things that use them.

BASEROM         := ../FE8U.gba
BUILDFILE       := Build.event
DEBUG_BUILDFILE := DebugBuild.event

# This is a filename stem that will have a folder prepended
# and a file extension appended.

OUT_NAME := Rewrite

# Component makefiles

include Code.mak
include EA.mak

include $(SRCDIR)/ChapterTitlesAsText/Makefile

# Targets:

all: nl cc

# You can optionally add `debug` to your target to build
# with debugging information, like `make all debug` or simply
# `make debug`. You can specify which core to build with by running
# `make nl` or `make cc` (or `make nl debug`/`make cc debug`).

# Additionally, `clean` and `veryclean` are available.

# If our only goal is `debug`, treat it as if it were also `all`.
ifeq (debug,$(MAKECMDGOALS))
debug: all
else
debug:
endif
	@# This is a hack to get make to be quiet.
