# neoOS

neoOS is an independent, Unix-like operating system project written primarily in C.

> **Status:** early-stage scaffold and architecture baseline.

This repository now contains a buildable kernel skeleton and a concrete roadmap for the full product vision:
- macOS-like desktop shell (menu bar, animated magnifying dock, traffic-light window controls)
- core built-in apps (file manager, terminal, image viewer, Snake)
- Python script execution in terminal
- DOOM support as a real compatibility milestone
- native networking stack
- multi-architecture support (x86_64 + ARM64/Raspberry Pi)

The OS is **not based on Linux**.

## Current capabilities

- Cross-target kernel scaffold in C
- Basic graphics/window-server architecture documented
- Build system split by architecture (`x86_64`, `aarch64`)
- Bootable ISO workflow documented for x86_64 using Limine + UEFI/BIOS

## Quick start

### 1) Install toolchains (Ubuntu/Debian example)

```bash
sudo apt-get update
sudo apt-get install -y \
  make clang lld nasm xorriso mtools qemu-system-x86 qemu-system-aarch64 \
  gcc-aarch64-linux-gnu
```

### 2) Build x86_64 kernel

```bash
make TARGET=x86_64
```

### 3) Build bootable x86_64 ISO

```bash
make iso TARGET=x86_64
```

### 4) Run in QEMU

```bash
make run TARGET=x86_64
```

## Build instructions for bootable ISO

The bootable ISO flow is implemented for `x86_64`:

1. Build kernel ELF (`build/x86_64/kernel.elf`).
2. Stage ISO files under `build/x86_64/isodir`.
3. Copy `boot/limine.cfg` and kernel into ISO tree.
4. Use `xorriso` to generate `build/x86_64/neoOS-x86_64.iso`.
5. (Optional) Install Limine boot sectors if your local Limine tooling is present.

Command:

```bash
make iso TARGET=x86_64
```

## ARM / Raspberry Pi status

The build system supports `TARGET=aarch64` for kernel compilation and linker layout. For Raspberry Pi bring-up, the next milestone is a dedicated ARM boot image path (`.img`) with board-specific firmware handoff.

## Repository layout

```text
neoOS/
  boot/                  # bootloader configs
  docs/                  # architecture + roadmap
  kernel/
    include/             # kernel headers
    src/                 # kernel C sources
  scripts/               # build helper scripts
  Makefile
```

## Roadmap

See:
- `docs/ARCHITECTURE.md`
- `docs/ROADMAP.md`

## Publishing to GitHub main

I can generate clean commits and PR-ready history in this repo, but pushing directly to your GitHub `main` branch must be done from your authenticated environment (or CI bot) after review.
