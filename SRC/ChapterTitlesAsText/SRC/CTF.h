#ifndef GUARD_CTF_H
#define GUARD_CTF_H

#include "gbafe.h"

typedef u8  bool8;
typedef u16 bool16;
typedef u32 bool32;

#define CHAPTER_TITLE_WIDTH 192 // In pixels
#define TILE_SIZE_4BPP 32 // In bytes

// I'm using the same value to terminate each list.
#define TERMINATOR (-1)

enum TextControlCodes {
  MSG_END = 0x00,
  MSG_PAD = 0x1F,
};

extern struct {
  /*
   * This vanilla struct keeps track of some
   * chapter title info.
   */

  u16 backgroundTileID;
  u16 textTileID;

} gChapterTitleTileInfo; // FE8U 0x0203E78C

/*
 * Both chapter title text and cards have a set of 6 palettes,
 * with each palette having a light (highlighted) and dark variant.
 *
 * The palettes are:
 *   Easy
 *   Eirika Normal
 *   Ephraim Normal
 *   Eirika Difficult
 *   Ephraim Difficult
 *   Extra
 */

struct ChapterTitlePalette {

  u16 light[16];
  u16 dark[16];

};

extern struct ChapterTitlePalette gChapterTitleCardPalettes[6]; // FE8U 0x08A07AD8
extern struct ChapterTitlePalette gChapterTitleTextPalettes[6]; // FE8U 0x08A07C58

enum {

  /*
   * These bits make up the config passed to GetChapterTitlePalette.
   */

  CHAPTER_TITLE_CONFIG_CARD      = (1 << 0),
  CHAPTER_TITLE_CONFIG_DARK      = (1 << 1),
  CHAPTER_TITLE_CONFIG_DIFFICULT = (1 << 2),
  CHAPTER_TITLE_CONFIG_EXTRA     = (1 << 3),
  CHAPTER_TITLE_CONFIG_COMMON    = (1 << 4),
  CHAPTER_TITLE_CONFIG_EIRIKA    = (1 << 5),
  CHAPTER_TITLE_CONFIG_EPHRAIM   = (1 << 6),
  CHAPTER_TITLE_CONFIG_STATUS    = (1 << 7),

};

enum {
  /*
   * These are for the `ChapterState` struct's `chapterStateBits`.
   */

  PLAY_FLAG_STATSCREENPAGE0 = (1 << 0),
  PLAY_FLAG_STATSCREENPAGE1 = (1 << 1),
  PLAY_FLAG_POSTGAME        = (1 << 2),
  PLAY_FLAG_TUTORIAL        = (1 << 3),
  PLAY_FLAG_PREPSCREEN      = (1 << 4),
  PLAY_FLAG_COMPLETE        = (1 << 5),
  PLAY_FLAG_HARD            = (1 << 6),
  PLAY_FLAG_7               = (1 << 7),

  PLAY_FLAG_STATSCREENPAGE_SHIFT = 0,
  PLAY_FLAG_STATSCREENPAGE_MASK = PLAY_FLAG_STATSCREENPAGE0 | PLAY_FLAG_STATSCREENPAGE1,

};

const struct ROMChapterData* GetChapterDefinition(unsigned chIndex); // FE8U 0x08034618

// CLib doesn't have a fleshed-out ROM chapter data struct, so I'm stealing from
// the decomp:

struct ChapterMap {
  u8  obj1Id;
  u8  obj2Id;
  u8  paletteId;
  u8  tileConfigId;
  u8  mainLayerId;
  u8  objAnimId;
  u8  paletteAnimId;
  u8  changeLayerId;
};

struct ROMChapterData_ {

  /* 00 */ const char* internalName;

  /* 04 */ struct ChapterMap map;

  /* 0C */ u8  initialFogLevel;
  /* 0D */ bool8  hasPrepScreen; // left over from FE7

  /* 0E */ u8 chapTitleId;
  /* 0F */ u8 chapTitleIdInHectorStory; // left over from FE7

  /* 10 */ u8 initialPosX;
  /* 11 */ u8 initialPosY;
  /* 12 */ u8 initialWeather;
  /* 13 */ u8 battleTileSet;

