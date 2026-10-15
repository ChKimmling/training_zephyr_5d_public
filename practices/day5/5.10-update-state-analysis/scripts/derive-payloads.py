#!/usr/bin/env python3
from pathlib import Path
import sys

if len(sys.argv) != 3:
    raise SystemExit(f"Usage: {sys.argv[0]} ZEPHYR_BIN OUTPUT_DIR")

source = Path(sys.argv[1])
output_dir = Path(sys.argv[2])
base = source.read_bytes()
output_dir.mkdir(parents=True, exist_ok=True)

variants = {
    "current.bin": b"\nTRAINING_IMAGE=current-1.4.0+10\n",
    "candidate.bin": b"\nTRAINING_IMAGE=candidate-2.0.0+20\n",
    "rollback.bin": b"\nTRAINING_IMAGE=rollback-1.3.5+9\n",
}

for name, marker in variants.items():
    destination = output_dir / name
    destination.write_bytes(base + marker)
    print(f"Derived payload: {destination}")
