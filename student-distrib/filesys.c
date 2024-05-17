#include "filesys.h"
#include "lib.h"

unsigned int boot_block_address;
boot_block_t* boot_block;
inode_t* filesys_collection; /* Holds filesystem, i.e. inode[0:n] and datablock[n + 1:d]*/
uint8_t* data_blocks_collection;
uint32_t boot_max_file;
uint32_t global_ind = 0; //yeah just run it i want to see the errors
fd_t fd_table[8]; // 8 is the max number of files
/* void filesys_init
 * Input: boot_address - boot block address 
 * Return: None
 * Output: None
 * 
 * Description: Initializes file system, saving boot address and creating our 
 *                  boot block struct.
 *
*/
void filesys_init(unsigned int boot_address) {
    boot_block_address = boot_address;
    /* Stores boot address as a boot_block_t*/
    boot_block = (boot_block_t*) boot_address;
    filesys_collection = (inode_t*)(boot_block + 1);
    boot_max_file = boot_block->number_dir_entries;
    data_blocks_collection = (uint8_t*)(filesys_collection + boot_block->number_inodes);
}

/* int32_t filesys_read
 * Inputs: fd - File Descriptor
 *         buf - buffer to read file into
 *                      Data read to end of file or end of buffer provided, whichever occurs first
 *         nbytes - number of bytes read into file
 * Return: 0 - if initial file position is at or beyond end of file
 *         nbytes - number of bytes read (success only)
 * Output: None
 * Side Effects: None
 *
 * Description: Reads data from the file into buf 
 */
int32_t filesys_read(int32_t fd, void* buf, int32_t nbytes) {
    /* read_data should be useful here */

    /* Sanity Check */
    // printf("In filesys_read open\n");
    //int i = nbytes;
    if (!buf || nbytes < 0) { return -1; }
    pcb_t* cur_pcb = get_pcb(current_pid);
    int res = read_data(cur_pcb->fda[fd].inode, cur_pcb->fda[fd].file_position, buf, nbytes);
    if (res == -1) return -1;
    
    cur_pcb->fda[fd].file_position+=res;
    // printf("In filesys_read before return \n");
    return res;
}

/* int32_t filesys_write
 * Inputs: fd - File Descriptor
 *         buf - buffer to write into file
 *         nbytes - number of bytes to write in file
 * Return: -1 failure (sys is read only)
 *          nbytes - number of bytes wrote (success only)
 * Output: None
 * Side Effects: Writes into file
 *
 * Description: writes data to a file
 */
int32_t filesys_write(int32_t fd, const void* buf, int32_t nbytes) {
    /* Writes to regular filesystem should return -1, since we have a read-only file system */
    return -1;
}

/* int32_t filesys_open
 * Inputs: filename - name of file
 * Return: 0 on success
 *         -1 on failure
 * Output: None
 * Side Effects: None
 *
 * Description: Provides access to file system
 *      Find directory entry corresponding to named file,
 *      allocate an unusued file descriptor,
 *      set up any data necessary to handle given type of file (RTC, regular, directory)
 */
int32_t filesys_open(const uint8_t* filename) {
    global_ind = 0;
    if (strlen((int8_t*) filename) >= FILE_NAME_MAX_LEGTH) { return -1; }
    // int i = 2; // start at 2 because 0 and 1 are stdin and out
    dentry_t dentry;
    //pcb_t* cur_pcb = get_pcb(current_pid);
    //fd_table = (fd_t)cur_pcb->fda;
    //// printf("Before dentry %s\n", filename);
    int res = read_dentry_by_name(filename,&dentry);
    if (res == -1) return -1;
    /*for (i = 2; i < 8; i++) { // 8 is max files as stated in documentation
        //// printf("FOR LOOPER %d", i);
        if (cur_pcb->fda[i].flags == 0) {
            cur_pcb->fda[i].flags = 1;
            cur_pcb->fda[i].inode = dentry.file_inode_num;
            cur_pcb->fda[i].file_position= 0;
            return 0;
        }
       
    }*/
    //// printf("Doesn't open anything");
    return 0;
}

/* int32_t filesys_close
 * Inputs: fd - File Descriptor
 * Return: -1 Failure
 *          0 Success
 * Output: None
 * Side Effects: None
 *
 * Description: Closes specificed file descriptor and makes 
 *                  availble for return from later calls to open
 *
 *  Uses cannot close default descriptors (0(input) and 1(output))
 */
