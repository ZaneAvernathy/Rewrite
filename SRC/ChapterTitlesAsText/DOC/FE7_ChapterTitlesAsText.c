
#include "FE7.h"

/*
 * This is a decomp of FE7's chapter title drawing code.
 * It's included here for reference.
 * For matching assembly, build using old_agbcc using
 * the flags `-O2 -mthumb-interwork`.
 */

#define CHAPTER_TITLE_WIDTH 192

enum ChapterTitleFont {
  // This defines the order of characters within the chapter
  // title font, and is used when mapping from ASCII.
  // There's probably a better way to do this.
  // Presumably you'd have the font generator make this.
  CTF_A, CTF_B, CTF_C, CTF_D, CTF_E, CTF_F, CTF_G, CTF_H,
  CTF_I, CTF_J, CTF_K, CTF_L, CTF_M, CTF_N, CTF_O, CTF_P,
  CTF_Q, CTF_R, CTF_S, CTF_T, CTF_U, CTF_V, CTF_W, CTF_X,
  CTF_Y, CTF_Z, CTF_a, CTF_b, CTF_c, CTF_d, CTF_e, CTF_f,
  CTF_g, CTF_h, CTF_i, CTF_j, CTF_k, CTF_l, CTF_m, CTF_n,
  CTF_o, CTF_p, CTF_q, CTF_r, CTF_s, CTF_t, CTF_u, CTF_v,
  CTF_w, CTF_x, CTF_y, CTF_z, CTF_0, CTF_1, CTF_2, CTF_3,
  CTF_4, CTF_5, CTF_6, CTF_7, CTF_8, CTF_9, CTF_Dash, CTF_Apostrophe,
  CTF_Colon, CTF_Period,
  CTF_Space = 128,
};

#define CTF_SPACE_WIDTH 3

// I'm not 100% on the first four parts of this struct.

struct GlyphTableEntry {
  /* 00 */ u8 maxKerning; /*
    The maxKerning of a glyph is how many pixels
    of the glyph can be combined with the previous
    glyph, up to the previous glyph's nextKerning.
    */
  /* 01 */ u8 nextKerning; /*
    The nextKerning of a glyph is the number of pixels
    it requires in order to be kerned with the next glyph.
    */
  /* 02 */ u8 logicalWidth; /*
    The logicalWidth of a glyph is how wide it is considered
    to be when not kerning with the next glyph.
    */
  /* 03 */ u8 logicalKernedWidth; /*
    The logicalKernedWidth of a glyph is how wide it is considered
    to be when kerning with the next glyph.
    */
  /* 04 */ u8 cellWidth; /*
    The cellWidth of a glyph is how many pixels
    are horizontally used for the glyph in the font.
    This is always a multiple of 8 pixels
    (a graphics tile) and used values are
    8 (for thinner glyphs) and 16 (for wider glyphs).
    */
  /* 05 */ u8 width; /*
    The width of a glyph is how wide a glyph
    is treated in pixels, which may be smaller than
    the glyph's graphical width.
    */
  /* 06 */ u8 upperMargin; /*
    The upperMargin of a glyph is how many pixels
    from the top of the cell to the first occupied
    row of pixels in the glyph.
    */
  /* 07 */ u8 lowerMargin; /*
    The lowerMargin of a glyph is how many pixels
    tall a glyph is, measured from the top of the cell.
    */
};

extern const struct GlyphTableEntry gGlyphTable[]; // FE7U 0x08CC2784
extern const u8 gChapterTitleFont[]; // FE7U 0x0840260C

int GetChapterTitleFontGlyphPixelOffset(int fontCharacter) {
  /*
   * Given the index of a character in the font, return
   * how many pixels into the font the character's graphics are.
   */

  int result = 0;
  const struct GlyphTableEntry* current = gGlyphTable;

  for (fontCharacter; fontCharacter != 0; fontCharacter--) {
    result += current++->cellWidth;
  }

  return result;
}

extern const char NoChapterNameMessage[]; // FE7U 0x08404BA0 "none chapter message = %c"

