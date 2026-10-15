#!/usr/bin/env python3
from pathlib import Path
import sys

if len(sys.argv) != 4:
    raise SystemExit(f"Usage: {sys.argv[0]} INPUT_IMAGE OUTPUT_IMAGE SLOT_SIZE")

source = Path(sys.argv[1])
destination = Path(sys.argv[2])
slot_size = int(sys.argv[3], 0)
image = source.read_bytes()

if len(image) > slot_size:
    raise SystemExit(f"Image size {len(image)} exceeds slot size {slot_size}")

destination.write_bytes(image + bytes([0xFF]) * (slot_size - len(image)))
print(f"Padded slot without trailer request: {destination}")
