#ifndef _COMMEN_H
#define _COMMEN_H

#define FAILURE 1
#define SUCCESS 0
#define FALSE 0
#define TRUE 1
#define READ_FILE_SUFFIX ".as"
#define WRITE_FILE_SUFFIX ".am"
#define OBJECT_FILE_SUFFIX ".ob"
#define ENTRIES_FILE_SUFFIX ".ent"
#define EXTERNALS_FILE_SUFFIX ".ext"

/*===============================================
define the list of saved words
===============================================*/
static char * saved_words[] = {".data", ".string", ".struct", ".entry", ".extern", "hlt", "rts", "jsr",
			"prn", "get", "bne", "jmp", "dec", "inc", "clr", "not", "lea", "sub",
			"add", "cmp", "mov"};

/*==============================================
this function write the text from a list to a new or existing file
it gets a list type from which it takes the text, and a file name to give to the new file
return FAILURE if the file fail to oppen, and SUCCESS otherways
===============================================*/
int write_file(list* main_list, char* file_name);

#endif /*_COMMEN_H*/

