
#ifndef ASSEMBLER_SECOND_SCAN_H
#define ASSEMBLER_SECOND_SCAN_H

#include <stdio.h>

#define DEL "\n \t,"
#define LINE_MAX 81

/*
 * reads the file secondly,
 * it processes the information regarding operands
 * linked to each instruction in the code,
 * and stores it beside after the instruction line information,
 * which was stored in first scan
 */
void scan_second(FILE * file);

typedef enum {SRC, DST}operand;
#endif /*ASSEMBLER_SECOND_SCAN_H*/