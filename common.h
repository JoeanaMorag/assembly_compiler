#ifndef _COMMEN_H
#define _COMMEN_H

/*#define PRINT_DEBUG_MILESTONE*/
/*#define PRINT_DEBUG*/

#define FAILURE 1
#define SUCCESS 0
#define FALSE 0
#define TRUE 1

#define LINE_LENGTH 80
#define LABLE_LENGTH 30
#define MACHINE_RAM 256


#define READ_FILE_SUFFIX ".as"
#define WRITE_FILE_SUFFIX ".am"
#define OBJECT_FILE_SUFFIX ".ob"
#define ENTRIES_FILE_SUFFIX ".ent"
#define EXTERNALS_FILE_SUFFIX ".ext"


/*===============================================
define the list of directives saved words
===============================================*/
static char * directives [] = {".data", ".string", ".struct", ".entry", ".extern"};

/*===============================================
define the list of commands saved words
===============================================*/
static char * commands [] = {"hlt", "rts", "jsr", "prn", "get", "bne",
					  "jmp", "dec", "inc", "clr", "not", "lea",
					  "sub", "add", "cmp", "mov"};

/*===============================================
define the list of saved registers names
===============================================*/
static char * registers [] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "PSW"};

/*==============================================
this function write the text from a list to a new or existing file
it gets a list type from which it takes the text, and a file name to give to the new file
return FAILURE if the file fail to oppen, and SUCCESS otherways
===============================================*/
int write_file(list* main_list, char* file_name);

#endif /*_COMMEN_H*/