  // This may need a type change.
  /* 14 */ u16 easyModeLevelMalus      : 4;
  /* 14 */ u16 difficultModeLevelBonus : 4;
  /* 14 */ u16 normalModeLevelMalus    : 4;

  /* 16 */ u16 mapBgmIds[11];

  /* 2C */ u8 mapCrackedWallHeath;

  /* 2D */ u8 turnsForTacticsRankAInEliwoodStory[2]; // left over from FE7
  /* 2F */ u8 turnsForTacticsRankAInHectorStory[2]; // left over from FE7
  /* 31 */ u8 turnsForTacticsRankBInEliwoodStory[2]; // left over from FE7
  /* 33 */ u8 turnsForTacticsRankBInHectorStory[2]; // left over from FE7
  /* 35 */ u8 turnsForTacticsRankCInEliwoodStory[2]; // left over from FE7
  /* 37 */ u8 turnsForTacticsRankCInHectorStory[2]; // left over from FE7
  /* 39 */ u8 turnsForTacticsRankDInEliwoodStory[2]; // left over from FE7
  /* 3B */ u8 turnsForTacticsRankDInHectorStory[2]; // left over from FE7

  /* 3D */ u8 unk3D; // padding?

  /* 3E */ u16 gainedExpForExpRankAInEliwoodStory[2]; // left over from FE7
  /* 42 */ u16 gainedExpForExpRankAInHectorStory[2]; // left over from FE7
  /* 46 */ u16 gainedExpForExpRankBInEliwoodStory[2]; // left over from FE7
  /* 4A */ u16 gainedExpForExpRankBInHectorStory[2]; // left over from FE7
  /* 4E */ u16 gainedExpForExpRankCInEliwoodStory[2]; // left over from FE7
  /* 52 */ u16 gainedExpForExpRankCInHectorStory[2]; // left over from FE7
  /* 56 */ u16 gainedExpForExpRankDInEliwoodStory[2]; // left over from FE7
  /* 5A */ u16 gainedExpForExpRankDInHectorStory[2]; // left over from FE7

  /* 5E */ u16 unk5E; // padding?

  /* 60 */ u32 goldForFundsRankInEliwoodStory[2]; // left over from FE7
  /* 68 */ u32 goldForFundsRankInHectorStory[2]; // left over from FE7

  /* 70 */ u16 chapTitleTextId;
  /* 72 */ u16 chapTitleTextIdInHectorStory; // left over from FE7

  /* 74 */ u8 mapEventDataId;
  /* 75 */ u8 gmapEventId;

  /* 76 */ u16 divinationTextIdBeginning; // left over from FE7
  /* 78 */ u16 divinationTextIdInEliwoodStory; // left over from FE7
  /* 7A */ u16 divinationTextIdInHectorStory; // left over from FE7
  /* 7C */ u16 divinationTextIdEnding; // left over from FE7
  /* 7E */ u8 divinationPortrait; // left over from FE7
  /* 7F */ u8 divinationFee; // left over from FE7

  /* 80 */ u8 prepScreenNumber;
  /* 81 */ u8 prepScreenNumberInHectorStory; // left over from FE7
  /* 82 */ u8 merchantPosX;
  /* 83 */ u8 merchantPosXInHectorStory; // left over from FE7
  /* 84 */ u8 merchantPosY;
  /* 85 */ u8 merchantPosYInHectorStory; // left over from FE7

  /* 86 */ s8 victorySongEnemyThreshold;
  /* 87 */ bool8 fadeToBlack;

  /* 88 */ u16 statusObjectiveTextId;
  /* 8A */ u16 goalWindowTextId;
  /* 8C */ u8 goalWindowDataType;
  /* 8D */ u8 goalWindowEndTurnNumber;
  /* 8E */ u8 protectCharacterIndex;

  /* 8F */ u8 destPosX;
  /* 90 */ u8 destPosY;

  /* 91 */ u8 unk91; // ?
  /* 92 */ u8 unk92; // ?
  /* 93 */ u8 unk93; // ?

};

int GetWMChapterID(int chapterID); // FE8U 0x080BCFDD
int GetNextWMLocation(struct GMapData*); // FE8U 0x080BD015

extern u8 gWMMonsterSpawnLocations[]; // FE8U 0x08206948
extern u8 gWMMonsterSpawnsSize; // FE8U 0x08206951

