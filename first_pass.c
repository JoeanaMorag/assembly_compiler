#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "linked_list.h"
#include "common.h"

/*
************************************** Declerations **************************************
*/

/*==============================================
define the label structure for the label table
===============================================*/
typedef struct labels_struct
{
	char name[LABLE_LENGTH];
	int address;
	int label_type;
	int is_extern;
	int is_entry;
} label;

/*==============================================
A/R/E modes ordered by their numeric value
===============================================*/
enum A_R_E{ABSOLUTE, RELOCATABLE, EXTERNAL};

/*==============================================
this function counts the number of labels in the code
it gets a pointer to a list which holds the code lines
returns the number of labels in the code
===============================================*/
int label_count(list* main_list);

/*==============================================
this function builds the labels table
it gets a pointer to a list which holds the code lines
and a pointer to a label counter
returns a pointer to the lable table array
===============================================*/
label * build_label_table(list* main_list, int* num_of_label);

/*==============================================
this function check if a label is legal
it gets the lable name and the line number
returns TRUE if label is legal or FALSE otherways
===============================================*/
int legal_label(char label_name[], int line_num);

/*==============================================
this function check if a label characters are all alphanumeric
it gets the lable name
returns TRUE if label is all alphanumeric or FALSE otherways
===============================================*/
int is_alphanumeric(char label_name[]);

/*==============================================
this function check if a label is not a saved word
it gets the lable name
returns TRUE if the label is a saved word or FALSE otherways
===============================================*/
int is_label_saved_word(char label_name[]);

/*==============================================
missing
===============================================*/
int label_in_table (label label_table[], int num_of_label, char label_name[]);

/*==============================================
missing
===============================================*/
int is_command(char expression[]);

/*==============================================
missing
===============================================*/
int is_directive(char expression[]);

/*==============================================
debugging
===============================================*/
void print_lable_table(label label_table[], int num_of_label);

/*
************************************** Implementation **************************************
*/

int first_pass_handel(list * main_list)
{
	unsigned int data_arr[MACHINE_RAM];
	unsigned int instruct_arr[MACHINE_RAM];
	int line_num = 1; /*code lines starts from 1*/

	char str[ARRAY_SIZE];
	char* expression;
	
	node* ptr = main_list->head;
	
	
	label* label_table;
	int num_of_label;
	int label_table_count = 0;
	
	int ic = 100;/*memory image starts from address 100*/
	int dc = 0;
	
	int status = SUCCESS;
	
	label_table = build_label_table(main_list, &num_of_label);

#ifdef PRINT_DEBUG
	printf("test for label table first name after initiation: %s\n", label_table[0].name);
#endif /* PRINT_DEBUG */
	
	while(ptr != NULL)
	{
		/*find the first word in the line*/
		strcpy(str, ptr->data);
		expression = strtok(str," ");

#ifdef PRINT_DEBUG
		printf("expression is: %s\n", expression);
#endif /* PRINT_DEBUG */
		
		if(expression[strlen(expression)-1] == ':')/*this is a label definition*/
		{
			expression[strlen(expression)-1] = '\0';

#ifdef PRINT_DEBUG	
			printf("expression after (:) remove: %s\n", expression);
#endif /* PRINT_DEBUG */			
			
			if(legal_label(expression, line_num) == FALSE)
			{
				status = FAILURE;
			}
			else if (label_in_table(label_table, num_of_label, expression) == TRUE)
			{
				status = FAILURE;
			}
			else
			{
#ifdef PRINT_DEBUG
				printf("THIS IS A NEW LABEL (%s)\n", expression);
#endif /* PRINT_DEBUG */				
				
				/*!!!!!!NEED TO ADD THE SIZE OF THE LEBEL- THE L PARAM!!!!!!!!!!!!*/
				
				/*place new label name*/
				strcpy(label_table[label_table_count].name, expression);
				
				/*find the second word in the line*/
				expression = strtok(NULL," ");
				if(expression == NULL)
				{
					printf("Error in line (%d): only label without command or directive\n", line_num);
					status = FAILURE;
				}
				/*place new label address*/
				else if(is_command(expression) == TRUE)
				{
					label_table[label_table_count].address = ic;
					ic++;
				}
				else if(is_directive(expression) == TRUE)
				{
					label_table[label_table_count].address = dc;
					dc++;
				}
				else/*an unknown command or directive*/
				{
					printf("Error in line (%d): unknown command or directive (%s)\n", line_num, expression);
					status = FAILURE;
				}	
				
				/*place new label info*/
				label_table[label_table_count].label_type = RELOCATABLE;
				label_table[label_table_count].is_extern = FALSE;
				label_table[label_table_count].is_entry = FALSE;
				label_table_count++;
			}
		}
		else if(strcmp(expression, ".extern") == 0)/*extern definition*/
		{
#ifdef PRINT_DEBUG
			printf("this is an extern entry\n");
#endif /* PRINT_DEBUG */
			
			/*find the second word in the line*/
			expression = strtok(NULL," ");
			
			if(label_in_table(label_table, num_of_label, expression) == TRUE)
			{
				printf("Error in line (%d): label was defined more than one time (%s)\n", line_num, expression);
				status = FAILURE;
			}
			else
			{
				/*place new label info*/
				strcpy(label_table[label_table_count].name, expression);
				label_table[label_table_count].address = 0;
				label_table[label_table_count].label_type = EXTERNAL;
				label_table[label_table_count].is_extern = TRUE;
				label_table[label_table_count].is_entry = FALSE;
				label_table_count++;
			}
		}
		ptr = ptr->next;
		line_num++;
	}
	
	/*if (entry)*/
	/*else ---->instruct or data with no label*/
	
#ifdef PRINT_DEBUG
	print_lable_table(label_table, num_of_label);
#endif /* PRINT_DEBUG */
	
	free(label_table);
	
	return status;
}

