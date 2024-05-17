#ifndef _FILE_SYS_H
#define _FILE_SYS_H

#include "types.h"
#include "syscall.h"
#include "lib.h"

#define BLOCK_SIZE                             4096 /* 4kB = 4096 bytes */
#define DATA_BLOCK_MAX                         (BLOCK_SIZE / 4) - 1 //1024


/******************NOTES*************************
- Each Inode contains 1023 data blocks
- Each File contains 1023*4096 bytes max

-Inode
[ 0th block = 15]  ---> datablocks[15]
[ 1st block = 20]
[ 2nd block = 40]

!!Inode contains the index of the data block to be accessed
    Inode doesnt contain data block itself, just the index

4kb file_sys[N + D]
inode_arr = file_sys[1:]
inode_arr[0] = inode[0]
inode[0] -> [ 0th block = 15]
            [ 1st block = 20]
            [ 2nd block = 40]
int x  = file_sys -> num_inodes;

inode js first blockis :

ionde_arr[x + 4096*inode[j]]

*****************NOTES************************/


/* BOOT_BLOCK_MAX is different then max entires, since we still need to loop through
the first entry */
#define BOOT_BLOCK_MAX                     64

/* 
File name 32B - 32 characters max, where each character is a 2 byte/16bit hex code (Each directory...)
File type 4 bytes/32 bits
inode# 4B bytes /32 bits
Reserverd 24B
See Appendix A 8.1 for more understanding 
*/
#define FILE_NAME_MAX_LEGTH                 32
#define RESERVED_DENTRY_MAX                 24

typedef struct dentry_t {
    uint8_t file_name[FILE_NAME_MAX_LEGTH];
    uint32_t file_type;                    
    uint32_t file_inode_num;
    uint8_t dentry_reserved[RESERVED_DENTRY_MAX];
} dentry_t;

/* 
inode: https://wiki.osdev.org/File_Systems#inodes
*/

/* */
typedef struct inode_t {
    int32_t length;
    uint32_t data_blocks[DATA_BLOCK_MAX];
} inode_t;


/*
 * See Appendix A 8.1 for more info/visual
 * #dir entries is 4 bytes / 32 bits
 * # inodes (n) is 4 bytes / 32 bits
 * # data blocks 4 bytes / 32 bits
 * 52 bytes are reservered
 * 64 bytes are for directory entires (Each a dentry_t struct)
*/
#define RESERVED_BOOT_BLOCK_MAX         52
#define BOOT_BLOCK_MAX_ENTRIES          63 /* Not 64, since first statistics and directory entry is 64B */
typedef struct boot_block_t {
    uint32_t number_dir_entries;
    uint32_t number_inodes;
    uint32_t number_data_blocks;
    uint8_t boot_block_reserved[RESERVED_BOOT_BLOCK_MAX];
    dentry_t dir_entries[BOOT_BLOCK_MAX_ENTRIES];
} boot_block_t;



/* Used to initialize file system and get memory filesystem address from kernel */
void filesys_init(unsigned int boot_address);

/* 
Parameters are designed to work with Appendix B. 
See Appendix B and Appendix A to view read, write, open, and close files
*/

/* Reads data from a file into buf*/

int32_t filesys_read(int32_t fd, void* buf, int32_t nbytes);

/* Writes data from a file - read only system so return -1*/
int32_t filesys_write(int32_t fd, const void* buf, int32_t nbytes);

/* Provides access to file init any temporary strucutes*/
int32_t filesys_open(const uint8_t* filename);

/* undo what was done in open function */
int32_t filesys_close(int32_t fd);

/* Finds the file with fname and fills dentry_t with files info */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

/* Files the file with index and reads file info into dentry_T*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

/* reads the data from file with corresponding inode number using offset and length*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* Reads data from a file into buf filename by filename*/
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);

/* Writes data from a file - read only system so return -1*/
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);

/* opens a directory file - note the filetype*/
int32_t directory_open(const uint8_t* filename);

/* closes a directory */
int32_t directory_close(int32_t fd);


int32_t dir_read_helper(uint32_t inode, dentry_t* dentry);

#endif
