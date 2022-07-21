#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"
#include "common.h"


/*
************************************** Implementation **************************************
*/

void init(list* main_list)
{
	main_list->head = NULL;
	main_list->tail = NULL;
}

void add_node(list* main_list, char data[])
{
	node* ptr;
	
	/*create node*/
	ptr = (node*)malloc(sizeof(node));
	if(!ptr)
	{
		printf("memory allocation failed");
		return;
	}
	
	strcpy(ptr->data,data);
	ptr->next = NULL;
	ptr->prev = NULL;
	
	/*place node*/
	if(main_list->head == NULL)
	{
		main_list->head = ptr;
		main_list->tail = ptr;	
	}
	else
	{
		main_list->tail->next = ptr;
		ptr->prev = main_list->tail;
		main_list->tail = ptr;
	}
}


void add_middle_node(list* main_list, node* some_node, char data[])
{
	node* new_ptr;
	
	/*create node*/
	new_ptr = (node*)malloc(sizeof(node));
	if(!new_ptr)
	{
		printf("memory allocation failed");
		return;
	}
	
	strcpy(new_ptr->data,data);
	
	if(some_node == main_list->tail)
		main_list->tail = new_ptr;
		
	new_ptr->prev = some_node;
	new_ptr->next = some_node->next;
	
	some_node->next->prev = new_ptr;
	some_node->next = new_ptr;
	
}

void print_list(list* main_list)
{
	node* ptr;
	ptr = main_list->head;
	while(ptr != NULL)
	{
		printf("%.*s\n", ARRAY_SIZE, ptr->data);
		ptr = ptr->next;
	}
}

void delete_node (list * main_list, node* some_node)
{
	/*the node to delete isn't in the list*/
	if(some_node == NULL)
		return;
		
	/*if the node to be delete is the head of the list*/
	if(main_list->head != NULL && some_node == main_list->head)
	{
		if(main_list->head == main_list->tail)
			main_list->tail = NULL;
		some_node->next->prev = NULL;
		main_list->head = some_node->next;
		free(some_node);
		return;
	}
	else if(some_node == main_list->tail)
		main_list->tail = some_node->prev;
	else	
		some_node->next->prev = some_node->prev;
		
	some_node->prev->next = some_node->next;
	free(some_node);
}

int is_list_empty(list* main_list)
{
	int empty = FALSE;
	if(main_list->head == NULL)
		empty = TRUE;
		
	return empty;
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

void terminate(list* main_list)
{
	node* ptr;
	node* head_ptr;
	head_ptr = main_list->head;
	
	while(head_ptr)
	{
		ptr = head_ptr;
		head_ptr = head_ptr->next;
		free(ptr);
	}
	
	main_list->head = NULL;
	main_list->tail = NULL;
	
}

