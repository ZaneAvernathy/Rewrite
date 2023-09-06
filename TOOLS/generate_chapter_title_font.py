#!/usr/bin/python3

"""
Convert loose chapter title font glyphs into font data.

This script takes a folder's filepath as an argument and builds a set of files
from all of the glyphs that it finds: a set of font images containing all
of the glyphs, cut into pages that contain at most 256 tiles each, and a
metadata file containing information about each glyph.

"""

import sys
import re
from pathlib import Path
from argparse import ArgumentParser, RawTextHelpFormatter
from itertools import islice

desc = """Convert glyph images into a chapter title font.

This script will search the input folder for .png images that follow the naming
scheme '<unicode codepoint>[ <width>][ <upper margin>][ <lower margin>].png'.

For example, if you had a 'latin capital A' glyph that you wanted to be
considered 4px wide in-game, your glyph's filename would be '000041 04.png'.
Each part of the glyph's filename should be specified using hexadecimal (ASCII)
numbers and may optionally be padded with zeroes.

Glyph images are always 16px tall and are either 8px or 16px wide. Glyph images
should all be indexed images with the same 12 palettes; the palette is not
verified by this script.

An optional file named 'Whitespace.txt' may be placed in the folder to specify
codepoints of space characters and their widths. Each line of the file should
follow the scheme '<unicode codepoint> <width>'. For example, specifying the
width of an ASCII space character would look like '000020 3'. The numbers here
may be optionally padded with zeroes.

An optional file named 'Kerning.txt' may be placed in the folder to specify
pairs of codepoints that may be kerned together. Each line should be
'<left-side unicode codepoint> <right-side unicode codepoint> <adjustment>'
where the two codepoints are the characters to kern and the adjustment
is the number of pixels to move the right-side glyph from its normal position.
For example, to have 'A' and 'V' kern two pixels closer than normal when
adjacent, the line would be '000041 000056 -2'. A negative adjustment moves the
characters closer together while a positive one moves them farther apart.
Instead of codepoints, you may optionally use the character itself, surrounded
by single quotes. For instance: "'A' 'V' -2".

The outputs produced will be a set of images containing all of the glyphs,
a metadata file containing information about each glyph, a whitespace
metadata file containing information about whitespace characters, a kerning
metadata file containing kerning information, and an installer file that links
everything together.

The font glyphs are sliced into images containing the glyphs in order of
their codepoints. These images need to be converted into binaries and then
compressed before they can be used. The generated installer expects you to
do that yourself, as it's outside the scope of this script.

The metadata file is a tab-separated table that contains the glyph's codepoint,
width in pixels, whether the glyph's cell is 8 or 16 pixels wide,
upper margin (the distance between the top of the cell and the highest pixel of
the glyph), lower margin (the distance between the top of the cell and the
lowest pixel of the glyph), font page, and kerning information.

The whitespace metadata file is a tab-separated table that contains the
whitespace character's codepoint and width in pixels.

The installer expects that the metadata and whitespace metadata files
be converted into files readable by Event Assembler by some other tool. It
also expects that you create a palette binary from the first generated font
page image, containing all 12 text color palettes.

The kerning metadata file is an Event Assembler syntax file that consists of
groups of kerning information. Each group begins with a label that is named
after the left-side character and is followed by one or more macros that
contain the right-side character's codepoint and the adjustment value for
the pair. After the right-side macros, the list is terminated with a -1 word.

The installer file is an Event Assembler syntax file that is '#include'ed by
the Chapter Titles as Text EA installer, and shouldn't be '#include'ed by user
code.

"""

MIN_CELL_SIZE = 8
MAX_CELL_SIZE = 16
MAX_PAGE_TILES = 256
MAX_PAGES = 16

valid_cell_sizes = range(MIN_CELL_SIZE, MAX_CELL_SIZE + 8, 8)


class Error(Exception):
  """Generic exception class."""


glyph_file_pattern = re.compile(r"""
    (?P<codepoint>[0-9a-fA-F]+)
    (?:\s+(?P<width>[0-9a-fA-F]+))?
    (?:\s+(?(width)(?P<upper_margin>[0-9a-fA-F]+)))?
    (?:\s+(?(upper_margin)(?P<lower_margin>[0-9a-fA-F]+)))?
    \.png
  """, re.VERBOSE)

