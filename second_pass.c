#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "common.h"

/*==============================================
define the 32 base for the addresses and code translation
===============================================*/
char base32[32] = {'!', '@', '#', '$', '%', '^', '&', '*', '<', '>', 'a', 'b', 'c',
					'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
					'q', 'r', 's', 't', 'u', 'v'};

/*==============================================
this function check if there are entry labels in the file
===============================================*/
int is_there_entry(label* label_table, int label_count)
{
	int is_entry = FALSE;
	int i;
	
	for(i = 0; i < label_count; i++)
	{
		if(label_table[i].is_entry == TRUE)
				is_entry = TRUE;
			break;
	}
	return is_entry;
}

/*==============================================
this function check if the label is extern
===============================================*/
int is_this_extern(int index, char** label_by_index, label* label_table, int label_count)
{
	int is_extern = FALSE;
	int i;
	
	for(i = 0; i < label_count; i++)
	{
		if(strcmp(label_by_index[index], label_table[i].name) == 0)
		{
			if(label_table[i].is_extern == TRUE)
				is_extern = TRUE;
			break;
		}
	}
	return is_extern;
}

/*==============================================
this function retrive the missing label address
===============================================*/
int find_label_address(int index, char** label_by_index, label* label_table, int label_count)
{
	char name[LABLE_LENGTH] = {'0'};
	int address = -1;
	int i;
	
	strcpy(name, label_by_index[index]);
	
	for(i = 0; i < label_count; i++)
	{
		if(strcmp(name, label_table[i].name) == 0)
		{
			address = label_table[i].address;
			break;
		}
	}
	return address;
}

int extract_num_bits(int word, int start_bit)
{
	int num_bits;
	int bit_mask = 31;/*a mask of 11111 bits*/
	
	bit_mask = bit_mask << start_bit;
	num_bits = bit_mask & word;/*find the digit bits for the specified place in the number*/
	num_bits = num_bits >> start_bit; /*push the resulte back to the LSB*/
	
	return num_bits;
}

void num_convert_to_32(int word, char base32_num[])
{
	/*reads digits from left to right*/
	base32_num[0] = base32[extract_num_bits(word, 5)];
	base32_num[1] = base32[extract_num_bits(word, 0)];
	base32_num[2] = '\0';
}

/*==============================================
this function insert a new node to the extern list
===============================================*/
void insert_extern(list* extern_list, int address, char* label_name)
{
	char address_base32[3] = {'\0'};/*the 32 base number will contain no more than 2 digits*/
	char node_data[LINE_SIZE] = {'\0'};
	
	num_convert_to_32(address, address_base32);
	
	sprintf(node_data, "%s %s", label_name, address_base32);
	
	add_node(extern_list, node_data);
}

int print_ext_file(char* file_name, list* extern_list)
{
	FILE * fp;
	int status = SUCCESS;
	char * ext_file_name;
	node * ptr = extern_list->head;
	
	ext_file_name = file_name_suffix(file_name, EXTERNALS_FILE_SUFFIX);
	
	if(!(fp = fopen(ext_file_name,"w+")))
	{
		fprintf(stderr,"cannot open file (%s)\n", ext_file_name);
		status = FAILURE;
	}
	
	if(status == SUCCESS)
	{
		while(ptr != NULL)
		{
			fprintf(fp, "%s\n", ptr->data);
			ptr = ptr->next;
		}
	}
	free(ext_file_name);
	fclose(fp);
	return status;
}

int print_ent_file(char* file_name, label* label_table, int label_count)
{
	FILE * fp;
	int status = SUCCESS;
	char * ent_file_name;
	char address_base32[3] = {'\0'};/*the 32 base number will contain no more than 2 digits*/
	int i;
	
	ent_file_name = file_name_suffix(file_name, ENTRIES_FILE_SUFFIX);
	
	if(!(fp = fopen(ent_file_name,"w+")))
	{
		fprintf(stderr,"cannot open file (%s)\n", ent_file_name);
		status = FAILURE;
	}
	
	if(status == SUCCESS)
	{
		for(i = 0; i < label_count; i++)
		{
			if(label_table[i].is_entry == TRUE)
			{
				num_convert_to_32(label_table[i].address, address_base32);
				
				fprintf(fp, "%s %s\n", label_table[i].name, address_base32);
			}
		}
	}
	free(ent_file_name);
	fclose(fp);
	return status;
}

int print_ob_file(char* file_name, int instruct_arr[], int ic, int data_arr[], int dc)
{
	FILE * fp;
	int status = SUCCESS;
	
	char * ob_file_name;
	/*becouse our words have 10 bits, the 32 base number will contain no more than 2 digits*/
	char first_param[3] = {'\0'};
	char second_param[3] = {'\0'};
	int address = MEMORY_LODING_PLACE;
	int i;
	
	ob_file_name = file_name_suffix(file_name, OBJECT_FILE_SUFFIX);
	
	if(!(fp = fopen(ob_file_name,"w+")))
	{
		fprintf(stderr,"cannot open file (%s)\n", ob_file_name);
		status = FAILURE;
	}
	
	if(status == SUCCESS)
	{
		num_convert_to_32((ic - MEMORY_LODING_PLACE), first_param);
		num_convert_to_32(dc, second_param);
		
		fprintf(fp, "%s %s\n\n", first_param, second_param);/*print object file headline*/
		
		/*print instrutions image*/
		for(i = 100; i < ic; address++, i++)
		{
			num_convert_to_32(address, first_param);
			num_convert_to_32(instruct_arr[i], second_param);
			
			fprintf(fp, "%s %s\n", first_param, second_param);
		}
		
		/*print data image*/
		for(i = 0; i < dc; address++, i++)
		{
			num_convert_to_32(address, first_param);
			num_convert_to_32(data_arr[i], second_param);
			
			fprintf(fp, "%s %s\n", first_param, second_param);
		}
	}
	free(ob_file_name);
	fclose(fp);
	return status;
}

int second_pass_handler(int instruct_arr[], int ic, int data_arr[], int dc, label* label_table,
					    int label_count, char** label_by_index, char file_name[])
{
	int status = SUCCESS;
	int index_num;
	
	int entry_flag = FALSE;
	int extern_flag = FALSE;
	int is_extern;
	list extern_list;
	
	int code = -1;
	int i;
	
	init(&extern_list);
	
	for(i = MEMORY_LODING_PLACE; i < ic; i++)
	{
		if((instruct_arr[i] & 1) == 1)
		{
			index_num = instruct_arr[i] >> 2;
			
			is_extern = is_this_extern(index_num, label_by_index, label_table, label_count);
			if(is_extern == TRUE)
			{
				code = 1;
				insert_extern(&extern_list, i, label_by_index[index_num]);
				extern_flag = TRUE;
			}	
			else
			{
				code = find_label_address(index_num, label_by_index, label_table, label_count);
				
				if(code != -1)
				{
					code = code << 2;
					code = code | 2;
				}
			}
			
			instruct_arr[i] = code;
		}
	}
	
	entry_flag = is_there_entry(label_table, label_count);
	
	status = print_ob_file(file_name, instruct_arr, ic, data_arr, dc);
	
	if(status == SUCCESS && extern_flag == TRUE)
		status = print_ext_file(file_name,&extern_list);
	
	if(status == SUCCESS && entry_flag == TRUE)
		status = print_ent_file(file_name,label_table,label_count);
	
	terminate(&extern_list);
	return status;
}



