#!/usr/bin/env python3
"""Generate a Tabler icon name -> codepoint map from the webfont CSS files.

Produces assets/data/tabler_glyphs.json with two tables ("outline" and
"filled"). Icon names are stored without the leading "ti-" prefix. The
filled webfont uses unsuffixed names (e.g. "heart"), so callers that ask for
"heart-filled" should strip the suffix and look it up in the filled table.

Usage:
    python3 scripts/gen_glyph_map.py <outline.css> <filled.css> <out.json>
"""
import json
import re
import sys

RULE = re.compile(r"\.ti-([a-z0-9-]+):before\s*\{\s*content:\s*\"\\([0-9a-fA-F]+)\"")


def parse(path: str) -> dict:
    with open(path, "r", encoding="utf-8") as fh:
        text = fh.read()
    table = {}
    for name, cp in RULE.findall(text):
        table[name] = int(cp, 16)
    return table


def main() -> None:
    outline_css, filled_css, out_json = sys.argv[1], sys.argv[2], sys.argv[3]
    data = {"outline": parse(outline_css), "filled": parse(filled_css)}
    with open(out_json, "w", encoding="utf-8") as fh:
        json.dump(data, fh, separators=(",", ":"), sort_keys=True)
    print(f"outline glyphs: {len(data['outline'])}")
    print(f"filled glyphs : {len(data['filled'])}")


if __name__ == "__main__":
    main()
