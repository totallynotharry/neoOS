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
)

missing=0
for path in "${required[@]}"; do
  if ! grep -Fxq "$path" <<<"$TREE"; then
    echo "MISSING: $path"
    missing=1
  fi
done

config_paths=(
  "/limine.conf"
  "/limine.cfg"
  "/boot/limine.conf"
  "/boot/limine.cfg"
)

found_config=0
for cfg in "${config_paths[@]}"; do
  if grep -Fxq "$cfg" <<<"$TREE"; then
    found_config=1
    break
  fi
done

if [[ $found_config -eq 0 ]]; then
  echo "MISSING: Limine config file (looked for /limine.conf, /limine.cfg, /boot/limine.conf, /boot/limine.cfg)"
  missing=1
fi

if [[ $missing -ne 0 ]]; then
  echo "ISO check FAILED"
  exit 2
fi

echo "ISO check PASSED"
