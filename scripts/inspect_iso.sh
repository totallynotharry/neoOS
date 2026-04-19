#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <path-to-iso>"
  exit 1
fi

ISO_PATH="$1"
if [[ ! -f "$ISO_PATH" ]]; then
  echo "ERROR: ISO file not found: $ISO_PATH"
  exit 1
fi

require_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "ERROR: required command not found: $1"
    exit 1
  fi
}

require_cmd xorriso

echo "Inspecting $ISO_PATH"
TREE="$(xorriso -indev "$ISO_PATH" -find / -type f -print 2>/dev/null || true)"
echo "$TREE"

required=(
  "/boot/limine-bios-cd.bin"
  "/boot/limine-uefi-cd.bin"
  "/limine-bios.sys"
  "/boot/kernel.elf"
  "/boot/limine.cfg"
)

missing=0
for path in "${required[@]}"; do
  if ! grep -Fxq "$path" <<<"$TREE"; then
    echo "MISSING: $path"
    missing=1
  fi
done

if [[ $missing -ne 0 ]]; then
  echo "ISO check FAILED"
  exit 2
fi

echo "ISO check PASSED"
