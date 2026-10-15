#!/usr/bin/env python3
import argparse
from pathlib import Path

BOOT_MAGIC = bytes.fromhex("77c295f360d2ef7f3552500f2cb67980")
BOOT_MAX_ALIGN = 8

parser = argparse.ArgumentParser(description="Set selected MCUboot training trailer flags")
parser.add_argument("input_image", type=Path)
parser.add_argument("output_image", type=Path)
parser.add_argument("--copy-done", action="store_true")
parser.add_argument("--image-ok", action="store_true")
args = parser.parse_args()

image = bytearray(args.input_image.read_bytes())
if not image.endswith(BOOT_MAGIC):
    raise SystemExit("Image has no supported MCUboot trailer magic")

image_ok_offset = len(image) - len(BOOT_MAGIC) - BOOT_MAX_ALIGN
copy_done_offset = image_ok_offset - BOOT_MAX_ALIGN

if args.copy_done:
    image[copy_done_offset] = 0x01
if args.image_ok:
    image[image_ok_offset] = 0x01

args.output_image.write_bytes(image)
print(
    f"Patched trailer: {args.output_image} "
    f"copy_done={args.copy_done} image_ok={args.image_ok}"
)
