#include <stdlib.h>
#include "tables.h"
#include "second_scan.h"
#include "help_functions.h"
#include <errno.h>
#include <ctype.h>

/* add one memory place at symbol table.*/
static result add_place_st();
/* add one memory place to places at data table.*/
static result add_place_dt();
/* add one memory place to external table.*/
static result add_place_ext_t();

/*holds all the valid collectors */
regist registers[] = {
        {"r0",0X0},
        {"r1",0X1},
        {"r2",0X2},
        {"r3",0X3},
        {"r4",0X4},
        {"r5",0X5},
        {"r6",0X6},
        {"r7",0X7},
};


/*
 * holds all the information about valid combinations of command-group-src addressing-dst addressing.
 * for example, 'mov' command is interpolated like this:
 * 0X5 - 101  : constant,
 * 0X2 - 10   : group 2 operands
 * 0x0 - 0000 : opcode of mov
 * 0xf - 1111 : src addressing valid methods (according to lited bits in order)
 * 0xa - 1010 : dst addressing valid methods (according tolited bits in order)
 */
inst_data instructions[] = {
        {"mov",0X5,0X2,0X0 ,0Xf,0xa},
        {"cmp",0X5,0X2,0X1 ,0Xf,0xf},
        {"add",0X5,0X2,0X2 ,0Xf,0xa},
        {"sub",0X5,0X2,0X3 ,0Xf,0xa},
        {"not",0X5,0X1,0X4 ,0X0,0xa},
        {"clr",0X5,0X1,0X5 ,0X0,0xa},
        {"lea",0X5,0X2,0X6 ,0X1,0xa},
        {"inc",0X5,0X1,0X7 ,0X0,0xa},
        {"dec",0X5,0X1,0X8 ,0X0,0xa},
        {"jmp",0X5,0X1,0X9 ,0X0,0xa},
        {"bne",0X5,0X1,0Xa ,0X0,0xa},
        {"red",0X5,0X1,0Xb ,0X0,0xa},
        {"prn",0X5,0X1,0Xc ,0X0,0xf},
        {"jsr",0X5,0X1,0Xd ,0X0,0xa},
        {"rts",0X5,0X0,0Xe ,0X0,0X0},
        {"stop",0X5,0X0,0Xf,0X0,0X0},
        {NULL}
};

/* commands holds an array of all pairs of guiding words,
 each pair is raely a command struct, witch contains the guiding word string, and pointer to correlate function */
guidance guidances[] = {
        {".data",add_nums},
        {".string",add_letters},
        {".entry",sign_entry},
        {".extern",add_extern},
        {NULL,NULL}
};
/*add a symbol to the symbol table*/
result add_to_st(const char * name, unsigned int address, bool ext, LINE_KIND INST_or_INFO) {
    int len_name;
    if (!name)return fail;
    len_name = strlen(name);
    if (in_st(name)){/*case already in the st.*/
        print_error("%s symbol declare more than once.",name);
        return fail;
    }/*chick in the correct length.*/
    if (len_name > MAX_SYMBOL_NAME + (*(name+len_name-1) == ':') ? 1 : 0)/*symbol have a limit length*/
        print_error("The name %s for a symbol is to long.",name);
    if (!isalpha(*name))/*symbol mast start with a letter*/
        print_error("Symbol '%s' dose't start with an alphabetic character.",name);

    add_place_st();/*add a place at the st*/
    /*put all the data in place.*/
    strncpy((st.sym + get_size(st) - 1)->name, name, *(name + strlen(name) - 1) == ':' ?
                                                    MIN(MAX_SYMBOL_NAME, strlen(name) - 1) : MAX_SYMBOL_NAME);
    (st.sym + get_size(st) - 1)->address = address;
    (st.sym + get_size(st) - 1)->external = ext;
    (st.sym + get_size(st) - 1)->act_or_inf = INST_or_INFO;
    (st.sym + get_size(st) - 1)->is_entry = NO;
    return success;
}

/* add one memory place to st.*/
static result add_place_st(){
    if(!get_size(st)) {/*case empty.*/
        st.sym = malloc(sizeof(symbol));
        if (!st.sym){
            print_error("fail allocate memory to symbol table because: %s", strerror(errno));
            exit(errno);
        }
    }
    else {/*case not empty*/
        symbol * temp = realloc(st.sym, sizeof(symbol)*(get_size(st)+1));
        if (!temp){
            print_error("fail allocate memory to symbol table because: %s", strerror(errno));
            exit(errno);
        }
        st.sym = temp;
    }
    memset((st.sym+get_size(st))->name,'\0',MAX_SYMBOL_NAME+1);
    st.size +=1;
    return success;
}
/*add a number to the data table*/
void add_to_dt( int num) {
    add_place_dt();
    (dt.data_info+get_size(dt)-1)->num = num;
}

/* add one memory place to dt.*/
static result add_place_dt(){
    data * temp;
    if(!dt.data_info){/*if the table is empty*/
        temp = malloc(sizeof(data));
        if(!temp){
            print_error("fail allocate memory to data table because: %s", strerror(errno));
            exit(errno);
        }
    }/*if not empty*/
    else if(!(temp = realloc(dt.data_info,sizeof(data)*(get_size(dt)+1)))){
        print_error("fail allocate memory to data table because: %s", strerror(errno));
        exit(errno);
    }dt.data_info = temp;
    dt.size +=1;
    return success;
}
/*add a symbol to ext_mention.*/
result add_to_eu(const char * _ext_name, unsigned int _address) {
    if(!_ext_name)return fail;
    add_place_ext_t();
    strncpy((ext_table.exter+get_size(ext_table)-1)->ext_name,_ext_name,MAX_SYMBOL_NAME);
    (ext_table.exter+get_size(ext_table)-1)->address = _address;
    return success;
}

/* add one memory place to ext_table.*/
static result add_place_ext_t() {
    if(!get_size(ext_table)) {/*case empty.*/
        ext_table.exter = malloc(sizeof(ext_mention));
        if (!ext_table.exter) {
            print_error("fail allocate memory for external_usage because: %s", strerror(errno));
            exit(errno);
        }
    }
    else {/*case not empty*/
        ext_mention * temp = realloc(ext_table.exter, sizeof(ext_mention)*(get_size(ext_table)+1));
        if (!temp){
            print_error("fail allocate memory for external_usage because: %s", strerror(errno));
            exit(errno);
        }
        ext_table.exter = temp;
    }
    ext_table.size++;
    return success;
}
