#!/usr/bin/python3

"""
Slice font graphics into individual glyphs.

This script slices a sheet of glyph graphics into individual glyph images,
naming the files according to their unicode codepoint.
"""

import sys
import re
from argparse import ArgumentParser
from pathlib import Path
from itertools import product

desc = """Slice font graphics into individual glyphs."""
epilog = """
The output glyph graphics are either 8x16 or 16x16 pixels in size and are
named according to their unicode codepoints. The first cell in the sheet will
be given the codepoint specified by the 'starting_codepoint' parameter, and
the image will be sliced left-to-right, top-to-bottom, assigning the sliced
glyphs successive codepoints.

Blank glyphs in the sheet are not included in the output.

Example usage:
slice_glyphs.py "./ASCII.png" "./GLYPHS" 0x000000
"""
sheet_image_help = """
an indexed .png image made up of glyphs in 16x16 cells
"""
output_folder_help = """
the folder where sliced glyphs will be placed
"""
starting_codepoint_help = """
the unicode codepoint of the top-left glyph in the sheet
"""

codepoint_pattern = re.compile(r"""
    (?P<prefix>\$|0x)?
    (?P<value>[0-9a-fA-F]{1,6})
    (?P<suffix>(?(prefix)$|h))
  """, re.VERBOSE)


class Error(Exception):
  """Generic exception class."""


def _int_from_hex(value):
  """Convert a string into an integer."""
  match = codepoint_pattern.match(value)
  if match is None:
    raise ValueError(f"Unable to parse codepoint {value:s}.")
  return int(match.group("value"), 16)


def main():
  """Slice graphics."""
  try:
    from PIL import Image
  except ImportError:
    raise Error(
        "This script requires Pillow. See: "
        "https://pillow.readthedocs.io/en/stable/installation.html"
      )

  parser = ArgumentParser(description=desc, epilog=epilog)
  parser.add_argument(
      "sheet_image_file",
      type=Path,
      help=sheet_image_help,
      metavar="sheet_image"
    )
  parser.add_argument(
      "output_folder",
      type=Path,
      help=output_folder_help,
    )
  parser.add_argument(
      "starting_codepoint",
      type=_int_from_hex,
      help=starting_codepoint_help,
    )
  args = parser.parse_args()

  if not (si := args.sheet_image_file).exists():
    raise Error(f"Unable to open '{si}'.")

  if not (of := args.output_folder).is_dir():
    raise Error(f"Cannot find directory '{of}'.")

  sheet_image = Image.open(args.sheet_image_file)

  if not ((sheet_image.width % 16 == 0) and (sheet_image.height % 16 == 0)):
    raise Error(
        f"Sheet image size must be a multiple of (16, 16), "
        f"got {sheet_image.size}."
      )

  if not (sheet_image.mode == "P"):
    raise Error("Sheet image must use indexed color.")

  current = args.starting_codepoint

  glyph_coords = product(
      range(0, sheet_image.height, 16),
      range(0, sheet_image.width, 16)
    )
  for y, x in glyph_coords:

    glyph_im = sheet_image.crop((x, y, x + 16, y + 16))
    if (bbox := glyph_im.getbbox()) is not None:
      left, upper, right, lower = bbox

      glyph_im = glyph_im.crop((0, 0, 16 if (right > 8) else 8, 16))

      glyph_im.save(args.output_folder.joinpath(f"{current:06X}.png"))

    current += 1


if __name__ == '__main__':
  sys.exit(main())
