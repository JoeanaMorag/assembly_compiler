#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessor.h"

/*==============================================
this program read an unkown amount of text from a given file
find the macro instructions in it, replce theme with their defenitions
and prints the processed code to a new .am file
the main function gets the files name from the terminal
===============================================*/
int main(int argc, char* argv[])
{
	char * read_file_name;
	char * write_file_name;
	list new_list;
	list* macro_table;
	int macro_table_size;
	int error_flag = FALSE;
	int i = 1;
	
	if((command_check(argc)) == FAILURE)
		exit(0);
	
	while(i < argc){		
			read_file_name = file_name_suffix(argv[i], READ_FILE_SUFFIX);
			
			if((read_file(&new_list,read_file_name)) == FAILURE)
				exit(0);
			
			clean_list(&new_list);
			
			/*!!!!!*/
			printf("=============================\n");
			printf("list after whitespaces remove\n");
			print_list(&new_list);
			
			printf("=============================\n");
			macro_table = macro_array(&new_list, &macro_table_size);
			printf("macro_table of size %d was created\n", macro_table_size);
			
			printf("=============================\n");
			if(macro_replace(&new_list, macro_table, &macro_table_size) == TRUE)
			{	
				error_flag = TRUE;
			}
			printf("macro_table:\n");
			/*!!!!!*/
			macro_arr_print(macro_table, macro_table_size);
			
			/*!!!!FREE!!!!*/
			macro_array_terminate(macro_table, macro_table_size);
			free(macro_table);
			
			if(error_flag == FALSE)
			{
				write_file_name = file_name_suffix(argv[i], WRITE_FILE_SUFFIX);
			
				write_file(&new_list,write_file_name);
			}
			
			/*printf("=============================\n");
			printf("list after macro replace\n");
			print_list(&new_list);NEED TO CREATE NEW FILE*/
	
			terminate(&new_list);
			free(read_file_name);
			i++;
		      }
	
	return 0;
}

char * file_name_suffix(char *argv, char data[])
{
	char * ptr;
	ptr = (char*)malloc(strlen(argv) * sizeof(char) +
			    strlen(data) * sizeof(char) + 1*sizeof(char));
	if(!ptr)
	{
		printf("memory allocation failed\n");
		return NULL;
	}
	
	strncpy(ptr, argv, strlen(argv));
	strncpy(ptr + strlen(argv), data, strlen(data));
	ptr[strlen(argv) + strlen(data)] = '\0';
	
	return ptr;
}

int command_check(int argc)
{
	if(argc == 1)
	{
		printf("missing file name\n");
		return FAILURE;
	}
	
	else
		return SUCCESS;
}


int read_file(list* main_list, char* file_name)
{
	FILE* fd;
	char data[ARRAY_SIZE] = {'\0'};
	int data_size;
	
	if(!(fd = fopen(file_name,"r+")))
	{
		fprintf(stderr,"cannot open file (%s)\n", file_name);
		return FAILURE;
	}
	
	init(main_list);
	
	while(fgets(data, ARRAY_SIZE, fd))
	{
		data_size = strlen(data);
		if(data_size > 0 && data[data_size -1] == '\n')
			data[data_size -1] = '\0';
		add_node(main_list, data);
		memset(data,'\0',ARRAY_SIZE);
		
	}
	
	fclose(fd);
	return SUCCESS;

}

int write_file(list* main_list, char* file_name)
{
	FILE* fd;
	node* ptr = main_list->head;
	
	if(!(fd = fopen(file_name,"w+")))
	{
		fprintf(stderr,"cannot open file (%s)\n", file_name);
		return FAILURE;
	}
	
	while(ptr != NULL)
	{
		fputs(ptr->data, fd);
		fputs("\n", fd);
		ptr = ptr->next;
	}
	
	fclose(fd);
	return SUCCESS;
}