kerning_line_pattern = re.compile(r"""
    (?:'(?P<left_character>[^\'])'|(?P<left_codepoint>[0-9a-fA-F]+))
    \s+(?:'(?P<right_character>[^\'])'|(?P<right_codepoint>[0-9a-fA-F]+))
    \s+(?P<adjustment>-?[0-9a-fA-F]+)
  """, re.VERBOSE)

whitespace_line_pattern = re.compile(r"""
    (?P<codepoint>[0-9a-fA-F]+)
    \s+(?P<width>[0-9a-fA-F]+)
  """, re.VERBOSE)


installer_text = """
ALIGN 4

gCTFPageImagePointers:
{page_pointers}

#ifdef __DEBUG
  MESSAGE Chapter Title Font Image Pointers gCTFPageImagePointers to CURRENTOFFSET
#endif // __DEBUG

ALIGN 4; CTFPagesStart:

{page_inclusions}

#ifdef __DEBUG
  MESSAGE Chapter Title Font Image Graphics CTFPagesStart to CURRENTOFFSET
#endif // __DEBUG

ALIGN 4; gChapterTitleTextPalettes:
#incbin "CTF_Generated_Palette.pal"

#ifdef __DEBUG
  MESSAGE Chapter Title Font Palette gChapterTitleTextPalettes to CURRENTOFFSET
#endif // __DEBUG

ALIGN 4; gCTFMetadata:
  #include "CTF_Generated_Metadata.tsv.event"
  WORD (-1)

#ifdef __DEBUG
  MESSAGE Chapter Title Font Metadata gCTFMetadata to CURRENTOFFSET
#endif // __DEBUG

ALIGN 4; gCTFLookup:
{lookup_entries}
  WORD (-1)

#ifdef __DEBUG
  MESSAGE Chapter Title Font Lookup gCTFLookup to CURRENTOFFSET
#endif // __DEBUG

ALIGN 4; gCTFWhitespace:
{whitespace}
  WORD (-1)

#ifdef __DEBUG
  MESSAGE Chapter Title Font Whitespace gCTFWhitespace to CURRENTOFFSET
#endif // __DEBUG

ALIGN 4; gCTFKerning:
{kerning}
  WORD (-1)

#ifdef __DEBUG
  MESSAGE Chapter Title Font Kerning gCTFKerning to CURRENTOFFSET
#endif // __DEBUG

"""

whitespace_installer_text = \
  '  #include "CTF_Generated_Whitespace.tsv.event"'
kerning_installer_text = \
  '  #include "CTF_Generated_Kerning.event"'

page_pointer_template = "  POIN gCTFGeneratedPage{page:02d}"
page_inclusion_template = """
ALIGN 4; gCTFGeneratedPage{page:02d}:
#incbin "CTF_Generated_Page_{page:02d}.4bpp.lz77"
"""
lookup_template = "  ChapterTitleLookupEntry(0x{glyph:06X}, CTF_{glyph:06X})"


def batched(iterable, n):
    """Batch data into tuples of length n. The last batch may be shorter."""
    # Taken from https://docs.python.org/3/library/itertools.html?highlight=itertools#itertools-recipes
    # batched('ABCDEFG', 3) --> ABC DEF G
    if n < 1:
        raise ValueError('n must be at least one')
    it = iter(iterable)
    while batch := tuple(islice(it, n)):
        yield batch


def process_kerning_file(filename):
  """Parse kerning data into a dict."""
  kerning = {}

  with filename.open("r") as k:
    raw_kerning_lines = [
        l_ for l_ in k.readlines()
        if l_.strip()
      ]

  for line in raw_kerning_lines:
    match = kerning_line_pattern.match(line)

    if match is None:
      raise Error(
          f"Unable to parse kerning definition: '{line}'."
        )

    if (l_c := match.group("left_character")) is not None:
      left_codepoint = ord(l_c)
    else:
      left_codepoint = int(match.group("left_codepoint"), 16)

    if (r_c := match.group("right_character")) is not None:
      right_codepoint = ord(r_c)
    else:
      right_codepoint = int(match.group("right_codepoint"), 16)
    adjustment = int(match.group("adjustment"), 16)

    if left_codepoint not in kerning:
      kerning[left_codepoint] = []

    kerning[left_codepoint].append((right_codepoint, adjustment))

  return kerning