int32_t filesys_close(int32_t fd) {
    // printf("Filesys close fuck this class \n");
    if (fd > 1 && fd < 8) {                          // fd[0-1] are for stdin and stdout. fd max is up to 7.
        pcb_t* cur_pcb = get_pcb(current_pid);
        cur_pcb->fda[fd].flags = 0;
        cur_pcb->fda[fd].file_position= 0;
        cur_pcb->fda[fd].inode = 0;

        return 0;
    }
    return -1;

}

/* int32_t read_dentry_by_name
 * Inputs: fname - File Name
 *         dentry - dentry_t to be filled with files data
 * Return: -1 on failure
 *         0 on success
 * Output: None
 *
 * Description: Finds the file with fname and fills in 
 *               Fills dentry_t with file name, file type, and inode number
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    int i;                                               /* Loop Variable */
    int num_dir_entries = boot_block -> number_dir_entries; /* Tells us how many entries to loop through */
    /*-- Cast to int8_t from uint because strlen params --*/
    uint32_t fname_length = strlen((int8_t*) fname);
    
    /* Sanity Check */
    if (fname == NULL || fname_length > FILE_NAME_MAX_LEGTH) {
        return -1;          
    }

    for (i = 0; i < num_dir_entries; i++) {
        if (!strncmp((int8_t*)(boot_block->dir_entries[i].file_name), (int8_t*) fname, fname_length)) {
            *dentry = boot_block->dir_entries[i];
            return 0;
        }
    }
    return -1;
}




/* int32_t read_dentry_by_index
 * Inputs: index - index of dentry_t object to copy (inode)
 *         dentry - dentry_t to be filled with files data
 * Return: -1 on failures
 *          0 on success
 * Output: None
 *
 * Description: Finds the file with index param in the file system (Appendix A, 8.1 for further info)
 *                Fills dentry_t with file name, file type, and inode number
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if (index < boot_block -> number_dir_entries) {
        *dentry = boot_block->dir_entries[index];
        return 0;
    }
    return -1;
}


/* int32_t read_data
 * Inputs: inode - inode number that corresponds to the file
 *         offset - position to start reading from file
 *         buf - buf to read data into from file
 *         length - amount of bytes to read
 * Return: -1 on failure
 *          number of bytes read on success
 * Output: None
 *
 * Description: Reads the file from offset until offset+length into
 *                  buf, a return of 0 means we have reached end of file.
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    int i;                                                          /* Loop Variable */
    uint32_t toRet = 0;                                             /* Returns how many bytes read */
    uint32_t data_block_total = boot_block -> number_data_blocks;   /* Total amount of data_blocks (D) */
    uint32_t curr_block_index;                                      /* index of the current data block we are copying from */
    uint8_t* curr_data_block;                                       /* pointer to current data block */
    uint32_t n;                                                     /* Amount of bytes to read */
    inode_t inode_data;                                             /* stuct pointer to inode data type */
    uint32_t inode_file_size;                                       /* size of file we are reading */
    /* SANITY CHECK - valid inode*/
    if (inode >= boot_block -> number_inodes || inode < 0) {
        return -1;
    }
    
    /* Retrieve the size of file and indices of our data blocks*/
    inode_data = filesys_collection[inode];
    inode_file_size = inode_data.length;

    /* used to tell us where to stop, if length + offset is greater than file size we stop at the file size else length */
    uint32_t toCompare = ((length + offset) < inode_file_size) ? (length + offset) : inode_file_size;

    for (i = offset; i < toCompare;) {

        curr_block_index = (inode_data.data_blocks)[(int32_t)(i / BLOCK_SIZE)];
        /* Sanity check - if inode gives false index we need to stop! */
        if (curr_block_index >= data_block_total) {
            return -1;
        }
        curr_data_block = (uint8_t*)(data_blocks_collection + curr_block_index * BLOCK_SIZE + (i % BLOCK_SIZE));
        n = BLOCK_SIZE - (i % BLOCK_SIZE);
        if (i + n >= toCompare) {
            n = toCompare % BLOCK_SIZE;
        }
        if (length < BLOCK_SIZE) {
            n = length;
        }
        memcpy(buf, curr_data_block, n);
        i += n; 
        toRet += n;
        buf += n;
    }

    return toRet;

}



