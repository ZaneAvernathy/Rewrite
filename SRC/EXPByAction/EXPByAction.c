
#include "gbafe.h"

s8 CanBattleUnitGainLevels(struct BattleUnit* bu);
void CheckBattleUnitLevelUp(struct BattleUnit* bu);

struct EXPActionEntry
{
  union
  {
    s16 terminator;
    struct
    {
      u16 actionIndex : 8;
      u16 experience  : 8;
    };
  };
};

extern const struct EXPActionEntry gEXPByActionList[];
extern const u16 gEXPByActionDefaultExperience;

#define TERMINATOR (-1)

void BattleApplyMiscActionExpGains(void)
{
  /*
   * This function handles granting experience for
   * dancing, stealing, and summoning in vanilla.
   * This hack checks for alternative experience values
   * in a list based on which action was performed.
   */

  if (UNIT_FACTION(&gBattleActor.unit) != FACTION_BLUE)
    return;

  if (!CanBattleUnitGainLevels(&gBattleActor))
    return;

  if (gChapterData.chapterStateBits & (1 << 7)) // TODO: chapter state bits
    return;

  u8 experience = gEXPByActionDefaultExperience;
  u8 action = gActionData.unitActionType;

  unsigned i = 0;
  struct EXPActionEntry entry = gEXPByActionList[i];
  while (entry.terminator != TERMINATOR)
  {
    if (entry.actionIndex == action)
    {
      experience = entry.experience;
      break;
    }

    i += 1;
    entry = gEXPByActionList[i];
  }

  gBattleActor.expGain = experience;
  gBattleActor.unit.exp += experience;

  CheckBattleUnitLevelUp(&gBattleActor);

}