def process_whitespace_file(filename):
  """Parse whitespace data into a dict."""
  whitespace = {}
  with filename.open("r") as w:
    raw_whitespace_lines = [
        l_ for l_ in w.readlines()
        if l_.strip()
      ]

  for line in raw_whitespace_lines:
    match = whitespace_line_pattern.match(line)

    if match is None:
      raise Error(
          f"Unable to parse whitespace character definition: '{line}'."
        )

    whitespace_codepoint = int(match.group("codepoint"), 16)
    whitespace_width = int(match.group("width"), 16)

    whitespace[whitespace_codepoint] = whitespace_width

  return whitespace


def build_metadata_file(metadata, kerning, filename):
  """Construct the metadata file from the metadata."""
  metadata_lines = [
      "\t".join([
          "ChapterTitleFontEntry()",
          "Codepoint",
          "Width",
          "CellWidthFlag",
          "UpperMargin",
          "LowerMargin",
          "Page",
          "Tile",
          "Kerning",
        ])
    ]

  for glyph, (width, flag, upper, lower, page, tile) in metadata.items():

    line = "\t".join([
        f"CTF_{glyph:06X}",
        f"0x{glyph:06X}",
        f"{width:d}",
        f"{flag}",
        f"{upper:d}",
        f"{lower:d}",
        f"{page:d}",
        f"{tile:d}",
        f"CTF_Kerning_{glyph:06X}" if glyph in kerning else "NoKerning",
      ])
    metadata_lines.append(line)

  metadata_lines.append("")

  with filename.open("w") as o:
    o.write("\n".join(metadata_lines))


def build_whitespace_file(whitespace, filename):
  """Construct the whitespace file from the whitespace data."""
  whitespace_lines = [
      "\t".join([
          "ChapterTitleWhitespaceEntry()",
          "Codepoint",
          "Width",
        ])
    ]

  for character, width in whitespace.items():

    line = "\t".join([
        f"CTF_Whitespace_{character:06X}",
        f"0x{character:06X}",
        f"{width:d}"
      ])
    whitespace_lines.append(line)

  whitespace_lines.append("")

  with filename.open("w") as o:
    o.write("\n".join(whitespace_lines))


def build_kerning_file(kerning, filename):
  """Construct the kerning file from the kerning data."""
  kerning_lines = ["\n"]
  for left, right_list in kerning.items():
    kerning_lines.append(f"ALIGN 4; CTF_Kerning_{left:06X}:\n")
    for right, adjustment in right_list:
      kerning_lines.append(
          f"ChapterTitleKerningEntry(0x{right:06X}, {adjustment:d})\n"
        )
    kerning_lines.append("WORD (-1)\n\n")

  with filename.open("w") as o:
    o.writelines(kerning_lines)


def build_installer(metadata, whitespace, kerning, pagecount, filename):
  """Build the final installer file."""
  page_pointers = "\n".join([
      page_pointer_template.format(page=i)
      for i in range(pagecount)
    ])
  page_inclusions = "\n".join([
      page_inclusion_template.format(page=i)
      for i in range(pagecount)
    ])
  lookup_entries = "\n".join([
      lookup_template.format(glyph=chunk[0])
      for chunk in batched(metadata.keys(), 32)
    ])
  whitespace = whitespace_installer_text if whitespace else ""
  kerning = kerning_installer_text if kerning else ""

  installer = installer_text.format(
      page_pointers=page_pointers,
      page_inclusions=page_inclusions,
      lookup_entries=lookup_entries,
      whitespace=whitespace,
      kerning=kerning,
    )

  with filename.open("w") as o:
    o.write(installer)