signed MapCharacterToChapterTitleFont(char* text) {
  /*
   * Given the position of a character within some text,
   * get the index of the character within the chapter
   * title font.
   */

  char buffer[32];
  char temp;

  // This relies on the font be ordered such that
  // it contains the uninterrupted ASCII ranges
  // A-Z, a-z, and 0-9.

  temp = *text - 'A';
  if (temp <= 'Z' - 'A')
    return *text - 'A' + CTF_A;

  temp = *text - 'a';
  if (temp <= 'z' - 'a')
    return *text - 'a' + CTF_a;

  temp = *text - '0';
  if (temp <= 9)
    return *text - '0' + CTF_0;

  if (*text == '-') return CTF_Dash;
  if (*text == '\'') return CTF_Apostrophe;
  if (*text == ':') return CTF_Colon;
  if (*text == '.') return CTF_Period;
  if (*text == ' ') return CTF_Space;

  // If a character doesn't appear in the font, this
  // throws an error message onto the stack, which gets
  // discarded upon leaving the function. I'm guessing
  // that this actually printed the message at some point.

  else {
    sprintf(buffer, NoChapterNameMessage, *text);
    return -1;
  }
}

void DrawChapterTitleCharacter(u32* font, u32* dest, int fontCharacter, int x) {
  /*
   * This draws a chapter title font character to VRAM, offset `x` pixels
   * horizontally.
   */

  int glyphOffset;

  int currentColumn;
  int currentRow;

  const struct GlyphTableEntry* glyphDimensions;

  int fontXPosition;
  int fontYPosition;
  int destXPosition;

  u32* glyphPosition;
  int shift;
  u32 fontSlice;
  u32* destPosition;

  glyphOffset = GetChapterTitleFontGlyphPixelOffset(fontCharacter);
  fontXPosition = glyphOffset & 0xFF;
  fontYPosition = (glyphOffset >> 8) * 16; // Cells are 16px tall.

  glyphDimensions = &gGlyphTable[fontCharacter];

  // The math here is messy in order for the decomp to match, so
  // I'll break things into individual parts.

  // We're looping through the pixels in the gylph-to-copy
  // starting with the topmost populated row to the bottommost
  // populated row. Pixels are copied left to right across rows
  // until it hits the glyph's set width.

  currentRow = glyphDimensions->upperMargin;

  while (currentRow < glyphDimensions->lowerMargin) {

    currentColumn = 0;
    while (currentColumn < glyphDimensions->width) {
      int sliceXPosition = fontXPosition + currentColumn;
      destXPosition = x + currentColumn;

      // `glyphPosition` is the address of the row of pixels being
      // read within the font. Each of the following parts contributes
      // to the overall offset.

      // `((sliceXPosition >> 3) << 3)` is an annoying way of getting
      // `sliceXPosition & ~7`, which is the glyph's horizontal
      // position in the font in terms of full tiles.

      // `(((fontYPosition + currentRow) >> 3) << (3 + 5))` is essentially
      // `((fontYPosition + currentRow) & ~7) * 32`
      // and is the glyph's vertical position into the font in full tiles.
      // The 32 is because there are 32 tiles per row in the font.

      // Finally, `((fontYPosition + currentRow) & 7)` is how far vertically
      // we are into the font tile.

      glyphPosition = ((sliceXPosition >> 3) << 3) + font + (((fontYPosition + currentRow) >> 3) << (3 + 5)) + ((fontYPosition + currentRow) & 7);

      // This gets us how many bits into the tile we are
      // multiplied by 4 bits per pixel.

      shift = (sliceXPosition & 7) << 2;

      // The `15` here is `0b1111`, to extract the 4 bits
      // that make up the pixel.

      // We only do the copying if the pixel isn't blank.

      fontSlice = *glyphPosition & (15 << shift);
      if (fontSlice) {

        // The parts of this mirror the `glyphPosition` parts above, but don't have
        // to account for copying from a specific part of a font.

        destPosition = ((destXPosition >> 3) << 3) + dest + ((currentRow >> 3) << (3 + 5)) + (currentRow & 7);

        // Unshift our pixel from our position-in-font-tile
        // amount and then shift it by the position-in-VRAM
        // amount, `((destXPosition & 7) << 2)`.

        fontSlice = fontSlice >> shift;
        fontSlice = fontSlice << ((destXPosition & 7) << 2);

        // All of that to combine a single pixel per iteration.

        *destPosition |= fontSlice;
      }
      currentColumn++;
    }
    currentRow++;
  }
}

