#!/usr/bin/env python3
"""Give the two Tabler icon webfonts distinct family names.

Upstream ships both `tabler-icons.ttf` and `tabler-icons-filled.ttf` with the
*same* embedded family name ("tabler-icons"). When both are registered with
Qt's font database they collapse into a single family and one face shadows the
other, so half the glyphs render as tofu. Renaming the families makes them
addressable independently.

Usage: python3 scripts/rename_fonts.py <font.ttf> "<New Family>"
"""
import sys

from fontTools.ttLib import TTFont


def rename(path: str, family: str) -> None:
    ps = family.replace(" ", "")
    font = TTFont(path)
    name = font["name"]
    for rec in name.names:
        if rec.nameID in (1, 16):  # family / typographic family
            rec.string = family
        elif rec.nameID in (4,):  # full name
            rec.string = family
        elif rec.nameID in (6,):  # postscript name
            rec.string = ps
    font.save(path)
    print(f"{path}: family -> {family!r}")


def main() -> None:
    if len(sys.argv) != 3:
        print(__doc__)
        raise SystemExit(1)
    rename(sys.argv[1], sys.argv[2])


if __name__ == "__main__":
    main()
