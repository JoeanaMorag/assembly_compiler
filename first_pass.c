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
	int is_defined;
	int is_extern;
	int is_entry;
} label;

/*==============================================
A/R/E modes ordered by their numeric value
===============================================*/
enum A_R_E{ABSOLUTE, RELOCATABLE, EXTERNAL};

/*==============================================
enum of commands ordered by their opcode
===============================================*/
enum COMMANDS {MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC,
			   JMP, BNE, GET, PRN, JSR, RTS, HLT};

/*==============================================
directive instructions
===============================================*/
enum DIRECTIVES {DATA, STRING, STRUCT};

/*==============================================
enum of line types
===============================================*/
enum LINE_TYPES {COMMAND, DIRECTIVE};

/*==============================================
enum of addressing methood for operands
===============================================*/
enum ADDRESSING_METHOODS {IMMEDIATE, DIRECT, STRUCT_ACCESS, REGISTER};

/*==============================================
enum for the operands direction
===============================================*/
enum OPERAND_DIRECTION {SOURCE, DEST};


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
this function parse a given line of code and stores its different components
it gets the needed components as pionters, ans the line as a string
returns SUCCESS in case the parse complited, or FAILURE otherways
===============================================*/
int parse_line(char data[], label* label_table,int* out_label_index, int* label_count, int* line_type,
			   int* line_command,char** out_op_source1, char** out_op_source2, char** out_op_dest1,
			   char** out_op_dest2, int* op_s_address_methood, int* op_d_address_methood, int* word_num,
			   int line_num);

int insert_num_data(char op_str[], int data_arr[], int* dc);

void insert_str_data(char str[], int data_arr[], int* dc);

void encode_first_word(int instruct_arr[], int* ic, int command, int source_address_methood,
					   int dest_address_methood);

/*-----------------------*/

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
this function check if a label is already in the label table
it gets the label table, the number of labels already store in the table,
and the label name to check
returns the label index if found in table, or NOT_FOUND otherways
===============================================*/
int label_in_table (label label_table[], int num_of_label, char label_name[]);

/*==============================================
this function check if an expression is a known command
it gets an expression string
return TRUE if the expression is a known command, or FALSE otherways
===============================================*/
int is_command(char expression[]);

/*==============================================
this function check if an expression is a known directive
it gets an expression string
return TRUE if the expression is a known directive, or FALSE otherways
===============================================*/
int is_directive(char expression[]);

/*==============================================
this function check if an expression is a known register
it gets an expression string
return TRUE if the expression is a known register, or FALSE otherways
===============================================*/
int is_register(char expression[]);

/*==============================================
this function finds the number of the command acoording the COMMANDS enum
it gets the command name string
returns tne index of the command in the COMMANDS enum, or NOT_FOUND otherways
===============================================*/
int find_command(char command_name[]);

/*==============================================
this function finds the number of the directive acoording the DIRECTIVES enum
it gets the directive name string
returns tne index of the directive in the DIRECTIVES enum, or NOT_FOUND otherways
===============================================*/
int find_directive(char directive_name[]);

/*==============================================
this function find the expected number of operands for each command
it gets the command number (in the COMMANDS enum)
returns the number of operands for the command, or NOT_FOUND in case it failed
===============================================*/
int num_of_operands(int command);

/*==============================================
this function check if a givven directive is legal
it gets the directive type number (in the DIRECTIVES enum), 
the directive data as string, and the line nmber (for error notifications)
returns TRUE if the directive is legal, or FALSE otherways
===============================================*/
int legal_directive(int directive_type, char data[], int line_num);


void encode_first_word(int instruct_arr[], int* ic, int command, int source_address_methood,
					   int dest_address_methood);

/*==============================================
debugging
===============================================*/
void print_lable_table(label label_table[], int num_of_label);
void print_dc_arr(int array[], int dc);
void print_ic_arr(int array[], int ic);
void print_in_bits(int num, int num_of_bits);

