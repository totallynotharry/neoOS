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
