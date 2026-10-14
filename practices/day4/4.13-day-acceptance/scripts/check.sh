#!/bin/sh
set -eu

workspace=${1:-.}
project="$workspace/training-zephyr/practices/day4/4.13-day-acceptance/reference"
build="$workspace/build/day4-4.13"
log="$build/acceptance.log"

west build -p always -b native_sim "$project" -d "$build"
timeout 8 "$build/zephyr/zephyr.exe" >"$log" 2>&1 || test "$?" -eq 124
grep -q 'TCP_STATUS PASS' "$log"
west twister -T "$workspace/training-zephyr/practices/day4/4.07-probe-tests/solution" \
  -p native_sim --inline-logs \
  --outdir "$workspace/build/day4-4.07-twister"
printf '%s\n' 'Tag 4 acceptance: PASS'

