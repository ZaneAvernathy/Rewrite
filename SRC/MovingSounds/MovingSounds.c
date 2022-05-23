
#include "gbafe.h"

/*
 * `lowPrioritySoundOffset` is a value to add to the
 * fetched sound IDs if sProcScr_MUStepSound already has
 * a idSound1 playing and u60_buggedmaybe is not set.
 * You should have lower-priority copies of your movement sounds
 * in the sound table after the normal priority ones.
 */
struct MU_StepSfx {
  /* 00 */ u16 loopSize;
  /* 02 */ u16 lowPrioritySoundOffset;
  /* 04 */ u16 data[];
};

extern const u8 gStepSoundClasses[255];
extern const struct MU_StepSfx* gStepSoundPointers[];

u8 MU_ComputeDisplayPosition(struct MUProc* proc, struct Vec2* out);
void MU_StartStepSfx(int soundId, int b, int hPosition);

void MU_AdvanceStepSfxReplacement(struct MUProc* proc)
{
  /*
   * This function picks a class's movement sounds and
   * handles looping through them as the unit moves.
   * 
   * The vanilla function uses two different switches,
   * one for mounted classes and another for nonmounted
   * classes, to determine what sounds should play.
   * 
   * Tequila's FE8 walking sounds fix uses a mapping of
   * classes: sound types and a mapping of
   * sound IDs: sound info pointers.
   * 
   * This replacement does the same thing, but now in C!
   */

  const struct MU_StepSfx* pStepSoundDefinition;

  unsigned cursor;
  struct Vec2 position;

  u8 soundType = gStepSoundClasses[proc->displayedClassId];
  pStepSoundDefinition = gStepSoundPointers[soundType];

  // Don't play a sound for certain classes.

  if (pStepSoundDefinition == NULL)
    return;

  cursor = Mod(proc->stepSoundTimer++, pStepSoundDefinition->loopSize);
  MU_ComputeDisplayPosition(proc, &position);

  // A sound of 0 means `don't play anything`.

  if (pStepSoundDefinition->data[cursor])
  {
    MU_StartStepSfx(
      pStepSoundDefinition->data[cursor],
      pStepSoundDefinition->lowPrioritySoundOffset,
      position.x
    );
  }

}
