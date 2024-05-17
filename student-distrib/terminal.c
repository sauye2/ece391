#include "terminal.h"
int enter_val = 0;

void terminal_init() {
    int i;


    for(i = 0; i < MAX_TERMINALS; i++) {
        memset(terminal[i].keyboard_buffer,0,KEYBOARD_BUF_SIZE);
        terminal[i].active = 0;
        terminal[i].screen_X = 0;
        terminal[i].screen_Y = 0;
        terminal[i].pid = -1;
    }
    /*for (i = 0; i < 3; i++) {
        terminal[i] =(uint8_t*)(VIDEO_ADDR+(i+1)*SIZE_4KB)
    }*/
    terminal[0].video_mem_addr = TERMINAL1;
    terminal[1].video_mem_addr = TERMINAL2;
    terminal[2].video_mem_addr = TERMINAL3;
    terminal[0].active = 1;
    cur_term = 0;

}
/* int32_t terminal_read()
 * Inputs:  fd - file descriptor
 *          buf - buffer we read the keyboard buffer into
 *          nbytes - num of bytes from buf
 * Return: number of bytes read into buf
 * Output: NONE
 * Side Effects: NONE
 *
 * Description: reads from Keyboard buffer, into buf, returns number of bytes read
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    int i;                      /* Loop Variable */
    int toRet = 0;              /* Number of bytes read */
    uint8_t ascii_hex;           /* Holds ascii val for key to be put on screen */
    // In case nbytes is incorrect, we will use KEYBOARD_BUF_SIZE
    uint32_t loop_size = (nbytes <= KEYBOARD_BUF_SIZE) ? nbytes : KEYBOARD_BUF_SIZE;
    sti();
    //enter_val = 0;
    while (!enter);
    cli();
    enter = 0;
    for (i = 0; i < loop_size; i++) {
        


        /* Save key and erase from keyboard buffer */
        ascii_hex = terminal[cur_term].keyboard_buffer[i];
        ((char*)buf)[i] = ascii_hex;

                // Sanity check
        if (ascii_hex == '\0' || (i == (loop_size - 1) && ascii_hex != ASCII_ENTER)) {
            //putc(ASCII_ENTER);
            ((char*)buf)[i] = ASCII_ENTER;
            toRet++;
            break;
        }
        //keyboard_buf[i] = 0x00;
        toRet++;
        if (ascii_hex == ASCII_ENTER) { break; }
    }
    /*for (i = 0; i < loop_size; i++) {

    }*/
    sti();
    return toRet;
}

/* int32_t terminal_write()
 * Inputs:  fd - 
 *          buf -
 *          nbytes - 
 * Return: Number of bytes read into buf 
 *          -1 if failed
 * Output: NONE
 * Side Effects: Writes to screen from buf
 *
 * Description: Writes to screen from buf, return number of 
 * bytes written, or -1 for failure
 *
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    // printf("In terminal_write open\n");
    int i;                      /* Loop Variable */
    uint8_t ascii_hex;           /* Holds ascii val for key to be put on screen */
    // if (buf == NULL || nbytes < 0) return -1;
    if (buf == NULL || nbytes < 0) {
        return -1;
    }
    cli();

    for (i = 0; i < nbytes; i++) {
        ascii_hex = ((char*)buf)[i];

        if (ascii_hex != 0x00) {
            putc(ascii_hex);
        }
    }
    sti();
    // printf("Leaving terminal_write\n");
    return nbytes;
}

/* int32_t terminal_open()
 * Inputs: filename - 
 * Return: 0 on success,
 *         -1 on failure
 * Output:
 * Side Effects:
 *
 * Description: initializes terminal, return 0 or -1
 *
 *
*/
int32_t terminal_open(const uint8_t* filename) {
    return 0;
}

/* int32_t terminal_close()
 * Inputs: fd - 
 * Return: 0 if success
 *         1 if failure
 * Output:
 * Side Effects:
 *
 * Description: clears terminal speicifc variables
 *
 *
*/
int32_t terminal_close(int32_t fd) {
    return 0;
}

/*void keyboard_to_terminal(int8_t ascii_char) {

}*/

/* int32_t terminal_switch()
 * Inputs: terminal_index
 * Return: 0 if success
 *         1 if failure
 * Output:
 * Side Effects:
 * Description: Function to switch between 3 maximum terminals
*/
int32_t terminal_switch(uint32_t terminal_index){

    if(terminal_index < 0 || terminal_index > 2){            // Should only support 3 terminals, so index 0 to 2
        return -1;
    }
    if(terminal_index == cur_term) {
        return -1;
    }
    int i = 0;
    int sum = 0;
    for(i = 0; i < 3; i++) {
        sum+=terminal[i].num_processes;
    }
    if(terminal[terminal_index].num_processes == 0 && sum >= 6) {
        printf("6 processes running, close one to open a new shell\n");
        return -1;
    }

    cli();
    memcpy((char*)terminal[cur_term].video_mem_addr, (char*)VIDEO, FOURKB);
    memcpy((char*)VIDEO, (uint32_t*)terminal[terminal_index].video_mem_addr, FOURKB);

    terminal[cur_term].active = 0;
    terminal[terminal_index].active = 1;
    terminal[cur_term].pid = current_pid;
    cur_term = terminal_index;
    current_pid = terminal[cur_term].pid;
    if(terminal[cur_term].num_processes == 0) {
        sys_execute((uint8_t*)"shell");
    }
    update_cursor(terminal[cur_term].screen_X,terminal[cur_term].screen_Y);

    sti();

    /*
    - move old terminal to memory
    - move new terminal to physical screen
    - display new terminal
    
    */

    return 0;
}





