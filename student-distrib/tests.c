#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"
#include "rtc.h"
#include "filesys.h"

#define PASS 1
#define FAIL 0


/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
/* Div by zero
 * 
 * Divides by zero
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 */
int div_by_zero() {
	TEST_HEADER;

	int a = 0;
	int b;
	b = 1/a;
	return FAIL;
}
/* test invalid opcode
 * 
 * accesses an opcode that is not real.
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 */
int test_invalid_opcode() {
	TEST_HEADER;
	__asm__ volatile ("ud2");
	return FAIL;
}
/* test mult exceptions
 * 
 * divides by zero, then uses an invalid opcode.
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 */
int test_mult_exceptions() {
	TEST_HEADER;
	int a = 0;
	int b;
	b = 1/a;
	__asm__ volatile ("ud2");
	return FAIL;
}
/* test system int
 * 
 * Does  system interrupt.
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 */
int test_system_int() {
	TEST_HEADER;
	__asm__ ("int   $0x80");
	return FAIL;
}
// add more tests here

/* Checkpoint 2 tests */

/* int terminal_test
 * Input: None
 * Return: None
 * Output: Prints characters to screen
 * Description: Tests keyboard and terminal working together
*/
int terminal_test() {
	TEST_HEADER;
	char buf[KEYBOARD_BUF_SIZE];			// Buffer that holds key
	int r;				// Holds return from terminal_read
	int w;			// Holds return from terminal_write
	
	printf("Terminal test: \n");
	terminal_open(0);
	while(1) {
		r = terminal_read(0, buf, KEYBOARD_BUF_SIZE);
		w = terminal_write(0, buf, r);

	}
	terminal_close(0);
	printf("Bytes read: %d, Bytes wrote: %d", r, w);
	return PASS;
} 


/* int test_rtc_driver
 * Input: None
 * Return: None
 * Output: Prints 1s and newlines to screen
 * Description: Tests RTC and changing Freq
*/
int test_rtc_driver() {
	TEST_HEADER;
	uint32_t temp = 0;
	temp = temp + rtc_open(NULL);                                // if rtc function was successful, should be adding 0 to temp
	uint32_t i;
	uint32_t j;
	printf("\n");
	printf("\n");
	for(i = 2; i <= 1024; i = i * 2){
		printf("\n");
		clear();
		//startatfront();
		temp = temp + rtc_write(NULL, &i, sizeof(uint32_t));     // if rtc function was successful, should be adding 0 to temp
		for(j = 0; j < i; j++){
			temp = temp + rtc_read(NULL, NULL, NULL);            // if rtc function was successful, should be adding 0 to temp
			printf("%d", 1);
			if (j % 80 == 0 && j != 0) { printf("\n"); }
		}
	}
	printf("\n");
	if(temp == 0){                                               // if temp is not equal to 0, means a function failed
		return PASS;
	}
	else{
		return FAIL;
	}
}

/* int test_rtc_driver
 * Input: None
 * Return: None
 * Output: Prints 1s and newlines to screen
 * Description: Tests RTC and changing Freq
*/
int32_t test_filesys() {
	/*open 0 if length < 32 -1 else */
	/*close should return 0 */
	/* write should always be -1 */
	/* pass in verylargetext...name.txt should fail*/
	/* pass in verlargetext....name.tx*/
	TEST_HEADER;
	int32_t open_check, write_check, close_check;
	char buf[KEYBOARD_BUF_SIZE];
	open_check = filesys_open((uint8_t*)"frame0.txt");
	if(open_check == 0){
		printf("Open1 Pass ");
	}
	else if(open_check == -1){
		printf("Open1 Fail ");
	}

	open_check = filesys_open((uint8_t*)"verylargetextwithverylongname.txt");
	if(open_check == -1){
		printf("Open2 Pass ");
	}
	else if(open_check == 0){
		printf("Open2 Fail ");
	}

	write_check = filesys_write(0, buf, 0);
	if(write_check == -1){
		printf("Write Pass ");
	}
	else if(write_check != -1){
		printf("Write Fail ");
	}

	close_check = filesys_close(0);
	if(close_check == 0){
		printf("Close Pass ");
	}
	else if(close_check != 0){
		printf("Close Fail ");
	}
	clear();
	printf("\n\n\n testing read_dentry \n");
	dentry_t dentry_frame;
	read_dentry_by_name((const uint8_t*)"fish", &dentry_frame);
	//read_dentry_by_index(4, &dentry_frame);
	uint32_t inode = dentry_frame.file_inode_num;

	/* 5000 is just an offset to test that we are 
	correctly reading the file starting specifically at offset */
	uint32_t offset = 5000;
	
	/* File size is 36164 but we are doing 55000 to test that we 
	do not read more than the (file size - offset).*/
	uint32_t length = 55000;
	uint8_t buffff[length]; 
	int32_t toret = read_data(inode, offset, buffff, length);

	// 31164 is file_size - offset = 36164 - 5000 = 31164
	printf("Read_data returns %d and correct is 31164 \n ", toret);
	// read_dentry_by_name(fish)*/
	return PASS;
	

}
/* int test_dir
 * Input: None
 * Return: None
 * Output: file names
 * Description: test directory read
*/
int32_t test_dir() {
	TEST_HEADER;
	int i;
	int8_t buf[2000];
	clear();
	//startatfront();
	printf("\n");
	for (i = 0; i < 63; i++) {
		if (directory_read(i, buf, 32) == -1) {
			break;
		}
		printf("%s\n", (int8_t*)buf);
	}
	return PASS;
}


