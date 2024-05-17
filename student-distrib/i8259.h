/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H


#include "types.h"

/*

https://wiki.osdev.org/8259_PIC
See link above for detailed code and descirptions for further understanding

*/

/* Ports that each PIC sits on */
#define MASTER_8259_PORT    0x20 /* Base addr for master PIC */
#define SLAVE_8259_PORT     0xA0 /* IO Base addr for slave PIC */


#define MASK 0xFF

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1                0x11
#define ICW2_MASTER         0x20
#define ICW2_SLAVE          0x28
#define ICW3_MASTER         0x04
#define ICW3_SLAVE          0x02
#define ICW4                0x01

#define MASTER_8259_DATA    MASTER_8259_PORT + 1
#define SLAVE_8259_DATA     SLAVE_8259_PORT + 1

#define SLAVE_IRQ           2 /*Slave is located at IRQ2 per discussion slides */

#define MAX_IRQ             15 /* 0-7 is Primary PIC, 8-15 are slave PIC*/
#define MIN_IRQ             0
#define MASTER_PIC_MAX      8  /* 7 is our Master PIC*/

#define MASK                0xFF

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60
#define IRQNUM              16

/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
