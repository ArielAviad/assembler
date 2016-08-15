all:
	gcc -ansi -Wall -pedantic main.c tables.c help_functions.c first_scan.c second_scan.c create_files.c -o assembler