def main():
  """Process glyphs into sheets and metadata."""
  try:
    from PIL import Image
  except ImportError:
    raise Error(
        "This script requires Pillow. See: "
        "https://pillow.readthedocs.io/en/stable/installation.html"
      )

  parser = ArgumentParser(
      description=desc,
      formatter_class=RawTextHelpFormatter
    )
  parser.add_argument(
      "folder",
      type=Path,
      help="A folder that contains glyph images."
    )

  args = parser.parse_args()

  if not args.folder.is_dir():
    raise NotADirectoryError(args.folder)

  glyph_files = sorted([
      f for f in args.folder.glob("*.png")
      if glyph_file_pattern.match(str(f.name)) is not None
    ])

  if not glyph_files:
    raise Error(
        f"Could not find any glyph files in folder."
      )

  # This will get combined with the glyph metadata later,
  # along with building up its output file, but we want to
  # catch errors here early because the file is easier to parse.

  kerning = {}
  if (kf := args.folder.joinpath("Kerning.txt")).exists():
    kerning = process_kerning_file(kf)

  # Same deal with the whitespace; it's faster to catch errors
  # than going through all of the glyphs, so we do it first.

  whitespace = {}
  if (wf := args.folder.joinpath("Whitespace.txt")).exists():
    whitespace = process_whitespace_file(wf)

  # Maybe naive, but I'm going to use the first glyph's palette for
  # the first page's palette.

  ref_pal = Image.open(glyph_files[0]).getpalette()

  current_page = Image.new("P", (256, 64))
  current_page.putpalette(ref_pal)

  font_images = []
  font_page = 0
  x, y = 0, 0

  metadata = {}

  for glyph_file in glyph_files:

    glyph_image = Image.open(glyph_file)

    if glyph_image.mode != "P":
      raise Error(
          f"Glyph image '{glyph_file}' must be an indexed image."
        )

    if (h := glyph_image.height) != 16:
      raise Error(
          f"Glyph image '{glyph_file}' must have a height of 16 pixels, "
          f"got {h}."
        )

    if (w := glyph_image.width) not in valid_cell_sizes:
      raise Error(
          f"Glyph image '{glyph_file}' must have a width in pixels of "
          f"one of {list(valid_cell_sizes)}, got {w}."
        )

    match = glyph_file_pattern.match(str(glyph_file.name))
    glyph_codepoint = int(match.group("codepoint"), base=16)

    # It's unlikely to happen, but ensure that the glyph hasn't
    # been defined as whitespace already.

    if glyph_codepoint in whitespace:
      raise Error(
          f"Codepoint '{glyph_codepoint:06X} already defined as whitespace."
        )

    glyph_bbox = glyph_image.getbbox()
    _, glyph_upper_margin, glyph_width, glyph_lower_margin = glyph_bbox

    # We might need to emit the current page if the glyph doesn't fit.

    required_tiles = (w * h) // (8 * 8)

    if (required_tiles + ((32 * y) + x)) > MAX_PAGE_TILES:

      if len(font_images) == MAX_PAGES:
        raise Error(
            "Cannot create font page; too many font pages."
          )

      font_images.append(current_page)

      x, y = 0, 0
      font_page += 1

      current_page = Image.new("P", (256, 64))
      current_page.putpalette(ref_pal)

    # We might need to move to the next row if the glyph
    # doesn't fit.

    if (((x * 8) + w) > 256):
      x = 0
      y += 2

    current_page.paste(glyph_image, (x * 8, y * 8))

    # Next, override the glyph's metadata if the filename specified data.

    if (mw := match.group("width")) is not None:
      glyph_width = mw

    if (mu := match.group("upper_margin")) is not None:
      glyph_upper_margin = mu

    if (ml := match.group("lower_margin")) is not None:
      glyph_lower_margin = ml

    # Finally, save the metadata.

    metadata[glyph_codepoint] = (
        glyph_width,
        True if (w == MAX_CELL_SIZE) else False,
        glyph_upper_margin,
        glyph_lower_margin,
        font_page,
        ((32 * y) + x),
      )

    x += (w // 8)

  # Don't forget to emit the last font page

  if current_page.getbbox() is not None:

    if len(font_images) == MAX_PAGES:
      raise Error(
          "Cannot create font page; too many font pages."
        )

    font_images.append(current_page)

  # Finally, build the output files.

  metadata_file = args.folder.joinpath("CTF_Generated_Metadata.tsv")
  build_metadata_file(metadata, kerning, metadata_file)

  if whitespace:
    whitespace_file = args.folder.joinpath("CTF_Generated_Whitespace.tsv")
    build_whitespace_file(whitespace, whitespace_file)

  if kerning:
    kerning_file = args.folder.joinpath("CTF_Generated_Kerning.event")
    build_kerning_file(kerning, kerning_file)

  for i, im in enumerate(font_images):
    im.save(args.folder.joinpath(f"CTF_Generated_Page_{i:02d}.png"))

  pagecount = len(font_images)
  installer_file = args.folder.joinpath("CTF_Generated_Installer.event")
  build_installer(metadata, whitespace, kerning, pagecount, installer_file)

  return 0


if __name__ == "__main__":
  sys.exit(main())
