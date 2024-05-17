#include "idt.h"
#include "syscall.h"
//extern int idt_ex = 0;
/*
 * IDT exception handler functions
 *   DESCRIPTION: A list of functions that act as handlers for exceptions specified in the x86 ISA manual
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints that a specific exception has occured. Currently also should freeze every
 */

/*extern int get_except();
extern void reset_except();

int get_except() { return idt_ex; }
void reset_except() { idt_ex = 0; }*/

void divide_error(){
     // //MY_ASM_MACRO_NO_ERROR(DIV_ERR);
    
    printf("Divide_Error Exception occured.\n");
    sys_halt(0);
}

void debug(){
    
     // //MY_ASM_MACRO_NO_ERROR(DEBUGG);
    printf("Debug Exception occured.\n");
    sys_halt(0);
}

void nmi(){
    
     // //MY_ASM_MACRO_NO_ERROR(NMII);
    printf("NMI Exception occured.\n");
    sys_halt(0);
}

void breakpoint(){
    
     //MY_ASM_MACRO_NO_ERROR(BREAKPT);
    printf("Breakpoint Exception occured.\n");
    sys_halt(0);
}

void overflow(){
    
     //MY_ASM_MACRO_NO_ERROR(OVERFLOWW);
    printf("Overflow Exception occured.\n");
    sys_halt(0);
}

void BOUND_range_exceeded(){
    
     //MY_ASM_MACRO_NO_ERROR(BOUND_RANGE);
    printf("BOUND_Range_Exceeded Exception occured.\n");
    sys_halt(0);
}

void invalid_opcode(){
    
     //MY_ASM_MACRO_NO_ERROR(INV_OPCODE);
    printf("Invalid_Opcode Exception occured.\n");
    sys_halt(0);
}

void device_not_available(){
    
     //MY_ASM_MACRO_NO_ERROR(DEV_NA);
    printf("Device_Not_Available Exception occured.\n");
    sys_halt(0);
}

void double_fault(){
    
     //MY_ASM_MACRO(DOUBLE_FLT);
    printf("Double_Fault Exception occured.\n");
    sys_halt(0);
}

void coprocesor_segment_overrun(){
    
     //MY_ASM_MACRO_NO_ERROR(COPROC_SEG);
    printf("Coprocessor_Segment_Overrun Exception occured.\n");
    sys_halt(0);
}

void invalid_TSS(){
    
     //MY_ASM_MACRO(INVALID_TSS);
    printf("Invalid_TSS Exception occured.\n");
    sys_halt(0);
}

void segment_not_present(){
    
     //MY_ASM_MACRO(SEG_NOT_PRES);
    printf("Segment_Not_Present Exception occured.\n");
    sys_halt(0);
}

void stack_fault(){
    
     //MY_ASM_MACRO(STACK_FAULTT);
    printf("Stack_Fault Exception occured.\n");
    sys_halt(0);
}

void general_protection(){
     //MY_ASM_MACRO(GEN_PROTECT);
    printf("General_Protection Exception occured.\n");
    sys_halt(0);
}

void page_fault(){
    idt_ex = 1;
     //MY_ASM_MACRO(PAGE_FAULTT);
    printf("Page_Fault Exception occured.\n");
    sys_halt(0);
}

void x87_FPU_floating_point_error(){
    
     //MY_ASM_MACRO_NO_ERROR(X87_FPU_FLOAT);
    printf("x87_FPU_Floating_Point_Error Exception occured.\n");
    sys_halt(0);
}

void alignment_check(){
    
     //MY_ASM_MACRO(ALIGN_CHECK);
    printf("Alignment_Check Exception occured.\n");
    sys_halt(0);
}

void machine_check(){
    
     //MY_ASM_MACRO_NO_ERROR(MACHINE_CHECK);
    printf("Machine_Check Exception occured.\n");
    sys_halt(0);
}

void SIMD_floating_point(){
    
     //MY_ASM_MACRO_NO_ERROR(SIMD_FLOATING);
    printf("SIMD_Floating_Point Exception occured.\n");
    sys_halt(0);
}

void virtualization(){
    
     //MY_ASM_MACRO_NO_ERROR(VIRTUALIZATIONN);
    printf("Virtualization Exception occured.\n");
    sys_halt(0);
}

void control_protection(){
    
     //MY_ASM_MACRO(CONTROL_PROCT);
    printf("Control_Protection Exception occured.\n");
    sys_halt(0);
}
 
