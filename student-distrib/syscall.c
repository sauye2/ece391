#include "syscall.h"

int32_t current_pid = -1;
int idt_ex = 0;



/* This will hold the individual processes */
uint32_t pid_arr[MAX_PCB] = {0};
// Note: course staff on discord said test// printf only uses execute, write, and halt.

void init_fot_ptrs(){                            // call this function in kernel.c
    rtc_table.read = rtc_read;
    rtc_table.write = rtc_write;
    rtc_table.open = rtc_open;
    rtc_table.close = rtc_close;

    dir_table.read = directory_read;
    dir_table.write = directory_write;
    dir_table.open = directory_open;
    dir_table.close = directory_close;

    filesys_table.read = filesys_read;
    filesys_table.write = filesys_write;
    filesys_table.open = filesys_open;
    filesys_table.close = filesys_close;

    stdin_table.read = terminal_read;
    stdin_table.write = bad_call;            // input only reads not writes
    stdin_table.open = terminal_open;
    stdin_table.close = terminal_close;

    stdout_table.read = bad_call;            // output only writes not reads
    stdout_table.write = terminal_write;
    stdout_table.open = terminal_open;
    stdout_table.close = terminal_close;

    initialize_table.read = bad_call;            // output only writes not reads
    initialize_table.write = bad_call;
    initialize_table.open = bad_call;
    initialize_table.close = bad_call;
}


/*
 * sys_halt()
 *   DESCRIPTION: Terminates a process
 *   INPUTS: status
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: Terminates a process and returns the specified value to its parent process
 */
int32_t sys_halt (uint8_t status) {
    terminal[cur_term].num_processes--;

    /*
    1. Restore the ebp by setting the ebp register to the value we saved in execute
    2. Restore the esp register by setting the esp register to the ebp register
    3. pop ebp from the stack into the ebp register - what frame did we just pop?
    4. Call ret, where do we actually return to now?*/
    cli();
    int32_t toRet = (int32_t)status;
    if (idt_ex == 1) {
        toRet = EXCEPTRET;
    }
    idt_ex = 0;

    pcb_t *cur_pcb = get_pcb(current_pid);
    pid_arr[current_pid] = 0;

    if(cur_pcb->parent_pid == -1 ) {
        current_pid = 0;
        terminal[cur_term].pid = -1;
        sys_execute((uint8_t*)"shell");        
        //return 0;
    }
    

    uint32_t i = 0;
    
    for(i=0; i < MAX_FILES; i++) {                                      // we start from 2 because 1 and 0 are stdin and stdout
        if(cur_pcb->fda[i].flags != 0) {                  // gonna change to i=0 and call close()
            cur_pcb->fda[i].flags = 0;
            sys_close(i);                                 // can remove
        }
    
    }
    terminal[cur_term].pid = cur_pcb->parent_pid;
    int32_t curpid = cur_pcb->parent_pid;
 //   cur_pcb = get_pcb(curpid);
    current_pid = curpid;

    uint32_t* paging_addr = get_physmem(current_pid); // should we use current_pid instead?? 
    pagedir[32] =  (((unsigned int) paging_addr) | 391);
    //Present, R/w, U/s, Size, Global = 2**0+2**1+2**2+2**7+2**8
    // 32 stands for the 32nd page b/c 32 pages * 4 MB each page = 128MB (Appendix C) 
    flush_tlb();

   // asm volatile ("movl %0, %%ebp, %0" : "=r" (cur_pcb->ebp) );
    //asm volatile ("movl %0, %%esp" : "=r" (cur_pcb->esp) );
    //if 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHTMB - (EIGHTKB * (curpid)) - FOURB;
    // Reset paging for parent process



    // might be par_pcb->esp/ebp instead
    // status -> %%eax
    // cur_pcb->esp -> %%esp
    // cur_pcb->ebp -> %%ebp
    //pcb_t* parent_pcb = get_pcb(current_pid);
    asm volatile(
     
        "movl  %0, %%eax \n"      
        "movl  %1, %%ebp \n"   
        "movl  %2, %%esp \n" 
    
        "leave           \n"
        "ret             \n"             
        :                                                                                        // Output Operands
        : "r"((uint32_t)toRet), "r"(cur_pcb->ebp), "r"(cur_pcb->esp)                             // eip -> is the mem address that tells us entry point from user-level virt mem.
        : "%eax", "%ebp", "%esp"                                                                 // Clobbers
    );

    return 0;

}

