#include <stdarg.h>
#include <stdio.h>
#include "help_functions.h"
#include "tables.h"
#include <ctype.h>

#define NUM_ONE_EXTRA (2)
#define NUM_TOW_EXTRA (3)
#define NUM_ZERO_EXTRA (1)
/* gets pointer to a parameter, and returns whether the parameter is acceptable dinamic addressing */
static bool is_dynamic(const char * _param);
/* deals with zero extra words to the givven instruction as given in action */
static void zero_inst(const inst_data *p_inst_data);
/* deals with one extra words to the givven instruction as given in ptr */
static void one_inst(const inst_data *ptr);
/* deals with two extra words to the givven instruction as given in ptr */
static void tow_inst(const inst_data *p_inst_data);
/*return the address type of _word.*/
static address get_address_type(const char * _word);
/*return if the first extra word have a comma after it for the tow_inst.*/
static bool is_comma_after(char *param);
/*if there are tow extra words it's puts a '\0' at the end of the first one.*/
static void set_first_extra(char **p_str);

extern guidance guidances[];
extern inst_data instructions[];
extern regist registers[];

/* add extern symbol to the extern symbol table (.string)*/
result add_extern(){
    char * psymbol = strtok(NULL," \n\t");
    char * delete = NULL;
    if (!psymbol){
        print_error("Missing a symbol after an .extern declaration.");
        return fail;
    }
    if (!is_symbol(psymbol, NO)){/*chick if a legal symbol name*/
        print_error("Not legal symbol after .extern. %s",psymbol);
        return fail;
    }/*add to the symbol table*/
    add_to_st(psymbol, EXTERNAL_ADDRESS, YES, NO);
    if ((delete = strtok(NULL,"\n"))){/*chick no extra words.*/
        print_error("To many arguments after .extern delete \"%s\".",delete);
    }
    return success;
}
/*add strings to the data table (.string)*/
result add_letters() {
    char * del = "\n",* in = strtok(NULL,del);
    /*char * end_str;*/
    if (!in){/*cse no string*/
        print_error("No string after .string");
        return fail;
    }/*case missing '"' at the begnning.*/
    skipspce(in);
    if(*in != BEGIN_STRING){
        print_error("String does't start with '%c'",BEGIN_STRING);
        --in;
    };
    for(++in;*in && *in != END_STRING;++in) {/*enter all the letters to the dt.*/
        add_to_dt(*in);
    }
    add_to_dt(END_OF_STRING);
    if(!*in)/*case missing '"' at the end of the string*/
        print_error("String does't end with '%c'.",END_STRING);
    else if ((*(++in) ||  (in = strtok(in+1,SPACES))))/*case there's more information after the end og the sting.*/
        print_error(".String have to many arguments delete: \"%s\".",in);
    return success;
}

/*adds num to the data table (.data)*/
result add_nums() {
    char * number,* end, *del = ",\n";
    int n;
    while((number = strtok(NULL, del))){/*get the number*/
        n = (int)strtol(number,&end,0);/*convert the number*/
        if (*end && !isspace(*end)) {/*case not a hole number.*/
            print_error(".data: Must be a hole number \"%s\" isn't.",number);
            continue;
        }
        skipspce(end);
        if (*end)/*case missing a comma.*/
            print_error("Missing a coma before the numbers '%s'.",end);
        add_to_dt(n);/*add the number to the data table.*/
    }
    return success;
}


/*return YES/NO whether a string is a valid name of a symbol or not.*/
bool is_symbol(const char *_name, bool first) {
    if (!_name)return NO;
    /*if it's not something else it's a symbol.*/
    if (is_reg(_name) || in_guidances(_name) || is_instruction(_name))/*case something else*/
        return NO;
    if (first && *(_name+strlen(_name)-1) != ':')/*case it starts a line need to be a ':'*/
        print_error("Symbol missing a ':' at the end.");
    return YES;
}

/*returns the address as its conde for the it.*/
int address_to_num(address _address){
    switch (_address){
        case immediate:
            return IMMEDIATE;
        case direct:
            return DIRECT;
        case dynamic:
            return DYNAMIC;
        case direct_register:
            return REG;
        default:
            return fail;
    }
}

/*returns if a string is already in the symbol table.*/
symbol * in_st(const char *_symbol) {
    symbol * psymbol;
    int i;
    if (!_symbol)
        return NULL;
    for (i = 0,psymbol = st.sym;
         i < get_size(st) && strcmp(psymbol->name,_symbol);
         ++i,++psymbol);/*goes throw the st.*/
    return  i >= get_size(st) ? NULL : psymbol;
}

/*add the instruction to the it*/
void add_instruction_table(const inst_data _inst) {
    /*select the current function to handle the instruction.*/
    _inst.group == 0 ? zero_inst(&_inst) : (_inst.group == 1 ? one_inst(&_inst) : tow_inst(&_inst));
}