void system_call(){
    printf("System Call has occured.\n");
   /*uint32_t x = tss.eax;
    switch(tss.eax){
        case 1:
            return halt((tss.ebx & ((1<<8)-1));
        case 2:
            return execute((tss.ebx & ((1<<8)-1));
        case 3:
            return read(tss.ebx, tss.ecx, tss.edx);
        case 4:
            return write(tss.ebx, tss.ecx,tss.edx);
        case 5:
            return open((tss.ebx & ((1<<8)-1));
        case 6:
            return close(tss.ebx);
        case 7:
            return getargs((tss.ebx & ((1<<8)-1),tss.ecx);
        case 8:
            return vidmap(tss.ebx,tss.ecx);
        case 9:
            return set_handler(tss.ebx,tss.ecx);
        case 10:
            return sigreturn(); 
    }
    return -1;*/
    sys_halt(0);
}
/*
int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);
*/


/*
 * initialize_idt()
 *   DESCRIPTION: Initializes the IDT
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes the IDT
 */
void initialize_idt(){
    int i;
    for(i = 0; i < NUM_VEC; i++){             // NUM_VEC contains the number of vectors in idt[]
        idt[i].present = 0;
        idt[i].seg_selector = KERNEL_CS;
        idt[i].size = 1;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;

        if(i < NUM_EXCEPTIONS && i != 15){    // Index 15 does not have interrupt
            idt[i].dpl = 0;                   // Interrupt and exception handlers should have DPL set to 0.
            idt[i].present = 1;
        }
        if(i == SYS_CALL){                        // 0x80 specifies index for system call
            idt[i].dpl = 3;                   // System call handler should have DPL set to 3
            idt[i].present = 1;
            idt[i].reserved3 = 1;             // reserved3 is 1 for system call for trap gate
        }
    }

    // set each exception in the IDT array
    SET_IDT_ENTRY(idt[DIV_ERR], divide_error);
    SET_IDT_ENTRY(idt[DEBUGG], debug);
    SET_IDT_ENTRY(idt[NMII], nmi);
    SET_IDT_ENTRY(idt[BREAKPT], breakpoint);
    SET_IDT_ENTRY(idt[OVERFLOWW], overflow);
    SET_IDT_ENTRY(idt[BOUND_RANGE], BOUND_range_exceeded);
    SET_IDT_ENTRY(idt[INV_OPCODE], invalid_opcode);
    SET_IDT_ENTRY(idt[DEV_NA], device_not_available);
    SET_IDT_ENTRY(idt[DOUBLE_FLT], double_fault);
    SET_IDT_ENTRY(idt[COPROC_SEG], coprocesor_segment_overrun);
    SET_IDT_ENTRY(idt[INVALID_TSS], invalid_TSS);
    SET_IDT_ENTRY(idt[SEG_NOT_PRES], segment_not_present);
    SET_IDT_ENTRY(idt[STACK_FAULTT], stack_fault);
    SET_IDT_ENTRY(idt[GEN_PROTECT], general_protection);
    SET_IDT_ENTRY(idt[PAGE_FAULTT], page_fault);
    SET_IDT_ENTRY(idt[X87_FPU_FLOAT], x87_FPU_floating_point_error);
    SET_IDT_ENTRY(idt[ALIGN_CHECK], alignment_check);
    SET_IDT_ENTRY(idt[MACHINE_CHECK], machine_check);
    SET_IDT_ENTRY(idt[SIMD_FLOATING], SIMD_floating_point);
    SET_IDT_ENTRY(idt[VIRTUALIZATIONN], virtualization);
    SET_IDT_ENTRY(idt[CONTROL_PROCT], control_protection);
    
    SET_IDT_ENTRY(idt[SYS_CALL], syscall_handler);
    
    
    /*Set all driver interrupts to have present bit = 1, and reserved three to specify trap gate*/
    idt[KEYBOARD_HAND].present = 1;
    idt[KEYBOARD_HAND].reserved3 = 1;
    idt[RTC_HAND_PORT].present = 1;
    idt[RTC_HAND_PORT].reserved3 = 1;
    idt[PIT_HAND].present = 1;
    idt[PIT_HAND].reserved3 = 1;

    SET_IDT_ENTRY(idt[KEYBOARD_HAND], KEYBOARD_HANDLER_ASM);
    //SET_IDT_ENTRY(idt[RTC_HAND_PORT], rtc_handler);
    SET_IDT_ENTRY(idt[RTC_HAND_PORT], RTC_HANDLER_ASM);   // was 0x28 but switched for magic numbers
    SET_IDT_ENTRY(idt[PIT_HAND], PIT_HANDLER_ASM);       // define 32 as index for pit


    lidt(idt_desc_ptr);
    
}

// void handler(uint32_t vector, uint32_t eflags, reg_t regs, uint32_t error_code) {

// }

// void handler_no_error(uint32_t vector, uint32_t eflags, reg_t regs) {

// }

