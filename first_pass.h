#ifndef _FIRST_PASS_H
#define _FIRST_PASS_H

/*==============================================
this function handles the first pass on the .am file
it stores the lables into the lable array and incode varbles in direct and register addressing methoods
it gets a pointer to a list which store the file text
returns SUCCESS if no errors were found or FAILURE otherways
===============================================*/
int first_pass_handel(list * main_list);

#endif /*_FIRST_PASS_H*/