/*
************************************** Implementation **************************************
*/

int first_pass_handel(list * main_list)
{
	int data_arr[MACHINE_RAM] = {0};
	int instruct_arr[MACHINE_RAM] = {0};

	int line_num = 1; /*code lines starts from 1*/
	char line[LINE_SIZE];
	
	node* ptr = main_list->head;
	
	label* label_table;
	int num_of_label;
	int label_table_count = 0;
	int label_index;
	
	/*line components*/
	int line_type;
	int line_command;
	int op_sour_address_methood;
	int op_des_address_methood;
	int word_num;
	
	/*operands*/
	char* op_source1 = NULL;
	char* op_source2 = NULL;
	char* op_dest1 = NULL;
	char* op_dest2 = NULL;
	
	int ic = 100;/*memory image starts from address 100*/
	int dc = 0;
	
	int status = SUCCESS;
	
	label_table = build_label_table(main_list, &num_of_label);

#ifdef PRINT_DEBUG
	printf("test for label table first name after initiation: %s\n", label_table[0].name);
#endif /* PRINT_DEBUG */
	
	
	while(ptr != NULL)
	{
		strcpy(line, ptr->data);
		
		status = parse_line(line, label_table, &label_index, &label_table_count, &line_type, &line_command,
						    &op_source1, &op_source2, &op_dest1, &op_dest2, &op_sour_address_methood,
							&op_des_address_methood, &word_num, line_num);

		if(line_type == DIRECTIVE && status == SUCCESS)
		{
			/*update the data array*/
			if(label_index != NOT_FOUND)/*store the address for the label*/
				label_table[label_index].address = dc;
		
			if(line_command == STRUCT)
			{
				/*use op_source1 for num and op_source2 for string*/
				insert_num_data(op_source1, data_arr, &dc);
				insert_str_data(op_source2, data_arr, &dc);
			}
			else if(line_command == STRING)
			{
				insert_str_data(op_source1, data_arr, &dc);
			}
			else if(line_command == DATA)
			{
				insert_num_data(op_source1, data_arr, &dc);
			}
			
			printf("the dc array is:\n");
			print_dc_arr(data_arr, dc);
		}
		else if(line_type == COMMAND && status == SUCCESS)
		{
			encode_first_word(instruct_arr, &ic, line_command, op_sour_address_methood,
							  op_des_address_methood);
			
			printf("the ic array is:\n");
			print_ic_arr(instruct_arr, ic);
		}
		
		
		if(status == SUCCESS)/*free operands allocation*/
		{
			free(op_source1);
			free(op_source2);
			free(op_dest1);
			free(op_dest2);
		}
		
		ptr = ptr->next;
		line_num++;
		word_num = 0;
	}
#ifdef PRINT_DEBUG
	print_lable_table(label_table, num_of_label);
#endif /* PRINT_DEBUG */
	
	free(label_table);/*free label table allocation*/
	
	return status;
}

int label_count(list* main_list)
{
	node* ptr= main_list->head;
	char str[LINE_SIZE];
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
	label* label_table = NULL;
	
	*num_of_label = label_count(main_list);
	if(*num_of_label > 0)
	{
		label_table = (label*)malloc((*num_of_label) * sizeof(label));
	
		if(label_table == NULL)
		{
			printf("memory allocation failed\n");
			return label_table;
		}
		memset(label_table, 0, (*num_of_label) * sizeof(label));
	}

#ifdef PRINT_DEBUG
	printf("number of labels in this file is: %d\n", *num_of_label);
#endif /* PRINT_DEBUG */

	return label_table;
} 

/***********************************************/

