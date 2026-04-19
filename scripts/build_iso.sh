#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build/x86_64"
ISO_DIR="$BUILD_DIR/isodir"
ISO_OUT="$BUILD_DIR/neoOS-x86_64.iso"

mkdir -p "$ISO_DIR/boot"
cp "$BUILD_DIR/kernel.elf" "$ISO_DIR/boot/kernel.elf"
cp "$ROOT_DIR/boot/limine.cfg" "$ISO_DIR/boot/limine.cfg"

if [[ ! -f "$ISO_DIR/limine-bios.sys" ]]; then
  echo "ERROR: $ISO_DIR/limine-bios.sys is missing. Run: make prepare-limine"
  exit 1
fi

xorriso -as mkisofs \
  -b boot/limine-bios-cd.bin \
  -no-emul-boot \
  -boot-load-size 4 \
  -boot-info-table \
  --efi-boot boot/limine-uefi-cd.bin \
  -efi-boot-part --efi-boot-image --protective-msdos-label \
  "$ISO_DIR" -o "$ISO_OUT"

echo "ISO created: $ISO_OUT"
