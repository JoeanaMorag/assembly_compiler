#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "common.h"
#include "macro.h"

/*
************************************** Declerations **************************************
*/

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
this function check if a macro name is a saed word
it gets the string of the macro name
returns TRUE if the macro is a saved word or FLASE otherways
===============================================*/
int is_macro_saved_word(char macro_name[]);


/*
************************************** Implementation **************************************
*/

int macro_count(list* main_list)
{
	node* ptr= main_list->head;
	char str[LINE_SIZE];
	char* token;
	int count = 0;
	
	while(ptr != NULL)
	{
		strcpy(str, ptr->data);
		token = strtok(str," ");
		if(strcmp(token,"macro") == 0)
			count++;
		
		ptr = ptr->next;
	}
	
	return count;
}

list* macro_array(list* main_list, int* count_size)
{
	list* macro_array;
	int i;
	
	*count_size = macro_count(main_list);
	macro_array = (list*)malloc((*count_size) * sizeof(list));
	
	if(macro_array == NULL)
	{
		printf("memory allocation failed\n");
		return NULL;
	}
	
	for(i = 0; i < (*count_size); i++)
	{
		init(&macro_array[i]);
	}
	return macro_array;
}

int existing_macro(list* macro_table, char* macro_name, int* table_size)
{
	int i, length;
	int macro_index = -1;
	
	length = *table_size;
		
	for(i = 0; i < length; i++)
	{
#ifdef PRINT_DEBUG
		printf("macro_name is: %s\n", macro_name);
#endif /* PRINT_DEBUG */
		if(is_list_empty(&macro_table[i]) == TRUE)
			break;
		if(strcmp(macro_name, macro_table[i].head->data) == 0)	
			macro_index = i;
	}
	
	
	return macro_index;
	
}

int is_macro_saved_word(char macro_name[])
{
	int is_saved_word = FALSE;
	int length;
	int i;
	
	length = sizeof(directives)/sizeof(directives[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(macro_name, directives[i]) == 0)
		{
			printf("macro name may not be a directive (%s)", directives[i]);
			is_saved_word = TRUE;
		}
	}
	
	length = sizeof(commands)/sizeof(commands[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(macro_name, commands[i]) == 0)
		{
			printf("macro name may not be a command (%s)", commands[i]);
			is_saved_word = TRUE;
		}
	}
	
	length = sizeof(registers)/sizeof(registers[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(macro_name, registers[i]) == 0)
		{
			printf("macro name may not be a register name (%s)", registers[i]);
			is_saved_word = TRUE;
		}
	}
	
	return is_saved_word;
}

