
#include "gbafe.h"
#include "anime.h"

bool CheckEkrHitDone(void);

#define ANINS_COMMAND_GET_COUNT(instruction) ((instruction) >> 8) & 0xFF

void AnimInterpret_HandleCommandReplacement(void* r0, void* r1, struct Anim* anim, u32 instruction)
{
  /*
   * This replaces the `ANIM_INS_TYPE_COMMAND` case for `AnimInterpret`'s
   * big switch statement.
   *
   * This hack allows for looped animations to occur while waiting for
   * HP changes to occur. It changes the animation script instruction
   * `85 00 00 01` into `85 00 XX 01`, where `XX` is the number of preceeding
   * script instructions to repeat while waiting.
   */

  anim->state2 = (anim->state2 & 0xFFF) | ANIM_BIT2_COMMAND;

  anim->commandQueue[anim->commandQueueSize] = ANINS_COMMAND_GET_ID(instruction);
  anim->commandQueueSize++;

  anim->timer = 1;

  switch (ANINS_COMMAND_GET_ID(instruction))
  {

    case ANIM_CMD_WAIT_01:

      if ( !(CheckEkrHitDone()) )
      {
        anim->pScrCurrent -= ANINS_COMMAND_GET_COUNT(instruction);
      }

      // Fall through

    case ANIM_CMD_WAIT_02:
    case ANIM_CMD_WAIT_03:
    case ANIM_CMD_WAIT_04:
    case ANIM_CMD_WAIT_05:
    case ANIM_CMD_WAIT_13:
    case ANIM_CMD_WAIT_18:
    case ANIM_CMD_WAIT_2D:
    case ANIM_CMD_WAIT_39:
    case ANIM_CMD_WAIT_52:
      anim->pScrCurrent--;
      break;

  } // switch (ANINS_COMMAND_GET_ID(instruction))

}