/*==============================================
check the legality and parse an int from a string
===============================================*/
int parse_int(char str[], int* out_num)
{
	int status = SUCCESS;
	int length = strlen(str);
	int i = 0;
	printf("the num string is: %s\n", str);
	
	if(str[0] == '-' || str[0] == '+')
	{
		if(length > 1)
			i++;
		else
			status = FAILURE;
	}	
	
	
	for(; i < length; i++)
	{
		if(isdigit(str[i]) == 0)
		{
			status = FAILURE;
			break;
		}
	}
	
	if((status == SUCCESS) && (out_num != NULL))
		*out_num = atoi(str);
	
	return status;
}

int insert_num_data(char op_str[], int data_arr[], int* dc)
{
	int status = SUCCESS;
	char data[LINE_SIZE];
	char* num_token;
	
	strcpy(data, op_str);
	num_token = strtok(data, ",");
	
	while(num_token)
	{
		status = parse_int(num_token, /*(int*)*/&data_arr[*dc]);
		
		if(status == FAILURE)
			break;
		
		data_arr[*dc] = data_arr[*dc] & BITS_RESET;/*reset the 11th bit and on to zero*/
		*dc = *dc + 1;
		num_token = strtok(NULL, ",");
	}
	
	return status;
}

void insert_str_data(char str[], int data_arr[], int* dc)
{
	int length = strlen(str);
	int i;
	
	for(i = 0; i < length; i++)
	{
		data_arr[*dc] = str[i];
		*dc = *dc + 1;
	}
	data_arr[*dc] = (int)'\0';/*null terminatior at the and of the string*/
	*dc = *dc + 1;
}

void encode_first_word(int instruct_arr[], int* ic, int command, int source_address_methood,
					   int dest_address_methood)
{
	int opcode;
	int source;
	int dest;
	int final_word;
	
	if(num_of_operands(command) == 2)
	{
		opcode	= (command << 6);
		source	= (source_address_methood << 4);
		dest	= (dest_address_methood << 2);
		
		final_word = opcode | source | dest;
	}
	else if(num_of_operands(command) == 1)
	{
		opcode = (command << 6);
		dest   = (dest_address_methood << 2);
		
		final_word = opcode | dest;
	}
	else
	{
		final_word = (command << 6);/*all other bits are empty*/
	}
	
	printf("the final word is:\n");
	print_in_bits(final_word, WORD_SIZE);
	printf("\n");
	
	instruct_arr[*ic] = final_word;
	*ic = *ic + 1;
	
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
	
	length = sizeof(registers)/sizeof(registers[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(label_name, registers[i]) == 0)
		{
			if(strcmp(label_name, registers[i]) == 0)
			is_saved_word = TRUE;
		}
	}
		
	return is_saved_word;
}

int label_in_table (label label_table[], int num_of_label, char label_name[])
{
	int in_table = NOT_FOUND;
	int i;
	
	for(i = 0; i < num_of_label; i++)
	{
		if(strcmp(label_table[i].name, label_name) == 0)
		{
			in_table = i;
			break;
		}	
	}
	
#ifdef PRINT_DEBUG
	printf("label (%s) in table\n", (in_table != NOT_FOUND) ? "found" : "NOT found");
#endif /* PRINT_DEBUG */	
	
	return in_table;
}

int is_command(char expression[])
{
	int i;
	int is_command = FALSE;
	int length = sizeof(commands)/sizeof(commands[0]);
	
	
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

int is_register(char expression[])
{
	int i;
	int is_register = FALSE;
	int length = sizeof(registers)/sizeof(registers[0]);;
	
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(registers[i], expression) == 0)
			is_register = TRUE;	
	}
	return is_register;
}

int find_command(char command_name[])
{
	int i;
	int command_num = NOT_FOUND;
	int length = sizeof(commands)/sizeof(commands[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(command_name, commands[i]) == 0)
		{
			command_num = i;
			break;
		}
	}
	
	return command_num;
}

int find_directive(char directive_name[])
{
	int i;
	int directive_num = NOT_FOUND;
	int length = sizeof(directives)/sizeof(directives[0]);
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(directive_name, directives[i]) == 0)
		{
			directive_num = i;
			break;
		}
	}
	
	return directive_num;
}

