#!/usr/bin/python3

"""
Simple Tab-separated values -> Event Assembler table converter

This takes formatted .tsv files and converts them into structured
data for use with Event Assembler.
"""

import sys
import csv
from argparse import ArgumentParser
from pathlib import Path
from string import ascii_letters, digits
from typing import Any


def mangle(text: str) -> str:
  """Remove illegal characters."""
  return "".join([
      c for c in text
      if (c in ascii_letters) or (c in digits) or (c == "_")
    ])


# In order, these are:
# Command prefix: The character(s) that the command type must start with
# Parameters prefix: The character(s) that get placed after the command+spacer
#   but before the parameters
# Parameters suffix: The character(s) that get placed after the parameters
# Parameters spacer: The character(s) that get placed after the command but
#   before the parameters perfix
# Parameter separator: The character(s) that get placed between each parameter
DEFAULT_COMMAND = ("", "", "", " ", " ")
COMMAND_TYPES = [
    ("#", "", "", " ", " "),
    ("", "(", ")", "", ", "),
  ]


def get_command_format(command: str) -> tuple[str, str, str]:
  """
  Expand an entry command shorthand.

  In order to support the various types of commands that
  EA can lay down, we need to come up with some kind of
  shorthand to tell the formatter how to lay down command
  parameters:

  #somedirective -> #somedirective foo bar
  SOMERAW        -> SOMERAW foo bar
  SomeMacro()    -> SomeMacro(foo, bar)
  """
  c_text = command.strip("#()")

  matched_type = None
  for command_type in COMMAND_TYPES:
    c_prefix, p_prefix, p_suffix, p_spacer, p_separator = command_type
    param_wrapper = p_prefix + p_suffix

    if (command.startswith(c_prefix) and command.endswith(param_wrapper)):
      matched_type = command_type
      break

  if matched_type is None:
    matched_type = DEFAULT_COMMAND

  c_prefix, p_prefix, p_suffix, p_spacer, p_separator = matched_type

  return (f"{c_prefix}{c_text}{p_spacer}{p_prefix}", p_separator, p_suffix)


def process(table: Path) -> None:
  """Process a single .tsv into a .event."""
  if not table.exists():
    sys.exit(f"Unable to find '{table}'.")

  with table.open(mode="r", encoding="UTF-8") as t:
    sheet = csv.reader(t, dialect=csv.excel_tab)
    rows = [row for row in sheet]

  # The top-leftmost cell should contain a command for each entry
  # and optionally a starting index to begin defining table indices as.

  match rows[0][0].split():
    case [command, index]:
      c_prefix, p_separator, c_suffix = get_command_format(command)
      start_index = int(index, 16) if index.startswith("0x") else int(index)
    case [command]:
      c_prefix, p_separator, c_suffix = get_command_format(command)
      start_index = 0
    case _:
      sys.exit(f"Unable to parse top-leftmost cell in '{table}'.")

  fields = rows[0][1:]

  definitions, data = [], []

  def add_def(name: str, value: Any, pad: str = "") -> None:
    """Create an EA-style definition."""
    value = value if len(str(value).split()) == 1 else f'"{value}"'
    definitions.append(f'{pad}#define {name} {value}\n')

  for i, [name, *items] in enumerate(rows[1:], start=start_index):

    add_def(mangle(name), i)
    for j, item in enumerate(items):
      field = mangle(name + fields[j])
      add_def(field, item, "  ")

    definitions.append("\n")

    params = p_separator.join([mangle(name + field) for field in fields])
    data.append(f"{c_prefix}{params}{c_suffix}\n")

  outfile = table.with_suffix(table.suffix + ".event")
  with outfile.open("w", encoding="UTF-8") as o:
    o.writelines(definitions)
    o.writelines(data)


def main() -> int:
  """Convert one or more tables from the command line."""
  parser = ArgumentParser(
      description="Convert tab-separated tables into Event Assembler sources.",
    )
  parser.add_argument(
      "tables",
      metavar="table",
      nargs="+",
      type=Path,
      help="A tab-separated values file."
    )
  args = parser.parse_args()

  for table in set(args.tables):
    process(table)

  return 0


if __name__ == "__main__":
  sys.exit(main())
