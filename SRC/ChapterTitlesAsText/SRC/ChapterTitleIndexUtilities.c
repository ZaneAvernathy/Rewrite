
#include "gbafe.h"
#include "CTF.h"

/*
 * Utilities for working with chapter title IDs.
 */

char* GetChapterTitle(unsigned titleID) {
  /*
   * Given a pending chapter title ID, fetch the
   * chapter title text for the chapter.
   */

  const struct ChapterTitleEntry* entry;
  char* chapterTitle;

  if (titleID > gChapterTitleEntryCount)
    titleID = gDefaultChapterTitleID;

  entry = &gChapterTitles[titleID];

  if (entry->specialID < 0)
    chapterTitle = gSpecialChapterTitles[ABS(entry->specialID) - 1];
  else
    chapterTitle = GetStringFromIndex(entry->textID);

  return chapterTitle;
}

int GetChapterTitleID(struct ChapterState* chapter) {
  /*
   * Get a (non-skirmish) chapter's title ID.
   */

  if (chapter == NULL)
    return gNoDataChapterTitleID;

  if (chapter->chapterStateBits & PLAY_FLAG_POSTGAME)
    return gCreatureCampaignChapterTitleID;

  if (chapter->chapterStateBits & PLAY_FLAG_COMPLETE)
    return gEpilogueChapterTitleID;

  // Cast here is because CLib lacks a good version of the
  // ROM chapter data struct.

  return ((const struct ROMChapterData_*)(GetChapterDefinition(chapter->chapterIndex)))->chapTitleId;
}

int GetSkirmishChapterTitleID(struct ChapterState* chapter) {
  /*
   * Get a skirmish's chapter title ID.
   */

  signed worldMapChapterID;
  int i;

  if (chapter == NULL)
    return gDefaultChapterTitleID;

  worldMapChapterID = GetWMChapterID(chapter->chapterIndex);

  if ((chapter->chapterStateBits & PLAY_FLAG_POSTGAME) || GetNextWMLocation(&gGMData) != worldMapChapterID) {

    for (i = 0; i < gWMMonsterSpawnsSize; i++) {

      if (worldMapChapterID == gWMMonsterSpawnLocations[i])
        return gSkirmishStartingChapterTitleID + i;

    }

  }

  // Cast here is because CLib lacks a good version of the
  // ROM chapter data struct.

  return ((const struct ROMChapterData_*)(GetChapterDefinition(chapter->chapterIndex)))->chapTitleId;
}
