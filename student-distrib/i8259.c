/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  --  set to 0xFF = 1111 1111 to mask all int. upon init*/
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 --  set to 0xFF = 1111 1111 to mask all int. upon init*/

/* Initialize the 8259 PIC */
void i8259_init(void) {
    /* Do not need to wait due to fact we emulate hardware, and not actually running it */
    /*outb(data, port) - init control word then port num*/


    outb(ICW1, MASTER_8259_PORT); /*start init sequence (in cascade mode!)*/
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_DATA);    /*ICW2: Master PIC Vecrtor Offset*/
    outb(ICW2_SLAVE, SLAVE_8259_DATA);      /*ICW2: Slave PIC Vector Offset */
    outb(ICW3_MASTER, MASTER_8259_DATA);    /*ICW3: Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)*/
    outb(ICW3_SLAVE, SLAVE_8259_DATA);      /*ICW3: Tell Slave PIC its cascade identity (0000 0010)*/
    outb(ICW4, MASTER_8259_DATA);           /*ICW4: Set PICS to use 8086 Mode */
    outb(ICW4, SLAVE_8259_DATA);

    /* Enable IRQ2 where our Slave PIC is located at in master PIC*/
    enable_irq(2);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    /*If 0-7 we use master PIC */
    if (irq_num < MASTER_PIC_MAX && irq_num >= MIN_IRQ) {
        master_mask = master_mask & ~(1 << irq_num);
        outb(master_mask, MASTER_8259_DATA);
    /*Else we use Slave PIC*/
    } else if (irq_num <= MAX_IRQ) {
        irq_num -= MASTER_PIC_MAX;
        /*irq needs to be 0-7, even if it is in slave PIC*/
        slave_mask = slave_mask & ~(1 << irq_num);
        outb(slave_mask, SLAVE_8259_DATA);
    }

}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    /*If 0-7 we use master PIC */
    if (irq_num < MASTER_PIC_MAX && irq_num >= MIN_IRQ) {
        master_mask = master_mask | (1 << irq_num);
        outb(master_mask, MASTER_8259_DATA);
    } else if (irq_num <= MAX_IRQ) {
        irq_num -= MASTER_PIC_MAX;
        /*irq needs to be 0-7, even if it is in slave PIC*/
        slave_mask = slave_mask | (1 << irq_num);
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num < MASTER_PIC_MAX && irq_num >= MIN_IRQ) {
        /* As stated in header file, EOI gets ord with int number */
        outb(EOI | irq_num, MASTER_8259_PORT); /*Sends EOI to master PIC*/
    } else if (irq_num <= MAX_IRQ) {
        /*Need to adjust IRQ, as well as send EOI to BOTH pics*/
        irq_num -= MASTER_PIC_MAX;
        outb(EOI | irq_num, SLAVE_8259_PORT); /* send EOI to slave PIC*/
        outb(EOI | SLAVE_IRQ, MASTER_8259_PORT); /*send EOI to master PIC where slave PIC resides */
    }
}
