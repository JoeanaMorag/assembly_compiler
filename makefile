assembly_compiler: main.o macro.o linked_list.o first_pass.o
	gcc -ansi -Wall -pedantic main.o macro.o linked_list.o first_pass.o -o assembly_compiler

main.o: main.c common.h linked_list.h macro.h 
	gcc -c -ansi -Wall -pedantic main.c -o main.o

macro.o: macro.c common.h linked_list.h macro.h
	gcc -c -ansi -Wall -pedantic macro.c -o macro.o

linked_list.o: linked_list.c common.h linked_list.h
	gcc -c -ansi -Wall -pedantic linked_list.c -o linked_list.o
	
first_pass.o: first_pass.c common.h linked_list.h
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o
