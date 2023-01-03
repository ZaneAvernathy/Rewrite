
#include "gbafe.h"

bool CheckEventId(u16 flag);
#define TestEventFlagSet CheckEventId

// Vanilla palettes
extern const u16 gPal_MapSprite[];
extern const u16 gPal_MapSpriteArena[];
extern const u16 gPal_NotMapSprite[];

// CLib (20190316) doesn't know what this is.
#define gPal_LightRune gPal_NotMapSprite

struct AllegiancePaletteEntry {
  /* 00 */ const u16* pPalette;
  /* 04 */ bool (*pCondition)(u16);
  /* 08 */ u16 allegiance;
  /* 0A */ u16 param;
};

// This value denotes that the parameter field is ignored.
#define ALPAL_IGNORE (-1)

extern const struct AllegiancePaletteEntry gAllegiancePalettes[];

// Going to give each palette slot a name
enum
{
  PS_ARENA = 0x1B,
  PS_PLAYER,
  PS_ENEMY,
  PS_NPC,
  PS_GRAY,
};

// This is a table that maps (EAstdLib) allegiance to PalRAM palette index
const u8 gAllegiancePaletteSlotLUT[] = {
  /* Player    */ PS_PLAYER,
  /* NPC       */ PS_NPC,
  /* Enemy     */ PS_ENEMY,
  /* Arena 4th */ PS_ARENA,
};

void LoadMapSpritePalettes()
{
  /*
   * This function copies the map sprite palettes
   * into PalRAM.
   * 
   * This hack adds the ability to specify different
   * palettes to be loaded for the three factions based
   * on user-defined conditions, like during certain
   * chapters or if an event flag has been set.
   */

  ApplyPalettes(gPal_MapSprite, PS_PLAYER, 4);

  if (gGameState.statebits & 0x40) // TODO: chapter state bits
    ApplyPalette(gPal_MapSpriteArena, PS_ARENA);
  else
    ApplyPalette(gPal_LightRune, PS_ARENA);

  unsigned i = 0;
  struct AllegiancePaletteEntry current = gAllegiancePalettes[i];

  while (current.pPalette)
  {
    if (current.pCondition((u16)current.param))
      ApplyPalette(current.pPalette, gAllegiancePaletteSlotLUT[current.allegiance]);

    i++;
    current = gAllegiancePalettes[i];
  }

}

/*
 * Using `AlPal` to abbreviate here to avoid the possibility
 * of confusing these with `AP` routines.
 */

struct AlPalConditionChapterAndFlagParam {
  /* 00 */ u16 chapterIndex : 8;
  /* 01 */ u16 eventFlag    : 8;
};

bool AlPalConditionChapterAndFlag(struct AlPalConditionChapterAndFlagParam param)
{
  /*
   * This is a condition function for use with AllegiancePaletteEntry
   * Returns true if it is the specified chapter and
   * the event flag is set.
   */

  bool result = true;

  if ((s8)param.chapterIndex != ALPAL_IGNORE)
    result &= (gChapterData.chapterIndex == param.chapterIndex);

  if ((s8)param.eventFlag != ALPAL_IGNORE)
    result &= TestEventFlagSet(param.eventFlag);

  return result;
}
