#ifndef _SECOND_PASS_H
#define _SECOND_PASS_H

/*==============================================
this function handles the second pass of the .am file
it fills the missing labels addresses, and compute the output .ob, .en, .ext files
returns SUCCESS if the files created or FAILURE otherways
===============================================*/
int second_pass_handler(int instruct_arr[], int ic, int data_arr[], int dc, label* label_table,
					    int label_count, char** label_by_index, char file_name[]);

#endif /*_SECOND_PASS_H*/