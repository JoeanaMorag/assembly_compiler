#define ARRAY_SIZE 80
#define READ_FILE_SUFFIX ".as"
#define WRITE_FILE_SUFFIX ".am"
#define OBJECT_FILE_SUFFIX ".ob"
#define ENTRIES_FILE_SUFFIX ".ent"
#define EXTERNALS_FILE_SUFFIX ".ext"
#define FAILURE 1
#define SUCCESS 0
#define FALSE 0
#define TRUE 1

/*==============================================
define the node structure of the linked list
===============================================*/
typedef struct _node
{
	char data[ARRAY_SIZE];
	struct _node* next;
	struct _node* prev;
} node;

/*===============================================
define the linked list structure
===============================================*/
typedef struct _list
{
	node* head;
	node* tail;
} list;

/*===============================================
define the list of saved words
===============================================*/
static char * saved_words[] = {".data", ".string", ".struct", ".entry", ".extern", "hlt", "rts", "jsr",
			"prn", "get", "bne", "jmp", "dec", "inc", "clr", "not", "lea", "sub",
			"add", "cmp", "mov"};

/*===============================================
this function add the right suffix to a certain file name
it gets a pointer to the file name and the suffix string it needs to add to it
returns a pointer to combined string of the file name with suffix
===============================================*/
char * file_name_suffix(char *argv, char data[]);

/*===============================================
this function removes white spaces from a string
it gets a pointer to the string it needs to operate on
===============================================*/
void remove_white_spaces(char * str);

/*===============================================
this function clean the linked list from comments, white spaces or empty lines
it gets a pointer to the list it needs to clean
===============================================*/
void clean_list(list * main_list);

/*==============================================
this function initiate the list head and tail to null
it gets a pointer to the list which needs to be initiate
===============================================*/
void init(list* main_list);

/*==============================================
this function add a new node to the end of the linked list
it gets a pointer to the list to which it needs to add a node, 
and a data array which needs to fill the data field in the node
===============================================*/
void add_node(list* main_list, char data[]);

/*===============================================
this function add a new node to the middle of the linked list
it gets a pointer to the list to which it needs to add a node, 
a data array which needs to fill the data field in the node
and the node in the list which the new node should be after
===============================================*/
void add_middle_node(list* main_list, node* some_node, char data[]);

/*==============================================
this function prints the whole data fields linked list
it gets a pointer to the list it needs to print
===============================================*/
void print_list(list* main_list);

/*===============================================
this function check if a list is empty
it gets a pointer to the list it needs to check
returns TRUE if the list is empty or FALSE otherways
===============================================*/
int is_list_empty(list* main_list);

/*==============================================
this function delete the linked list and free the memory allocation
it gets a pinter to the list it needs to delete
===============================================*/
void terminate(list* main_list);

/*==============================================
this function delete a node from the list and free the memory allocation it got
it gets a pinter to the list and to the node it needs to delete
===============================================*/
void delete_node (list * main_list, node* some_node);

/*==============================================
this function check the command line from the terminal
it gets the number of arguments that was in the command line
return SUCCESS if the command line contains the program name and one file name
returns FAILURE for any other command line (with more or less file names)
===============================================*/
int command_check(int argc);

/*==============================================
this function read the text from the file given in the command line
it gets a list type in which to store the text, and the file name
return FAILURE if the file fail to oppen, and SUCCESS otherways
===============================================*/
int read_file(list* main_list, char* file_name);

/*==============================================
this function write the text from a list to a new or existing file
it gets a list type from which it takes the text, and a file name to give to the new file
return FAILURE if the file fail to oppen, and SUCCESS otherways
===============================================*/
int write_file(list* main_list, char* file_name);

/*==============================================
this function count the number of "macro" mentions in a list
it gets a pointer to the list it needs to check
returns the number of "macro" mentions in the list
===============================================*/
int macro_count(list* main_list);

/*==============================================
this function check if a given macro is alredy stored in the macro array
it gets a pointer to the macro table, a pointer to the macro name and the macr table size
returns the index in the table if the macro exist in it or -1 otherways
===============================================*/
int existing_macro(list* macro_table, char* macro_name, int* table_size);

/*==============================================
this function allocate memory for a macro table to a givven code text
it gets a pointer to the list with the code text, 
and a pointrt to the number of macro mantions in that text
returns a pionter to the macro table
===============================================*/
list* macro_array(list* main_list, int* count_size);

/*==============================================
this function find and replace macro instructions in a linked list
it gets the list to be modify, the macro table to store and check for macros
and a pointer to that table size
returns TRUE if an error was found in the process of FLASE otherways
===============================================*/
int macro_replace(list* main_list, list* macro_table, int* table_size);

/*==============================================
this function delete the macro table and free the memory allocation
it gets a pinter to the list it needs to delete
===============================================*/
void macro_array_terminate(list* macro_array, int table_count);

/*==============================================
this function check if a macro name is a saed word
it gets the string of the macro name
returns TRUE if the macro is a saved word or FLASE otherways
===============================================*/
int is_macro_saved_word(char macro_name[]);

/*==============================================
!!!!!!!!!
===============================================*/
void macro_arr_print(list* macro_arr, int table_count);

