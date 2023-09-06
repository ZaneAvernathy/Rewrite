
#include "gbafe.h"
#include "CTF.h"

/*
 * This file handles the code that does the drawing of the
 * chapter titles.
 */

void LoadChapterTitleGfx(int vramTile, unsigned titleID) {
  /*
   * Draw a chapter's title to VRAM using a font.
   *
   * Like the vanilla FE7 chapter title code, this
   * assumes that you don't have any text control
   * codes inside of your chapter title.
   */

  char* chapterTitle;
  u8* vramPosition;
  const struct FontEntry* fontCharacter;
  int characterWidth;
  int position;
  const struct FontEntry* previous;
  signed page;

  chapterTitle = GetChapterTitle(titleID);
  vramPosition = VRAM + (vramTile * TILE_SIZE_4BPP);

  gChapterTitleTileInfo.textTileID = vramTile & 0x3FF;

  CpuFastFill(0, (void*)vramPosition, 32 * 2 * TILE_SIZE_4BPP);

  position = GetChapterTitlePadding(chapterTitle);
  fontCharacter = NULL;
  previous = NULL;

  page = -1;

  while (*chapterTitle != MSG_END && *chapterTitle != MSG_PAD) {

    // The output of this can be one of two things:

    // When the character is a glyph, `fontCharacter` contains a
    // pointer to the glyph's metadata and `characterWidth` is
    // the number of bytes to advance in the text stream.

    // When the character is whitespace, `fontCharacter` contains
    // NULL and `characterWidth` is a packed value of the form
    // `(whitespaceWidth << 16) | width` where `whitespaceWidth`
    // is the width of the glyph in pixels and `width` is the width
    // of the character in bytes.

    characterWidth = ReadChapterTitleUTF8Character(chapterTitle, &fontCharacter);

    if (fontCharacter == NULL) {

      position += (characterWidth >> 16);
      previous = NULL;
      chapterTitle += (characterWidth & 0xFFFF);

    } else {

      if ((previous != NULL) && (previous->matchList != NULL))
        position += TryGetKerningAdjustment(previous->matchList, fontCharacter->codepoint);

      if (fontCharacter->page != page) {

        SetChapterTitleFontPage(fontCharacter->page);
        page = fontCharacter->page;

      }

      DrawChapterTitleCharacter(
        (u32*)gGenericBuffer,
        (u32*)vramPosition,
        fontCharacter,
        position
        );

      position += fontCharacter->width - 1;
      previous = fontCharacter;

      chapterTitle += characterWidth;

    }
  }
}

void DrawChapterTitleCharacter(u32* font, u32* dest, const struct FontEntry* fontCharacter, int x) {
  /*
   * Draws a single glyph to VRAM, offset `x` pixels horizontally
   * within the space allocated for the chapter title.
   *
   * The way this works is taken from vanilla FE7U. See `DOC/FE7_ChapterTitlesAsText.c`.
   */

  int currentColumn;
  int currentRow;

  int fontXPosition;
  int fontYPosition;

  int destXPosition;

  int sliceXPosition;
  u32* glyphPosition;
  u32* destPosition;
  int shift;
  u32 fontSlice;

  fontXPosition = (fontCharacter->tile % 32) * 8;
  fontYPosition = (fontCharacter->tile / 32) * 8;

  currentRow = fontCharacter->upperMargin;

  while (currentRow < fontCharacter->lowerMargin) {

    currentColumn = 0;
    while (currentColumn < fontCharacter->width) {

      sliceXPosition = fontXPosition + currentColumn;
      destXPosition = x + currentColumn;

      glyphPosition = ((sliceXPosition >> 3) << 3) + font + (((fontYPosition + currentRow) >> 3) << (3 + 5)) + ((fontYPosition + currentRow) & 7);

      shift = (sliceXPosition & 7) << 2;

      fontSlice = *glyphPosition & (15 << shift);
      if (fontSlice) {

        destPosition = ((destXPosition >> 3) << 3) + dest + ((currentRow >> 3) << (3 + 5)) + (currentRow & 7);

        fontSlice = fontSlice >> shift;
        fontSlice = fontSlice << ((destXPosition & 7) << 2);

        *destPosition |= fontSlice;
      }
      currentColumn++;
    }
    currentRow++;
  }
}

int GetChapterTitlePadding(char* text) {
  /*
   * Gets the number of pixels to indent a chapter title by.
   */

  int position;
  const struct FontEntry* fontCharacter;
  int characterWidth;
  const struct FontEntry* previous;

  position = 0;
  previous = NULL;
  fontCharacter = NULL;

  while (*text != MSG_END && *text != MSG_PAD) {

    characterWidth = ReadChapterTitleUTF8Character(text, &fontCharacter);

    if (fontCharacter == NULL) {

      position += (characterWidth >> 16);
      previous = NULL;
      text += (characterWidth & 0xFFFF);

    } else {

      if ((previous != NULL) && (previous->matchList != NULL))
        position += TryGetKerningAdjustment(previous->matchList, fontCharacter->codepoint);

      position += fontCharacter->width - 1;
      previous = fontCharacter;

      text += characterWidth;

    }

  }

  return ((CHAPTER_TITLE_WIDTH - position) / 2);
}
