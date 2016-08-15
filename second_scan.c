#include <stdlib.h>
#include <errno.h>
#include "second_scan.h"
#include "tables.h"
#include "help_functions.h"

#define NO_EXTRA (0)
#define ONE_EXTRA (1)

extern regist registers[];
extern inst_data instructions[];
extern guidance guidances[];

/*
 * function encodes all operands related
 * to instruction pointer by calling
 * different function for one or tew operands
 */
static void encode_op_ins(inst_data *);
/*
 * function extracts the wanted bit's from  'from' and
 * return the extract bits as number.
 * for dynamic addressing.
 */
static unsigned int extract_bits(unsigned int from, unsigned int start_bit, unsigned int end_bit);
/* function processes the information from instructions having one operand (group 1) */
static void one_extra();
/* function processes the information from instructions having tow operands (group 2) */
static void tow_extra();
/* function encodes given operand into 'instruction table' as bing addressed into registers r0-r7 */
static void direct_reg(it_line *current, operand SRC_or_DST);
/* unction encodes given operand into 'instruction table' as dynamic addressed */
static void dynamic_add(it_line *current);
/* encodes given operand into 'instruction table' as direct addressed */
static void direct_add(it_line *current, operand SRC_or_DST);
/* function encodes given operand into 'instruction table' as immediate addressed */
static void immediate_add(it_line *current);
/*
 * function determines what addressing type is
 * in the input instruction operand,
 * and calls the correlate function
 * to end the process of encoding the operand
 */
static void add_operand(it_line *current, operand SRC_or_DST, int address_type);

/*store the number of errors that been in the file until now.*/
extern int num_of_errors;
/*count the current line that input in the instruction table.*/
int IC;

#define START_COMMENT_LINE ';'

/*
 * reads the file secondly,
 * it processes the information regarding operands
 * linked to each instruction in the code,
 * and stores it beside after the instruction line information,
 * which was stored in first scan.
 */
void scan_second(FILE * file){
    char line[LINE_MAX],* pword;
    guidance * gud;
    char * del = "\t \n";
    if (!file)
        return;
    /*start the second scan just if no errors fond in the first scan.*/
    if (num_of_errors != 0)
        return;
    IC=0;
    fseek(file, 0L, SEEK_SET);/*start to read the file from the beginning.*/
    for (NLINE = 1;fgets(line, LINE_MAX, file);++NLINE){
        pword = strtok(line,del);
        if(!pword || *pword == START_COMMENT_LINE)     /* case empty line or comment line */
            continue;
        if (is_symbol(pword, YES) && !(pword = strtok(NULL, del)))    /* case first word is recognized as a symbol */
            continue;
        /* case word is recognized as guiding word ( .entry/.extern/.data/.string ). */
        if ((gud = in_guidances(pword))){
            if (strcmp(gud->name,".entry") == 0)     /* if 'entry', call correlate function, relaying information collected at first scan*/
                gud->foo();
            continue;
        }
        encode_op_ins( is_instruction(pword)); /* operands of the instruction is being encoded inside 'instruction table' */
    }
}

/*
 * function extracts the wanted bit's from  'from' and
 * return the extract bits as number.
 * for dynamic addressing.
 */
static void encode_op_ins(inst_data * p_inst_data){
    if(!p_inst_data)
        return;
    if (p_inst_data->group == NO_EXTRA) {/*case no extra words, no need to code anything.*/
        ++IC;
        return;
    }
    /**/
    (p_inst_data->group == ONE_EXTRA) ?  (one_extra()) : (tow_extra());
    ++IC;
}
/* function processes the information from guidance having tow operands (group 2) */
static void tow_extra() {
    /*set current to point to the instruction that  handle now.*/
    it_line * current = it.ins_act+IC;
    (current+1)->hole = 0;
    ++IC;
    /*case both of the extra words are registers address.*/
    if(current->inst.src_op==current->inst.dst_op && current->inst.dst_op==REG){
        direct_reg(current + 1,SRC);
        direct_reg(current + 1, DST);
        return;
    }
    (current+2)->hole = 0;
    add_operand(current + 1, SRC, current->inst.src_op);
    ++IC;
    add_operand(current + 2, DST, current->inst.dst_op);
}

/* function processes the information from guidance having one operand (group 1) */
static void one_extra() {
    /*set current to point to the instruction that  handle now.*/
    it_line * current = it.ins_act+IC;
    ++IC;
    (current+1)->hole = 0;
    add_operand(current + 1, DST, current->inst.dst_op);
}
/*
 * function determines what addressing type is
 * in the input instruction operand,
 * and calls the correlate function
 * to end the process of encoding the operand
 */
