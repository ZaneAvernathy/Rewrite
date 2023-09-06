
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