#define calc_add_to_it(X,Y) ((X) == direct_register && (Y) == direct_register ? 1 : 2)
/*deal with tow extra words for instructions as given in ptr.*/
static void tow_inst(const inst_data * p_inst_data) {
    char * p_inst1 = NULL,* p_inst2 = NULL;
    char * delete = NULL;
    it_line * line;
    address add_type1,add_type2;
    if (!p_inst_data)return;
    line = it.ins_act + get_size(it);
    /*case no enouph extra words*/
    if (!(p_inst1 = strtok(NULL,",\n"))){
        print_error("Missing extra words for '%s'.",p_inst_data->name);
    }
    if(!is_comma_after(p_inst1)){/*case missing a comma between the extra words.*/
        print_error("Missing a comma between the extra words at '%s'.",p_inst_data->name);
        p_inst1 = strtok(p_inst1,"\t \n");
    }
    if(!(p_inst2 = strtok(NULL,SPACES))) {/*case missing the second extra word.*/
        print_error("Missing extra words for '%s'.", p_inst_data->name);
        return;
    }
    set_first_extra(&p_inst1);
    add_type1 = get_address_type(p_inst1);/*get the address's type*/
    add_type2 = get_address_type(p_inst2);
    /*case one of the address's isn't legal.*/
    if (!(add_type1 & p_inst_data->src_code) || !(add_type2 & p_inst_data->dis_code))/*chick the address's are currect.*/
        print_error("Not legal address \"%s\" for '%s'.",!(add_type1 & p_inst_data->src_code) ? p_inst1 : p_inst2,p_inst_data->name);
    line->hole = 0;/*inter the values.*/
    line->inst.notuse = p_inst_data->nuse;
    line->inst.group = p_inst_data->group;
    line->inst.opcode = p_inst_data->opcode;
    line->inst.src_op = address_to_num(add_type1);
    line->inst.dst_op = address_to_num(add_type2);
    if ((get_size(it) + (calc_add_to_it(add_type1,add_type2))+1) > MAX_LENGTH_IT) {
        print_error("The file is to big the maximum is %d memory words", MAX_LENGTH_IT);
        return;
    }
    set_size(it,get_size(it) + calc_add_to_it(add_type1,add_type2)+1);/*add places in the it table.*/
    if ((delete = strtok(NULL,SPACES)))/*chick that there are no more extra words.*/
        print_error("To many extra words for %s delete \"%s\".",p_inst_data->name,delete);
}
/*if there are tow extra words it's puts a '\0' at the end of the first one.*/
static void set_first_extra(char **p_str) {
    char * temp;
    if (!p_str)return;
    skipspce(*p_str);
    temp = *p_str;
    for (; !isspace(*temp) && *temp; ++temp);
    *temp = '\0';
}

/*returns YES/NO whether the first extra word has a comma after it and before second word, if there is another wirds.*/
static bool is_comma_after(char *param) {
    char * temp;
    temp = param;
    skipspce(temp);
    for (;!isspace(*temp) && *temp;++temp);/*skip the word*/
    skipspce(temp);
    return *temp ? NO : YES;
}
/*deal with one extra words for instructions as given in ptr.*/
static void one_inst(const inst_data * ptr) {
    char * pinst;
    it_line * line;
    address add_type;
    if (!ptr)return;
    line = it.ins_act + get_size(it);/*set pointer to the location of the input.*/
    if (!(pinst = strtok(NULL,SPACES))){
        print_error("Missing extra word for '%s'.",ptr->name);
        return;
    }
    add_type = get_address_type(pinst);/*get the address type*/
    if (!(add_type & ptr->dis_code))
        print_error("Not legal address \"%s\" for '%s'.",pinst,ptr->name);
    /*inter the values.*/
    line->hole = 0;
    line->inst.notuse = ptr->nuse;
    line->inst.group = ptr->group;
    line->inst.opcode = ptr->opcode;
    line->inst.dst_op = address_to_num(add_type);
    if ((get_size(it) + get_size(it)+NUM_ONE_EXTRA) > MAX_LENGTH_IT) {
        print_error("The file is to big the maximum is %d memory words", MAX_LENGTH_IT);
        return;
    }
    set_size(it,get_size(it)+NUM_ONE_EXTRA);/*set the size of it.*/
    if ((pinst = strtok(NULL,"\n")))/*chick no more extra words.*/
        print_error("To many extra words for '%s' delete \"%s\".",ptr->name,pinst);
}

/*deal with zero extra words for instructions as given in action.*/
static void zero_inst(const inst_data *p_inst_data) {
    it_line * temp;
    char * delete;
    temp = (it.ins_act+get_size(it));/*set pointer to the enter location*/
    /*enter the information*/
    temp->hole = 0;
    temp->inst.notuse = (*p_inst_data).nuse;
    temp->inst.opcode = (*p_inst_data).opcode;
    if ((get_size(it)+NUM_ZERO_EXTRA) > MAX_LENGTH_IT) {
        print_error("The file is to big the maximum is %d memory words", MAX_LENGTH_IT);
        return;
    }
    set_size(it,get_size(it)+NUM_ZERO_EXTRA);
    if ((delete = strtok(NULL,SPACES))) {/*case extra word*/
        print_error("To many arguments for '%s' delete %s", p_inst_data->name,delete);
    }
}

