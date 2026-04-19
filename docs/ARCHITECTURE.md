# neoOS Architecture (from-scratch, non-Linux)

## 1. Kernel model

- Language: C (with minimal assembly only where required)
- Kernel type: modular monolithic (microkernel-like boundaries where practical)
- Scheduler: preemptive, priority-based, SMP-aware
- Memory: virtual memory + copy-on-write + slab allocator + mmap-style API
- IPC: message channels + shared memory regions

## 2. Platform bring-up

### x86_64
- UEFI + Limine boot path
- APIC/HPET/PIT timer handling
- ACPI parsing for hardware discovery

### ARM64 (Raspberry Pi class)
- UEFI-compatible route where available, otherwise board firmware handoff
- Generic Interrupt Controller (GIC)
- Device Tree parsing

## 3. Graphics stack

- KMS-like display abstraction in kernel mode
- User-space compositor (`neowm`) with:
  - top menu bar with system clock
  - animated dock with icon magnification on pointer hover
  - window decorations with red/yellow/green buttons
  - draggable + resizable windows on all edges/corners

Pipeline:
1. App draws to shared surface buffer
2. Compositor applies transforms/animations
3. GPU/FB backend presents final frame

## 4. Window server + UI model

- Client/server protocol over IPC sockets
- Event model: pointer, keyboard, window lifecycle, drag/resize, focus
- Compositor owns z-order, clipping, hit-testing, and animation timing

## 5. Core apps

- **File Manager**: directory tree, copy/move/delete, file metadata panel
- **Terminal**: PTY support, shell, process control, pipes/redirection
- **Image Viewer**: PNG/JPEG decode, zoom/pan, fullscreen
- **Snake**: sample graphics/input/game loop app

## 6. Python execution plan

Terminal will execute Python scripts via one of these staged approaches:
1. Port CPython directly to neoOS libc/syscall ABI (long-term native route)
2. Short-term compatibility runtime layer with required POSIX-like interfaces

## 7. DOOM support milestone

Goal: run the original DOOM source port (e.g., chocolate-doom style target).

Prerequisites:
- working file I/O
- timer and keyboard input
- framebuffer or software-rendered window target
- audio output (initially optional for first frame milestone)

## 8. Networking stack

Layer plan:
- NIC drivers (e1000 first for emulators)
- Ethernet + ARP
- IPv4 + ICMP
- UDP + TCP
- DNS + DHCP user daemons

Initial success criterion: fetch a webpage via a minimal userland tool.

## 9. Multi-architecture policy

- Common kernel core in `kernel/src/common`
- Arch code in `kernel/src/arch/x86_64` and `kernel/src/arch/aarch64`
- Single syscall surface + ABI stability tests

## 10. Security baseline

- Per-process virtual address spaces
- User/kernel privilege separation
- Capability-based handles for kernel objects
- Signed package roadmap for app distribution
