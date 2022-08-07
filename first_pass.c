#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "linked_list.h"
#include "common.h"

extern char * directives[5];
extern char * commands[16];
extern char * registers[8];

/*
************************************** Declerations **************************************
*/

/*==============================================
commands ordered by their opcode
===============================================*/
enum COMMANDS {MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC,
			   JMP, BNE, GET, PRN, JSR, RTS, HLT};

/*==============================================
directive instructions
===============================================*/
enum DIRECTIVES {DATA, STRING, STRUCT};

enum LINE_TYPES {COMMAND, DIRECTIVE};

enum LABEL_TYPES {DATA_TYPE, CODE_TYPE};

/*==============================================
addressing methood for operands
===============================================*/
enum ADDRESSING_METHOODS {IMMEDIATE, DIRECT, STRUCT_ACCESS, REGISTER};

enum OPERAND_DIRECTION {SOURCE, DEST};

/*==============================================
this function counts the number of labels in the code
===============================================*/
int label_count(list* main_list);

/*==============================================
this function builds the labels table
===============================================*/
label * build_label_table(list* main_list, int* label_count);

/*==============================================
this function builds an index array for the labels that needs to be encode in the second pass
===============================================*/
char** build_label_by_index(int label_count);

void free_label_by_index(char** label_by_index, int label_count);

/*==============================================
this function parse a given line of code and stores its different components
while looking for errors in the code
returns SUCCESS in case the parse complited with no errors, or FAILURE otherways
===============================================*/
int parse_line(char data[], label* label_table,int* out_label_index, int* label_count, int* line_type,
			   int* line_command,char** out_op_source1, char** out_op_source2, char** out_op_dest1,
			   char** out_op_dest2, int* op_s_address_methood, int* op_d_address_methood, int line_num);

int insert_num_data(char op_str[], int data_arr[], int* dc);

void insert_str_data(char str[], int data_arr[], int* dc);

void encode_first_word(int instruct_arr[], int* ic, int command, int source_address_methood,
					   int dest_address_methood);

void encode_operands(int instruct_arr[], int* ic, int source_address_methood, char* op_source1,
					 char* op_source2, int dest_address_methood, char* op_dest1, char* op_dest2,
					 char** label_by_index, int label_count);

int label_defin_check(label* label_table, int label_count);

void encode_first_word(int instruct_arr[], int* ic, int command, int source_address_methood,
					   int dest_address_methood);



/*
************************************** Implementation **************************************
*/

