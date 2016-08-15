
#ifndef ASSEMBLER_FIRST_SCAN_H
#define ASSEMBLER_FIRST_SCAN_H

#include "tables.h"

/*
 * scan_first reads the file at first,
 * it processes the information of instructions and symbol decelerations,
 * and stores the information in data structures designated for this sake.
 */
void scan_first(FILE *);
#endif /*ASSEMBLER_FIRST_SCAN_H*/
