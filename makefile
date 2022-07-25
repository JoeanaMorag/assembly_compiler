assembly_compiler: main.o macro.o linked_list.o first_pass.o
	gcc -ansi -Wall -pedantic -g main.o macro.o linked_list.o first_pass.o -o assembly_compiler

main.o: main.c common.h linked_list.h macro.h 
	gcc -c -ansi -Wall -pedantic -g main.c -o main.o

macro.o: macro.c common.h linked_list.h macro.h
	gcc -c -ansi -Wall -pedantic -g macro.c -o macro.o

linked_list.o: linked_list.c common.h linked_list.h
	gcc -c -ansi -Wall -pedantic -g linked_list.c -o linked_list.o
	
first_pass.o: first_pass.c common.h linked_list.h
	gcc -c -ansi -Wall -pedantic -g first_pass.c -o first_pass.o
