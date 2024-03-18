
.macro SET_FUNC name, value
  .global \name
  .type   \name, %function
  .set    \name, \value
.endm

.macro SET_DATA name, value
  .global \name
  .type   \name, %object
  .set    \name, \value
.endm

@ This is different than CLib's gpARM_HuffmanTextDecomp in that
@ it is the actual offset of the routine in IWRAM.
SET_FUNC gARM_HuffmanTextDecomp, 0x03003C0C

SET_FUNC GetChapterTitlePalette,    0x080895B5
SET_FUNC GetChapterTitleID,         0x08089769
SET_FUNC GetSkirmishChapterTitleID, 0x0808979D

SET_FUNC GetWMChapterID,    0x080BCFDD
SET_FUNC GetNextWMLocation, 0x080BD015

SET_FUNC CheckEkrHitDone, 0x080522CD

SET_DATA gBgMapTarget, 0x2024CA8
SET_DATA gChapterTitleTileInfo, 0x0203E78C
SET_DATA gChapterTitleCardPalettes, 0x08A07AD8
@ SET_DATA gChapterTitleTextPalettes, 0x08A07C58
SET_DATA gWMMonsterSpawnLocations, 0x08206948
SET_DATA gWMMonsterSpawnsSize, 0x08206951
