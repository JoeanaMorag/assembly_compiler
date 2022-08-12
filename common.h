#ifndef _COMMEN_H
#define _COMMEN_H

/*a print debugging defenitions*/
/*#define PRINT_DEBUG_MILESTONE
#define PRINT_DEBUG*/

#define FAILURE 1
#define SUCCESS 0
#define FALSE 0
#define TRUE 1
#define NOT_FOUND -1

#define LINE_LENGTH 80
#define LABLE_LENGTH 30
#define MACHINE_RAM 256
#define BITS_RESET 1023
#define WORD_SIZE 10
#define MEMORY_LODING_PLACE 100

#define READ_FILE_SUFFIX ".as"
#define WRITE_FILE_SUFFIX ".am"
#define OBJECT_FILE_SUFFIX ".ob"
#define ENTRIES_FILE_SUFFIX ".ent"
#define EXTERNALS_FILE_SUFFIX ".ext"

/*==============================================
define the label structure for the label table
===============================================*/
typedef struct labels_struct
{
	char name[LABLE_LENGTH];
	int address;
	int label_type;
	int is_defined;
	int is_extern;
	int is_entry;
} label;

/*==============================================
this function write text from a list to a new or existing file
===============================================*/
int write_file(void* main_list, char* file_name);

/*==============================================
this function combine a file name with the givven suffix
===============================================*/
char * file_name_suffix(char *argv, char data[]);

/*==============================================
debugging
===============================================*/
void print_lable_table(label label_table[], int label_count);
void print_dc_arr(int array[], int dc);
void print_ic_arr(int array[], int ic);
void print_in_bits(int num, int num_of_bits);

#endif /*_COMMEN_H*/

