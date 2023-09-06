
SLICE_CTF_GLYPHS  := $(PYTHON3) $(TOOLSDIR)/slice_glyphs.py
GENERATE_CTF_FONT := $(PYTHON3) $(TOOLSDIR)/generate_chapter_title_font.py

CTFDIR := $(SRCDIR)/ChapterTitlesAsText

WHITESPACE := $(wildcard $(CTFDIR)/GLYPHS/Whitespace.txt)
KERNING    := $(wildcard $(CTFDIR)/GLYPHS/Kerning.txt)

GENERATED_WHITESPACE := $(WHITESPACE:Whitespace.txt=%CTF_Generated_Whitespace.tsv)
GENERATED_KERNING    := $(KERNING:Kerning.txt=%CTF_Generated_Kerning.event)

GLYPH_SOURCES := $(wildcard $(CTFDIR)/SHEETS/*.png)
GLYPH_SOURCES_STRIPPED := $(foreach sheet,$(GLYPH_SOURCES),$(basename $(notdir $(sheet))))

FONT_BASE     = %CTF_Generated_Page_$(number).png
FONT_NUMBERS := $(shell seq -s ' ' -f '%02g' 0 15)
GENERATED_FONT_PAGES := $(foreach number,$(FONT_NUMBERS),$(FONT_BASE))

FONT_PALETTE_SOURCE := $(firstword $(wildcard $(CTFDIR)/SHEETS/*.png))
GENERATED_FONT_PALETTE := %GLYPHS/CTF_Generated_Palette.pal

INSTALLER_BASE := GLYPHS/CTF_Generated_Installer.event
INSTALLER_FULL := $(CTFDIR)/$(INSTALLER_BASE)

GENERATED_INSTALLER := %$(INSTALLER_BASE)
GENERATED_METADATA  := %GLYPHS/CTF_Generated_Metadata.tsv

CTF_GENERATED := $(GENERATED_INSTALLER) $(GENERATED_METADATA) $(GENERATED_WHITESPACE) $(GENERATED_KERNING) $(GENERATED_FONT_PAGES)

DEPS += $(GENERATED_FONT_PAGES) $(GENERATED_FONT_PALETTE)

$(CTF_GENERATED) &: $(WHITESPACE) $(KERNING) $(GLYPH_SOURCES)
	@( \
	for sheet in $(GLYPH_SOURCES_STRIPPED); \
	do $(SLICE_CTF_GLYPHS) "$(CTFDIR)/SHEETS/$${sheet}.png" "$(CTFDIR)/GLYPHS/" 0x$${sheet}; \
	done \
	) && \
	$(GENERATE_CTF_FONT) "$(CTFDIR)/GLYPHS/" && \
	($(EADEP) $(INSTALLER_FULL) --add-missings | sed -e ':a;N;$!ba;s/\n/ /g' | xargs $(MAKE))

$(GENERATED_FONT_PALETTE): $(FONT_PALETTE_SOURCE)
	@$(NOTIFY_PROCESS)
	@$(PNG2DMP) "$<" --palette-only > "$@"

.PRECIOUS: $(CTF_GENERATED) $(GENERATED_FONT_PAGES) $(GENERATED_FONT_PAGES:%.png=%.4bpp.lz77) $(GENERATED_FONT_PALETTE)

# Cleaning stuff

ifneq (,$(findstring clean,$(MAKECMDGOALS)))

  GENERATED_FILES := $(wildcard $(CTFDIR)/GLYPHS/CTF_Generated_*)
  GENERATED_FILES += $(wildcard $(CTFDIR)/GLYPHS/*.png)

endif

clean::
	@$(RM) $(GENERATED_FILES)
