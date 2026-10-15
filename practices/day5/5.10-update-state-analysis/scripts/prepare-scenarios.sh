#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 2 ]]; then
	printf 'Usage: %s BUILD_DIR OUTPUT_DIR\n' "$0" >&2
	exit 2
fi

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
reference_dir=$(cd "$script_dir/../reference" && pwd)
build_dir=$1
output_dir=$2
source_bin="$build_dir/zephyr/zephyr.bin"
key_dir="$output_dir/keys"
payload_dir="$output_dir/payloads"
image_dir="$output_dir/images"
scenario_dir="$output_dir/scenarios"
private_key="$key_dir/training-ec-p256.pem"
public_key="$key_dir/training-ec-p256-public.pem"
slot_size=0x4000

if [[ ! -f "$source_bin" ]]; then
	printf 'Missing build artifact: %s\n' "$source_bin" >&2
	exit 2
fi

mkdir -p "$key_dir" "$payload_dir" "$image_dir" "$scenario_dir"

if [[ -e "$private_key" && ! -e "$public_key" ]] ||
   [[ ! -e "$private_key" && -e "$public_key" ]]; then
	printf 'Incomplete training key pair in %s\n' "$key_dir" >&2
	exit 2
fi

if [[ ! -f "$private_key" ]]; then
	previous_umask=$(umask)
	umask 077
	"$script_dir/imgtool.sh" keygen --key "$private_key" --type ecdsa-p256
	"$script_dir/imgtool.sh" getpub --key "$private_key" \
		--encoding pem --output "$public_key"
	umask "$previous_umask"
	chmod 600 "$private_key"
	chmod 644 "$public_key"
fi

"$script_dir/derive-payloads.py" "$source_bin" "$payload_dir"

common_args=(
	--key "$private_key"
	--header-size 0x200
	--slot-size "$slot_size"
	--align 4
	--pad-header
)

"$script_dir/imgtool.sh" sign "${common_args[@]}" \
	--version 1.4.0+10 --security-counter 10 \
	"$payload_dir/current.bin" "$image_dir/current-unpadded.bin"
"$script_dir/pad-slot.py" "$image_dir/current-unpadded.bin" \
	"$image_dir/current-clean-slot.bin" "$slot_size"
"$script_dir/imgtool.sh" sign "${common_args[@]}" \
	--version 1.4.0+10 --security-counter 10 --confirm \
	"$payload_dir/current.bin" "$image_dir/current-confirmed.bin"
"$script_dir/imgtool.sh" sign "${common_args[@]}" \
	--version 2.0.0+20 --security-counter 20 --test \
	"$payload_dir/candidate.bin" "$image_dir/candidate-test.bin"
"$script_dir/imgtool.sh" sign "${common_args[@]}" \
	--version 1.3.5+9 --security-counter 9 --test \
	"$payload_dir/rollback.bin" "$image_dir/rollback-test.bin"

for scenario_name in \
	01-pending-test \
	02-test-unconfirmed \
	03-test-confirmed \
	04-rollback-candidate
do
	mkdir -p "$scenario_dir/$scenario_name"
	cp "$reference_dir/scenarios/$scenario_name.json" \
		"$scenario_dir/$scenario_name/scenario.json"
done

cp "$image_dir/current-confirmed.bin" "$scenario_dir/01-pending-test/primary.bin"
cp "$image_dir/candidate-test.bin" "$scenario_dir/01-pending-test/secondary.bin"

"$script_dir/patch-trailer.py" "$image_dir/candidate-test.bin" \
	"$scenario_dir/02-test-unconfirmed/primary.bin" --copy-done
cp "$image_dir/current-clean-slot.bin" \
	"$scenario_dir/02-test-unconfirmed/secondary.bin"

"$script_dir/patch-trailer.py" "$image_dir/candidate-test.bin" \
	"$scenario_dir/03-test-confirmed/primary.bin" --copy-done --image-ok
cp "$image_dir/current-clean-slot.bin" \
	"$scenario_dir/03-test-confirmed/secondary.bin"

cp "$image_dir/current-confirmed.bin" \
	"$scenario_dir/04-rollback-candidate/primary.bin"
cp "$image_dir/rollback-test.bin" \
	"$scenario_dir/04-rollback-candidate/secondary.bin"

printf 'Prepared scenarios in %s\n' "$scenario_dir"
