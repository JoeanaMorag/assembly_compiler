preprocessor: preprocessor.o linked_list.o
	gcc -ansi -Wall -g preprocessor.o linked_list.o -o preprocessor
preprocessor.o: preprocessor.c preprocessor.h
	gcc -c -ansi -Wall preprocessor.c -o preprocessor.o
linked_list.o: linked_list.c preprocessor.h
	gcc -c -ansi -Wall linked_list.c -o linked_list.o
