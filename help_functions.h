
#ifndef ASSEMBLER_HELP_FUNCTIONS_H
#define ASSEMBLER_HELP_FUNCTIONS_H

#include "tables.h"

#define BEGIN_STRING '"'
#define END_STRING '"'
#define END_OF_STRING (0)
#define SPACES " \t\n"

#define IMMEDIATE (0)
#define DIRECT (1)
#define DYNAMIC (2)
#define REG (3)

#define SET_ERROR_FILE_NAME(X,Y)  ((X) = (Y))
#define skipspce(X) for(;isspace(*(X));++(X))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define INITIAL_ADRS (100)

#define EXTERNAL_ADDRESS (0)
#define DECIMAL_NUMBER (10)
/* returns a pointer to the symbol, if the givven string is a symbol name */
symbol * in_st(const char *_symbol);
/* returns a pointer to the command, if the givven string is a command name */
guidance * in_guidances(const char * _name);
/* get the index of a table it,st and td where _name appears in the table*/
regist * is_reg(const char * _name);
/* returns a pointer to the action, if the givven string is an action name */inst_data * is_instruction(const char * _name);
int address_to_num(address _address);
/* prints an error to stderror */
void print_error(const char * fmt,...);
/*return YES/NO whether a string is a valid name of a symbol or not.*/
bool is_symbol(const char *_name, bool first);
/* return the symbol address.*/
int get_sy_address(const symbol *sym);
/*sign a symbol as entry.*/
result sign_entry();

/* add the instruction to the it*/
void add_instruction_table(const inst_data _instr);
/* add extern symbol to the extern symbol table (.string)*/
result add_extern();
/* adds number to the data table (.data)*/
result add_nums();
/* adds strings to the data table (.string)*/
result add_letters();
/* add a symbol to the symbol table.*/
/*count the line in the file that is read.*/
int NLINE;

#endif /*ASSEMBLER_HELP_FUNCTIONS_H*/
