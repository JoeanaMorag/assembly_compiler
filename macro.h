#ifndef _MACRO_H
#define _MACRO_H

/*==============================================
this function handles the spread of macro instructions in the code file
and create a new ".am" file upon success
returns SUCCESS if no errors were found and a file has been created, or FAILURE otherways
===============================================*/
int handle_macro(list* new_list, char macro_result_file_path[]);

/*==============================================
this function allocate memory for a macro table to a given code text
===============================================*/
list* macro_array(list* main_list, int* count_size);

/*==============================================
this function find and replace macro instructions in a linked list
returns TRUE if an error was found in the process of FLASE otherways
===============================================*/
int macro_replace(list* main_list, list* macro_table, int* table_size);

/*==============================================
this function delete the macro table and free the memory allocation
===============================================*/
void macro_array_terminate(list* macro_array, int table_count);

#endif /*_MACRO_H*/
