
#ifndef ASSEMBLER_TABLES_H
#define ASSEMBLER_TABLES_H

#include <string.h>
#include <stdlib.h>

#define MAX_SYMBOL_NAME (30)
#define MAX_LENGTH_IT (1000)

typedef enum {fail,success}result;
typedef enum {immediate = 01, direct = 02, dynamic = 04,direct_register = 0X8}address;
typedef enum {ABSOLUTE,EXTERNAL,RELOCATABLE}ARE;
typedef enum {NO,YES}bool;
typedef enum {INST,INFO}LINE_KIND;

/* act_data struct contains 5 fields, represent valid combination between command-group-src addressing-dst addressing.
src_code & dst_code represent acceptable addressing methods according to lited bites in order 0-3 in the field, in binary code */
typedef struct{
    char * name;                /*name.*/
    unsigned int nuse:3;        /*un used bits.*/
    unsigned int group:2;       /*how many words for the instruction.*/
    unsigned int opcode:4;      /*what the opcode.*/
    unsigned int src_code:4;    /*what legal src code are legal.*/
    unsigned int dis_code:4;    /*what legal dis_code are legal.*/
}inst_data;

/* register struct,  represents registers, witch contains his ordinal number and his name */
typedef struct {
    char * name;
    unsigned int code:6;
}regist;

/*
 * guidance struct, witch contains the guiding word string,
 * and pointer to the correlate function
 */
typedef struct{
    const char *name;
    result (*foo)();
}guidance;

/* 'instruction' union represents all types of instruction line code, witch appear in instruction table */
typedef union {
    struct {/*instruction*/
        unsigned int ERA:2;
        unsigned int dst_op:2;
        unsigned int src_op:2;
        unsigned int opcode:4;
        unsigned int group:2;
        unsigned int notuse:3;
    }inst;
    struct {
        unsigned int era:2;
        unsigned int dst_rgs:6;
        unsigned int src_rgs:6;
    }rgs_line; /* operand collector */
    struct {
        unsigned int era:2;
        unsigned int address:13;
    }addrs_line; /* operand adress */
    struct {
        unsigned int era:2;
        int num:13;
    }number;  /* immidiate operand */
    unsigned int hole:15;/*all of the bits at once.*/
}it_line;

/* symbol struct represents a word in the symbol table.*/
typedef struct {
    char name[MAX_SYMBOL_NAME + 1];     /* name */
    bool external;                      /* external y/n */
    LINE_KIND act_or_inf;               /* is instruction or guide */
    bool is_entry;                      /* entry y/n */
    unsigned int address:13;/*address*/
}symbol;

/* data word, represent an int number for 'data' or char for 'string' */
typedef union {
    unsigned int num:15;
}data;

/*data table, holds an array of data, and current is an index of the one currently dealing with*/
struct {
    size_t size;
    data * data_info;
}dt;

/*instruction table, holds an array of instructions, and current is an index of the one currently dealing with*/
struct {
    size_t size;    /*count elements store.*/
    it_line ins_act[MAX_LENGTH_IT];
}it; /*instruction*/

/*symbol table*/
struct {
    size_t size;
    symbol * sym;
}st;

/*ext_mention struct reffers to a mention of an external symbol in the file*/typedef struct {
    char ext_name[MAX_SYMBOL_NAME+1];
    unsigned int address:13;
}ext_mention;

/* external tablee, holds an array of pointers to external symbol mentions in file,
size is a pointer to current symbol dealing with*/
struct {
    size_t size;
    ext_mention * exter;
}ext_table;
/*add a symbol to the symbol table*/
result add_to_st(const char *name, unsigned int address, bool ext, LINE_KIND status);
/*add a number to the data table*/
void add_to_dt(int num);
/*add a symbol to ext_mention.*/
result add_to_eu(const char * _ext_name, unsigned int _address);

/*fet the size of a table*/
#define get_size(X) ((X).size)
/*set the current of any table: it,st or td.*/
#define set_size(X,Y) ((X).size = (Y))
/*clear all the tables st or dt.*/
#define clear_table(X,Y) if(get_size(X))free((Y));\
                            (Y) = NULL;\
                         set_size((X),0);
/*clear all the tables.*/
#define clear_all_tables() \
    clear_table(dt,dt.data_info);\
    clear_table(st,st.sym);\
    clear_table(ext_table,ext_table.exter);\
    set_size(it,0)\

#endif /*ASSEMBLER_TABLES_H*/