/*
 * sys_execute()
 *   DESCRIPTION: system execute program
 *   INPUTS: command - space seperated input containing filename and arguments
 *   OUTPUTS: int32_t containing -1 for failure, 0-255 for any interrupt
 *   RETURN VALUE: int32_t
 *   SIDE EFFECTS: 
 */
int32_t sys_execute (const uint8_t* command) {
    /* command  -> "frame0.txt command...."
        1. Check if filename is valid and also an executable
        2. Check to see if we reached max PCBs *** DON'T KNOW****
        3. Map virtual memory to a physical page to the common virtual page
            a. New page ( P=1, PS=0, U/S = 1)
        4. Load program from filesys into memory directly
        5. Save curr EBP
        6. Context Switch
        7. IRET
    */

    // Basic NULL check (empty command)
    /*if(terminal[cur_term].num_processes >= 4) {
        printf("Too many processes for one terminal\n");
        return -1;
    }*/
    /*if(terminal[0].num_processes + terminal[1].num_processes + terminal[2].num_processes >= 6 ||
    (terminal[0].num_processes + terminal[1].num_processes >= 5 && cur_term != 2) || (terminal[1].num_processes + terminal[2].num_processes >= 5 && cur_term != 0)||
    (terminal[0].num_processes + terminal[2].num_processes >= 5 && cur_term != 1)) {
        printf("Overall too many processes\n");
        return -1;
    }*/

    
    
    if(command == NULL){
        return -1;
    }
    //int ret;
    int i;                                              // Loop variable
    int index;                                          // variable used to loop through command
    int cmd_len = strlen((int8_t*)command);         // length of the argument
    uint8_t filename[MAXBUFSIZE] = {};                      // 32 for max number of characters in filename. buffer to store filename (extracted from command)
    uint8_t args[MAXBUFSIZE] = {};                          // 32 for max characters for arguments. buffer to store args
    int arg_index = 0;                                  // used to loop through array holding args
    dentry_t dentry;
    uint8_t elf_buf[4];                                // last 4 bytes to check for magic elf executable check. ELF = executable and linkable format
    uint32_t entry_point_buf;                        // used to store entry address for program


    /*                          1. PARSE ARGS                               */
    if (cmd_len == 0) return -1;
    for (index = 0; index < cmd_len; index++){
        if(command[index] == ' '){                      // encountered first space
            break;                      
        }else{
            filename[index] = command[index];           // add to filename
        }
    }

    if (index < cmd_len){                               // Grab the rest (arguments)
        index++;                                        // Skip the first space
        while(index < cmd_len){
            args[arg_index++] = command[index++];
        }
    }




    /*                      2. CHECK FILENAME FROM ARG IS VALID AND FILE IS EXECUTABLE                            */
    if(read_dentry_by_name(( uint8_t*) filename,&dentry) == -1){
        return -1;                                      // Couldn't find file, return -1
    }
    
    if(read_data(dentry.file_inode_num,0,elf_buf,4) == -1){
        return -1; // read_data failed
    }
    
    if((elf_buf[0] != 0x7F) || (elf_buf[1] != 0x45) || (elf_buf[2] != 0x4C) || (elf_buf[3] != 0x46) ){
        return -1;                                      // 0x7F, 0x45, 0x4C, 0x46 represent a magic number that identifies a file as an executable
    }

    /*                      3. CHECK IF WE HAVE REACHED MAX PCBS                           */
    
//mp_term.num_processes++;mp_term.num_processes++;   return -1;
    



    pcb_t* cur_pcb;

    // for loop may need some help
    
    //so just start from 3 so we can't put things in stdout/in/exc frm start?
    for (i = 0; i < MAX_PCB; i++) {
        if (pid_arr[i] == 0) {
            // we found an open spot
            pid_arr[i] = 1;
            current_pid = i;               
            cur_pcb = get_pcb(current_pid);
            // pagefault here from double shell
            cur_pcb -> pid = i; 
            cur_pcb->parent_pid = terminal[cur_term].pid;
            terminal[cur_term].pid = current_pid;
            // Set PCB's pid to current pid number (i)
            break;
        } else if (i == 5 && pid_arr[i] != 0) {                   // 5 is index for 6 which max processes allowed
            // No available pids currently, return -1
            printf("Too many processes \n");
            return -1;
        }
    }
   
    // Example: initial current_pid = 0; parent = 0; current_pid = 1;
    /*if(current_pid != 0){
        cur_pcb->parent_pid = parent;
        parent = current_pid;
    }
    else{                                      // if(current_pid == 0) so first process
        cur_pcb->parent_pid = -1;
        parent = -1;
    }*/
    strncpy((int8_t*)cur_pcb->arguments, (int8_t*)(args), THIRTY_TWOB);
    // At this point, pid and parent_pid are initialized for the pcb. fda[], ebp, eip, and esp left to initialize
    // 0, 1, are stdout, in , and 2nd is the executable file from start
    cur_pcb->fda[0].fot_ptr = &stdin_table;
    cur_pcb->fda[0].inode = 0;
    cur_pcb->fda[0].file_position = 0;
    cur_pcb->fda[0].flags = 1;
    cur_pcb->fda[1].fot_ptr = &stdout_table;
    cur_pcb->fda[1].inode = 0;
    cur_pcb->fda[1].file_position = 0;
    cur_pcb->fda[1].flags = 1;

    for(i = FD_START; i < MAX_FILES; i++){          // Initialize fda[2-7] because 0-1 are stdin/stdout
        cur_pcb->fda[i].fot_ptr = &initialize_table;
        cur_pcb->fda[i].inode = 0;
        cur_pcb->fda[i].file_position = 0;
        cur_pcb->fda[i].flags = 0;
    }
    
    // At this point, fda[] is initialized. Eventually, when open is called, fda will be loaded with correct driver




    /*                      4. MAP VIRTUAL MEMORY TO A PHYSICAL PAGE                           */

    uint32_t* paging_addr = get_physmem(current_pid); // should we use current_pid instead?? 
    pagedir[32] =  (((unsigned int) paging_addr) | 391);
    //Present, R/w, U/s, Size, Global = 2**0+2**1+2**2+2**7+2**8
    // 32 stands for the 32nd page b/c 32 pages * 4 MB each page = 128MB (Appendix C) 
    flush_tlb();
    
    



    /*                      5. LOAD PROGRAM FROM FILESYS INTO MEM                           */
    // Load file into memory
    
   
    // entry_point_buf is our user start  
    if(read_data(dentry.file_inode_num, 24, (uint8_t*)(&entry_point_buf),4) == -1){ // 24 is start of entry point in exe file and we need to read 4 bytes from that as stated in Appendix C
        return -1; // read_data failed
    }
    terminal[cur_term].num_processes+=1;

    uint32_t offset = 0;
    uint8_t* buf_mem = (uint8_t*)START_ADDR;

    // Read into our memory
    read_data(dentry.file_inode_num, offset, buf_mem, FOURMB);
    // get from user

    tss.ss0 = KERNEL_DS; /* Set privilege level to 0*/
    /* 8MB - 4B*8kB*current_pid since userspace starts at 0x8400000*/
    // Either current_pid or (current_pid - 1)
    tss.esp0 = EIGHTMB - (EIGHTKB * (current_pid)) - FOURB;


    // 6. Save current EBP / ESP
    // Found syntax in https://stackoverflow.com/questions/2114163/reading-a-register-value-into-a-c-variable
    uint32_t save_ebp, save_esp;
    asm volatile ("movl %%ebp, %0" : "=r" (save_ebp) );
    asm volatile ("movl %%esp, %0" : "=r" (save_esp) );
    cur_pcb->ebp = save_ebp;
    cur_pcb->esp = save_esp;
    
    uint32_t context_esp = USER_PROG - FOURB;     // base of user stack = 132MB - 4B 

    
   

    /*
        - Still need to actually copy file contents into memory
        - Create ASM function for context switching
        - init fd descriptors (should be done)
        - save cur ebp? From discussion
        - iret
        - check snap
    */

    /* Context Switch
        - Push order is SS, ESP, EFLAGS, CS, EIP
        - USER_DS for SS 
        - USER_CS for CS
        - ESP should be ??
            -> Userspace stack pointer at start is always 0x8400000
            -> I think its supposed to be base of user stack: 132MB - 4B (0x83FFFFC)
        - EFLAGS
            -> Think about your implementation and if interrupts are disabled currently, if so what do you need to do?
        - EIP
            -> Think back to what we know about what the executable expects.
        - Info on syntax for pushing input operands found here: 
        https://dl.acm.org/doi/fullHtml/10.5555/3024956.3024958#:~:text=The%20%E2%80%9Cmemory%E2%80%9D%20clobber%20tells%20the,before%20executing%20the%20inline%20asm.
    */
    
    
    
    sti();
    asm volatile(
        "pushl  %0 \n"             // Push USER_DS
        "pushl  %1 \n"             // Push ESP: (132MB - 4B = 0x83FFFFC)
        "pushfl    \n"             // Push EFLAGS
        "pushl  %2 \n"             // Push USER_CS
        "pushl  %3 \n"             // Push EIP
        "iret      \n"             // Call iret
        :                                                                                        // Output Operands
        : "r"(USER_DS), "r"(context_esp), "r"(USER_CS), "r"(entry_point_buf)                     // eip -> is the mem address that tells us entry point from user-level virt mem.
        : "memory"                                                                               // Clobbers
    );
    
    return 0;

}

