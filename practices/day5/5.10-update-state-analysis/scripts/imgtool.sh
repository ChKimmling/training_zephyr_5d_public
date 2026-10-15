#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
workspace_root=$(cd "$script_dir/../../../../.." && pwd)
python_bin="$workspace_root/.venv/bin/python"
imgtool_py="$workspace_root/bootloader/mcuboot/scripts/imgtool.py"

if [[ ! -x "$python_bin" ]]; then
	printf 'Missing workspace Python: %s\n' "$python_bin" >&2
	exit 2
fi

if [[ ! -f "$imgtool_py" ]]; then
	printf 'Missing imgtool: %s\n' "$imgtool_py" >&2
	exit 2
fi

exec "$python_bin" "$imgtool_py" "$@"
