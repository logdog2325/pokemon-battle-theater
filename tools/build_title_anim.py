#!/usr/bin/env python3
"""Build the animated title screen assets: 5 logo-composited frames sharing a
single 256-color palette, emitted as GBA Mode-4 .bitmap files (38400 bytes
each, row-major 8bpp) + one shared .bgpal (512 bytes, BGR555).

The artist delivered 5 logo-LESS animation frames (the battle scene with
flickering flames / Sceptile poses) plus a standalone transparent logo. We
composite the logo onto every frame at a fixed position so it stays
rock-steady while the scene animates behind it. A SHARED palette across all
frames means animation = swap pixel indices only; the palette loads once and
never changes (no inter-frame color flashing, and the VBlank page-flip only
moves bitmap data).

The in-ROM title plays these 5 frames ping-pong (0,1,2,3,4,3,2,1) to match
the artist's GIF.

Usage:
    python3 tools/build_title_anim.py SRC_DIR LOGO_PNG OUT_DIR
      SRC_DIR   folder containing 1.png .. 5.png (240x160 each)
      LOGO_PNG  standalone transparent logo (composited onto every frame)
      OUT_DIR   where battle_theater_f0.bitmap .. f4.bitmap + .bgpal land
"""
import sys
import os
from PIL import Image

# Logo placement (matched to the artist's TITLE SCREEN.png composition).
# v2.0.9: shrunk 160->138 and nudged down so "BATTLE THEATER" clears
# Charizard's face on the right. Kept slightly left of center (x=40) so the
# banner's right edge stays off the Charizard sprite.
LOGO_W = 138
LOGO_X = 40
LOGO_Y = 6
NUM_FRAMES = 5


def to_bgr555(r, g, b):
    return ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3)


def main():
    if len(sys.argv) != 4:
        sys.exit("usage: build_title_anim.py SRC_DIR LOGO_PNG OUT_DIR")
    src_dir, logo_png, out_dir = sys.argv[1:4]

    logo = Image.open(logo_png).convert("RGBA")
    lh = int(logo.height * LOGO_W / logo.width)
    logo = logo.resize((LOGO_W, lh), Image.LANCZOS)

    # Load + logo-composite each frame.
    frames = []
    for i in range(NUM_FRAMES):
        p = os.path.join(src_dir, f"{i + 1}.png")
        fr = Image.open(p).convert("RGBA")
        if fr.size != (240, 160):
            sys.exit(f"ERROR: {p} is {fr.size}, must be 240x160")
        fr.alpha_composite(logo, (LOGO_X, LOGO_Y))
        frames.append(fr.convert("RGB"))

    # Build ONE shared 256-color palette from all frames stacked vertically.
    stack = Image.new("RGB", (240, 160 * NUM_FRAMES))
    for i, fr in enumerate(frames):
        stack.paste(fr, (0, 160 * i))
    pal_img = stack.quantize(colors=256, method=Image.MAXCOVERAGE)

    # Emit shared palette (256 BGR555 entries, little-endian).
    pal = pal_img.getpalette()
    ncol = len(pal) // 3
    pal_bytes = bytearray()
    for i in range(256):
        if i < ncol:
            r, g, b = pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]
        else:
            r = g = b = 0
        v = to_bgr555(r, g, b)
        pal_bytes.append(v & 0xFF)
        pal_bytes.append((v >> 8) & 0xFF)
    assert len(pal_bytes) == 512
    with open(os.path.join(out_dir, "battle_theater_full.bgpal"), "wb") as f:
        f.write(pal_bytes)

    # Remap every frame onto the shared palette (NO dithering — dithering would
    # make static background pixels shimmer between frames). Emit one .bitmap
    # per frame.
    for i, fr in enumerate(frames):
        q = fr.quantize(palette=pal_img, dither=Image.NONE)
        idx = q.tobytes()
        assert len(idx) == 240 * 160, len(idx)
        name = "battle_theater_full.bitmap" if i == 0 else f"battle_theater_f{i}.bitmap"
        with open(os.path.join(out_dir, name), "wb") as f:
            f.write(idx)
        # Frame 0 keeps the legacy name so the existing single-frame INCBIN
        # path still resolves if the animation is ever reverted.

    print(f"OK: {NUM_FRAMES} frames, shared {ncol}-color palette -> {out_dir}")


if __name__ == "__main__":
    main()