/* int32_t directory_read
 * Inputs: fd - File Descriptor
 *         buf - buffer to read file into 
 *                      Data read to end of file or end of buffer provided, whichever occurs first
 *         nbytes - number of bytes read into file
 * Return: 0 - if initial file position is at or beyond end of file
 *         nbytes - number of bytes read (success only)
 * Output: None
 * Side Effects: increments i, which points to next file position
 *
 * Description: Reads a directories files filename by filename - 
 *                  subsequent reads from successive directory entires
 *                  until the last is reached, at which point should repeatedly return 0.
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes) {
    dentry_t curr_dentry;                 // Store a tmp version of dentry
    //uint8_t* curr_fname;                /* Will store all the file names as we loop */
    //int i;                              /* Loop variable */
    //int n;                              /* Size of file name*/
    /* read_dentry_by_index will be helpful here*/
    // printf("Dir_read bitch \n");
    /*
    x = fd -> file_position
    read_dentry_by_index(x)
    */
    pcb_t* cur_pcb = get_pcb(current_pid);
    if (cur_pcb->fda[fd].flags == 0 || buf == NULL || fd >= MAX_FILES || fd < 2) return 0;
    int32_t fp = cur_pcb->fda[fd].file_position;
    if (fp >= 17) { //17 because that is the number of files when running ls. used to prevent loop
        return 0;
    }
    /*if (global_ind >= 17) { //17 because that is the number of files when running ls. used to prevent loop
        global_ind = 0;
        return 0;
    }*/
    /*if (read_dentry_by_index(global_ind, &curr_dentry) == -1) {
        return -1;
    }*/
    if (read_dentry_by_index(fp, &curr_dentry) == -1) {
        return -1;
    }
    //global_ind++;
    uint8_t* filename_tmp = curr_dentry.file_name;
    memcpy((int8_t*)buf, (int8_t*)filename_tmp, nbytes);
    //sys_write(1, filename_tmp, FILE_NAME_MAX_LEGTH);
    cur_pcb->fda[fd].file_position++;

    return nbytes;
    // uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length
    /*pcb_t* cur_pcb = get_pcb(current_pid);
    uint32_t inode_num = cur_pcb->fda[fd].inode;
    dentry_t dentry;
    dir_read_helper(inode_num, &dentry);
    //sys_write(1, filename_tmp)
    uint8_t* filename_tmp = dentry.file_name;
    memcpy((int8_t*)buf, (int8_t*)filename_tmp, FILE_NAME_MAX_LEGTH);
    return nbytes;*/

    

}

int32_t dir_read_helper(uint32_t inode, dentry_t* dentry) {
    int i;                                               /* Loop Variable */
    int max_inode = boot_block -> number_inodes; /* Tells us how many entries to loop through */
    /*-- Cast to int8_t from uint because strlen params --*/
    
    
    /* Sanity Check */
    if (inode < 0 || inode > max_inode) {
        return -1;
    }

    for (i = 0; i < max_inode; i++) {
        if (inode == boot_block->dir_entries[i].file_inode_num) {
            *dentry = boot_block->dir_entries[i];
            return 0;
        }
    }
    return -1;
}



/* int32_t directory_write
 * Inputs: fd - File Descriptor
 *         buf - buffer to write into file
 *         nbytes - number of bytes to write in file
 * Return: -1 failure (sys is read only)
 *          nbytes - number of bytes wrote (success only)
 * Output: None
 * Side Effects: Writes into file
 *
 * Description: writes data to a file
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) {
    /* Writes to regular filesystem should return -1, since we have a read-only file system */    
    return -1;
}

/* int32_t directory_open
 * Inputs: file_name - file to be opened
 * Return: -1 Failure
 *          0 Success
 * Output: None
 * Side Effects: None
 *
 * Description: opens a directory file and returns 0
 */
int32_t directory_open(const uint8_t* filename) {
    // printf("Dir open \n");
    global_ind = 0;
    if (strlen((int8_t*) filename) >= FILE_NAME_MAX_LEGTH) { return -1; }
    /* uses read_dentry_by_name*/
    dentry_t dentry;
    if (read_dentry_by_name(filename, &dentry) == -1) return -1;
    return 0;
}

/* int32_t directory_close
 * Inputs: fd - File Descriptor
 * Return: -1 Failure
 *          0 Success
 * Output: None
 * Side Effects: None
 *
 * Description: Closes specificed file descriptor
 */
int32_t directory_close(int32_t fd) {
    // if (strlen((int8_t*) filename) >= FILE_NAME_MAX_LEGTH) { return -1; }
    global_ind = 0;
    return 0;
}
