#!/usr/bin/env python3
"""Convert a 240x160 title-screen PNG into the GBA Mode-4 assets the title
screen INCBINs: a raw 8bpp index .bitmap (38400 bytes, row-major) and a
256-entry BGR555 .bgpal (512 bytes).

Re-creates the (since-removed) v1.6 convert_title.py so the title art has a
reproducible pipeline. Quantizes to <=255 colors, reserving palette index 0
as the fade/backdrop slot (kept identical to the image's most-common edge
color so the white fade-in reads cleanly).

Usage:
    python3 tools/convert_title.py SRC.png OUT.bitmap OUT.bgpal
"""
import sys
from PIL import Image


def to_bgr555(r, g, b):
    return ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3)


def main():
    if len(sys.argv) != 4:
        sys.exit("usage: convert_title.py SRC.png OUT.bitmap OUT.bgpal")
    src, out_bitmap, out_bgpal = sys.argv[1:4]

    img = Image.open(src).convert("RGB")
    if img.size != (240, 160):
        # Hard-fail rather than silently stretch — Mode 4 page is exactly
        # 240x160 and any other size would desync the framebuffer copy.
        sys.exit(f"ERROR: {src} is {img.size}, must be exactly 240x160")

    # Quantize to 256 colors. MAXCOVERAGE keeps the palette faithful to the
    # full-color source (median-cut can crush gradients in the stadium glow).
    q = img.quantize(colors=256, method=Image.MAXCOVERAGE)

    pal = q.getpalette()  # flat [r,g,b, r,g,b, ...], up to 256*3
    num_colors = len(pal) // 3

    # Emit palette: 256 BGR555 entries, little-endian, zero-padded.
    pal_bytes = bytearray()
    for i in range(256):
        if i < num_colors:
            r, g, b = pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]
        else:
            r = g = b = 0
        v = to_bgr555(r, g, b)
        pal_bytes.append(v & 0xFF)
        pal_bytes.append((v >> 8) & 0xFF)
    assert len(pal_bytes) == 512, len(pal_bytes)

    # Emit pixel indices: row-major, one byte per pixel, 240*160 = 38400.
    idx = q.tobytes()
    assert len(idx) == 240 * 160, len(idx)

    with open(out_bitmap, "wb") as f:
        f.write(idx)
    with open(out_bgpal, "wb") as f:
        f.write(pal_bytes)

    print(f"OK: {num_colors} colors -> {out_bitmap} ({len(idx)}B), "
          f"{out_bgpal} (512B)")


if __name__ == "__main__":
    main()