/* Checkpoint 3 tests */

/* int test_exec
 * Input: None
 * Return: None
 * Output: prints to screen
 * Description: tests syscall - stdout
*/
int32_t test_exec() {
	TEST_HEADER;
	clear();
	sys_execute((uint8_t *) "testprint");
	uint8_t* s = (uint8_t*)"Hello, if this ran, the program was correct. Yay!\n";
	printf("Sys write returns: %d\n", sys_write(1, s, strlen((int8_t*)s)));
	return PASS;
}

int32_t test_ls() {
	int i;				// Loop variable
	clear();
	//printf("\n");
	sys_execute((uint8_t*) ".");
	int8_t buf[2000];
	printf(" \nSys_open return = %d \n", sys_open((uint8_t*) "."));
	sys_read(2, buf, 32);
	sys_write(1, buf, 32);
	for (i = 0; i < 63; i++) {
		if (sys_read(2, buf, 32) == -1) {
			break;
		}
		sys_write(1, buf, 32);
	}
	printf("Sys_close returns = %d \n", sys_close(2));
	return PASS;
}


/* Checkpoint 4 tests */
/* testprint, hello, counter, ls, pingpong, syserr*/

int32_t test_exec2() {
	TEST_HEADER;
	clear();
	sys_execute((uint8_t *) "testprint");
	return PASS;
}


int32_t test_shell() {
	TEST_HEADER;
	clear();
	sys_execute((uint8_t*) "shell");
	return PASS;
}
int32_t test_hello() {
	TEST_HEADER;
	clear();
	sys_execute((uint8_t*) "hello");
	return PASS;
}
int32_t test_counter() {
	TEST_HEADER;
	clear();
	sys_execute((uint8_t*) "counter");
	return PASS;
}
/*int32_t test_read() {
	TEST_HEADER;
	char buffer[200];
	int32_t read = sys_open((uint8_t*)"frame0.txt");
	//sys_read(read, buffer, 32);
	//sys_write(read, buffer, 32);
	//sys_close(read);
	return PASS;
}*/
int32_t test_pingpong() {
	TEST_HEADER;
	sys_execute((uint8_t*) "pingpong");
	return PASS;
}
int32_t test_fish() {
	TEST_HEADER;
	clear();
	sys_execute((uint8_t*) "fish");
	return PASS;
}
int32_t test_syserr() {
	TEST_HEADER;
	clear();
	//sys_execute((uint8_t*) "shell");
	sys_execute((uint8_t*) "syserr");
	return PASS;
}
//ALL: cat grep hello ls pingpong counter shell sigtest testprint syserr


/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	printf("Launching tests");
	//test_interrupts();
	//TEST_OUTPUT("idt_test", idt_test());
	// terminal_test();
	//TEST_OUTPUT("div_zero", div_by_zero());

	//TEST_OUTPUT("test_invalid_opcode", test_invalid_opcode());

	//TEST_OUTPUT("test_mult_exceptions", test_mult_exceptions());
	//TEST_OUTPUT("test_system_int", test_system_int());

	//TEST_OUTPUT("test_rtc_driver", test_rtc_driver());
	//TEST_OUTPUT("terminal_test", terminal_test());
	//TEST_OUTPUT("test_filesys", test_filesys());
	//TEST_OUTPUT("test_dir", test_dir());
	//TEST_OUTPUT("test_exec-sys_write", test_exec());
	//TEST_OUTPUT("test_ls", test_ls());
	//TEST_OUTPUT("test_fish", test_fish());
	//TEST_OUTPUT("test_syserr", test_syserr());
	//TEST_OUTPUT("test_hello", test_hello());

	//TEST_OUTPUT("test_exec2-sys_write", test_exec2());
	//TEST_OUTPUT("test_pingpong", test_pingpong());
	TEST_OUTPUT("Test Shell", test_shell());
	//while(1);
	// launch your tests here
}

