# x86 Operating System Kernel

A small Unix-like operating system kernel written in C and x86 assembly. This project implements core OS components including interrupt handling, paging, device drivers, a read-only filesystem, system calls, process loading, and terminal I/O.

Built as a low-level systems project to understand how an operating system boots, manages memory, handles hardware interrupts, loads user programs, and exposes kernel services through a syscall interface.

## Overview

This kernel runs in an x86/QEMU environment and includes a custom bootable kernel image, user-level test programs, and a simple read-only filesystem. The project focuses on the core mechanics of operating systems rather than relying on an existing OS runtime or standard library.

Key areas implemented:

- Interrupt Descriptor Table setup for CPU exceptions, hardware interrupts, and system calls
- Programmable interrupt controller initialization
- Keyboard and RTC device drivers
- Virtual memory setup with paging
- Read-only filesystem with directory entries, inodes, and data blocks
- File descriptor abstraction with per-device operation tables
- System calls for executing programs, halting processes, reading, writing, opening, and closing files
- Terminal input/output with keyboard buffering
- User program loading through ELF validation and context switching
- Experimental multi-terminal and scheduler support

## Technical Highlights

### Kernel and Interrupt Handling

The kernel initializes descriptor tables, the PIC, device interrupts, paging, filesystem state, terminal support, and syscall dispatch. Exception handlers are registered through the IDT, while hardware interrupts are routed through assembly stubs into C handlers.

Implemented interrupt-related components include:

- CPU exception handlers
- Keyboard interrupt handling
- RTC interrupt handling
- PIT interrupt setup
- System call interrupt entry point
- IDT initialization and privilege-level setup

### Virtual Memory

Paging is initialized manually by setting up page directory and page table entries. The kernel maps video memory, kernel memory, and user program memory regions, then enables paging through control register updates.

The memory system supports:

- Kernel page mapping
- Video memory mapping
- User program virtual address mapping
- TLB flushing after page table changes
- Per-process physical memory selection

### Filesystem

The filesystem is a simple read-only filesystem built around a boot block, directory entries, inodes, and data blocks.

Implemented filesystem operations include:

- Initialize filesystem metadata from boot module memory
- Read directory entries by name
- Read directory entries by index
- Read file data by inode, offset, and length
- Directory read support
- Regular file read support
- Read-only write behavior

### System Calls and Process Loading

The syscall layer provides a small Unix-like interface between user programs and the kernel.

Implemented syscall behavior includes:

- `halt`
- `execute`
- `read`
- `write`
- `open`
- `close`
- `getargs`
- `vidmap`

The `execute` path validates executable files using ELF magic bytes, loads program data into user memory, sets up paging, initializes process control block state, configures file descriptors, and transitions from kernel mode into user mode with `iret`.

### File Descriptor Abstraction

Each process has a file descriptor table that maps descriptors to operation tables. This allows the same syscall interface to dispatch to different backends, including:

- Terminal input
- Terminal output
- RTC
- Directories
- Regular files

This mirrors the Unix-style idea that devices and files can be accessed through a common read/write/open/close interface.

### Terminal and Keyboard Input

The terminal layer works with the keyboard driver to support buffered keyboard input and terminal output. The keyboard handler supports printable characters, shift/caps behavior, backspace, enter, tab expansion, control shortcuts, and terminal-switch key combinations.

## Repository Structure

```text
.
├── student-distrib/      # Kernel source, drivers, paging, syscall layer, tests, build files
├── syscalls/             # User-space programs and syscall wrappers
├── fsdir/                # Files included in the generated read-only filesystem image
├── fish/                 # Fish animation/demo program assets
├── createfs              # Filesystem image creation utility
├── elfconvert            # ELF conversion utility
├── buglog.txt            # Debugging notes and issue history
└── README.md
