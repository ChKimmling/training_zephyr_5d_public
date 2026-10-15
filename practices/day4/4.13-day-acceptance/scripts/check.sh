#!/bin/sh
set -eu

workspace=${1:-.}
script_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
exercise_dir=$(dirname "$script_dir")
day4_dir=$(dirname "$exercise_dir")
project="$exercise_dir/reference"
build="$workspace/build/day4-4.13"
log="$build/acceptance.log"

west build -p always -b native_sim "$project" -d "$build"
timeout 8 "$build/zephyr/zephyr.exe" >"$log" 2>&1 || test "$?" -eq 124
grep -Eq 'client response 1: seq=1 .*quality=OK .*connected=1' "$log"
grep -Eq 'client response 2: seq=2 .*quality=OK .*connected=1' "$log"
grep -q 'TCP_STATUS PASS' "$log"
west twister -T "$day4_dir/4.07-probe-tests/solution" \
  -p native_sim --inline-logs \
  --outdir "$workspace/build/day4-4.07-twister"
printf '%s\n' 'Tag 4 acceptance: PASS'

