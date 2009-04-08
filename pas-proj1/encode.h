#ifndef ENCODE_H
#define ENCODE_H

#include "types.h"
#include "symtab.h"
#include "tree.h"

/*External reference to variables in other files*/
extern int base_offset_stack[BS_DEPTH];
extern int bo_top;

/*Function declarations*/
void declareVariable(ST_ID, TYPE);
int getAlignSize(TYPE);
int getSkipSize(TYPE);
void incrementStack();
void decrementStack();

/*Fenner's functions*/
void enter_func_body(char * global_func_name, TYPE type, int loc_var_size);
void exit_func_body(char * global_func_name, TYPE type);
void enter_main_body();
void exit_main_body();
void encode_expr(EXPR expr);
int get_local_var_offset();

#endif
