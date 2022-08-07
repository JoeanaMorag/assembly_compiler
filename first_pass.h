#ifndef _FIRST_PASS_H
#define _FIRST_PASS_H

/*==============================================
this function handles the first pass on the .am file
it stores the lables into the lable array and incode varbles in direct and register addressing methoods
it gets a pointer to a list which store the file text
returns SUCCESS if no errors were found or FAILURE otherways
===============================================*/
int first_pass_handel(list * main_list, int data_arr[], int instruct_arr[], int* dc, int* ic,
					  label** label_table, char*** label_by_index, int* label_count);

void free_label_by_index(char** label_by_index, int label_count);

#endif /*_FIRST_PASS_H*/