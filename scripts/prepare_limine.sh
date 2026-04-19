#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build/x86_64"
ISO_BOOT_DIR="$BUILD_DIR/isodir/boot"
LIMINE_DIR="$BUILD_DIR/limine"
LIMINE_REF="v9.x-binary"

mkdir -p "$ISO_BOOT_DIR"

if [[ -f "$ISO_BOOT_DIR/limine-bios-cd.bin" && -f "$ISO_BOOT_DIR/limine-uefi-cd.bin" ]]; then
  echo "Limine CD binaries already present in $ISO_BOOT_DIR"
  exit 0
fi

if command -v limine >/dev/null 2>&1; then
  echo "Found limine executable in PATH; copying nearby CD boot files if available"
  LIMINE_BIN="$(command -v limine)"
  LIMINE_PREFIX="$(cd "$(dirname "$LIMINE_BIN")/.." && pwd)"
  for p in \
    "$LIMINE_PREFIX/share/limine/limine-bios-cd.bin" \
    "$LIMINE_PREFIX/share/limine/limine-uefi-cd.bin" \
    "/usr/share/limine/limine-bios-cd.bin" \
    "/usr/share/limine/limine-uefi-cd.bin"; do
    if [[ -f "$p" ]]; then
      cp "$p" "$ISO_BOOT_DIR/"
    fi
  done
fi

if [[ ! -f "$ISO_BOOT_DIR/limine-bios-cd.bin" || ! -f "$ISO_BOOT_DIR/limine-uefi-cd.bin" ]]; then
  echo "Fetching Limine sources ($LIMINE_REF)..."
  rm -rf "$LIMINE_DIR"
  git clone --depth 1 --branch "$LIMINE_REF" https://github.com/limine-bootloader/limine.git "$LIMINE_DIR"
  make -C "$LIMINE_DIR"

  BIOS_CANDIDATES=(
    "$LIMINE_DIR/limine-bios-cd.bin"
    "$LIMINE_DIR/bin/limine-bios-cd.bin"
  )

  UEFI_CANDIDATES=(
    "$LIMINE_DIR/limine-uefi-cd.bin"
    "$LIMINE_DIR/bin/limine-uefi-cd.bin"
  )

  for f in "${BIOS_CANDIDATES[@]}"; do
    [[ -f "$f" ]] && cp "$f" "$ISO_BOOT_DIR/limine-bios-cd.bin" && break
  done

  for f in "${UEFI_CANDIDATES[@]}"; do
    [[ -f "$f" ]] && cp "$f" "$ISO_BOOT_DIR/limine-uefi-cd.bin" && break
  done
fi

if [[ ! -f "$ISO_BOOT_DIR/limine-bios-cd.bin" || ! -f "$ISO_BOOT_DIR/limine-uefi-cd.bin" ]]; then
  echo "ERROR: Could not locate limine-bios-cd.bin and limine-uefi-cd.bin"
  exit 1
fi

echo "Limine CD binaries prepared in $ISO_BOOT_DIR"