int first_pass_handel(list * main_list, int data_arr[], int instruct_arr[], int* dc, int* ic,
					  label** p_label_table, char*** label_by_index, int* label_count)
{
	int line_num = 1; /*code lines starts from 1*/
	char line[LINE_SIZE];
	label* label_table = NULL;
	
	node* ptr = main_list->head;
	
	int label_table_count = 0;
	int label_index;
	
	/*line components*/
	int line_type;
	int line_command;
	int op_sour_address_methood;
	int op_des_address_methood;
	
	/*operands*/
	char* op_source1 = NULL;
	char* op_source2 = NULL;
	char* op_dest1 = NULL;
	char* op_dest2 = NULL;
	
	int status = SUCCESS;
	int i;
	
	label_table = build_label_table(main_list, label_count);
	*label_by_index = build_label_by_index(*label_count);

#ifdef PRINT_DEBUG
	printf("test for label table first name after initiation: %s\n", label_table[0].name);
#endif /* PRINT_DEBUG */
	
	
	while(ptr != NULL)
	{
		strcpy(line, ptr->data);
		
		status = parse_line(line, label_table, &label_index, &label_table_count, &line_type, &line_command,
						    &op_source1, &op_source2, &op_dest1, &op_dest2, &op_sour_address_methood,
							&op_des_address_methood, line_num);

		if(line_type == DIRECTIVE && status == SUCCESS)
		{
			printf("label_index is: %d\n", label_index);
			/*update the data array*/
			if(label_index != NOT_FOUND)/*store the address for the label*/
			{
				label_table[label_index].address = *dc;
				label_table[label_index].label_type = DATA_TYPE;
			}
		
			if(line_command == STRUCT)
			{
				/*use op_source1 for num and op_source2 for string*/
				insert_num_data(op_source1, data_arr, dc);
				insert_str_data(op_source2, data_arr, dc);
			}
			else if(line_command == STRING)
			{
				insert_str_data(op_source1, data_arr, dc);
			}
			else if(line_command == DATA)
			{
				insert_num_data(op_source1, data_arr, dc);
			}
		}
		else if(line_type == COMMAND && status == SUCCESS)
		{
			/*update the data array*/
			if(label_index != NOT_FOUND)/*store the address for the label*/
			{
				label_table[label_index].address = *ic;
				label_table[label_index].label_type = CODE_TYPE;
			}
				
			encode_first_word(instruct_arr, ic, line_command, op_sour_address_methood,
							  op_des_address_methood);
			
			/*add number of words and encoding of registers and numbers*/
			encode_operands(instruct_arr, ic, op_sour_address_methood, op_source1, op_source2,
							op_des_address_methood, op_dest1, op_dest2, *label_by_index, *label_count);
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
	}
#ifdef PRINT_DEBUG_MILESTONE
	printf("the label_table in first pass is:\n");
	print_lable_table(label_table, *label_count);

	printf("the ic array is:\n");
	print_ic_arr(instruct_arr, *ic);

	printf("the dc array is:\n");
	print_dc_arr(data_arr, *dc);
#endif /* PRINT_DEBUG_MILESTONE */

	/*check that all the labels where define or had declared as extern*/
	status = label_defin_check(label_table, label_table_count);

	/*update data type labels with ic counter*/
	for(i = 0; i < *label_count; i++)
	{
		if(label_table[i].label_type == DATA_TYPE)
			label_table[i].address = label_table[i].address + *ic;
	}
	
	(*p_label_table) = label_table;
	
	return status;
}

int label_counter(list* main_list)
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

label * build_label_table(list* main_list, int* label_count)
{
	label* label_table = NULL;
	
	*label_count = label_counter(main_list);
	if(*label_count > 0)
	{
		label_table = (label*)malloc((*label_count) * sizeof(label));
	
		if(label_table == NULL)
		{
			printf("memory allocation failed\n");
			return label_table;
		}
		memset(label_table, 0, (*label_count) * sizeof(label));
	}

#ifdef PRINT_DEBUG
	printf("number of labels in this file is: %d\n", *label_count);
#endif /* PRINT_DEBUG */

	return label_table;
} 

char** build_label_by_index(int label_count)
{
	char** new_array = NULL;
	int i;
	
	new_array = (char**)malloc((label_count) * sizeof(char*));
	if(new_array == NULL)
	{
		printf("memory allocation failed\n");
		return new_array;
	}
	for(i = 0; i < label_count; i++)
	{
		new_array[i] = (char*)malloc((LABLE_LENGTH) * sizeof(char));
		if(new_array[i] == NULL)
		{
			printf("memory allocation failed\n");
			return new_array;
		}
		memset(new_array[i], 0, LABLE_LENGTH);
	}
	
	return new_array;
}

void free_label_by_index(char** label_by_index, int label_count)
{
	int i;
	for(i = 0; i < label_count; i++)
	{
		free(label_by_index[i]);
	}
}

int label_defin_check(label* label_table, int label_count)
{
	int check = SUCCESS;
	int i;
	
	for(i = 0; i < label_count; i++)
	{
		if(label_table[i].is_defined == FALSE && label_table[i].is_extern == FALSE)
		{
			printf("Error in file: missing label (%s) definiton\n", label_table[i].name);
			check = FAILURE;
		}
	}
	return check;
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

int is_register(char expression[])
{
	int i;
	int is_register = NOT_FOUND;
	int length = sizeof(registers)/sizeof(registers[0]);
	
	
	for(i = 0; i < length; i++)
	{
		if(strcmp(registers[i], expression) == 0)
			is_register = i;	
	}
	return is_register;
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

/*==============================================
this function checks if a label is legal
===============================================*/
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

/*==============================================
this function finds the number of the command acoording the COMMANDS enum
===============================================*/
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

/*==============================================
this function finds the number of the directive acoording the DIRECTIVES enum
===============================================*/
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

/*==============================================
check the legality and parse an int from a string
===============================================*/
int parse_int(char str[], int* out_num)
{
	int status = SUCCESS;
	int length = strlen(str);
	int i = 0;
	
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

/*==============================================
this function check if a label is already in the label table
===============================================*/
int label_in_table (label label_table[], int label_count, char label_name[])
{
	int in_table = NOT_FOUND;
	int i;
	
	for(i = 0; i < label_count; i++)
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

int legal_struct_name(char struct_name[], int line_num)
{
	int legal_name = TRUE;
	char label_part[LABLE_LENGTH] = {0};
	char num_part[LABLE_LENGTH] = {0};
	int num;
	int scan_status;
	
	scan_status = sscanf(struct_name,"%[^.].%[^\n]", label_part, num_part);
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
this function insert int data to the data array
===============================================*/
int insert_num_data(char op_str[], int data_arr[], int* dc)
{
	int status = SUCCESS;
	char data[LINE_SIZE];
	char* num_token;
	
	strcpy(data, op_str);
	num_token = strtok(data, ",");
	
	while(num_token)
	{
		status = parse_int(num_token, &data_arr[*dc]);
		
		if(status == FAILURE)
			break;
		
		data_arr[*dc] = data_arr[*dc] & BITS_RESET;/*reset the 11th bit and on to zero*/
		*dc = *dc + 1;
		num_token = strtok(NULL, ",");
	}
	
	return status;
}

/*==============================================
this function insert string data to the data array
===============================================*/
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

/*==============================================
this function find the expected number of operands for each command
===============================================*/
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
	
#ifdef PRINT_DEBUG
	printf("the final word is:\n");
	print_in_bits(final_word, WORD_SIZE);
	printf("\n");
#endif /* PRINT_DEBUG */	
	
	instruct_arr[*ic] = final_word;
	*ic = *ic + 1;
	
}

int find_label_index(char** label_by_index, char label_name[], int array_length)
{
	int label_index = NOT_FOUND;
	int i;

	for(i = 0; i < array_length; i++)
	{
		if(strcmp(label_by_index[i], label_name) == 0)
			label_index = i;
		else if(label_by_index[i][0] == '\0')
			break;
	}
	
	if(label_index == NOT_FOUND)
	{
		strcpy(label_by_index[i], label_name);
		label_index = i;
	}
	return label_index;
}

/*==============================================
this function encodes a place holder for the label to be encoded in second pass
using the label index from the label by index table
===============================================*/
void encode_miss_label(int instruct_arr[], int* ic, char** label_by_index, char label_name[], int arr_length)
{
	int label_index ;
	
	label_index = find_label_index(label_by_index, label_name, arr_length);
	
	if(label_index != NOT_FOUND)
	{
		label_index = label_index << 2;
		label_index = label_index | 1;
		
		instruct_arr[*ic] = label_index;
	}
}

void encode_operands(int instruct_arr[], int* ic, int source_address_methood, char* op_source1,
					 char* op_source2, int dest_address_methood, char* op_dest1, char* op_dest2,
					 char** label_by_index, int label_count)
{
	int dest;
	int source;
	int final_word;
#ifdef PRINT_DEBUG
	printf("source_address_methood is (%d)\n", source_address_methood);
	printf("dest_address_methood is (%d)\n", dest_address_methood);
#endif /* PRINT_DEBUG */
	
	if(source_address_methood == REGISTER && dest_address_methood == REGISTER)
	{
		source = is_register(op_source1);
		dest   = is_register(op_dest1);
		
		source = source << 6;
		dest   = dest   << 2;
		
		final_word = source | dest;
		
		instruct_arr[*ic] = final_word;
		*ic = *ic + 1;
	}
	else if(op_dest1 != NULL)/*the command has at least one operand*/
	{
		if(op_source1 != NULL)
		{
			if(source_address_methood == IMMEDIATE)
			{
				parse_int(op_source1, &source);
				source = source << 2;
				instruct_arr[*ic] = source;
			}
			else if(source_address_methood == REGISTER)
			{
				source = is_register(op_source1);
				source = source << 6;
				instruct_arr[*ic] = source;
			}
			else if(source_address_methood == STRUCT)
			{
				encode_miss_label(instruct_arr, ic, label_by_index, op_source1, label_count);
				*ic = *ic + 1;
				
				parse_int(op_source2, &source);/*encode the field number*/
				source = source << 2;
				instruct_arr[*ic] = source;
			}
			else
				encode_miss_label(instruct_arr, ic, label_by_index, op_source1, label_count);
			
			*ic = *ic + 1;
		}
		
		if(dest_address_methood == IMMEDIATE)
		{
			parse_int(op_dest1, &dest);
			dest = dest << 2;
			instruct_arr[*ic] = dest;
		}
		else if(dest_address_methood == REGISTER)
		{
			dest = is_register(op_dest1);
			dest = dest << 2;
			instruct_arr[*ic] = dest;
		}
		else if(dest_address_methood == STRUCT)
		{
			encode_miss_label(instruct_arr, ic, label_by_index, op_dest1, label_count);
			*ic = *ic + 1; /*save a spot to the label address*/
			
			parse_int(op_dest2, &instruct_arr[*ic]);/*encode the field number*/
		}
		else
			encode_miss_label(instruct_arr, ic, label_by_index, op_dest1, label_count);
		*ic = *ic + 1;
	}
}

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
		else if(is_register(operand) != NOT_FOUND)
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
			   char** out_op_dest2, int* op_s_address_methood, int* op_d_address_methood, int line_num)
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
#ifdef PRINT_DEBUG_MILESTONE
		printf("-------------------\n");
		printf("line: %s\n", data);
#endif /* PRINT_DEBUG_MILESTONE */
		
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
		
		if(sscanf(data, ".entry %[^\n]", label_name) == 1)
		{
#ifdef PRINT_DEBUG_MILESTONE
			printf("this is an entry line. label name is: %s\n", label_name);
#endif /* PRINT_DEBUG_MILESTONE */
			
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
#ifdef PRINT_DEBUG_MILESTONE
			printf("this is an extern line. label name is: %s\n", label_name);
#endif /* PRINT_DEBUG_MILESTONE */
			
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
#ifdef PRINT_DEBUG
				printf("this is an instruction line with a label, label name is: "
					   "%s\n instruction line is: %s\n", label_name, instruction);
#endif /* PRINT_DEBUG */

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
			
			if(command_scan_status == 2)
			{
				if(is_directive(command) == TRUE)
				{
#ifdef PRINT_DEBUG_MILESTONE
					printf("this is a directive line, diractive is: %s\n", command);
#endif /* PRINT_DEBUG_MILESTONE */
					
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
#ifdef PRINT_DEBUG
						printf("the data stored for this directive is: (%s) (%s)\n",
							   *out_op_source1, *out_op_source2);
#endif /* PRINT_DEBUG */
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
#ifdef PRINT_DEBUG
						printf("the data stored for this directive is: %s\n", *out_op_source1);
#endif /* PRINT_DEBUG */
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
#ifdef PRINT_DEBUG
					printf("op_flag is: %d, line command number: %d and operands: %s\n", parse_operands, *line_command, operands);
#endif /* PRINT_DEBUG */
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
#ifdef PRINT_DEBUG
					printf("line command number is: %d\n", *line_command);
#endif /* PRINT_DEBUG */
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
#ifdef PRINT_DEBUG
							printf("line first operand is: %s.%s\n", *out_op_source1, *out_op_source2);
#endif /* PRINT_DEBUG */
						}
						else 
						{
							if(*op_s_address_methood == IMMEDIATE)
								sscanf(op_source, "#%[^\n]", op_source);
							
							*out_op_source1 = (char*)malloc((strlen(op_source) + 1) * sizeof(char));
							strcpy(*out_op_source1, op_source);
#ifdef PRINT_DEBUG
							printf("line first operand is: %s\n", *out_op_source1);
#endif /* PRINT_DEBUG */
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
#ifdef PRINT_DEBUG
							printf("line second operand is: %s.%s\n", *out_op_dest1, *out_op_dest2);
#endif /* PRINT_DEBUG */
						}
						else 
						{
							if(*op_d_address_methood == IMMEDIATE)
								sscanf(op_dest, "#%[^\n]", op_dest);
							
							*out_op_dest1 = (char*)malloc((strlen(op_dest) + 1) * sizeof(char));
							strcpy(*out_op_dest1, op_dest);
#ifdef PRINT_DEBUG
							printf("line second operand is: %s\n", *out_op_dest1);
#endif /* PRINT_DEBUG */
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
#ifdef PRINT_DEBUG
						printf("line second operand is: %s.%s\n", *out_op_dest1, *out_op_dest2);
#endif /* PRINT_DEBUG */
					}
					else 
					{
						if(*op_d_address_methood == IMMEDIATE)
							sscanf(op_dest, "#%[^\n]", op_dest);
						
						*out_op_dest1 = (char*)malloc((strlen(op_dest) + 1) * sizeof(char));
						strcpy(*out_op_dest1, op_dest);
#ifdef PRINT_DEBUG
						printf("line second operand is: %s\n", *out_op_dest1);
#endif /* PRINT_DEBUG */
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

void print_lable_table(label label_table[], int label_count)
{
	int i;
	for (i = 0; i < label_count; i++)
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