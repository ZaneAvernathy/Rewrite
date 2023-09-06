
#include "gbafe.h"
#include "CTF.h"

/*
 * UTF-8 text utilities
 */

int GetUTF8Displacement(char c, int shift) {
  /*
   * Gets the size of a UTF-8 encoded character
   * in bytes.
   */

  if (((c << shift) & 0x80) == 0)
    return shift;
  else
    return GetUTF8Displacement(c, shift + 1);
}

int ReadUTF8Character(char* text, int* outputCodepoint) {
  /*
   * Read a single UTF-8 character from a string.
   * Stores the character's codepoint in `outputcodepoint`
   * and returns its width in bytes.
   */

  int acc = 0;
  int width = 0;
  int i;

  if (*text < 0x80) {

    acc = *text;
    width = 1;

  } else {

    width = GetUTF8Displacement(*text, 0);
    acc = *text & (0x7F >> width);

    for (i = 1; i < width; i++) {

      acc = (acc << 6) | (text[i] & 0x7F);

    }
  }

  *outputCodepoint = acc;
  return width;

}
