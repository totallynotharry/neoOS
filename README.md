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
- Bootable ISO workflow for x86_64 using Limine + UEFI/BIOS
- GitHub Actions workflow to publish x86_64 ISO artifacts on releases/tags

## Linux/macOS quick start

### 1) Install toolchains (Ubuntu/Debian example)

```bash
sudo apt-get update
sudo apt-get install -y \
  make clang lld nasm xorriso mtools qemu-system-x86 qemu-system-aarch64 \
  gcc-aarch64-linux-gnu git
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

## Windows build method

### Recommended: WSL2 (Ubuntu)

1. Install WSL2 and Ubuntu.
2. Clone this repo inside WSL.
3. Follow the Linux quick start commands above.

This is the easiest reliable path because the OS toolchain and ISO tooling are Linux-native.

### Optional native path: MSYS2/Clang (advanced)

A native Windows flow is possible but more fragile because Limine and ISO tooling assumptions are POSIX-centric. If you need fully native Windows builds, keep the same Makefile targets and install compatible equivalents of: `clang`, `lld`, `make`, `xorriso`, `git`, and QEMU in your MSYS2 environment.

## Build instructions for bootable ISO

The bootable ISO flow is implemented for `x86_64`:

1. Build kernel ELF (`build/x86_64/kernel.elf`).
2. `make prepare-limine` fetches/copies Limine CD boot files into `build/x86_64/isodir/boot`.
3. Stage `boot/limine.cfg` and kernel into the ISO tree.
4. Use `xorriso` to generate `build/x86_64/neoOS-x86_64.iso`.

Commands:

```bash
make TARGET=x86_64
make prepare-limine
make iso TARGET=x86_64
```

## Release ISO automation (GitHub Releases)

A GitHub Actions workflow is included at `.github/workflows/release-iso.yml`.

- Trigger: push a tag like `v0.1.0` (or manual run).
- Output: `neoOS-x86_64.iso` uploaded as a release asset.

Example:

```bash
git tag v0.1.0
git push origin v0.1.0
```


## VirtualBox notes (important)

If you see:

- `PANIC: Failed to load stage 3`
- `have you copied limine-bios.sys ...`

then the ISO was built without `limine-bios.sys` in expected locations. This repo now copies it into `/`, `/boot`, and `/limine` inside the ISO during `make prepare-limine`.

Recommended VM settings for x86_64 testing:

- **Type**: Other/Unknown (64-bit)
- **Memory**: 512 MB or more
- **Chipset**: PIIX3 (works reliably for legacy BIOS path)
- **EFI**: Disabled for BIOS boot testing, Enabled only if you want UEFI path
- **Storage**: Attach ISO as optical drive (not hard disk)
- **Acceleration**: VT-x/AMD-V enabled

Rebuild steps after pulling latest changes:

```bash
make clean
make TARGET=x86_64
make prepare-limine
make iso TARGET=x86_64
```

## ARM / Raspberry Pi status

The build system supports `TARGET=aarch64` for kernel compilation and linker layout. For Raspberry Pi bring-up, the next milestone is a dedicated ARM boot image path (`.img`) with board-specific firmware handoff.

## Repository layout

```text
neoOS/
  .github/workflows/       # CI/CD including release ISO workflow
  boot/                    # bootloader configs
  docs/                    # architecture + roadmap
  kernel/
    include/               # kernel headers
    src/                   # kernel C sources
  scripts/                 # build helper scripts
  Makefile
```

## Roadmap

See:
- `docs/ARCHITECTURE.md`
- `docs/ROADMAP.md`

## Publishing to GitHub main

I can generate clean commits and PR-ready history in this repo, but pushing directly to your GitHub `main` branch must be done from your authenticated environment (or CI bot) after review.
