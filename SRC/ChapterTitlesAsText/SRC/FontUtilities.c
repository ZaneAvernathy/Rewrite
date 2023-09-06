
#include "gbafe.h"
#include "CTF.h"

/*
 * Utilities for working with the chapter title font.
 */

signed TryGetWhitespaceCharacterWidth(int codepoint) {
  /*
   * If the character is a whitespace character, return
   * the character's width in pixels.
   *
   * If the character is not a whitespace character, return -1.
   */

  signed width;
  int i;
  const struct WhitespaceEntry* entry;

  width = -1;

  i = 0;
  entry = &gCTFWhitespace[i];
  while (entry->terminator != TERMINATOR) {

    if (entry->codepoint == codepoint) {

      width = entry->width;
      break;

    }

    entry = &gCTFWhitespace[i++];

  }

  return width;
}

int GetInitialFontIndex(int codepoint) {
  /*
   * Get the starting metadata chunk index
   * of the codepoint.
   */

  int i;
  int index;
  const struct ChapterTitleFontLookupEntry* entry;

  i = 0;
  entry = &gCTFLookup[i];
  index = entry->index;
  while (entry->terminator != TERMINATOR) {

    if (entry->codepoint > codepoint) {
      break;
    }

    index = entry->index;
    entry = &gCTFLookup[i++];

  }

  return index;
}

int ReadChapterTitleUTF8Character(char* chapterTitle, const struct FontEntry** fontCharacter) {
  /*
   * Read a single UTF-8 encoded character from a
   * string and map it to the chapter title font.
   * Returns the width of the character in bytes
   * and the font glyph entry in `fontCharacter`.
   *
   * If the character is actually a whitespace character,
   * the `fontcharacter` will be NULL and the width returned
   * will be a packed value of the form `(whitespaceWidth << 16) | width`
   * where `whitespaceWidth` is the width of the glyph in pixels
   * and `width` is the width of the character in bytes.
   *
   * Like the vanilla FE7 chapter title code, this
   * assumes that you don't have any text control
   * codes inside of your chapter title.
   */

  int codepoint;
  int width;
  signed whitespaceWidth;
  int i;
  const struct FontEntry* entry;

  *fontCharacter = NULL;

  width = ReadUTF8Character(chapterTitle, &codepoint);

  whitespaceWidth = TryGetWhitespaceCharacterWidth(codepoint);
  if (whitespaceWidth != -1) {
    return ((whitespaceWidth << 16) | width);
  }

  i = GetInitialFontIndex(codepoint);
  entry = &gCTFMetadata[i];
  while (entry->terminator != TERMINATOR) {

    if (entry->codepoint == codepoint) {
      *fontCharacter = entry;
      break;
    }

    entry = &gCTFMetadata[i++];

  }

  // If the character isn't in the font.

  if (entry->terminator == TERMINATOR) {
    fontCharacter = NULL;
    return ((0 << 16) | width);
  }

  return width;
}

signed TryGetKerningAdjustment(const struct KernRightCharacter* kernableList, int target) {
  /*
   * Tries to get the kerning adjustment between two
   * characters. Returns 0 if the characters don't kern.
   */

  int adjustment = 0;

  while (kernableList->terminator != TERMINATOR) {

    if (kernableList->codepoint == target) {
      adjustment = kernableList->leftAdjust;
      break;
    }

    kernableList++;
  }

  return adjustment;
}

void SetChapterTitleFontPage(int page) {
  /*
   * Copies the requested chapter title font
   * page into VRAM.
   */
  Decompress((void*)(gCTFPageImagePointers[page]), (void*)gGenericBuffer);
}

void GetChapterTitlePalette(int config, int paletteID) {
  /*
   * Copies chapter title card/text palettes based on a config.
   *
   * This expects the palette structs to be in the order:
   *   Easy
   *   Eirika Normal
   *   Ephraim Normal
   *   Eirika Difficult
   *   Ephraim Difficult
   *   Extra
   */

  struct ChapterTitlePalette* pal;

  pal = (config & CHAPTER_TITLE_CONFIG_CARD)
    ? gChapterTitleCardPalettes
    : gChapterTitleTextPalettes
  ;

  if (!(config & CHAPTER_TITLE_CONFIG_STATUS)) {

    if ((config & CHAPTER_TITLE_CONFIG_EXTRA)) {

      pal += 5;

    } else {

      if (!(config & CHAPTER_TITLE_CONFIG_COMMON)) {

        if ((config & CHAPTER_TITLE_CONFIG_EIRIKA))
          pal += 1;

        if ((config & CHAPTER_TITLE_CONFIG_EPHRAIM))
          pal += 2;

        if ((config & CHAPTER_TITLE_CONFIG_DIFFICULT))
          pal += 2;

      }

    }

  }

  if ((config & CHAPTER_TITLE_CONFIG_DARK))
    pal = (struct ChapterTitlePalette*)((int)pal + offsetof(struct ChapterTitlePalette, dark));

  ApplyPalette((void*)pal, paletteID);

}
