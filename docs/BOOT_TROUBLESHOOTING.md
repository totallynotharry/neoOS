# neoOS Boot Troubleshooting

This guide focuses on common early-boot failures while testing `neoOS-x86_64.iso`.

## Error: `PANIC: Failed to load stage 3`

If Limine reports it cannot load stage 3, your ISO is missing `limine-bios.sys` or it is not in a searched path.

### Fix

Rebuild using:

```bash
make clean
make TARGET=x86_64
make prepare-limine
make iso TARGET=x86_64
```

`make prepare-limine` ensures these artifacts exist in the ISO tree:
- `boot/limine-bios-cd.bin`
- `boot/limine-uefi-cd.bin`
- `limine-bios.sys` (copied to `/`, `/boot`, `/limine`)

## Verify ISO contents before boot

Use:

```bash
./scripts/inspect_iso.sh build/x86_64/neoOS-x86_64.iso
```

Expected required entries:
- `/boot/limine-bios-cd.bin`
- `/boot/limine-uefi-cd.bin`
- `/limine-bios.sys`
- `/boot/kernel.elf`
- `/boot/limine.cfg`

## VirtualBox configuration checklist

- VM type: Other/Unknown (64-bit)
- RAM: 512MB+
- Chipset: PIIX3
- Attach ISO as optical drive
- Enable VT-x/AMD-V
- For BIOS path: disable EFI
- For UEFI path: enable EFI

## CI release caveat

Release workflow can publish a broken ISO if Limine assets are unavailable at build time.
Always inspect generated ISO contents when changing release tooling.


## Error: `Config File not Found`

If Limine boots but shows `Config File not Found`, ensure config exists in at least one default location.
The ISO build now places config at:
- `/limine.conf`
- `/limine.cfg`
- `/boot/limine.conf`
- `/boot/limine.cfg`

Rebuild the ISO and verify with:

```bash
./scripts/inspect_iso.sh build/x86_64/neoOS-x86_64.iso
```


## Error: `config file contains no valid entries`

This means Limine found the config file but rejected its syntax.
Use modern Limine syntax with a slash-prefixed entry and `key: value` options, for example:

```text
timeout: 0

/neoOS
    protocol: limine
    kernel_path: boot():/boot/kernel.elf
```

Avoid old-style forms like uppercase `PROTOCOL=` or entry labels like `:neoOS` with newer Limine versions.


## Error: `elf: Lower half PHDRs are not allowed`

This panic appears when using Limine native protocol with a low-half kernel ELF.
neoOS now avoids this in early bring-up by booting via:

- `protocol: multiboot2`
- `graphics: yes`

If you still hit this message, verify your ISO contains the latest `boot/limine.cfg` and rebuild from clean artifacts.


## Error: black screen after selecting neoOS

If boot succeeds but the screen stays black, prefer framebuffer mode in Limine config:

```text
graphics: yes
```

This repo now sets `graphics: yes` by default and draws directly to the framebuffer when available, with VGA text only as fallback.


## Display mode used by this repo

To maximize compatibility with VirtualBox BIOS boots during early bring-up, neoOS currently uses:

- `protocol: multiboot2`
- `graphics: yes`
- Multiboot2 framebuffer rendering (with VGA fallback)

This keeps display output reliable on VMs where direct VGA text mode is inconsistent.


## Error: colored glyphs/garbled symbols instead of readable text

That usually means VGA text memory writes are being attempted while the display is in graphics mode.
neoOS now prefers the Multiboot2 framebuffer tag and renders solid color blocks directly to the framebuffer when available, with VGA text only as fallback.


## Message: `Fallback text mode path` / `framebuffer unavailable`

This means the kernel did not receive a Multiboot2 framebuffer tag from the bootloader.
neoOS now embeds a framebuffer request (1024x768x32), but VM graphics settings can still block it.

Try in VirtualBox:
- Display Controller: **VMSVGA**
- Video Memory: **128 MB**
- Enable 3D Acceleration: **off** (for early bring-up)
- Boot in BIOS mode with the ISO attached as optical media

Then rebuild and boot again.
