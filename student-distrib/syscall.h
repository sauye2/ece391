#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "lib.h"
#include "filesys.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"
#include "paging.h"
#include "x86_desc.h"
#include "idt.h"

#define MAX_FILES     8
#define FD_START      2              // 0 is for stdin and 1 is for stdout
#define EIGHTMB       0x800000       // 8MB
#define FOURMB        0x400000
#define EIGHTKB       0x2000         // 8kB
#define FD_START_IDX  0
#define MAX_PCB       6
#define FOURB         4
#define THIRTY_TWOB   32
#define VID_INDEX     34

#define STDIN         0              // position of stdin in pcb
#define STDOUT        1              // position of stdout in pcb
#define USER_START    0x8000000
#define START_ADDR    0x8048000
#define USER_PROG     0x8400000

#define EXCEPTRET     256

#define MAXBUFSIZE    35

extern int idt_ex;
extern int32_t current_pid;
// Appendix B: System Calls
int32_t sys_halt (uint8_t status);
int32_t sys_execute (const uint8_t* command);
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t sys_open (const uint8_t* filename);
int32_t sys_close (int32_t fd);
int32_t sys_getargs (uint8_t* buf, int32_t nbytes);
int32_t sys_vidmap (uint8_t** screen_start);
int32_t sys_set_handler(int32_t signum, void* handler_address);
int32_t sys_sigreturn(void);

/* ASM functions in syscall_handler.S */
void RTC_HANDLER_ASM();
void KEYBOARD_HANDLER_ASM();
void init_fot_ptrs();
typedef struct file_operations_table{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} fot_t;


typedef struct file_descriptor{
    fot_t*   fot_ptr;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags;
} fd_t;

typedef struct pcb{              // idk if this is enough for pcb or if its correct -> slide 7 of discussion slides
    fd_t fda[MAX_FILES];
    int32_t pid;
    int32_t parent_pid;
    uint32_t ebp;
    uint32_t eip;
    uint32_t esp; 
    uint8_t arguments[THIRTY_TWOB];
} pcb_t;

// Helper functions
pcb_t* get_pcb(int32_t pid);
uint32_t* get_physmem(int32_t pid);
int32_t bad_call();

// File operation tables for the differnt drivers
fot_t rtc_table;
fot_t dir_table;
fot_t filesys_table;
fot_t stdin_table;                 // terminal driver
fot_t stdout_table;                // terminal driver
fot_t initialize_table;


#endif

