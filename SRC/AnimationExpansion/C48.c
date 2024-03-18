
#define THUMBLIB_VOLATILE

#include "gbafe.h"
#include "anime.h"
#include "thumblib3.h"

#define ANINS_COMMAND_GET_SOUND(instruction) ((instruction) >> 8) & 0xFFFF

void SomeBattlePlaySound_8071990(int songid, int volume);
void BattleAIS_ExecCommands_SwitchEnd(void);

inline void BattleAIS_ExecCommands_PlaySound(struct Anim* anim);

void THUMBLIB_FUNC BattleAIS_ExecCommands_PlaySoundWrapper(void)
{
  /*
   * This is an ugly wrapper for `BattleAIS_ExecCommands_PlaySound`.
   * Since the jump table entry we're replacing isn't a full-fledged
   * function, it isn't self-contained and needs r7 and a jump to outside
   * of the big switch statement.
   */

  register struct Anim* anim asm("r7");
  void* temp;

  BattleAIS_ExecCommands_PlaySound(anim);

  LDR_POOL(temp, &BattleAIS_ExecCommands_SwitchEnd);
  BX(temp);

}

inline void BattleAIS_ExecCommands_PlaySound(struct Anim* anim)
{
  /*
   * This replaces the `BattleAIS_ExecCommands` case for command 0x48
   * from `85 00 00 48` to `85 XX YY 48` where `XXYY` is a song ID to play.
   */

  u32 instruction = anim->pScrCurrent[-1];
  SomeBattlePlaySound_8071990(ANINS_COMMAND_GET_SOUND(instruction), 0x100);
}
