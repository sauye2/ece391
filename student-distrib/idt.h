#ifndef IDT_H
#define IDT_H

#include "x86_desc.h"
#include "lib.h"
#include "types.h"
//#include "assemblylinkage.S"
#include "keyboard.h"
#include "rtc.h"
#include "syscall_handler.h"

#define NUM_EXCEPTIONS 21

/*The position in idt array where each exception occurs */
#define DIV_ERR         0x00
#define DEBUGG          0x01
#define NMII            0x02
#define BREAKPT         0x03
#define OVERFLOWW       0x04
#define BOUND_RANGE     0x05
#define INV_OPCODE      0x06
#define DEV_NA          0x07
#define DOUBLE_FLT      0x08
#define COPROC_SEG      0x09
#define INVALID_TSS     0x0A
#define SEG_NOT_PRES    0x0B
#define STACK_FAULTT    0x0C
#define GEN_PROTECT     0x0D
#define PAGE_FAULTT     0x0E
#define X87_FPU_FLOAT   0x10
#define ALIGN_CHECK     0x11
#define MACHINE_CHECK   0x12
#define SIMD_FLOATING   0x13
#define VIRTUALIZATIONN 0x14
#define CONTROL_PROCT   0x15
#define SYS_CALL        0x80

/* Hex value of PIC port where interrupt is sent to. */
#define KEYBOARD_HAND   0x21
#define RTC_HAND_PORT   0x28
#define PIT_HAND        0x20

/*
Name            vector      Error Code
Div Zero        0x00        None
Debug Exception 0x01        None
NMI Interrupt   0x02        None
BreakPoint Excp 0x03        None
OF Exception    0x04        None
Bound Range     0x05        None
Invalid Opcode  0x06        None
Device NA       0x07        None
Double Fault    0x08        0
Coprocessor     0x09        None
Invalid TSS     0x0A        seg selector index
Segment Not Pres0x0B        seg selector index
Stack Fault ex  0x0C        0 or seg selector
General Prot.   0x0D        0 or segment selector or idt vect num
Page fault Exc  0x0E        Yes - see IA-32 5-46
x87 FPU error   0x10        None
Alignment       0x11        0
Machine-check   0x12        None
SIMD float-point0x13        None
virtualization  0x14        None
control_protect 0x15        YES
Sys Call        0x80        

Need to handle cases differently, those with error code will have an additional field produced.


*/

extern void divide_error();
extern void debug();
extern void nmi();
extern void breakpoint();
extern void overflow();
extern void BOUND_range_exceeded();
extern void invalid_opcode();
extern void device_not_available();
extern void double_fault();
extern void coprocesor_segment_overrun();
extern void invalid_TSS();
extern void segment_not_present();
extern void stack_fault();
extern void general_protection();
extern void page_fault();
extern void x87_FPU_floating_point_error();
extern void alignment_check();
extern void machine_check();
extern void SIMD_floating_point();
extern void virtualization();
extern void control_protection();
extern void system_call();/*
extern int32_t system_call();

extern int32_t halt (uint8_t status);
extern int32_t execute (const uint8_t* command);
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t fd);
extern int32_t getargs (uint8_t* buf, int32_t nbytes);
extern int32_t vidmap (uint8_t** screen_start);
extern int32_t set_handler (int32_t signum, void* handler_address);
extern int32_t sigreturn (void);*/
extern void initialize_idt();

/*pushal pushes and pops eax, ecx, edx, ebx, esp, ebp, esi, edi
use struct to contain values*/
 
typedef struct __attribute__ ((packed)) reg_t {
    uint32_t eax_;
    uint32_t ecx_;
    uint32_t edx_;
    uint32_t ebx_;
    uint32_t esp_;
    uint32_t ebp_;
    uint32_t esi_;
    uint32_t edi_;
} reg_t;

/*packed means we placed these all together in memory so they can just follow one after another*/


extern void handler(uint32_t vector, uint32_t eflags, reg_t regs, uint32_t error_code);
extern void handler_no_error(uint32_t vector, uint32_t eflags, reg_t regs);

#endif /* IDT_H */
