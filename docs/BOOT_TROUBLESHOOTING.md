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

Limine's native protocol expects a higher-half x86_64 kernel image.
If you see this panic, your kernel ELF was linked at a low virtual address.

This repo now links x86_64 at `0xffffffff80000000` and uses `-mcmodel=kernel` to produce a higher-half kernel binary.

Rebuild:

```bash
make clean
make TARGET=x86_64
make prepare-limine
make iso TARGET=x86_64
```


## Error: black screen after selecting neoOS

If boot succeeds but the screen stays black, force text mode in Limine config:

```text
graphics: no
```

This repo now sets `graphics: no` by default and writes a kernel banner to VGA text memory (`0xB8000`) as an early bring-up sanity check.
