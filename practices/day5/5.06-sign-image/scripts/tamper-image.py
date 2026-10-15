#!/usr/bin/env python3
from pathlib import Path
import sys

HEADER_SIZE = 0x200
PAYLOAD_OFFSET = 0x10

if len(sys.argv) != 3:
    raise SystemExit(f"Usage: {sys.argv[0]} INPUT_IMAGE OUTPUT_IMAGE")

source = Path(sys.argv[1])
destination = Path(sys.argv[2])
image = bytearray(source.read_bytes())
offset = HEADER_SIZE + PAYLOAD_OFFSET

if offset >= len(image):
    raise SystemExit(f"Image is too small for payload offset 0x{offset:x}")

image[offset] ^= 0x01
destination.write_bytes(image)
print(f"Changed one payload bit at image offset 0x{offset:x}: {destination}")