static void add_operand(it_line * current, operand SRC_or_DST, int address_type) {
    switch (address_type){
        case IMMEDIATE:{/*case immediate addressing*/
            immediate_add(current);
            break;
        }
        case DIRECT:{/*case direct addressing*/
            direct_add(current, SRC_or_DST);
            break;
        }
        case DYNAMIC:{/*case dynamic addressing*/
            dynamic_add(current);
            break;
        }
        case REG:{/*case direct register addressing*/
            direct_reg(current, SRC_or_DST);
            break;
        }
        default:print_error(NULL,"Not legal address type.\n"
                                 "Huge error in the program :(.\n"
                                 "run for your life.");
            break;
    }
}
/* function encodes given operand into 'instruction table' as immediate addressed */
static void immediate_add(it_line *current) {
    char * num = strtok(NULL,DEL);
    if (num){
        (current)->number.num = atoi(num+1);/*convert the number.*/
        (current)->number.era = ABSOLUTE;   /*set era to absolute*/
    }
    else current->number.num = 0;
}
/* encodes given operand into 'instruction table' as direct addressed */
static void direct_add(it_line *current, operand SRC_or_DST) {
    char * s_del = ", \n\t",*d_del = SPACES;
    char * string = strtok(NULL,SRC_or_DST == SRC ? s_del : d_del);
    symbol * sym;
    if (!(sym = in_st(string))){/*if the symbol isn't in the symbol table.*/
        print_error("Symbol %s used but not declare.", string ? string : "");
        return;
    }
    (current)->addrs_line.address = get_sy_address(sym);
    (current)->addrs_line.era = sym->external == YES ? EXTERNAL : RELOCATABLE;
    if(sym->external == YES)add_to_eu(sym->name,IC);/*if external insert to the external usage list.*/
    return;
}


/*
 * function extracts the wanted bit's from  'from' and store them in current
 * for dynamic addressing.
 */
#define CHAR_NUM_BITS (8)
static unsigned int extract_bits(unsigned int from, unsigned int start_bit, unsigned int end_bit) {
    int i,mask;
    unsigned int temp = 0;
    /*extract the range bit's*/
    for (i = start_bit,mask = 1<<(start_bit); i <= end_bit; ++i,mask<<=1) {
        temp |= ((mask & (from))>>start_bit);
    }/*case a minus number complete one's up to the end*/
    if(((mask>>1) & (from))) {
        for (; i < sizeof(unsigned int)*CHAR_NUM_BITS+end_bit; ++i, mask <<= 1)
            temp |= (mask >> start_bit);
    }
    return temp;
}

#define END_FIRST_NUM '-'
#define START_FIRST_NUM '['
/* encodes given operand into 'instruction table' as dynamic addressed */
static void dynamic_add(it_line *current) {
    symbol * sym;
    char symbol_part[MAX_SYMBOL_NAME+1] = {0};
    char * str,*p_num;
    char * del = "\t\n, ";
    unsigned int from,to;
    if (!(str = strtok(NULL,del)))return;
    if (!(p_num = strrchr(str,START_FIRST_NUM))) /*set pnum to point to the first number.*/
        return;
    strncpy(symbol_part,str,p_num-str);/*copy the symbol part to symbol_part*/
    if(!(sym = in_st(symbol_part))) {/*search for the symbol in sy.*/
        print_error("Symbol %s used but not declared.",symbol_part);
        return;
    }
    from = atoi(p_num+1);    /*get the first number.*/
    if (!(p_num = strrchr(p_num,END_FIRST_NUM)))
        return;
    to = atoi(p_num+1);     /*get the second number*/
    if (sym->external == YES){
        print_error("Dynamic addressing can't get external symbols.");
        return;
    }/*extract the bits*/
    if (sym->act_or_inf == INST) {
        current->number.num = extract_bits(
                (it.ins_act + sym->address)->hole, from, to);
    }
    else {
        current->number.num = extract_bits(
                (dt.data_info + sym->address)->num, from, to);
    }
    (current)->number.era = ABSOLUTE;
}

/* function encodes given operand into 'instruction table' as bing addressed into registers r0-r7 */
static void direct_reg(it_line * current, operand SRC_or_DST) {
    regist * reg = is_reg(strtok(NULL,DEL));
    if (!reg)return;
    if (SRC_or_DST == SRC) {
        current->rgs_line.src_rgs = reg->code;
    }
    else {
        current->rgs_line.dst_rgs = reg->code;
    }
    current->rgs_line.era = ABSOLUTE;
}