/*return the address type of _word.*/
static address get_address_type(const char * _word){
    if (is_reg(_word))/*if a collector*/
        return direct_register;
    if (*_word == '#'){/*if the first letter is '#' it's immediate*/
        char *end;
        strtol(_word+1,&end,DECIMAL_NUMBER);
        for(;isspace(*end);++end);
        if (*end)
            print_error("Expect a hole number after '#' delete \"%s\"",end);
        return immediate;
    }
    if(is_dynamic(_word))
        return dynamic;
    return direct;
}
#define START_FIRST_NUM '['
#define END_FIRST_NUM '-'
#define END_SECOND_NUM ']'
#define MAX_GAP_DYN (12)
#define MAX_BIT_DYN (14)
#define MIN_BIT_DYN (0)
/* gets pointer to a parameter, and returns whether the parameter is acceptable dynamic addressing */
static bool is_dynamic(const char * _param) {
    int begin,end;
    char *first_num,*last_num;
    char * end_num;
    if (!_param)
        return NO;
    if (!(first_num = strchr(_param,START_FIRST_NUM)))/*set a pointer to the beginning of the first number.*/
        return NO;
    if(!(last_num = strrchr(first_num,END_FIRST_NUM))) return NO;/*set a pointer to the beginning of the second number.*/
    if (! strrchr(last_num,END_SECOND_NUM))return NO;
    if (first_num == _param){/*case no symbol at the beginning.*/
        print_error("Dynamic address without a symbol");
        return YES;
    }
    begin = (int)strtol(first_num+1,&end_num,DECIMAL_NUMBER);/*convert the first number.*/
    if (end_num != last_num)    /**/
        print_error("Dynamic number need to be hole number But the first number isn't.");
    end = (int)strtol(last_num+1,&end_num,DECIMAL_NUMBER);
    if (end_num != strrchr(last_num,END_SECOND_NUM))
        print_error("Dynamic number need to be hole number But the second number isn't.");
    if( begin > end || (end > begin + MAX_GAP_DYN) || end > MAX_BIT_DYN || begin < MIN_BIT_DYN)
        print_error("Un valid range in dynamic address.\n"
                            "The correct range is from %d to %d and the max gap is %d",MIN_BIT_DYN,MAX_BIT_DYN,MAX_GAP_DYN);
    if (*++end_num)
        print_error("Dynamic address continue after numbers ends delete %s",end);
    return YES;
}

/*if _name is command return a pointer to the command else return NULL.*/
guidance * in_guidances(const char * _name) {
    int i;
    for(i = 0;guidances[i].name;++i)/*go throw guidances.*/
        if (strcmp(_name,guidances[i].name) == 0)/*case is guidances return a pointer to it.*/
            return guidances+i;
    return NULL;/*if not found return NULL*/
}
/*if _name is collector return a pointer to the collector else return NULL.*/
regist * is_reg(const char * _name){
    int i;
    for (i = 0; registers[i].name; ++i) {/*go throw registers.*/
        if (strcmp(_name,registers[i].name) == 0)/*case in registers return a pointer to it.*/
            return registers+i;
    }
    return NULL;/*if not found return NULL*/
}

/*if _name is action return a pointer to the action else return NULL.*/
inst_data * is_instruction(const char * _name) {
    int i;
    for (i = 0; instructions[i].name; ++i) {/*go throw instructions*/
        if (strcmp(_name,instructions[i].name) == 0) /*if found return a pointer to it*/
            return instructions+i;
    }
    return NULL;/*if not found return NULL*/
}
/* return the symbol address.*/
int get_sy_address(const symbol *sym) {
    if (!sym)return -1;
    if (sym->external)return EXTERNAL_ADDRESS;
    if (sym->act_or_inf == INST)return sym->address+INITIAL_ADRS;
    return sym->address+get_size(it)+INITIAL_ADRS;
}

/*sign a symbol as entry.*/
result sign_entry(){
    char * pword;
    symbol * sym;
    pword = strtok(NULL,"\n \t");
    if ((sym = in_st(pword)))/*if the symbol is in the st.*/
        sym->is_entry = YES;
    else{
        print_error(".entry %s Not declared in this file", pword);
        return fail;
    }/*chick that there's no more extra words.*/
    if(strtok(NULL,""))
        print_error("Too many arguments for entry");
    return success;
}

int num_of_errors = 0;
char * pfile = NULL;
/*prints an error and return YES.*/
void print_error(const char * fmt,...){
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        if (pfile)
            fprintf(stderr, "error: in file %s, at line %d:\n",pfile, NLINE);
        else
            fprintf(stderr, "error: ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
        va_end(args);
        ++num_of_errors;/*add one to the error counter*/
    }
}
