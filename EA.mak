
# Target, source definitions

ROM_SOURCE := $(ROOT)/$(BASEROM)

NL_CORE_TARGET := $(DESTDIR)/$(OUT_NAME).NLcore.gba
CC_CORE_TARGET := $(NL_CORE_TARGET:.NLcore.gba=.CCcore.gba)

# ColorzCore doesn't allow you to configure the path of
# its `.sym` file.

NL_CORE_SYM := $(NL_CORE_TARGET:.gba=.sym)
CC_CORE_SYM := $(CC_CORE_TARGET:.gba=.sym)

# ColorzCore lets you define symbols from the commandline
# while NLCore does not, so we use an alternate buildfile
# to set a debug flag.

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
  EVENT_MAIN := $(ROOT)/$(DEBUG_BUILDFILE)
else
  EVENT_MAIN := $(ROOT)/$(BUILDFILE)
endif

# Both cores have a different set of options
# and different names for some common options.

# Global flags, common to both cores.
EAFLAGS := -input:"$(EVENT_MAIN)"

NL_CORE_FLAGS := $(EAFLAGS) -output:"$(NL_CORE_TARGET)" -symOutput:"$(NL_CORE_SYM)"
CC_CORE_FLAGS := $(EAFLAGS) -output:"$(CC_CORE_TARGET)" --nocash-sym

# Yeah, EA doesn't like being run from elsewhere, it can't
# seem to find the raws folder, even when passed the `-raws:`
# option. We'll have to `cd` to get it to work.

# Param: either `NL` or `CC`
define run-ea =
cp -f "$(ROM_SOURCE)" "$($(1)_CORE_TARGET)"
cd "$(dir $(EA_$(1)))" && $(COMPAT) $(EA_$(1)) A FE8 $($(1)_CORE_FLAGS) $($(1)_ERROR)
endef

# NLCore doesn't return the proper exit status when erroring, so
# we have to use `cmp` to check if the copied output is the same
# as the base ROM.

define NL_ERROR =
; (if (cmp -s "$(NL_CORE_TARGET)" "$(ROM_SOURCE)") ; then \
  ($(RM) "$(NL_CORE_TARGET)" "$(NL_CORE_SYM)" && false) ; \
fi)
endef

# ColorzCore properly avoids creating a .sym file if it errors out,
# while NLCore will create the `.sym` regardless. I still have to
# delete old copies of the `.sym`s on error, regardless.

CC_ERROR = || ($(RM) "$(CC_CORE_TARGET)" "$(CC_CORE_SYM)" && false)

# Actually building the ROM.

DEPS := $(shell $(EADEP) $(EVENT_MAIN) -I $(EADIR) --add-missings)

$(DESTDIR)/$(OUT_NAME).%core.gba $(DESTDIR)/$(OUT_NAME).%core.sym &: $(EVENT_MAIN) $(DEPS) $(MAKEFILE_LIST) | $(DESTDIR)
	@$(call run-ea,$*)

# `nl` and `cc` are friendlier names for the targets.

nl: $(NL_CORE_TARGET)
cc: $(CC_CORE_TARGET)

.PHONY: nl cc

clean::
	@$(RM) $(DESTDIR)/*.*

veryclean:: clean
	@$(RM) -rf $(DESTDIR)