int num_of_operands(int command)
{
	int num_of_operands = NOT_FOUND;
	switch(command)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 6:
			num_of_operands = 2;
			break;
		
		case 4:
		case 5:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			num_of_operands = 1;
			break;
		case 14:
		case 15:
			num_of_operands = 0;
			break;
	}
#ifdef PRINT_DEBUG
	printf("num of opernds is: %d\n", num_of_operands);
#endif /* PRINT_DEBUG */

	return num_of_operands;
}

/*==============================================
missing
===============================================*/
int legal_data_direct(char data_str[], int line_num)
{
	int legal_data = TRUE;
	int length = strlen(data_str);
	int i;

	if(data_str[0] == ',')
	{
		printf("Erorr in line (%d), .data directive may not start with comma\n", line_num);
		legal_data = FALSE;
	}
	
	for(i = 0; i < length; i++)
	{
		if((isdigit(data_str[i]) == 0) && (data_str[i] != ',') && (data_str[i] != '-') && (data_str[i] !='+'))
		{
			printf("Erorr in line (%d), .data directive may not get this (%c)\n", line_num, data_str[i]);
			legal_data = FALSE;
			break;
		}
		else if(((data_str[i] == ',') || (data_str[i] == '-') || (data_str[i] == '+')) && (i > 0))
		{
			if(data_str[i-1] == data_str[i])
			{
				printf("Erorr in line (%d), double %c in .data directive\n", line_num, data_str[i]);
				legal_data = FALSE;
				break;
			}
		}
	}
	return legal_data;
	
}

/*==============================================
missing
===============================================*/
int legal_str_direct(char str_str[], int line_num)
{
	int legal_str = TRUE;
	int length = strlen(str_str);
	int i;
	
	if(str_str[0] != '"' || str_str[length - 1] != '"')
	{
		printf("Erorr in line (%d), string must start or end with \"\n", line_num);
		legal_str = FALSE;
	}
	for(i = 1; i < length - 1; i++)
	{
		if((str_str[i] < 32) || (str_str[i] > 126))/*illegal ASCII value*/
		{
			printf("Erorr in line (%d), illegal value in .string directive\n", line_num);
			legal_str = FALSE;
			break;
		}
	}
	return legal_str;
}

/*==============================================
missing
===============================================*/
int legal_struct_direct(char struct_str[], int line_num)
{
	int legal_struct = TRUE;
	int number;
	char string[LINE_SIZE] = {0};
	int scan_status;
	
	scan_status = sscanf(struct_str, "%d,%[^\n]", &number, string);
	if(scan_status == 0)
	{
		printf("Erorr in line (%d), .struct directive must start with an int\n", line_num);
		legal_struct = FALSE;
	}
	else if(legal_str_direct(string, line_num) == FALSE)
	{
		legal_struct = FALSE;
	}
	
	return legal_struct;
	
}

int legal_directive(int directive_type, char data[], int line_num)
{
	int legal = TRUE;
	switch(directive_type)
	{
		case DATA:
			legal = legal_data_direct(data, line_num);
			break;
			
		case STRING:
			legal = legal_str_direct(data, line_num);
			break;
			
		case STRUCT:
			legal = legal_struct_direct(data, line_num);
			break;
	}
	return legal;
}


/*==============================================
missing
===============================================*/
int legal_struct_name(char struct_name[], int line_num)
{
	int legal_name = TRUE;
	char label_part[LABLE_LENGTH] = {0};
	char num_part[LABLE_LENGTH] = {0};
	int num;
	int scan_status;
	
	scan_status = sscanf(struct_name,"%[^.].%[^\n]", label_part, num_part);
	printf("struct name (%s) scan_status is: %d\n", struct_name, scan_status);
	if(scan_status == 2)
	{
		if(isalpha(label_part[0]) == 0)/*struct must start with a character*/
		{
			printf("Error in line (%d): illegal struct name (%s) must start with letter\n", line_num, struct_name);
			legal_name = FALSE;
		}
		else 
		{
			if(parse_int(num_part, &num) == FAILURE)
			{
				printf("Error in line (%d): illegal struct name (%s) must have int after dot\n", line_num, struct_name);
				legal_name = FALSE;
			}
		}
	}
	else /*struct name must have label part and a number part*/
	{
		printf("Error in line (%d): illegal struct name (%s)\n", line_num, struct_name);
		legal_name = FALSE;
	}
	
	return legal_name;
}