/*
 * sys_read()
 *   DESCRIPTION: system read function. Reads data from keyboard, a file, RTC, or directory and returns the number of bytes read.
 *   INPUTS: fd, buffer, nbytes
 *   OUTPUTS: 
 *   RETURN VALUE: number of bytes read
 *   SIDE EFFECTS: 
 */
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes){       //should update file position in fd_t -> slide 18
    if((fd < FD_START_IDX) || (fd > MAX_FILES - 1) || (buf == NULL)){       // check if fd is within the boundaries and buf is not NULL, if not return -1
        return -1;
    }
    pcb_t* pcb = get_pcb(current_pid);
    if(pcb->fda[fd].flags == 0){                                // flags == 0 means that the file is currently not in use
        return -1;
    }
    // printf("After second if in sys_read \n");
    return pcb->fda[fd].fot_ptr->read(fd, buf, nbytes);       // will return 0 if close was successful, -1 if failed
}

/*
 * sys_write()
 *   DESCRIPTION: Writes data to the terminal or to a device.
 *   INPUTS: fd, buffer, nbytes
 *   OUTPUTS: 
 *   RETURN VALUE: -1 for failure, or number of bytes written on success
 *   SIDE EFFECTS: 
 */
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    // printf("Make it to sys_write\n");
    if((fd < FD_START_IDX) || (fd > MAX_FILES - 1) || (buf == NULL)){       // check if fd is within the boundaries and buf is not NULL, if not return -1
        return -1;
    }
    pcb_t* pcb = get_pcb(current_pid);
    if(pcb->fda[fd].flags == 0){                                // flags == 0 means that the file is currently not in use
        return -1;
    }

    return pcb->fda[fd].fot_ptr->write(fd, buf, nbytes);        // will return 0 if close was successful, -1 if failed
}

