#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

/*===============================================
this function removes white spaces from a string
it gets a pointer to the string it needs to operate on
===============================================*/
void remove_white_spaces(char * str);

/*===============================================
this function clean the linked list from comments, white spaces or empty lines
it gets a pointer to the list it needs to clean
===============================================*/
void clean_list(list * main_list);

/*==============================================
this function initiate the list head and tail to null
it gets a pointer to the list which needs to be initiate
===============================================*/
void init(list* main_list);

/*==============================================
this function add a new node to the end of the linked list
it gets a pointer to the list to which it needs to add a node, 
and a data array which needs to fill the data field in the node
===============================================*/
void add_node(list* main_list, char data[]);

/*===============================================
this function add a new node to the middle of the linked list
it gets a pointer to the list to which it needs to add a node, 
a data array which needs to fill the data field in the node
and the node in the list which the new node should be after
===============================================*/
void add_middle_node(list* main_list, node* some_node, char data[]);

/*==============================================
this function prints the whole data fields linked list
it gets a pointer to the list it needs to print
===============================================*/
void print_list(list* main_list);

/*===============================================
this function check if a list is empty
it gets a pointer to the list it needs to check
returns TRUE if the list is empty or FALSE otherways
===============================================*/
int is_list_empty(list* main_list);

/*==============================================
this function delete the linked list and free the memory allocation
it gets a pinter to the list it needs to delete
===============================================*/
void terminate(list* main_list);

/*==============================================
this function delete a node from the list and free the memory allocation it got
it gets a pinter to the list and to the node it needs to delete
===============================================*/
void delete_node (list * main_list, node* some_node);

#endif /*_LINKED_LIST_H*/
