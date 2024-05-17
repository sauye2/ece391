#define ASM 1
#include "x86_desc.h"

.text
.globl syscall_handler
.globl RTC_HANDLER_ASM
.globl KEYBOARD_HANDLER_ASM
.globl PIT_HANDLER_ASM
.align 4

syscall_handler:
    pushl %ebp
    pushl %edi 
    pushl %esi          # Push and save all registers
    pushfl
    pushl %edx          # edx - third parameter
    pushl %ecx          # ecx- Second parameter
    pushl %ebx          # ebx - First parameter Do not push EAX, since it holds the syscall number
    

    # Do not push EAX, since it holds the syscall number
    testl %eax, %eax
    jz syscall_fail     # Return -1 if invalid syscall
    cmpl $0, %eax
    jle syscall_fail
    cmpl $10, %eax
    jg syscall_fail
    call *jump_table(, %eax, 4)

syscall_return:
    popl %ebx
    popl %ecx
    popl %edx
    popfl
    popl %esi
    popl %edi
    popl %ebp
    iret                # iret used to exit from interrupt procedure


syscall_fail:
    movl $-1, %eax
    jmp syscall_return



jump_table:
    .long 0
    .long sys_halt
    .long sys_execute
    .long sys_read
    .long sys_write
    .long sys_open
    .long sys_close
    .long sys_getargs
    .long sys_vidmap
    .long sys_set_handler
    .long sys_sigreturn

RTC_HANDLER_ASM:
    pushal
    pushfl
    call rtc_handler
    popfl
    popal
    iret

KEYBOARD_HANDLER_ASM:
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret

PIT_HANDLER_ASM:
    pushal
    pushfl
    call pit_handler
    popfl
    popal
    iret


