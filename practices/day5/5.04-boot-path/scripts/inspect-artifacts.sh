#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
	printf 'Usage: %s BUILD_DIR\n' "$0" >&2
	exit 2
fi

build_dir=$1
exe="$build_dir/zephyr/zephyr.exe"
elf="$build_dir/zephyr/zephyr.elf"
map="$build_dir/zephyr/zephyr.map"

for tool in readelf nm objdump grep; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		printf 'Missing host tool: %s\n' "$tool" >&2
		exit 2
	fi
done

for artifact in "$exe" "$elf" "$map"; do
	if [[ ! -f "$artifact" ]]; then
		printf 'Missing build artifact: %s\n' "$artifact" >&2
		exit 2
	fi
done

printf 'Final native_sim executable\n'
readelf -h "$exe" | grep -E 'Class:|Type:|Machine:|Entry point address:'

printf '\nRelocatable Zephyr intermediate artifact\n'
readelf -h "$elf" | grep -E 'Class:|Type:|Machine:|Entry point address:'

printf '\nSelected sections in the executable\n'
readelf -S -W "$exe" | grep -E '[[:space:]][.](text|rodata|data|bss)[[:space:]]' || true

printf '\nSelected symbols in the executable\n'
nm -n "$exe" | grep -E 'boot_trace_|z_cstart| main$' || true

printf '\nMap references\n'
grep -n -E 'boot_trace_|z_cstart| main$' "$map" | head -n 40 || true

printf '\nInitialization sections in linker order\n'
grep -n -E '[.]z_init_(EARLY|PRE_KERNEL_1|PRE_KERNEL_2|POST_KERNEL|APPLICATION)' \
	"$map" | head -n 80 || true

printf '\nDisassembly labels\n'
objdump -d --demangle "$exe" | \
	grep -E '<boot_trace_(early|pre_kernel_1|pre_kernel_2|post_kernel|application)>:' || true
