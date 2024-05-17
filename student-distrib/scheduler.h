#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "types.h"
#include "x86_desc.h"
#include "idt.h"
#include "syscall.h"
#include "lib.h"
#include "terminal.h"
#include "paging.h"
#include "i8259.h"

/* The Mode/Command register at I/O address 0x43 contains the following:
Bits         Usage
6 and 7      Select channel :
                0 0 = Channel 0
                0 1 = Channel 1
                1 0 = Channel 2
                1 1 = Read-back command (8254 only)
4 and 5      Access mode :
                0 0 = Latch count value command
                0 1 = Access mode: lobyte only
                1 0 = Access mode: hibyte only
                1 1 = Access mode: lobyte/hibyte
1 to 3       Operating mode :
                0 0 0 = Mode 0 (interrupt on terminal count)
                0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                0 1 0 = Mode 2 (rate generator)
                0 1 1 = Mode 3 (square wave generator)
                1 0 0 = Mode 4 (software triggered strobe)
                1 0 1 = Mode 5 (hardware triggered strobe)
                1 1 0 = Mode 2 (rate generator, same as 010b)
                1 1 1 = Mode 3 (square wave generator, same as 011b)
0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
Information from osdev: https://wiki.osdev.org/Programmable_Interval_Timer
*/

#define PIT_IRQ     0
#define PIT_MODE3   0x36    // 0x00110110 -> Channel 0 | lobyte/hibyte | mode 3 (square wave generator)
#define PIT_MODE_P  0x43    // Mode/Command register (write only)
#define PIT_DATA_P  0x40    // Channel 0 data port (read/write)
#define PIT_FREQ    1193182   // 1193182 hz * 10 ms = 11931
#define HIGH_MASK   0xFF00
#define LOW_MASK    0x00FF


// Functions to initialize and handle PIT (Programmable Interrupt Timer)
void init_pit();
void pit_handler();
void scheduler();




#endif

