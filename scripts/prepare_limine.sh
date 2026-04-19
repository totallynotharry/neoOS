#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build/x86_64"
ISO_DIR="$BUILD_DIR/isodir"
ISO_BOOT_DIR="$ISO_DIR/boot"
ISO_LIMINE_DIR="$ISO_DIR/limine"
ISO_EFI_BOOT_DIR="$ISO_DIR/EFI/BOOT"
LIMINE_DIR="$BUILD_DIR/limine"
LIMINE_REF="v9.x-binary"

mkdir -p "$ISO_BOOT_DIR" "$ISO_LIMINE_DIR" "$ISO_EFI_BOOT_DIR"

copy_if_exists() {
  local src="$1"
  local dst="$2"
  if [[ -f "$src" ]]; then
    cp "$src" "$dst"
    return 0
  fi
  return 1
}

copy_from_prefix() {
  local prefix="$1"
  copy_if_exists "$prefix/limine-bios-cd.bin" "$ISO_BOOT_DIR/limine-bios-cd.bin" || true
  copy_if_exists "$prefix/limine-uefi-cd.bin" "$ISO_BOOT_DIR/limine-uefi-cd.bin" || true
  copy_if_exists "$prefix/limine-bios.sys" "$ISO_DIR/limine-bios.sys" || true
  copy_if_exists "$prefix/limine-bios.sys" "$ISO_BOOT_DIR/limine-bios.sys" || true
  copy_if_exists "$prefix/limine-bios.sys" "$ISO_LIMINE_DIR/limine-bios.sys" || true
  copy_if_exists "$prefix/BOOTX64.EFI" "$ISO_EFI_BOOT_DIR/BOOTX64.EFI" || true
  copy_if_exists "$prefix/BOOTIA32.EFI" "$ISO_EFI_BOOT_DIR/BOOTIA32.EFI" || true
}

if command -v limine >/dev/null 2>&1; then
  echo "Found limine executable in PATH; attempting to copy package assets"
  LIMINE_BIN="$(command -v limine)"
  LIMINE_PREFIX="$(cd "$(dirname "$LIMINE_BIN")/.." && pwd)"
  copy_from_prefix "$LIMINE_PREFIX/share/limine"
  copy_from_prefix "/usr/share/limine"
fi

if [[ ! -f "$ISO_BOOT_DIR/limine-bios-cd.bin" || ! -f "$ISO_BOOT_DIR/limine-uefi-cd.bin" || ! -f "$ISO_DIR/limine-bios.sys" ]]; then
  echo "Fetching Limine sources ($LIMINE_REF)..."
  rm -rf "$LIMINE_DIR"
  git clone --depth 1 --branch "$LIMINE_REF" https://github.com/limine-bootloader/limine.git "$LIMINE_DIR"
  make -C "$LIMINE_DIR"

  copy_from_prefix "$LIMINE_DIR"
  copy_from_prefix "$LIMINE_DIR/bin"
fi

if [[ ! -f "$ISO_BOOT_DIR/limine-bios-cd.bin" || ! -f "$ISO_BOOT_DIR/limine-uefi-cd.bin" || ! -f "$ISO_DIR/limine-bios.sys" ]]; then
  echo "ERROR: Missing required Limine artifacts: limine-bios-cd.bin, limine-uefi-cd.bin, limine-bios.sys"
  exit 1
fi

echo "Limine artifacts prepared under $ISO_DIR"