/*==============================================
missing
===============================================*/
int legal_int_operand(char operand[], int* out_num, int line_num)
{
	int legal_name = TRUE;
	int status;
	
	if(operand[1] == '-' || operand[1] == '+')
		status = parse_int(&operand[2], out_num);
	else
		status = parse_int(&operand[1], out_num);
	
	if(status == FAILURE)
	{
		printf("Error in line (%d): illegal operand (%s)\n", line_num, operand);
		legal_name = FALSE;
	}
	return legal_name;
}

/*==============================================
missing
===============================================*/
int find_address_methood(char operand[], int* address_methood, int command, int op_direction,
						 int forbidden_command, int line_num)
{
	int status = FAILURE;
	
	do
	{
		if(operand[0] == '#')
		{
			if(legal_int_operand(operand, NULL, line_num) == FALSE)
				break;
			else if(((op_direction == SOURCE) && (command == forbidden_command)) ||
					((op_direction == DEST) && (command != forbidden_command)))
			{
				printf("Error in line (%d): command cannot get a number operand (%s)\n",line_num, operand);
				break;
			}
			
			*address_methood = IMMEDIATE;
		}
		else if(strchr(operand, '.') != NULL)
		{
			if(legal_struct_name(operand, line_num) == FALSE)
				break;
			
			*address_methood = STRUCT_ACCESS;
		}
		else if(is_register(operand) == TRUE)
		{
			*address_methood = REGISTER;
		}
		else if(legal_label(operand, line_num) == TRUE)
			*address_methood = DIRECT;
		
		status = SUCCESS;
		
	}while(0);
	
	return status;
}

