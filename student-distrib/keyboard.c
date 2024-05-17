#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "terminal.h"
/* Ascii hex to test range - used to determine if number or letter*/
#define ascii_a             0x61
#define ascii_z             0x7A
#define ascii_A             0x41
#define ascii_Z             0x5A

/* 
Tells us if these buttons are being used or not 
Caps: 1 means on, 0 off
shift, alt, control: 1 means pressed, 0 not 
*/
uint8_t caps_lock = 0;
uint8_t left_shift = 0;
uint8_t right_shift = 0;
uint8_t control = 0;
uint8_t alt = 0;


int enter = 0;



/* Below is a table mapping scan codes into ascii (8 bit int) */
/* Got rid of ascii codes for special keys, and will instead map key code to functionality */
static unsigned char key_map[KEYMAP_SIZE][2] = {
    
    {0x00, 0x00}, {0x00, 0x00}, {0x31, 0x21}, {0x32, 0x40},         // NULL, esc, 1/!, 2/@, 
    {0x33, 0x23}, {0x34, 0x24}, {0x35, 0x25}, {0x36, 0x5E},         // 3/#, 4/$, 5/%, 6/^
    {0x37, 0x26}, {0x38, 0x2A}, {0x39, 0x28}, {0x30, 0x29},         // 7/&, 8/*, 9/(, 0/) 
    {0x2D, 0x5F}, {0x3D, 0x2B}, {0x08, 0x08}, {0x00, 0x00},         // -/_, =/+, BS/BS, TAB, TAB,
    {0x71, 0x51}, {0x77, 0x57}, {0x65, 0x45}, {0x72, 0x52},         // q/Q, w/W, e/E, r/R
    {0x74, 0x54}, {0x79, 0x59}, {0x75, 0x55}, {0x69, 0x49},         // t/T, y/Y, u/U, i/I
    {0x6F, 0x4F}, {0x70, 0x50}, {0x5B, 0x7B}, {0x5D, 0x7D},         // o/O, p/P, [/{, ]/}
    {0x0A, 0x0A}, {0x00, 0x00}, {0x61, 0x41}, {0x73, 0x53},         //Enter, left Ctrl, a/A, s/S
    {0x64, 0x44}, {0x66, 0x46}, {0x67, 0x47}, {0x68, 0x48},         // d/D, f/F, g/G, h/H
    {0x6A, 0x4A}, {0x6B, 0x4B}, {0x6C, 0x4C}, {0x3A, 0x3B},         // j/J, k/K, l/L, ;/:
    {0x27, 0x22}, {0x60, 0x7E}, {0x00, 0x00}, {0x5C, 0x7c},         // '/", `/~, shift, \/|
    {0x7A, 0x5A}, {0x78, 0x58}, {0x63, 0x43}, {0x76, 0x56},         // z/Z, x/X, c/C, v/V
    {0x62, 0x42}, {0x6E, 0x4E}, {0x6D, 0x4D}, {0x2C, 0x3C},         // b/B, n/N, m/M, ,/<
    {0x2E, 0x3E}, {0x2F, 0x3F}, {0x00, 0x00}, {0x00, 0x00},         // ./>, //?, shift, keypad * (ignore)
    {0x00, 0x00}, {0x20, 0x20}, {0x00, 0x00}, {0x00, 0x00},         //left alt, space, caps, F1

};

//unsigned char keyboard_buf[KEYBOARD_BUF_SIZE] = {};
//int buf_index; // index used to keep track of where we add the char in buffer


/* void keyboard_init(void)
 * Description: Enables IRQ2 to accept interrupts from keyboard
 * Inputs: void
 * Return Value: None
 * Output: None
 * Sideeffects: Enables interrupts from keyboard
*/
void keyboard_init() {
    /* Enable interrupts sent from keyboard */
    terminal[cur_term].buf_index = 0;
    enable_irq(KEYBOARD_IRQ);
    backinit();
}

/*void keyboard_handler(void)
 * Inputs: void
 * Description: When keyboard interrupt is sent, we handle it 
 * and put the character to screen/in our terminal
 * Return Value: None
 * Output: None
 * Sideeffects: Enables interrupts from keyboard
 *              Puts character to screen
 *              Writes to terminal
*/

