
/*
 * This is a set of definitions for use when compiling
 * `FE7_ChapterTitlesAsText.c`.
 */

// Stealing basic types from CLib

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t    s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef volatile u8   vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8   vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

// Vanilla functions

char* GetStringFromIndex(int index); // FE7U 0x08012C60
const struct ChapterDataEntry* GetChapterDefinition(int chapterID); // FE7U 0x08031574
int sprintf(char* buffer, const char* format, ...); // FE7U 0x080C0088
void CpuFastSet(const void* src, void* dst, unsigned config); // 0x080BFA0C
void Decompress(const void* src, void* dst); // FE7U 0x08013168

// Vanilla structures

struct ChapterDataEntry {
  // I'm too lazy to actually fill this out.
  u8 _pad[0x70];
  u16 chapterTitleTextIDs[2]; // Eliwood, Hector modes
};

extern u8 gGenericBuffer[]; // FE7U 0x02020140
extern u16 gUnknown0203E698[2]; // FE7U 0x0203E698

// Misc. values

enum TextControlCodes {
  MSG_END = 0x00,
  MSG_PAD = 0x1F,
};

enum ChapterIDs {
  CHAPTER_NO_DATA = 0x4A,
  CHAPTER_EPILOGUE = 0x4B,
  CHAPTER_TRIAL = 0x4C,
};

enum TextIDs {
  MSG_NO_DATA = 0x05D2, // '--NO DATA--[.]'
  MSG_EPILOGUE = 0x05D3, // '--EPILOGUE--'
  MSG_TRIAL = 0x05D4, // '--TRIAL--[.]'
};

#define TILE_SIZE_4BPP 32
#define VRAM 0x6000000

#define CPU_FAST_SET_SRC_FIXED 0x01000000
#define CpuFastFill(value, dest, size)                               \
{                                                                    \
    vu32 tmp = (vu32)(value);                                        \
    CpuFastSet((void *)&tmp,                                         \
               dest,                                                 \
               CPU_FAST_SET_SRC_FIXED | ((size)/(32/8) & 0x1FFFFF)); \
}
