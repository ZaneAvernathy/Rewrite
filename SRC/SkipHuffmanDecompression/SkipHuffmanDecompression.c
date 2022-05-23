
#include "gbafe.h"

extern void (*gpARM_HuffmanTextDecomp)(const char *, char *);
void String_CopyTo(char* dest, const char* source);

enum
{
  TEXT_END             = 0x00,
  TEXT_LOAD_PORTRAIT   = 0x10,
  TEXT_HUFFMAN_PADDING = 0x1F,
  TEXT_CONTROL_CODE    = 0x80,
};

void RemoveHuffmanPadding(char* text) // 0x0800A1C8
{
  /*
   * This function removes Huffman padding characters at
   * the end of a text entry. This is a lot of work
   * just to replace one byte, and it's weird that
   * it just leaves the padding bytes that end up at the
   * ends of odd-width lines.

   * This is slightly different from the vanilla function,
   * mostly in that it doesn't use an s16 for the position.
   * This ends up saving us a lot of space.
   */

  int pos = 0;
  char current;

  // Vanilla does this to avoid thinking that a 00
  // within a text code is the text terminator, but
  // I don't think that's really necessary considering that
  // other functions like String_CopyTo don't take these
  // into consideration.

  while (text[pos] != TEXT_END)
  {
    current = text[pos];

    if (current == TEXT_LOAD_PORTRAIT)
      pos += 2;

    else if (current == TEXT_CONTROL_CODE)
      pos += 1;

    pos++;
  }

  pos--;
  while (pos >= 0)
  {

    if (text[pos] != TEXT_HUFFMAN_PADDING)
      return;

    if (text[pos - 1] != TEXT_CONTROL_CODE)
      text[pos] = TEXT_END;

    pos--;
  }
}

void HuffmanTextDecompReplacement(const char* source, char* dest) // Original at 0x08002BA4
{
  /*
   * This is our actual code. Since we're using the uppermost
   * bit of our pointer to flag entries as uncompressed,
   * we can just treat the pointer like a signed value
   * and use that to select our copying method.
   */

  if ((s32)source > 0)
    gpARM_HuffmanTextDecomp(source, dest);

  else
  {
    source = (const char*)((int)source & (~0x80000000));
    String_CopyTo(dest, source);
  }
}