void keyboard_handler() {
    int count = 0;                      // Looping variable
    send_eoi(KEYBOARD_IRQ);
    uint8_t keyboard_input = inb(KEYBOARD_PORT);
    switch(keyboard_input) {
        case LEFT_SHIFT:
            left_shift = 1;
            return;
        case RIGHT_SHIFT:
            right_shift = 1;
            return;
        case CTRL:
            control = 1;
            return;
        case ALT:

            alt = 1;
            return;
        case CAPS:
            caps_lock = (caps_lock) ? 0 : 1;
            return;
        case TAB:
            /* 4 spaces!! */
            /* Do we have to worry about shift tab?*/
            do {
                // (KEYBOARD_BUF_SIZE - 2) - 2 so we exit one early leaving space for newline char
                //if (buf_index >= (KEYBOARD_BUF_SIZE - 2)) { break; }
                if (terminal[cur_term].buf_index >= (KEYBOARD_BUF_SIZE - 2)) { break; }

                count++;
                putc(ASCII_SPACE);
                //keyboard_buf[buf_index] = ASCII_SPACE;
                terminal[cur_term].keyboard_buffer[terminal[cur_term].buf_index] = ASCII_SPACE;
                terminal[cur_term].buf_index++;
                if (terminal[cur_term].buf_index >= (KEYBOARD_BUF_SIZE - 1)) { break; }
                //127 last, 127 size, > 0 
            } while (count != TAB_SPACE_MAX);
            return;
        case BACKSPACE:
            //break;
            if (terminal[cur_term].buf_index) {
                terminal[cur_term].buf_index--;
                popc();
            }
            return;
        case ENTER:
            // KEYBOARD_BUF_SIZE - 1 to check if we have filled array already
            // and need to make room
            if (terminal[cur_term].buf_index >= (KEYBOARD_BUF_SIZE - 1)) {
                terminal[cur_term].keyboard_buffer[KEYBOARD_BUF_SIZE - 1] = ASCII_ENTER;
            } else {
                terminal[cur_term].keyboard_buffer[terminal[cur_term].buf_index] = ASCII_ENTER;
            }
            enter = 1;
            terminal[cur_term].buf_index = 0;
            putc(ASCII_ENTER);
            //entering_shenanigans();
            return;
        case L_KEY:
            if (control) {
                putc(ASCII_ENTER);
                clear();
                //startatfront();
                return;
            }
            break;
        case LEFT_SHIFT_RELEASE:
            left_shift = 0;
            return;
        case RIGHT_SHIFT_RELEASE:
            right_shift = 0;
            return;
        case ALT_RELEASE:
            alt = 0;
            return;
        case CTRL_RELEASE:
            control = 0;
            return;
        case F1:
            if(alt == 1){
                // terminal[0].display = 1;
                // terminal[1].display = 0;
                // terminal[2].display = 0;
                terminal_switch(0);     // Must implement terminal_switch() function to switch between 3 terminals
            }
            return;
        case F2:

            if(alt == 1){
                // terminal[0].display = 0;
                // terminal[1].display = 1;
                // terminal[2].display = 0;

                terminal_switch(1);
            }
            return;
        case F3:

            if(alt == 1){
                // terminal[0].display = 0;
                // terminal[1].display = 0;
                // terminal[2].display = 1;

                terminal_switch(2);
            }
            return;
        default:
            break;
    }
    if (terminal[cur_term].buf_index >= KEYBOARD_BUF_SIZE - 1) { return; }
    /* Make sure it is a valid number*/
    if (keyboard_input >= KEYMAP_SIZE) { return; }

    /* Check if caps lock applies - letter or numeric*/
    uint8_t key_pressed = key_map[keyboard_input][0];
    if (!key_pressed) { return; }
    if ((key_pressed >= ascii_a && key_pressed <= ascii_z) || (key_pressed >= ascii_A && key_pressed <= ascii_Z)) {
        key_pressed = key_map[keyboard_input][(caps_lock) ^ (left_shift || right_shift)];
    } else {
        key_pressed = key_map[keyboard_input][(left_shift || right_shift)];
    }
    terminal[cur_term].keyboard_buffer[terminal[cur_term].buf_index] = key_pressed;

    /*if (terminal[cur_term].buf_index == KEY_WRAP) {
        putc_incr();
    }*/
    terminal[cur_term].buf_index++;
    putc(key_pressed);

}

