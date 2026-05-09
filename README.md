# Embedded-Firmwares

Minimal preemptive multitasking kernel for ARM Cortex-M3
(TI Stellaris LM3S6965) — no RTOS, no `libc`, no startup library.

## Features

- **Bare-metal boot**
  - Custom `Reset_Handler`
  - `.data` copy from FLASH → SRAM
  - `.bss` zero-init
- **Custom interrupt vector table**
  - Typed C struct (`ISR_VectorTable`)
  - Each vector named and type-checked
- **Scheduling**
  - `SysTick` pends `PendSV`
  - `PendSV` is `naked` → tail-calls scheduler in asm
  - Round-robin, rotating from current PCB
- **PSP / MSP separation**
  - Kernel: MSP, privileged
  - Process: PSP, unprivileged Thread
  - Initial exception frame forged by kernel
    (xPSR T-bit, PC, LR = `0xFFFFFFFD`, r0–r3, r12)
- **Static PSP pool**
  - Dedicated `.psp_pool` linker section, 32 KB
  - 8 × 4 KB per-process stacks
  - No heap, no `malloc`
- **Context switch**
  - `context_asm.S` — actual implementation
  - `context.c` — C reference (excluded from build)
  - `kernel_offsets.h` — SSOT for PCB offsets / enum values
  - `_Static_assert` guards C/asm drift at compile time
- **Polled UART terminal**
  - Color-coded prompt
  - ANSI line editor (←/→/Del/Backspace)
- **HardFault diagnostics**
  - PCB table dump
  - PSP frame dump

## Memory map

```
FLASH (rx) 0x00000000 ─ 256 KB
  .text         code
  .rodata       read-only data
  _etext        end of FLASH image (LMA of .data follows)

SRAM  (rwx) 0x20000000 ─ 64 KB
  .data         initialized globals (LMA in FLASH, VMA in SRAM)
  .bss          zero-initialized globals
  .stack         4 KB   MSP / kernel stack
  .psp_pool     32 KB   8 × 4 KB process stacks (grows downward)
```

Authoritative layout: `arm-cortex-m3/LM3S6965/lm3s6965.ld`.

## Project structure

```
arm-cortex-m3/LM3S6965/
├── sources/
│   ├── startup.c       Reset_Handler, vector table, fault & IRQ handlers
│   ├── process.c       PCB pool, init, ready-queue search, idle task
│   ├── context.c       scheduler (C reference, NOT compiled in)
│   ├── context_asm.S   scheduler + ctx_save/ctx_restore (actual)
│   ├── uart.c          polled UART driver
│   ├── terminal.c      interactive shell with line editing
│   └── hello.c         example user task
├── includes/
│   ├── kernel_offsets.h  SSOT for PCB offsets / enum values (asm + C)
│   ├── process.h         pcb_t, proc_state_t, kernel_start()
│   ├── context.h         ctx_save / ctx_restore / kernel_schedule
│   ├── lm3s6965.h        BSP: peripheral base, ISR_VectorTable
│   ├── core_cm3.h        CMSIS Cortex-M3 core
│   ├── cmsis_*           CMSIS helpers
│   └── types.h           addr_t, NULL
├── lm3s6965.ld         linker script
└── Makefile            build + asm dump target
```

## Prerequisites

- `arm-none-eabi-gcc`, `objcopy`, `size`
- GNU Make
- `qemu-system-arm` ≥ 10.0

Skip manual install with `./scripts/setup.sh` (see below).

## Quick start

### Setup

```sh
./scripts/setup.sh
```

- Installs ARM GCC toolchain, QEMU, and macOS runtime dep `zstd`
- Idempotent — re-running with everything present is a no-op

### Build

```sh
./scripts/build.sh         # builds/lm3s6965.{elf,bin}
./scripts/build.sh asm     # per-source .s under asms/
./scripts/build.sh clean
```

- Thin wrapper around `make -C arm-cortex-m3/LM3S6965`
- Args forwarded to make; `make` directly also works

### Run

```sh
qemu-system-arm -M lm3s6965evb -nographic \
    -kernel arm-cortex-m3/LM3S6965/builds/lm3s6965.elf
```

- `-nographic` routes UART0 to your terminal
- Exit QEMU: `Ctrl-A x`
- On boot you should see:

  ```
  qemu@lm3s6965:~$
  ```

  Input is echoed; arrow keys, Backspace, Delete edit in place.

## Architecture notes

### First dispatch

- `Reset_Handler`
  - Inits PSP pool
  - Registers `idle_task` and `terminal`
  - Starts SysTick
  - Spins on MSP
- First SysTick → `PendSV`
  - `g_current_running` is `NULL` → context save skipped
  - Scheduler picks first `READY` PCB
  - Exception return with `LR = 0xFFFFFFFD` → Thread + PSP

### PendSV is `naked`

- Handler tail-calls `kernel_schedule` in asm
- Avoids compiler prologue that would clobber the PSP frame

### SSOT for asm / C boundary

- `kernel_offsets.h` defines:
  - `PCB_OFFSET_STATE`, `PCB_OFFSET_PSP`
  - `PROC_STATE_RUNNING`, `PROC_STATE_READY`, `PROC_STATE_WAITING`
- C side: `_Static_assert` against `offsetof(pcb_t, …)` and enum values
- Asm side: same macros via `.S` preprocessing
- Struct/enum drift caught at compile time, not at runtime

## Status & limitations

- No process termination
- No priorities, no IPC, no syscall interface
- No MPU; nothing prevents one PSP from overflowing into its neighbor
- Terminal lives in Thread mode and prints directly (no syscall layer)

## License

MIT — see [LICENSE](LICENSE).
