#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
	printf 'Usage: %s OUTPUT_DIR\n' "$0" >&2
	exit 2
fi

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
output_dir=$1
private_key="$output_dir/training-ec-p256.pem"
public_key="$output_dir/training-ec-p256-public.pem"

if [[ -e "$private_key" || -e "$public_key" ]]; then
	printf 'Refusing to overwrite an existing training key in %s\n' "$output_dir" >&2
	exit 2
fi

mkdir -p "$output_dir"
umask 077
"$script_dir/imgtool.sh" keygen --key "$private_key" --type ecdsa-p256
"$script_dir/imgtool.sh" getpub --key "$private_key" --encoding pem --output "$public_key"
chmod 600 "$private_key"
chmod 644 "$public_key"

printf 'Private training key: %s\n' "$private_key"
printf 'Public verification key: %s\n' "$public_key"