void remove_white_spaces(char * str)
{
	int n = strlen(str);
	int i = 0, j = -1;
	int space_found = FALSE;/*flag that sets true if spcae was found*/
	
	/*go over leading spaces*/
	while(++j < n && (str[j] == ' '|| str[j] == '\t'));
	
	while(j < n)
	{
		if(str[j] != ' ' && str[j] != '\t')
		{
			str[i++] = str[j++];
		
			space_found = FALSE;
		}
		
		else if(str[j] == ' ' || str[j] == '\t')
		{	
			if(space_found == FALSE)
			{
				if(str[i] == '.' || str[j] == ',' || str[i] == '#')
					j++;
				else
				{
					str[i++] = ' ';
					space_found = TRUE;
				}
			}
			j++;
		}
	}
	str[i] = '\0';
}

void clean_list(list * main_list)
{
	node* ptr;
	node*  temp;
	ptr = main_list->head;
	while(ptr != NULL)
	{	
		remove_white_spaces(ptr->data);
		
		if(ptr->data[0] == ';' || ptr->data[0] == '\0')
		{
			temp = ptr;
			ptr = ptr->next;
			delete_node(main_list, temp);
		}
		else	
			ptr = ptr->next;
	}

}

int macro_count(list* main_list)
{
	node* ptr= main_list->head;
	char str[ARRAY_SIZE];
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
		/*printf("macro_name is: %s\n", macro_name);*/
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
	
	length = sizeof(saved_words)/sizeof(saved_words[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(macro_name, saved_words[i]) == 0)
		{
			printf("macro name may not be a saved word (%s)", saved_words[i]);
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
	char str[ARRAY_SIZE];
	char* macro_name;
	char* macro_name_check;
	int macro_index;/*store the macro index in the macro array if found*/
	int macro_flag = FALSE; /*falgs if a new macro is being processed*/
	int error_flag = FALSE;
	int macro_array_count = 0;
	
	while(ptr != NULL)
	{
		/*printf("----------------\n");
		printf("the line is this: %s\n", ptr->data);*/
		strcpy(str, ptr->data);
		macro_name = strtok(str," ");
		
		if(macro_flag == TRUE)
		{
			
			temp_ptr = ptr;
			ptr = ptr->next;
			
			/*printf("macro flag is ON\n");*/
			if(strcmp(macro_name,"endmacro") == 0)
			{
				delete_node(main_list, temp_ptr);
				macro_flag = FALSE;
				macro_array_count++;
				/*printf("macro flag is OFF\n");*/
			}
			else
			{
				add_node(&macro_table[macro_array_count], temp_ptr->data);
				delete_node(main_list, temp_ptr);
				/*printf("added node to the list in the array at index %d\n", macro_array_count);*/
			}
			
		}
		else if(strcmp(macro_name,"macro") == 0)
		{
			macro_flag = TRUE;
			temp_ptr = ptr;
			ptr = ptr->next;
			
			macro_name = strtok(NULL," ");
			/*printf("the macro new macro name is: %s\n", macro_name);*/
			
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
			/*printf("the line is not a macro\n");*/
			macro_index = existing_macro(macro_table, macro_name, table_size);
			/*printf("macro_index is: %d\n", macro_index);*/
			
			temp_ptr = ptr;
			ptr = ptr->next;
			
			if(macro_index != -1)
			{
				/*printf("a macro mention was found (%s)\n", macro_name);*/
				macro_ptr = macro_table[macro_index].head->next;
				
				while(macro_ptr != NULL)
				{
					/*printf("before adding a new node\n");*/
					add_middle_node(main_list, temp_ptr->prev, macro_ptr->data);
					/*printf("the new line in list is: %s\n", temp_ptr->prev->data);*/
					macro_ptr = macro_ptr->next;
				}
					
				delete_node(main_list, temp_ptr);
			}
		}
	}
	return error_flag;			
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
		/*printf("num %d list of macro array\n", i);*/
		print_list(&macro_arr[i]);
	}
}
