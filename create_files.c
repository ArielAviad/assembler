#include "create_files.h"
#include "second_scan.h"
#include <string.h>
#include <errno.h>
#include "tables.h"
#include "help_functions.h"

#define BASE (8)
#define CONVERT_NUMBER_LENGTH (6)
#define ENTRYS ".ent"
#define OBJECTS ".ob"
#define EXTERNS ".ext"

extern int num_of_errors;
/* definition of special base 8 */
char spacial_base[8] = {'!','@','#','$','%','^','&','*'};
/* create object file, holds the encoded base 8 output */
static void create_ob(char *name);
/* create entry file */
static void create_entry(char *name);
/* create extern file */
static void create_ext(char *name);
/* print_base8() function gets a file of machine code, and prints back into file by special base 8 code */
static void print_base8(FILE *pFILE, unsigned int num, bool print_5);
/* returns the file name, as a string*/
static char * get_file_name(char *name, char *type);

/* creats three output files, while not found errores */
void create_files(char * name) {
    if(num_of_errors > 0 || !name){
        return;
    }
    create_ob(name);
    create_entry(name);
    create_ext(name);
}

/* create_ext() functions creates an .ext file in case of no errors */
static void create_ext(char *name) {
    ext_mention * ext;
    FILE *ext_file;
    int i;
    if (!name)
        return;
    if (!(get_size(ext_table)))/*if empty*/
        return;
    if(!(ext_file = fopen(get_file_name(name,EXTERNS), "w"))) {/*create file*/
        fprintf(stderr, "Can't create file because: %s\n", strerror(errno));
        exit(errno);
    }/*print the data in the file*/
    for (ext = ext_table.exter, i = 0; i < get_size(ext_table); ++i, ++ext) {
        fprintf(ext_file,"%s\t",ext->ext_name);
        print_base8(ext_file,ext->address+INITIAL_ADRS, NO);
        fputc('\n', ext_file);
    }
}

/* create_entry() functions creates an .ent file in case of no errors */
static void create_entry(char *name) {
    symbol *sym;
    FILE *ent_file;
    int i;
    if (!name)
        return;
    /*search for an entry symbol*/
    for (sym = st.sym, i = 0; i < get_size(st); ++i, ++sym) {
        if (sym->is_entry) {
            if (!(ent_file = fopen(get_file_name(name,ENTRYS), "w"))) {/*if entry symbol exist create entry file*/
                fprintf(stderr, "Can't create file because: %s\n", strerror(errno));
                exit(errno);
            }
            else
                break;
        }
    }/*print in entry file.*/
    for (; i < get_size(st); ++i, ++sym) {
        if (sym->is_entry) {
            fprintf(ent_file, "%s\t", sym->name);
            print_base8(ent_file, get_sy_address(sym), NO);
            fputc('\n', ent_file);
        }
    }
}

/* create_ob() functions creates an .ob file of machine code output, in case of no errors */
void create_ob(char * name) {
    it_line * inst;
    FILE * ob_file;
    data * d;
    int i;
    if (!name)
        return;
    if (!get_size(it) && !get_size(dt))/*case no data and instructions have enterd.*/
        return;
    if (!(ob_file = fopen(get_file_name(name,OBJECTS),"w"))){
        fprintf(stderr,"%s",strerror(errno));
        exit(errno);
    }
    fprintf(ob_file,"       ");
    print_base8(ob_file, it.size, NO);/*print the data and instruction length.*/
    print_base8(ob_file, dt.size, NO);
    fprintf(ob_file,"\n");
    /*print all the instructions*/
    for (i = INITIAL_ADRS,inst = it.ins_act; i < get_size(it)+INITIAL_ADRS; ++i,++inst) {
        print_base8(ob_file, i, NO);
        print_base8(ob_file, inst->hole, YES);
        fprintf(ob_file,"\n");
    }/*print all the data.*/
    for (d = dt.data_info;i < get_size(dt)+get_size(it)+INITIAL_ADRS; ++i,++d) {
        print_base8(ob_file, i, NO);
        print_base8(ob_file, d->num, YES);
        fprintf(ob_file,"\n");
    }
}

/* print_base8() function gets a file of machine code, and prints back into file by special base 8 code */
void print_base8(FILE *file, unsigned int convert, bool print_5) {
    int index;
    char convert_naumber[CONVERT_NUMBER_LENGTH] = "!!!!!";
    if (!file)return;
    fprintf(file, " ");
    for(index = CONVERT_NUMBER_LENGTH-2;convert != 0;--index) {/*convert all the number*/
        convert_naumber[index] = spacial_base[convert%(BASE)];
        convert /=(BASE);
    }
    fprintf(file,"%s",(convert_naumber + (print_5 ? 0 : index+1)) );
}

/* returns the file name, as a string*/
static char *get_file_name(char *name, char *type) {
    static char file_name[MAX_FILE_NAME+1];
    char * last_dot;
    if (!name || !type){/**/
        *file_name = '\0';
        return file_name;
    }
    strncpy(file_name,name,MAX_FILE_NAME);
    (last_dot = strrchr(file_name, '.')) ? strcpy(last_dot, type) : strcat(file_name, type);
    return file_name;
}
