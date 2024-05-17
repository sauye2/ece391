#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"
#include "paging.h"
#include "lib.h"
#include "paging.h"
#include "keyboard.h"
#include "syscall.h"
#define WRAP_SIZE 80
#define FOURKB 0x1000
#define MAX_TERMINALS 3
/* 
Parameters are designed to work with Appendix B. 
See Appendix B to view read, write, open, and close files
*/

extern void terminal_init();

extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t terminal_open(const uint8_t* filename);

extern int32_t terminal_close(int32_t fd);

extern int32_t terminal_switch(uint32_t terminal_index);

//void keyboard_to_terminal(int8_t ascii_char);

typedef struct terminal_t{
    uint32_t screen_X;
    uint32_t screen_Y;
    uint32_t video_mem_addr;
    uint32_t active;
    uint32_t display;
    uint32_t saved_ebp;
    uint32_t saved_esp;
    uint32_t buf_index;
    uint32_t num_processes;
    uint32_t pid;
    uint8_t  keyboard_buffer[128];      // buffer used to store characters
} terminal_t;

terminal_t terminal[MAX_TERMINALS];
int cur_term;

#endif
