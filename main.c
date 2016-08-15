
#include <stdlib.h>
#include <stdio.h>
#include "first_scan.h"
#include "second_scan.h"
#include "create_files.h"
#include "help_functions.h"
#include <errno.h>

extern char * pfile;

/*
 * run_assembler function gets one or more files in assembly language,
 * and encodes the given assembly code to machine code.
 * this process has been executed by mainly tow stages,
 * first scan, and second scan and then create the files.
 */
static void run_assembler(char **files_names,int number_of_files);

/*
 * function calls run_assembler() to implement the assembly program on input files.
 * each input file, is encoded seperatly to different output file
 */
int main(int argc, char *argv[]) {
    run_assembler(argv,argc);/*run the assembler*/
    return EXIT_SUCCESS;
}

/*
 * run_assembler function gets one or more files in assembly language,
 * and encodes the given assembly code to machine code.
 * this process has been executed by mainly tow stages,
 * first scan, and second scan and then create the files.
 */
static void run_assembler(char **files_names,int number_of_files) {
    FILE *file;
    if (number_of_files == 1) {/*case no files entered to the program.*/
        fprintf(stdout, "No files entered to %s.\n", *files_names);
    }
    else {
        for (--number_of_files,++files_names;
             number_of_files > 0;
             --number_of_files,++files_names) {
            if ((file = fopen(*files_names , "r"))) {
                SET_ERROR_FILE_NAME(pfile,*files_names);/*set the file of errors to be the current file name.*/
                scan_first(file);           /*first scan*/
                scan_second(file);          /*second scan*/
                create_files(*files_names); /*create the outputs files of the current file.*/
                fclose(file);
            }/*case failed to open the file print an error.*/
            else {
                fprintf(stderr,"error: fail to open file \"%s\" because: %s\n", *files_names, strerror(errno));
            }
        }
    }
}