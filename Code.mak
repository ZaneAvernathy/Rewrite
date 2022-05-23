
# Code helpers

LYN_REFERENCE := $(CLIBDIR)/reference/FE8U-20190316.o

INCLUDE_DIRS := $(CLIBDIR)/include
INCFLAGS     := $(foreach dir,$(INCLUDE_DIRS), -I "$(dir)")

# Assembly/compilation flags
ARCH    := -mcpu=arm7tdmi -mthumb -mthumb-interwork
CFLAGS  := $(ARCH) $(INCFLAGS) -Wall -Os -mtune=arm7tdmi -ffreestanding -fomit-frame-pointer -mlong-calls
ASFLAGS := $(ARCH) $(INCFLAGS)

# Dependency flags
CDEPFLAGS = -MMD -MT "$*.o" -MT "$*.asm" -MF "$(CACHEDIR)/$(notdir $*).d" -MP
SDEPFLAGS = --MD "$(CACHEDIR)/$(notdir $*).d"

# Rules

# I don't think all of these actually need the cache dir, but
# it can't hurt to specify it.

%.lyn.event: %.o $(LYN_REFERENCE) | $(CACHEDIR)
	@$(NOTIFY_PROCESS)
	@$(LYN) "$<" "$(LYN_REFERENCE)" > "$@" || ($(RM) "$@" && false)

%.dmp: %.o | $(CACHEDIR)
	@$(NOTIFY_PROCESS)
	@$(OBJCOPY) -S $< -O binary $@

%.o: %.s | $(CACHEDIR)
	@$(NOTIFY_PROCESS)
	@$(AS) $(ASFLAGS) $(SDEPFLAGS) -I $(dir $<) $< -o $@ $(ERROR_FILTER)

# Skipping intermediate .s files becasue Stan says they break things.
%.o: %.c | $(CACHEDIR)
	@$(NOTIFY_PROCESS)
	@$(CC) $(CFLAGS) $(CDEPFLAGS) -g -c $< -o $@ $(ERROR_FILTER)

%.asm: %.c | $(CACHEDIR)
	@$(NOTIFY_PROCESS)
	@$(CC) $(CFLAGS) $(CDEPFLAGS) -S $< -o $@ -fverbose-asm $(ERROR_FILTER)

# Stan says that we need to avoid deleting intermediate .o files
# or dependency stuff will break, so:
# Also, I just want to keep intermediate files between builds, anyway.
.PRECIOUS: %.o %.asm %.lyn.event %.dmp

# Cleaning stuff

ifneq (,$(findstring clean,$(MAKECMDGOALS)))

  CFILES := $(shell find -type f -name '*.c')
  SFILES := $(shell find -type f -name '*.s')

  ASM_C_GENERATED := $(CFILES:.c=.o) $(SFILES:.s=.o) $(CFILES:.c=.asm)
  ASM_C_GENERATED += $(ASM_C_GENERATED:.o=.dmp) $(ASM_C_GENERATED:.o=.lyn.event)

endif

clean::
	@$(RM) $(ASM_C_GENERATED)
