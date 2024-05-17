#ifndef _SYSCALL_HANDLER_H
#define _SYSCALL_HANDLER_H

#include "rtc.h"
#include "keyboard.h"
#include "syscall.h"
#include "scheduler.h"

extern void RTC_HANDLER_ASM();
extern void KEYBOARD_HANDLER_ASM();
extern void syscall_handler();
extern void PIT_HANDLER_ASM();

#endif
