#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "lib.h"

#define KEYBOARD_IRQ        1
#define KEYBOARD_BUF_SIZE   128 /* max size is 128 byes */
#define KEYMAP_SIZE         60 /* Current character code size */
#define KEYBOARD_PORT       0x60 /* Port keyboard is sending data to*/

/* Scancodes for non ascii keys and special requirements */
#define LEFT_SHIFT          0x2A
#define RIGHT_SHIFT         0x36
#define CTRL                0x1D    // left and right have same code
#define ALT                 0x38    // left and right have same code
#define CAPS                0x3A
#define TAB                 0x0F
#define BACKSPACE           0x0E
#define ENTER               0x1C
#define L_KEY               0x26    // Used to test if ctrl +l or ctrl + L   

#define F1                  0x3B
#define F2                  0x3C
#define F3                  0x3D

#define ASCII_BACKSPACE     0x08
#define ASCII_SPACE         0x20
#define ASCII_ENTER         0x0A
#define KEY_WRAP 80

#define TAB_SPACE_MAX       4       // Used for tab since it is four spaces!

/* Scancodes for Releases */
#define LEFT_SHIFT_RELEASE  0XAA
#define RIGHT_SHIFT_RELEASE 0XB6
#define ALT_RELEASE         0XB8    // left and right have same code
#define CTRL_RELEASE        0x9D    // left and right have same code


extern unsigned char keyboard_buf[KEYBOARD_BUF_SIZE];
extern int enter;                   // Tells terminal if we have a newline


extern void keyboard_init();

void keyboard_handler();



#endif
