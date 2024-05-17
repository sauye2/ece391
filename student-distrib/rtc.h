#ifndef RTC_H
#define RTC_H

#include "lib.h"
#include "i8259.h"

#define RTC_IRQ         8
#define RTC_ADDR        0x70
#define RTC_DATA        0x71
#define RTC_UNINT       0x7F
#define RTC_INT         0x80
#define RTC_REGA        0x8A
#define RTC_REGB        0x8B
#define RTC_REGC        0x0C
#define BIT_6_MASK      0x40
#define UPPER_4_MASK    0xF0
#define DEFAULT_FREQ    0x02       // Default frequency for RTC device is 2 Hz

extern void init_rtc();
extern void rtc_handler();

extern void set_freq(int32_t frequency);
extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_close(int32_t fd);
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);



#endif