// These are the structs for our custom font.

 struct KernRightCharacter {
  /*
   * This is the right-side character in a
   * pair to be kerned or a terminator.
   */

  union {

    struct {

      u32 codepoint : 24; /*
        * The unicode codepoint of the character.
        */
      s8 leftAdjust : 8; /*
        * The number of pixels to adjust the right
        * character by. A negative number moves it
        * closer to the left character.
        */

    };

    s32 terminator;

    };

};

struct FontEntry {
  /*
   * This is the main data struct for glyphs in the font.
   */

  union {

    struct {

      u32 codepoint : 24; /*
        * This is the unicode codepoint of the character.
        */
      u8 width : 5; /*
        * This is the width of the character in pixels.
        */
      bool cellWidthFlag : 1; /*
        * When set, this flag indicates that the glyph's cell
        * is 16 pixels wide instead of 8.
        */
      u8 upperMargin; /*
        * The number of pixels between the top of the cell and
        * the top of the glyph graphics.
        */
      u8 lowerMargin; /*
        * The number of pixels between the top of the cell and
        * the bottom of the glyph graphics.
        */
      u8 page; /*
        * The font page of the glyph.
        */
      u8 tile; /*
        * The tile of the glyph within the font page.
        */
      const struct KernRightCharacter* matchList; /*
        * This is a pointer to a list of info that
        * is used to check if we can kern with a character
        * to the right and by how much.
        */

    };

    s32 terminator;

  };

};

struct WhitespaceEntry {
  /*
   * This struct is used to mark characters
   * that are purely whitespace.
   */

  union {

    struct {

      u32 codepoint : 24; /*
        * This is the character's unicode codepoint.
        */
      u8 width; /*
        * This is the width of the character in pixels.
        */

    };

    s32 terminator;

  };
};

struct ChapterTitleEntry {
  union {

    // This is either a text ID for the chapter title,
    // or an index into the special chapter title table.
    // The special chapter title indices are negative, and
    // this is used to delineate them from the normal text IDs.

    u16 textID;
    s16 specialID;

  };
};

struct ChapterTitleFontLookupEntry {
  /*
   * Instead of trawling through the entire font metadata
   * table to find a character, we slice up the table into
   * chunks. We find the chunk with the highest starting
   * codepoint that is still less than or equal to the
   * target codepoint and start looking there.
   */

  union {

    struct {

      u32 codepoint; /*
        * This is the first codepoint in the block.
        */
      u32 index; /*
        * This is the codepoint's index into the
        * metadata table.
        */

    };

    s32 terminator;

  };
};

extern const u16 gChapterTitleEntryCount;
extern const u8 gDefaultChapterTitleID;
extern const u8 gNoDataChapterTitleID;
extern const u8 gCreatureCampaignChapterTitleID;
extern const u8 gEpilogueChapterTitleID;

extern const u8 gSkirmishStartingChapterTitleID;

extern const struct ChapterTitleEntry gChapterTitles[];
extern char* gSpecialChapterTitles[];

extern const struct ChapterTitleFontLookupEntry gCTFLookup[];
extern const struct FontEntry gCTFMetadata[];
extern const struct WhitespaceEntry gCTFWhitespace[];

extern const u8* gCTFPageImagePointers[];

// These are the functions defined in our sources.

// UTF8.c
int GetUTF8Displacement(char c, int shift);
int ReadUTF8Character(char* text, int* outputCodepoint);

// DrawChapterTitle.c
void LoadChapterTitleGfx(int vramTile, unsigned titleID);
void DrawChapterTitleCharacter(u32* font, u32* dest, const struct FontEntry* fontCharacter, int x);
int GetChapterTitlePadding(char* text);

// FontUtilities.c
signed TryGetWhitespaceCharacterWidth(int codepoint);
int GetInitialFontIndex(int codepoint);
int ReadChapterTitleUTF8Character(char* chapterTitle, const struct FontEntry** fontCharacter);
signed TryGetKerningAdjustment(const struct KernRightCharacter* kernableList, int target);
void SetChapterTitleFontPage(int page);
void GetChapterTitlePalette(int config, int paletteID);

// ChapterTitleIndexUtilities.c
char* GetChapterTitle(unsigned titleID);

#endif // GUARD_CTF_H
