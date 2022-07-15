assembly_compiler: main.o macro.o linked_list.o
	gcc -ansi -Wall -pedantic main.o macro.o linked_list.o -o assembly_compiler

main.o: main.c common.h linked_list.h macro.h
	gcc -c -ansi -Wall -pedantic main.c -o main.o

macro.o: macro.c common.h linked_list.h macro.h
	gcc -c -ansi -Wall -pedantic macro.c -o macro.o

linked_list.o: linked_list.c common.h linked_list.h
	gcc -c -ansi -Wall -pedantic linked_list.c -o linked_list.o