int macro_replace(list* main_list, list* macro_table, int* table_size)
{
	node* ptr = main_list->head;
	node* temp_ptr;
	node* macro_ptr;
	char str[LINE_SIZE];
	char* macro_name;
	char* macro_name_check;
	int macro_index;/*store the macro index in the macro array if found*/
	int macro_flag = FALSE; /*falgs if a new macro is being processed*/
	int error_flag = FALSE;
	int macro_array_count = 0;
	
	while(ptr != NULL)
	{
#ifdef PRINT_DEBUG
		printf("----------------\n");
		printf("the line is this: %s\n", ptr->data);
#endif /* PRINT_DEBUG */
		strcpy(str, ptr->data);
		macro_name = strtok(str," ");
		
		if(macro_flag == TRUE)
		{
			
			temp_ptr = ptr;
			ptr = ptr->next;
			
#ifdef PRINT_DEBUG
			printf("macro flag is ON\n");
#endif /* PRINT_DEBUG */
			if(strcmp(macro_name,"endmacro") == 0)
			{
				delete_node(main_list, temp_ptr);
				macro_flag = FALSE;
				macro_array_count++;
#ifdef PRINT_DEBUG
				printf("macro flag is OFF\n");
#endif /* PRINT_DEBUG */
			}
			else
			{
				add_node(&macro_table[macro_array_count], temp_ptr->data);
				delete_node(main_list, temp_ptr);
#ifdef PRINT_DEBUG
				printf("added node to the list in the array at index %d\n", macro_array_count);
#endif /* PRINT_DEBUG */
			}
			
		}
		else if(strcmp(macro_name,"macro") == 0)
		{
			macro_flag = TRUE;
			temp_ptr = ptr;
			ptr = ptr->next;
			
			macro_name = strtok(NULL," ");
#ifdef PRINT_DEBUG
			printf("the macro new macro name is: %s\n", macro_name);
#endif /* PRINT_DEBUG */			
			macro_name_check = strtok(NULL," ");
			
			if((is_macro_saved_word(macro_name) == TRUE) || macro_name_check != NULL)
			{	
				if(macro_name_check != NULL)
					printf("macro name may not be more than one word\n");
				else
					printf("macro name can't ba a saved word (%s)\n", macro_name);
				macro_flag = FALSE;
				error_flag = TRUE;
				return error_flag;
			}
			else
			{
				add_node(&macro_table[macro_array_count], macro_name);
				delete_node(main_list, temp_ptr);
			}
		}
		else
		{
#ifdef PRINT_DEBUG
			printf("the line is not a macro\n");
#endif /* PRINT_DEBUG */

			macro_index = existing_macro(macro_table, macro_name, table_size);
			
#ifdef PRINT_DEBUG
			printf("macro_index is: %d\n", macro_index);
#endif /* PRINT_DEBUG */			
			temp_ptr = ptr;
			ptr = ptr->next;
			
			if(macro_index != -1)
			{
#ifdef PRINT_DEBUG
				printf("a macro mention was found (%s)\n", macro_name);
#endif /* PRINT_DEBUG */
				macro_ptr = macro_table[macro_index].head->next;
				
				while(macro_ptr != NULL)
				{
#ifdef PRINT_DEBUG
					printf("before adding a new node\n");
#endif /* PRINT_DEBUG */
					add_middle_node(main_list, temp_ptr->prev, macro_ptr->data);
#ifdef PRINT_DEBUG
					printf("the new line in list is: %s\n", temp_ptr->prev->data);
#endif /* PRINT_DEBUG */
					macro_ptr = macro_ptr->next;
				}
					
				delete_node(main_list, temp_ptr);
			}
		}
	}
	return error_flag;			
}

int handle_macro(list* new_list, char macro_result_file_path[])
{
	list* macro_table;
	int macro_table_size;
	int status = SUCCESS;


	printf("=============================\n");
	macro_table = macro_array(new_list, &macro_table_size);
	
#ifdef PRINT_DEBUG	
	printf("macro_table of size %d was created\n", macro_table_size);
		
	printf("=============================\n");
#endif /* PRINT_DEBUG */

	if(macro_replace(new_list, macro_table, &macro_table_size) == TRUE)
	{	
		status = FAILURE;
	}
	
#ifdef PRINT_DEBUG_MILESTONE
	printf("macro_table:\n");
	macro_arr_print(macro_table, macro_table_size);
#endif /* PRINT_DEBUG_MILESTONE */

	/*free memory allocation*/
	macro_array_terminate(macro_table, macro_table_size);
	free(macro_table);
			
	if(status == SUCCESS)
	{
		write_file(new_list,macro_result_file_path);
	}
	
	return status;
}

void macro_array_terminate(list* macro_array, int table_count)
{
	int i;
	for(i = 0; i < table_count; i++)
	{
		terminate(&macro_array[i]);
	}
}

void macro_arr_print(list* macro_arr, int table_count)
{
	int i;
	
	for(i = 0; i < table_count; i++)
	{	
#ifdef PRINT_DEBUG
		printf("num %d list of macro array\n", i);
#endif /* PRINT_DEBUG */
		print_list(&macro_arr[i]);
	}
}