int GetChapterTitlePadding(char* text) {
  /*
   * Gets the number of pixels into the allocated
   * VRAM space a chapter title begins.
   */

  u8 position = 0;
  u8 kernPosition = 0;
  int fontCharacter;

  const struct GlyphTableEntry* current;

  while (*text != MSG_END && *text != MSG_PAD) {

    fontCharacter = MapCharacterToChapterTitleFont(text);
    if (fontCharacter == CTF_Space) {

      if (kernPosition > position) {
        kernPosition += CTF_SPACE_WIDTH;
        position = kernPosition;
      } else {
        position += CTF_SPACE_WIDTH;
        kernPosition = position;
      }

    } else {

      current = &gGlyphTable[fontCharacter];
      if ((kernPosition - current->maxKerning) > (position - current->nextKerning)) {
        position = kernPosition;
      } else {
        kernPosition = position;
      }

      kernPosition += current->logicalWidth - 1;
      position += current->logicalKernedWidth - 1;

    }

    text++;
  }
  return (CHAPTER_TITLE_WIDTH - ((kernPosition + position) >> 1)) >> 1;
}

char* GetChapterTitle(signed index) {
  /*
   * Given a chapter ID, get the chapter's title.
   * If the chapter ID is negative, give the 'NO DATA' string.
   * Otherwise, the chapter ID is treated like a byte where
   * bit 0x80 denotes Hector mode.
   */

  char* chapterTitle;
  const struct ChapterDataEntry* chapter;

  if (index < 0)
    index = CHAPTER_NO_DATA;

  switch (index) {
    case CHAPTER_NO_DATA:
      chapterTitle = GetStringFromIndex(MSG_NO_DATA);
      break;
    case CHAPTER_EPILOGUE:
      chapterTitle = GetStringFromIndex(MSG_EPILOGUE);
      break;
    case CHAPTER_TRIAL:
      chapterTitle = GetStringFromIndex(MSG_TRIAL);
      break;
    default:
      chapter = GetChapterDefinition(index & 0x7F);
      chapterTitle = GetStringFromIndex(chapter->chapterTitleTextIDs[(index >> 7) & 1]);
      break;
  }

  return chapterTitle;
}

void DrawChapterTitleToVRAM(int vramTile, int chapterID) {
  /*
   * Draws a chapter title to VRAM, starting at a specific
   * VRAM tile (normally, an object tile).
   */

  char* chapterTitle;
  int vramPosition;
  u8 position;
  u8 kernPosition;
  int fontCharacter;
  const struct GlyphTableEntry* current;

  chapterTitle = GetChapterTitle(chapterID);
  vramPosition = VRAM + (vramTile << 5);
  position = GetChapterTitlePadding(chapterTitle);
  kernPosition = position;

  gUnknown0203E698[1] = vramTile & 0x3FF;
  
  CpuFastFill(0, (void*)vramPosition, 32 * 2 * TILE_SIZE_4BPP);
  Decompress((void*)gChapterTitleFont, (void*)gGenericBuffer);

  // There are two position counters: `position`, which is where the
  // next glyph will be placed under normal circumstances, and
  // `kernPosition`, which is where the glyph will be placed
  // if it is to be kerned with the previous glyph.

  // The math here is pretty funky and I'm not sure about the names.

  while (*chapterTitle != MSG_END && *chapterTitle != MSG_PAD) {
    fontCharacter = MapCharacterToChapterTitleFont(chapterTitle);
    if (fontCharacter == CTF_Space) {
      if (position > kernPosition) {
        position += CTF_SPACE_WIDTH;
        kernPosition = position;
      } else {
        kernPosition += CTF_SPACE_WIDTH;
        position = kernPosition;
      }

    } else {
      current = &gGlyphTable[fontCharacter];

      if ((position - current->maxKerning) > (kernPosition - current->nextKerning)) {
        kernPosition = position;
      } else {
        position = kernPosition;
      }

      DrawChapterTitleCharacter((u32*)gGenericBuffer, (u32*)vramPosition, fontCharacter, position);
      position += current->logicalWidth - 1;
      kernPosition += current->logicalKernedWidth - 1;
    }

    chapterTitle++;
  }
}
