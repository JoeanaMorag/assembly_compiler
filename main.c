#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "common.h"
#include "macro.h"
#include "first_pass.h"
#include "second_pass.h"

/*
************************************** Declerations **************************************
*/

/*===============================================
this function add the right suffix to a certain file name
it gets a pointer to the file name and the suffix string it needs to add to it
returns a pointer to combined string of the file name with suffix
===============================================*/
char * file_name_suffix(char *argv, char data[]);

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


/*
************************************** Implementation **************************************
*/

/*==============================================
This function handles all activities in the program
it gets the files names as a command line arguments
===============================================*/
int main(int argc, char* argv[])
{
	char * read_file_name;
	char * write_file_name;
	list new_list;
	
	label* label_table = NULL;
	char** label_by_index = NULL;/*store index for missing label in first pass*/
	int label_count = 0;
	int data_arr[MACHINE_RAM] = {0};
	int instruct_arr[MACHINE_RAM] = {0};
	int ic = 100;/*memory image starts from address 100*/
	int dc = 0;
	
	int status;
	int i = 1;
	
	if((command_check(argc)) == FAILURE)
		exit(0);
	
	while(i < argc)
	{		
			read_file_name = file_name_suffix(argv[i], READ_FILE_SUFFIX);
			
			if((read_file(&new_list,read_file_name)) == FAILURE)
				exit(0);
			
			printf("=============================\n");
			printf("file (%s)\n", read_file_name);
			
			clean_list(&new_list);
			
#ifdef PRINT_DEBUG_MILESTONE
			printf("=============================\n");
			printf("list after whitespaces remove\n");
			print_list(&new_list);
#endif /* PRINT_DEBUG_MILESTONE */
			
			write_file_name = file_name_suffix(argv[i], WRITE_FILE_SUFFIX);
			status = handle_macro(&new_list, write_file_name);
			
			free(write_file_name);
			if(status == SUCCESS)
			{
				status = first_pass_handel(&new_list, data_arr, instruct_arr,&dc, &ic, &label_table,
								  &label_by_index, &label_count);
			}
			
			if(status == SUCCESS)
			{
				status = second_pass_handler(instruct_arr, ic, data_arr, dc, label_table, label_count,
											 label_by_index, argv[i]);
			}
			
#ifdef PRINT_DEBUG_MILESTONE
			printf("=============================\n");
			printf("list after macro replace\n");
			print_list(&new_list);
#endif /* PRINT_DEBUG_MILESTONE */
			printf("=============================\n");
			
			/*free memory allocation*/
			free(label_table);/*free label table allocation*/
			free_label_by_index(label_by_index, label_count);
			free(label_by_index);
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
	char data[LINE_SIZE] = {'\0'};
	int data_size;
	
	if(!(fd = fopen(file_name,"r+")))
	{
		fprintf(stderr,"cannot open file (%s)\n", file_name);
		return FAILURE;
	}
	
	init(main_list);
	
	while(fgets(data, LINE_SIZE, fd))
	{
		data_size = strlen(data);
		if(data_size > 0 && data[data_size -1] == '\n')
			data[data_size -1] = '\0';
		add_node(main_list, data);
		memset(data,'\0',LINE_SIZE);
		
	}
	
	fclose(fd);
	return SUCCESS;

}

int write_file(void* main_list, char* file_name)
{
	FILE* fd;
	node* ptr = ((list*)main_list)->head;
	
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