/*
 * sys_open()
 *   DESCRIPTION: Provides access to file system. Finds directory corresponding to given file and sets up any data 
 *                  neccessary to handle diff type of file
 *   INPUTS: filename- pointer to a filename
 *   OUTPUTS: 
 *   RETURN VALUE: -1 for fail, else i
 *   SIDE EFFECTS: 
 */
// The open system call provides access to the file system. The call should find the directory entry corresponding to the
// named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
// RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1.
// 0 rtc, 1 directory, 2 file

int32_t sys_open (const uint8_t* filename){
    dentry_t dentry;
    int i;
    if(read_dentry_by_name(filename, &dentry) == -1){          // Fill dentry with file name, file type, and inode number if file name exists
        return -1;                                             // If read_dentry returns -1, then failed
    }
    pcb_t* pcb = get_pcb(current_pid);
    for(i = FD_START; i < MAX_FILES; i++){                      // Look for unused file descriptor -> fd can be 2-7 (0 and 1 are stdin/stdout)
        if(pcb->fda[i].flags == 0){                            // If flags == 0, then we have found an unused file descriptor
            pcb->fda[i].flags = 1;                             // Mark file descriptor as being used
            pcb->fda[i].inode = dentry.file_inode_num;
            pcb->fda[i].file_position = 0;                     // Initial file position is 0

            // After filling in information for file descriptor, check the dentry file type to determine which drivers to use
            if(dentry.file_type == 0){                         // Load RTC operations 
                pcb->fda[i].fot_ptr = &rtc_table;
            }
            if(dentry.file_type == 1){                         // Load directory operations
                pcb->fda[i].fot_ptr = &dir_table;
            }
            if(dentry.file_type == 2){                         // Load filesys operations
                pcb->fda[i].fot_ptr = &filesys_table;
            }
            pcb->fda[i].fot_ptr->open(filename);               // Open file using the correct file driver
            return i;
        }
        //return 0;
        // or return i (fd)?
    }
   // else {
        return -1;                                         // No free file descriptors
    //}

}

