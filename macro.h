#ifndef _MACRO_H
#define _MACRO_H

/*==============================================
this function handles the spread of macro instructions in the code file
and create a new ".am" file upon success
it gets a pointer to the list with the code text, 
and a string with the new file name
returns SUCCESS if no errors were found and a file has been created, or FAILURE otherways
===============================================*/
int handle_macro(list* new_list, char macro_result_file_path[]);

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
!!!!!!!!!
===============================================*/
void macro_arr_print(list* macro_arr, int table_count);

/*==============================================
this function delete the macro table and free the memory allocation
it gets a pinter to the list it needs to delete
===============================================*/
void macro_array_terminate(list* macro_array, int table_count);

#endif /*_MACRO_H*/
