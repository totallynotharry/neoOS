# neoOS Implementation Roadmap

## Phase 0 — Bootstrap (current)
- [x] repo scaffold
- [x] cross-target build structure
- [x] architecture and product roadmap docs

## Phase 1 — Minimal kernel boot
- [ ] early console + framebuffer text output
- [ ] memory map parsing
- [ ] IDT/exceptions (x86_64), vector table (aarch64)
- [ ] basic heap allocator

## Phase 2 — Processes and syscalls
- [ ] task scheduler
- [ ] userspace ELF loader
- [ ] syscall entry/dispatch
- [ ] VFS with ramfs

## Phase 3 — Desktop shell foundation
- [ ] compositor + input routing
- [ ] top menu bar with clock
- [ ] dock with magnification animation
- [ ] draggable/resizable windows + traffic-light controls

## Phase 4 — Core apps
- [ ] file manager
- [ ] terminal emulator + shell
- [ ] image viewer
- [ ] Snake

## Phase 5 — Networking + internet
- [ ] NIC driver (e1000)
- [ ] IPv4 stack
- [ ] TCP/UDP sockets
- [ ] DHCP + DNS tools

## Phase 6 — Compatibility milestones
- [ ] Python script execution from terminal
- [ ] DOOM running as user-space application

## Phase 7 — Hardware breadth
- [ ] stable x86_64 release image
- [ ] Raspberry Pi ARM64 bring-up image
- [ ] installer and updates