int label_count(list* main_list)
{
	node* ptr= main_list->head;
	char str[ARRAY_SIZE];
	char* first_word;
	int count = 0;
	
	while(ptr != NULL)
	{
		strcpy(str, ptr->data);
		first_word = strtok(str," ");
		
		if((first_word[strlen(first_word)-1] == ':') || (strcmp(first_word, ".extern") == 0))
		{	
#ifdef PRINT_DEBUG
			printf("this word is a label: %s\n", first_word);
#endif /* PRINT_DEBUG */
			count++;
		}
		ptr = ptr->next;
	}
	
	return count;
}

label * build_label_table(list* main_list, int* num_of_label)
{
	label* label_table;
	
	*num_of_label = label_count(main_list);
	label_table = (label*)malloc((*num_of_label) * sizeof(label));
	
	if(label_table == NULL)
	{
		printf("memory allocation failed\n");
		return label_table;
	}
	
	memset(label_table, 0, (*num_of_label) * sizeof(label));
	
#ifdef PRINT_DEBUG
	printf("number of labels in this file is: %d\n", *num_of_label);
#endif /* PRINT_DEBUG */

	return label_table;
} 

int legal_label(char label_name[], int line_num)
{
	int legal = TRUE;
	
	if(strlen(label_name) > LABLE_LENGTH)
	{
		printf("Error in line (%d): too long label name (%s)\n", line_num, label_name);
		legal = FALSE;
	}
	else if(isalpha(label_name[0]) == 0)
	{
		printf("Error in line (%d): label name must start with an alphabet letter (%s)\n", line_num, label_name);
		legal = FALSE;
	}
	else if(is_alphanumeric(label_name) == FALSE)
	{
		printf("Error in line (%d): label may containe only alphanumeric characters (%s)\n", line_num, label_name);
		legal = FALSE;
	}
	else if(is_label_saved_word(label_name) == TRUE)
	{
		printf("Error in line (%d): label cannot be a saved word (%s)\n", line_num, label_name);
		legal = FALSE;
	}
	
#ifdef PRINT_DEBUG
	printf("label legal status is: %d\n", legal);
#endif /* PRINT_DEBUG */	
	
	return legal;
}

int is_alphanumeric(char label_name[])
{
	int alphanumeric = TRUE;
	int i;
	
	for(i = 0; i < strlen(label_name); i++)
	{
		if(isalnum(label_name[i]) == 0)
		{
			alphanumeric = FALSE;
			return alphanumeric;
		}
	}
	return alphanumeric;
}

int is_label_saved_word(char label_name[])
{
	int is_saved_word = FALSE;
	int length;
	int i;
	
	length = sizeof(directives)/sizeof(directives[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(label_name, directives[i]) == 0)
			is_saved_word = TRUE;
	}
	
	length = sizeof(commands)/sizeof(commands[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(label_name, commands[i]) == 0)
			is_saved_word = TRUE;
	}
	
	return is_saved_word;
}

int label_in_table (label label_table[], int num_of_label, char label_name[])
{
	int in_table = FALSE;
	int i;
	
	for(i = 0; i < num_of_label; i++)
	{
		if(strcmp(label_table[i].name, label_name) == 0)
		{
			in_table = TRUE;
			break;
		}	
	}
	
#ifdef PRINT_DEBUG
	printf("label (%s) in table\n", (in_table == TRUE) ? "found" : "NOT found");
#endif /* PRINT_DEBUG */	
	
	return in_table;
}

int is_command(char expression[])
{
	int i;
	int is_command = FALSE;
	int length = sizeof(commands)/sizeof(commands[0]);;
	
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(commands[i], expression) == 0)
			is_command = TRUE;	
	}
	return is_command;
}

int is_directive(char expression[])
{
	int i;
	int is_directive = FALSE;
	int length = sizeof(directives)/sizeof(directives[0]);;
	
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(directives[i], expression) == 0)
			is_directive = TRUE;	
	}
	return is_directive;
}



void print_lable_table(label label_table[], int num_of_label)
{
	int i;
	for (i = 0; i < num_of_label; i++)
	{
		printf("======================\n");
		printf("label num %d:\n", i);
		printf("label num %d name is: %s\n", i, label_table[i].name);
		printf("label num %d address is: %d\n", i, label_table[i].address);
		printf("label num %d type is: %d\n", i, label_table[i].label_type);
		printf("label num %d extern is: %d\n", i, label_table[i].is_extern);
		printf("label num %d entry is: %d\n", i, label_table[i].is_entry);
	}
}