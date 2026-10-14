#!/bin/sh
set -eu

workspace=${1:-.}
project="$workspace/training-zephyr/practices/day4/4.02-environment-check/reference"
build="$workspace/build/day4-4.02"

west build -p always -b native_sim "$project" -d "$build"
timeout 3 "$build/zephyr/zephyr.exe" 2>&1 | tee "$build/smoke.log" || test "$?" -eq 124
grep -q "DAY4_SMOKE PASS" "$build/smoke.log"
grep -q 'motion_sensor: bmi160@68' "$build/zephyr/zephyr.dts"
grep -q 'CONFIG_SENSOR=y' "$build/zephyr/.config"

printf '%s\n' '4.02 environment check: PASS'

