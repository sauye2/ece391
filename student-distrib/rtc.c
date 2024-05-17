#include "rtc.h"

volatile int read_flag = 0;   // Flag for rtc_read(). Once interrupt has been handled and flag is changed to equal 1, rtc_read() can return 0.

/*
 * init_rtc()
 *   DESCRIPTION: Initializes the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes the RTC
 */
void init_rtc(){
    outb(RTC_ADDR, inb(RTC_ADDR) | RTC_INT);
    //cli();
    //outb(RTC_REGA, RTC_ADDR)
    //outb(0x20, RTC_DATA) bhow to write 0x20 to data

    outb(RTC_REGB, RTC_ADDR);
    char prev = inb(RTC_DATA);
    outb(RTC_REGB,RTC_ADDR);

    outb(prev | BIT_6_MASK, RTC_DATA);
    
    enable_irq(RTC_IRQ);
    //sti();
    outb(RTC_ADDR, inb(RTC_ADDR) & RTC_UNINT);

}
void rtc_handler() { 
    //test_interrupts();
    // unsigned long flags;  << where was this from? i commented it out
    outb(RTC_REGC, RTC_ADDR);
    inb(RTC_DATA);
    read_flag = 1;
    send_eoi(RTC_IRQ);
}

/*
 * set_freq()
 *   DESCRIPTION: Sets the frequency of the RTC device
 *   INPUTS: frequency
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets the frequency of the RTC device
 */
void set_freq(int32_t frequency){
    char freq;

    /* 
     * Check to see if the frequency is a power of two or not. 
     * If not, then it is not a valid frequency for the RTC
     * There is a one line formula that you can use to check if a number is a power of two:
     * if((n != 0) && ((n & (n - 1)) == 0))
    */

    if((frequency != 0) && ((frequency & (frequency - 1)) == 0)) {       // if frequency is divisible by 2 and not == 0:

        // Values for the associated frequency found on the RTC datasheet
        if(frequency == 2){
            freq = 0x0F;
        }
        else if(frequency == 4){
            freq = 0x0E;
        }
        else if(frequency == 8){
            freq = 0x0D;
        }
        else if(frequency == 16){
            freq = 0x0C;
        }
        else if(frequency == 32){
            freq = 0x0B;
        }
        else if(frequency == 64){
            freq = 0x0A;
        }
        else if(frequency == 128){
            freq = 0x09;   
        }
        else if(frequency == 256){
            freq = 0x08;
        }
        else if(frequency == 512){
            freq = 0x07;        
        }
        else if(frequency == 1024){           // Frequency can go up to 8192 Hz, but will limit to 1024 Hz
            freq = 0x06;
        }
        // else if(frequency == 2048){
        //     freq = 0x05;        
        // }
        // else if(frequency == 4096){
        //     freq = 0x04;
        // }
        // else if(frequency == 8192){
        //     freq = 0x03;        
        // }
    }

    // same process as in init_rtc() but with register A
    outb(RTC_ADDR, inb(RTC_ADDR) | RTC_INT);
    //cli();
    outb(RTC_REGA, RTC_ADDR);
    char prev = inb(RTC_DATA);
    outb(RTC_REGA,RTC_ADDR);
    outb((prev & UPPER_4_MASK) | freq, RTC_DATA);  // send frequency rate to register A
    //sti();
    outb(RTC_ADDR, inb(RTC_ADDR) & RTC_UNINT);
}

/*
 * rtc_open()
 *   DESCRIPTION: Open RTC and reset the frequency to 2Hz
 *   INPUTS: filename
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: Reset the frequency to 2Hz
 */
int32_t rtc_open(const uint8_t* filename){
    set_freq(DEFAULT_FREQ);                   // set frequency to 2 Hz
    return 0;
}

/*
 * rtc_close()
 *   DESCRIPTION: Closes RTC
 *   INPUTS: fd
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: Closes RTC device
 */
int32_t rtc_close(int32_t fd){
    return 0;
}

/* 
 * rtc_read()
 *   DESCRIPTION: Waits for RTC interrupt to occur and returns
 *   INPUTS: fd, buf, nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: Only returns after an RTC interrupt has occured (use flags)
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    read_flag = 0;              // Set read_flag
    while(read_flag == 0);      // Acts as an infinite loop until interrupt handler is finished and has set read_flag = 1 in line 35.
    return 0;                   // Return 0 once interrupt has occured
}

/*
 * rtc_write()
 *   DESCRIPTION: 
 *   INPUTS: fd, buf, nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: Should always accept only a 4-byte integer specifying the interrupt rate in Hz
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    if(buf == NULL){            // if buf is NULL, write fails
        return -1;
    }

    int32_t freq = *((uint32_t*)buf);            // otherwise, set frequency equal to rate specified in buf
    set_freq(freq);
    return 0;
}

