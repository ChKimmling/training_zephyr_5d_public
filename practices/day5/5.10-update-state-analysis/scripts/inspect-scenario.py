#!/usr/bin/env python3
import json
from pathlib import Path
import subprocess
import sys

BOOT_MAGIC = "77c295f360d2ef7f3552500f2cb67980"

if len(sys.argv) != 3:
    raise SystemExit(f"Usage: {sys.argv[0]} SCENARIO_DIR PUBLIC_KEY")

scenario_dir = Path(sys.argv[1])
public_key = Path(sys.argv[2])
imgtool = Path(__file__).with_name("imgtool.sh")
metadata = json.loads((scenario_dir / "scenario.json").read_text())


def status(value):
    if value == 1:
        return "SET"
    if value == 255:
        return "UNSET"
    if value is None:
        return "N/A"
    return f"INVALID({value:#x})"


def inspect(slot):
    image = scenario_dir / f"{slot}.bin"
    dump = subprocess.run(
        [str(imgtool), "dumpinfo", "--format", "json", str(image)],
        check=True,
        capture_output=True,
        text=True,
    ).stdout
    info = json.loads(dump[dump.index("{"):])
    verify = subprocess.run(
        [str(imgtool), "verify", "--key", str(public_key), str(image)],
        capture_output=True,
        text=True,
    )
    security_counter = None
    for tlv in info.get("tlv_area", {}).get("tlvs_prot", []):
        if tlv.get("type_name") == "SEC_CNT":
            security_counter = int.from_bytes(bytes.fromhex(tlv["data"]), "little")
    trailer = info.get("trailer", {})
    return {
        "version": info["header"]["version"],
        "counter": security_counter,
        "signature": "VALID" if verify.returncode == 0 else "INVALID",
        "magic": "GOOD" if trailer.get("magic") == BOOT_MAGIC else "UNSET",
        "copy_done": status(trailer.get("copy_done")),
        "image_ok": status(trailer.get("image_ok")),
    }


print(f"Scenario: {scenario_dir.name}")
print(f"Observation: {metadata['observation']}")
print(f"Minimum security counter: {metadata['minimum_security_counter']}")
print("slot       version       counter  signature  magic  copy_done  image_ok")
for slot_name in ("primary", "secondary"):
    data = inspect(slot_name)
    print(
        f"{slot_name:<10} {data['version']:<13} {str(data['counter']):<8} "
        f"{data['signature']:<10} {data['magic']:<6} "
        f"{data['copy_done']:<10} {data['image_ok']}"
    )
