#include <stdio.h>
#include "first_scan.h"
#include "help_functions.h"
#include "tables.h"

#define LINE_MAX (81)
#define START_COMMENT_LINE ';'
extern int num_of_errors;

/*
 * scan_first reads the file at first,
 * it processes the information of instructions and symbol decelerations,
 * and stores the information in data structures designated for this sake.
 */
void scan_first(FILE * file){
    /*
     * pword - points to current word from file is being processed.
     * psymbol - points to current symbol is being processed, in case its a symbol word.
     */
    char line[LINE_MAX],* pword,*psymbol = NULL;
    guidance * gud;
    inst_data * p_inst_data;
    char * del = "\t \n";
    clear_all_tables();                        /* will clear all the data structure tables: it, dt, st and eu. */
    if (!file)return;
    num_of_errors = 0;                         /*set the error canter to zero.*/
    for (NLINE = 1;fgets(line, LINE_MAX, file);++NLINE,psymbol = NULL) {
        pword = strtok(line,del);
        if(!pword || *pword == START_COMMENT_LINE)           /* case empty line or comment line */
            continue;
        if (is_symbol(pword, YES)){           /* case first word is recognized as a symbol, read next word */
            psymbol = pword;
            if (!(pword = strtok(NULL, del))){ /*case no word attach to the symbol.*/
                print_error("Symbol without an instruction.");
                continue;
            }
        }
        /* case word is recognized as guiding word (.entry/.extern/.data/.string ).*/
        if ((gud = in_guidances(pword))){
            if (strcmp(gud->name,".entry") == 0)                   /* case '.entry' , postpone execution at second scan*/
                continue;
            if (strcmp(gud->name,".extern") != 0)                  /* case '.data' or '.string' store the symbol in the symbol table*/
                add_to_st(psymbol, get_size(dt), NO, INFO);     /*add the symbol as information to the symbol table.*/
            gud->foo();                                            /* restore guiding information by calling correlated function*/
            continue;
        }
        /* case first word isn't guiding word, nor legal command, hens its a new symbol*/
        add_to_st(psymbol, get_size(it), NO, INST);
        if (!(p_inst_data = is_instruction(pword))){
            print_error("Not legal instruction %s '%s'", psymbol ? "after symbol" : "",pword);
            continue;
        }
        add_instruction_table(*p_inst_data);                             /*restore instruction information */
    }
}