/* 
 * sys_close()
 *   DESCRIPTION: Takes in fd and sets file to not in use and closes file 
 *   INPUTS: fd
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure, else 0 for sucess
 *   SIDE EFFECTS: none
 */
int32_t sys_close (int32_t fd){              
    if(fd < FD_START || fd > MAX_FILES - 1){       // check if fd is within the boundaries, if not return -1
        return -1;
    }
    pcb_t* pcb = get_pcb(current_pid);
    if(pcb->fda[fd].flags == 0){                   // flags == 0 means that the file is currently not in use
        return -1;
    }
    pcb->fda[fd].flags = 0;                        // set file to be not in use so we can close it
    return pcb->fda[fd].fot_ptr->close(fd);        // will return 0 if close was successful, -1 if failed
}

/*
 * sys_getargs()
 *   DESCRIPTION: Copies the program's command line arguments into a user-level buffer
 *   INPUTS: buf, nbytes
 *   OUTPUTS: none                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
 *   SIDE EFFECTS: Initialize the shell task's argument data to empty string
 */
int32_t sys_getargs (uint8_t* buf, int32_t nbytes){                         //checkpoint 4 (not needed for this cp)
    if(buf == NULL){
        return -1;
    }
    
    pcb_t* pcb = get_pcb(current_pid);
    if(pcb->arguments[0] == 0){
        
        return -1;
    }

    strncpy((int8_t*)buf, (int8_t*)pcb->arguments, nbytes);
    return 0;
}

/*
 * sys_vidmap()
 *   DESCRIPTION: Maps the text-mode video memory into user space at a pre-set virtual address
 *   INPUTS: screen_start
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: Adds another page mapping for the program -> a 4kB page
 */
int32_t sys_vidmap (uint8_t** screen_start){                                 //checkpoint 4 (not needed for this cp)  
    printf("Sys_vidmap");
    if((screen_start == NULL) || ((uint32_t)screen_start < USER_START) || ((uint32_t)screen_start > USER_START + FOURMB)){     // Check if screen start is NULL or within the bounds
     
    }
    
    pagedir[VID_INDEX] = (((unsigned int) vidmaptab)) | 7; //set present, user/supervisor to on, address points to pagetab

    vidmaptab[0] =  (((unsigned int) VIDTOPHYS)) | 7; //present, r/w, user/supervisor bits
    //pagetab[0] = pagetab[0] | 7;
    //pagetab[0] =  (((unsigned int) VIDTOPHYS) / PAGE_ALIGN) | 7; //present, r/w, user/supervisor bits

    flush_tlb();
    *screen_start = (uint8_t*)(USER_START + EIGHTMB);     // ** Need a value here for screen_start I dont know if this is correct **
    return 0;
}

/*
 * sys_set_handler()
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   SIDE EFFECTS: 
 */
int32_t sys_set_handler(int32_t signum, void* handler_address) {             //extra credit
    return 0;
}

/*
 * sys_sigreturn()
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   SIDE EFFECTS: 
 */
int32_t sys_sigreturn(void) {                                                    //extra credit
    return 0;
}
/*
 * get_physmem()
 *   DESCRIPTION: helper function for execute
 *   INPUTS: pid: process id
 *   OUTPUTS: none
 *   RETURN VALUE: s- address for physical memory
 *   SIDE EFFECTS: 
 */
uint32_t* get_physmem(int32_t pid){
    //uint32_t* s = (uint32_t*) (FOURMB * (pid + 2));             // PID0 = 8MB-8KB ; PID1 = 8MB-16KB = 8MB-8KB*(1+1)
    uint32_t* s = (uint32_t*) (EIGHTMB + (pid * FOURMB));
    return s;
}

/*
 * get_pcb()
 *   DESCRIPTION: helper function for execute
 *   INPUTS: pid - process id
 *   OUTPUTS: None
 *   RETURN VALUE: returns a pointer to PCB given a current process id
 *   SIDE EFFECTS: NA
 */
pcb_t* get_pcb(int32_t pid){
    return (pcb_t*)(EIGHTMB - EIGHTKB*(pid + 1));             // PID0 = 8MB-8KB ; PID1 = 8MB-16KB = 8MB-8KB*(1+1)
}

/*
 * bad_call()
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   SIDE EFFECTS: 
 */
int32_t bad_call(){            // placeholder for driver functions that don't do anything; should just return -1
    return -1;
}
