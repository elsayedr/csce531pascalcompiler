#ifndef ENCODE_H
#define ENCODE_H

#include "types.h"
#include "symtab.h"
#include "tree.h"

/*External reference to variables in other files*/
extern int base_offset_stack[BS_DEPTH];
extern int bo_top;
extern char* endLabels[100];
extern int endLabelCurr;

/*Function declarations*/
void declareVariable(ST_ID, TYPE);
int getAlignSize(TYPE);
int getSkipSize(TYPE);
int getFormalParameterOffset(TYPETAG);

/*Fenner's functions*/
void enter_func_body(char * global_func_name, TYPE type, int loc_var_offset);
void exit_func_body(char * global_func_name, TYPE type);
void enter_main_body();
void exit_main_body();
void encode_expr(EXPR expr);
int get_local_var_offset();
void new_exit_label();
char * old_exit_label();
char * current_exit_label();
BOOLEAN is_exit_label();
void encode_dispatch(VAL_LIST vals, char * label);
char * encode_for_preamble(EXPR var, EXPR init, int dir, EXPR limit);


#endif