int parse_line(char data[], label* label_table,int* out_label_index, int* label_count, int* line_type,
			   int* line_command,char** out_op_source1, char** out_op_source2, char** out_op_dest1,
			   char** out_op_dest2, int* op_s_address_methood, int* op_d_address_methood, int* word_num,
			   int line_num)
{
	char label_name [LINE_SIZE] = {0};
	int label_index;
	char command [LINE_SIZE] = {0};
	char instruction [LINE_SIZE] = {0};
	int command_scan_status;/*store if the command has operands or not*/
	
	int parse_operands = FALSE; /*a flag to notife that the line has opernds to parse*/
	char operands [LINE_SIZE] = {0};
	char op_source [LINE_SIZE] = {0};
	char op_source1 [LINE_SIZE] = {0};
	char op_source2 [LINE_SIZE] = {0};
	char op_dest [LINE_SIZE] = {0};
	char op_dest1 [LINE_SIZE] = {0};
	char op_dest2 [LINE_SIZE] = {0};
	
	int status = FAILURE;
	
	do
	{
		printf("-------------------\n");
		printf("line: %s\n", data);
		
		/*initiate operands to null*/
		*out_op_source1 = NULL;
		*out_op_source2 = NULL;
		*out_op_dest1 = NULL;
		*out_op_dest2 = NULL;
		
		/*initiate line variables*/
		*out_label_index = NOT_FOUND;
		*line_type = NOT_FOUND;
		*line_command = NOT_FOUND;
		*op_s_address_methood = NOT_FOUND;
		*op_d_address_methood = NOT_FOUND;
		*word_num = 0;
		
		if(sscanf(data, ".entry %[^\n]", label_name) == 1)
		{
			printf("this is an entry line. label name is: %s\n", label_name);
			if(legal_label(label_name, line_num) == FALSE)
				break;

			label_index = label_in_table(label_table, *label_count, label_name);
			if(label_index != NOT_FOUND)/*label already in table*/
			{
				if(label_table[label_index].is_entry == TRUE)
				{
					printf("Error in line (%d): label (%s) was defined as entry befor\n", line_num, label_name);
					break;
				}
				else if(label_table[label_index].is_extern == TRUE)
				{
					printf("Error in line (%d): label (%s) was defined as extern cannot be entry\n", line_num, label_name);
					break;
				}
				label_table[label_index].is_entry = TRUE;
			}
			else
			{
				strcpy(label_table[*label_count].name, label_name);
				label_table[*label_count].is_entry = TRUE;
				*label_count = *label_count + 1;
			}
			*line_type = DIRECTIVE;
		}
		else if(sscanf(data, ".extern %[^\n]", label_name) == 1)
		{
			printf("this is an extern line. label name is: %s\n", label_name);
			if(legal_label(label_name, line_num) == FALSE)
				break;
			
			label_index = label_in_table(label_table, *label_count, label_name);
			if(label_index != NOT_FOUND)
			{
				if(label_table[label_index].is_entry == TRUE)
				{
					printf("Error in line (%d): label (%s) was defined as entry cannot be extern\n", line_num, label_name);
					break;
				}
				else if(label_table[label_index].is_extern == TRUE)
				{
					printf("Error in line (%d): label (%s) was defined as extern befor\n", line_num, label_name);
					break;
				}
			}
			
			strcpy(label_table[*label_count].name, label_name);
			label_table[*label_count].is_extern = TRUE;
			*label_count = *label_count + 1;
			*line_type = DIRECTIVE;
		}
		else 
		{
			if(sscanf(data, "%[^:]: %[^\n]", label_name, instruction) == 2)
			{
				printf("this is an instruction line with a label, label name is: "
					   "%s\n instruction line is: %s\n", label_name, instruction);
				
				label_index = label_in_table(label_table, *label_count, label_name);
				
				if(legal_label(label_name, line_num) == FALSE)
					break;
				else if(label_index != NOT_FOUND)/*label already in table*/
				{
					if(label_table[label_index].is_defined == TRUE)/*label already defined*/
					{
						printf("Error in line (%d): label (%s) was defined befor\n", line_num, label_name);
						break;
					}
					else if(label_table[label_index].is_extern == TRUE)
					{
						printf("Error in line (%d): label (%s) is extern, cannot be define in this file\n", line_num, label_name);
						break;
					}
					label_table[label_index].is_defined = TRUE;
					*out_label_index = label_index;
				}
				else
				{
					strcpy(label_table[*label_count].name, label_name); /*copy the label name in to it's place*/
					label_table[*label_count].is_defined = TRUE;
					*out_label_index = *label_count;
					*label_count = *label_count + 1;
				}
			}
			else
			{
				strcpy(instruction,data); /*the whole line is an instruction line*/
#ifdef PRINT_DEBUG
				printf("this is an instruction line with no label,\ninstruction line is: %s\n", instruction);
#endif /* PRINT_DEBUG */
			}
			
			/*parse the line command or directive*/
			command_scan_status = sscanf(instruction, "%s %[^\n]", command, operands);
			
			/*debug check*/
			if(command_scan_status == EOF)
			{
				printf("WORRNING!! this should never happen... command_scan_status return EOF\n");
			}
			/*debug check*/
			
			if(command_scan_status == 2)
			{
				if(is_directive(command) == TRUE)
				{
					printf("this is a directive line, diractive is: %s\n", command);
					*line_command = find_directive(command);
					*line_type = DIRECTIVE;
					
					if(legal_directive(*line_command, operands, line_num) == FALSE)
						break;
					
					if(*line_command == STRUCT)
					{
						sscanf(operands, "%[^,],\"%[^\"]\"", op_source1, op_source2);
						
						*out_op_source1 = (char*)malloc((strlen(op_source1) + 1) * sizeof(char));
						strcpy(*out_op_source1, op_source1);
						*out_op_source2 = (char*)malloc((strlen(op_source2) + 1) * sizeof(char));
						strcpy(*out_op_source2, op_source2);
						printf("the data stored for this directive is: (%s) (%s)\n",
							   *out_op_source1, *out_op_source2);
					}
					else 
					{						
						if(*line_command == STRING)/*this is a string directive*/
						{
							sscanf(operands, "\"%[^\"]\"", operands);
							if(strcmp(operands, "\"\"") == 0)
							{
								printf("Error in line (%d): illegal string\n", line_num);
								break;
							}
						}
						
						*out_op_source1 = (char*)malloc((strlen(operands) + 1) * sizeof(char));
						strcpy(*out_op_source1, operands);
						printf("the data stored for this directive is: %s\n", *out_op_source1);
					}
				}
				else if(is_command(command) == FALSE)/*illegal command*/
				{
					printf("Error in line (%d): illegal command (%s)\n", line_num, command);
					break;
				}
				else if((find_command(command) == RTS) || (find_command(command) == HLT))
				{
					printf("Error in line (%d): command (%s) may not have operands\n", line_num, command);
					break;
				}
				else
				{
					*line_command = find_command(command);
					*line_type = COMMAND;
					parse_operands = TRUE;
					*word_num = *word_num + 1;
					printf("op_flag is: %d, line command number: %d and operands: %s\n", parse_operands, *line_command, operands);
				}
			}
			else if(command_scan_status == 1)
			{
				if(is_directive(command) == TRUE)
				{
					printf("Error in line (%d): directive (%s) have no data\n", line_num, command);
					break;
				}
				else if(is_command(command) == FALSE)/*illegal command*/
				{
					printf("Error in line (%d): illegal command (%s)\n", line_num, command);
					break;
				}
				else if(find_command(command) < RTS)
				{
					printf("Error in line (%d): command (%s) dose not have operands\n", line_num, command);
					break;
				}
				else
				{
					*line_command = find_command(command);
					*line_type = COMMAND;
					*word_num = *word_num + 1;
					printf("line command number is: %d\n", *line_command);
				}
			}
			
			/*parse operands*/
			if(parse_operands == TRUE)
			{
				if(num_of_operands(*line_command) == 2)/*command that gets two operands*/
				{
					if(sscanf(operands, "%[^,],%[^\n]", op_source, op_dest) == 2)
					{
						/*find source operand addressing methood*/
						find_address_methood(op_source, op_s_address_methood, SOURCE, *line_command,
											 LEA, line_num);
						
						if(*op_s_address_methood == STRUCT_ACCESS)
						{
							/*store the struct two components*/
							sscanf(op_source,"%[^.].%[^\n]", op_source1, op_source2);
	
							*out_op_source1 = (char*)malloc((strlen(op_source1) + 1) * sizeof(char));
							strcpy(*out_op_source1, op_source1);
							*out_op_source2 = (char*)malloc((strlen(op_source2) + 1) * sizeof(char));
							strcpy(*out_op_source2, op_source2);
							*word_num = *word_num + 2;/*add address to struct name and location index*/
							printf("line first operand is: %s.%s\n", *out_op_source1, *out_op_source2);
						}
						else 
						{
							if(*op_s_address_methood == IMMEDIATE)
								sscanf(op_source, "#%[^\n]", op_source);
							
							*out_op_source1 = (char*)malloc((strlen(op_source) + 1) * sizeof(char));
							strcpy(*out_op_source1, op_source);
							*word_num = *word_num + 1;
							printf("line first operand is: %s\n", *out_op_source1);
						}
						
						/*find destination operand addressing methood*/
						find_address_methood(op_dest, op_d_address_methood, DEST, *line_command,
											 CMP, line_num);
						
						if(*op_d_address_methood == STRUCT_ACCESS)
						{
							/*store the struct two components*/
							sscanf(op_dest,"%[^.].%[^\n]", op_dest1, op_dest2);

							*out_op_dest1 = (char*)malloc((strlen(op_dest1) + 1) * sizeof(char));
							strcpy(*out_op_dest1, op_dest1);
							*out_op_dest2 = (char*)malloc((strlen(op_dest2) + 1) * sizeof(char));
							strcpy(*out_op_dest2, op_dest2);
							*word_num = *word_num + 2;/*add address to struct name and location index*/
							printf("line second operand is: %s.%s\n", *out_op_dest1, *out_op_dest2);
						}
						else 
						{
							if(*op_d_address_methood == IMMEDIATE)
								sscanf(op_dest, "#%[^\n]", op_dest);
							
							*out_op_dest1 = (char*)malloc((strlen(op_dest) + 1) * sizeof(char));
							strcpy(*out_op_dest1, op_dest);
							*word_num = *word_num + 1;
							printf("line second operand is: %s\n", *out_op_dest1);
						}
					}
					else/*the command get less than two operands*/
					{
						printf("Error in line (%d): command (%s) is missing an operand\n", line_num, command);
						break;
					}
				}
				else if(num_of_operands(*line_command) == 1)/*command get one operand*/
				{
					strcpy(op_dest, operands);
					
					/*find destination operand addressing methood*/
					find_address_methood(op_dest, op_d_address_methood, DEST, *line_command,
										 PRN, line_num);
					
					if(*op_d_address_methood == STRUCT_ACCESS)
					{
						/*store the struct two components*/
						sscanf(op_dest,"%[^.].%[^\n]", op_dest1, op_dest2);

						*out_op_dest1 = (char*)malloc((strlen(op_dest1) + 1) * sizeof(char));
						strcpy(*out_op_dest1, op_dest1);
						*out_op_dest2 = (char*)malloc((strlen(op_dest2) + 1) * sizeof(char));
						strcpy(*out_op_dest2, op_dest2);
						*word_num = *word_num + 2;/*add address to struct name and location index*/
						printf("line second operand is: %s.%s\n", *out_op_dest1, *out_op_dest2);
					}
					else 
					{
						if(*op_d_address_methood == IMMEDIATE)
							sscanf(op_dest, "#%[^\n]", op_dest);
						
						*out_op_dest1 = (char*)malloc((strlen(op_dest) + 1) * sizeof(char));
						strcpy(*out_op_dest1, op_dest);
						*word_num = *word_num + 1;
						printf("line second operand is: %s\n", *out_op_dest1);
					}
				}
			}
			
		}	
		status = SUCCESS;
		
	} while(0);
	
	if(status == FAILURE)/*in case the function allocated memory befor reaching to an error*/
	{
		free(*out_op_source1);
		free(*out_op_source2);
		free(*out_op_dest1);
		free(*out_op_dest2);
	}
	
	return status;
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

void print_dc_arr(int array[], int dc)
{
	int i;
	
	for(i = 0; i< dc; i++)
	{
		printf("%d,", array[i]);
	}
	printf("\n");
	
	for(i = 0; i< dc; i++)
	{
		print_in_bits(array[i], WORD_SIZE);
		printf("\n");
	}
}

void print_ic_arr(int array[], int ic)
{
	int i;
	
	for(i = 100; i< ic; i++)
	{
		printf("%d,", array[i]);
	}
	printf("\n");
	
	for(i = 100; i< ic; i++)
	{
		print_in_bits(array[i], WORD_SIZE);
		printf("\n");
	}
}

void print_in_bits(int num, int num_of_bits)
{
	if(num_of_bits == 0)
		return;
	
	if(num & 1)
	{
		print_in_bits(num >> 1, num_of_bits-1);
		printf("1");
	}
	else
	{
		print_in_bits(num >> 1, num_of_bits-1);
		printf("0");
	}
	
}