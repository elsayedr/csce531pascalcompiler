/*Definitions to support tree.c*/

#ifndef TREE_H
#define TREE_H

#include "defs.h"
#include "types.h"
#include "symtab.h"

/*Reference to stack variables declared in gram.y*/
extern int base_offset_stack[BS_DEPTH];
extern int bo_top;

/*Defines a structure that represents a node in a linked list*/
typedef struct id_node
{
  /*Symbol table data record*/
  ST_ID id;
  
  /*Link to next element in the list*/
  struct id_node *next;

} ID_NODE, *ID_LIST;

/*Fenner's structures*/
typedef struct {
    struct exprnode * expr;
    ST_ID id;
} EXPR_ID;

typedef struct {
    ST_ID	id;
    TYPE	type;
} PTR_OBJ, FUNC_HEAD;

typedef struct val_node {
    long lo, hi;
    TYPETAG type;
    struct val_node * next;
} VAL_LIST_REC, *VAL_LIST;

typedef struct {
    char * name;
    int    offset;
} NAME_OFFSET;

typedef struct {
    TYPETAG	type;
    char * 	label;
    VAL_LIST	values;
} CASE_RECORD;

/*Structures for syntax tree nodes (EXPR and EXPR_LIST)*/
/*------------Added array_access to this*/
typedef enum {
    INTCONST, REALCONST, STRCONST, GID, LVAR, LFUN, NULLOP, UNOP, BINOP,
    FCALL, ERROR, ARRAY_ACCESS
} EXPR_TAG;

typedef enum {
    NULL_EOF_OP, NULL_EOLN_OP, NIL_OP
} EXPR_NULLOP;

typedef enum {
    CONVERT_OP, DEREF_OP, NEG_OP, ORD_OP, CHR_OP, UN_SUCC_OP, UN_PRED_OP,
    NOT_OP, ABS_OP, SQR_OP, SIN_OP, COS_OP, EXP_OP, LN_OP, SQRT_OP, ARCTAN_OP,
    ARG_OP, TRUNC_OP, ROUND_OP, CARD_OP, ODD_OP, EMPTY_OP, POSITION_OP,
    LASTPOSITION_OP, LENGTH_OP, TRIM_OP, BINDING_OP, DATE_OP, TIME_OP,
    UN_EOF_OP, UN_EOLN_OP, INDIR_OP, UPLUS_OP, NEW_OP, DISPOSE_OP, ADDRESS_OP,
    SET_RETURN_OP
} EXPR_UNOP;

typedef enum {
    ADD_OP, SUB_OP, MUL_OP, DIV_OP, MOD_OP, REALDIV_OP, EQ_OP, LESS_OP, LE_OP,
    NE_OP, GE_OP, GREATER_OP, SYMDIFF_OP, OR_OP, XOR_OP, AND_OP, BIN_SUCC_OP,
    BIN_PRED_OP, ASSIGN_OP
} EXPR_BINOP;

/*Used for lists of actual arguments to functions/procedures*/
typedef struct exprlistnode {
    struct exprnode * expr;
    struct exprlistnode * next;
} EXPR_LIST_NODE, * EXPR_LIST;

typedef struct exprnode {
    EXPR_TAG tag;
    TYPE type;
    union {
	long intval;
	double realval;
	char * strval;
	ST_ID gid;	/*For global variables and global functions*/
	struct {
	    BOOLEAN is_ref;
	    int offset;
	    int link_count;
	} lvar;
	struct {
	    char * global_name;
	    int link_count;
	} lfun;
	struct {
	    EXPR_NULLOP op;
	} nullop;
	struct {
	    EXPR_UNOP op;
	    struct exprnode * operand;
	} unop;
	struct {
	    EXPR_BINOP op;
	    struct exprnode * left, * right;
	} binop;
	struct {
	    struct exprnode * function;
	    EXPR_LIST args;
	} fcall;
	struct {
	    struct exprnode * gid;
	    EXPR_LIST index_list;
	} array_access;
    } u;
} EXPR_NODE, *EXPR;


typedef enum { DIR_EXTERNAL, DIR_FORWARD } DIRECTIVE;


/*Records the current function identifier to detect return value assigns*/
extern ST_ID func_id_stack[BS_DEPTH];
extern int fi_top;

/*Function definitions*/
ID_LIST id_prepend(ID_LIST, ST_ID);
INDEX_LIST index_append(INDEX_LIST, TYPE);
TYPE check_typename(ST_ID);
TYPE make_subrange(EXPR, EXPR);
TYPE make_array(INDEX_LIST, TYPE);
void make_type(ST_ID, TYPE);
void make_var(ID_LIST, TYPE);
void resolve_ptr_types();
void install_local_params(PARAM_LIST);
MEMBER_LIST make_members(ID_LIST, TYPE);
MEMBER_LIST member_concat(MEMBER_LIST, MEMBER_LIST);
PARAM_LIST make_params(ID_LIST, TYPE, BOOLEAN);
PARAM_LIST param_concat(PARAM_LIST, PARAM_LIST);
TYPE make_func(PARAM_LIST, TYPE);


/*Fenner's functions for 2nd installment*/
EXPR_LIST expr_list_reverse(EXPR_LIST list);
EXPR_LIST expr_prepend(EXPR_LIST list, EXPR expr);
int process_var_decl(ID_LIST ids, TYPE type, int cur_offset);
void build_func_decl(ST_ID id, TYPE type, DIRECTIVE dir);
int enter_function(ST_ID id, TYPE type, char * global_func_name);
EXPR make_intconst_expr(long val, TYPE type);
EXPR make_realconst_expr(double val);
EXPR make_strconst_expr(char * str);
EXPR make_id_expr(ST_ID id);
EXPR make_null_expr(EXPR_NULLOP op);
EXPR make_un_expr(EXPR_UNOP op, EXPR sub);
EXPR make_bin_expr(EXPR_BINOP op, EXPR left, EXPR right);
EXPR make_fcall_expr(EXPR func, EXPR_LIST args);
EXPR make_error_expr();
EXPR sign_number(EXPR_UNOP op, EXPR num);
EXPR makeConvertNode(EXPR sub, TYPE to);
EXPR checkAssign(EXPR assign);
EXPR check_assign_or_proc_call(EXPR lhs, ST_ID id, EXPR rhs);
BOOLEAN is_lval(EXPR expr);
void expr_free(EXPR expr);
void expr_list_free(EXPR_LIST list);
void id_list_free(ID_LIST list);
char * get_global_func_name(ST_ID id);
EXPR cFoldUnop(EXPR eNode);
EXPR cFoldBinop(EXPR eNode);
EXPR checkVariable(EXPR eNode, TYPE argType, TYPE paramType);
EXPR promoteInt(EXPR eNode);

/*Definitions for Part 3*/

/*Holds endlabels in a stack
For generality I only put the end labels for loops since a break
statement that occurs inside of an if which is inside of a while loop
will exit the if and the while*/
extern char* endLabels[100];
/*Current index of the end label*/
extern int endLabelCurr;

/*Case record stack variables*/
extern CASE_RECORD caseRecords[100];
extern int caseTop;

void pushEndLabel(char* endLabel);
char* popEndLabel();
char * peekEndLabel();

/*While Loop Definitions*/
char* whileInit(EXPR eNode);
char* whileCond();
void whileLoop(char *start, char *end);

/*If Statement Definitions*/
char* ifInit(EXPR eNode);
char* ifClose(char* ifend);
void elseClose(char* elseend);

/*Checkss to make sure an expression is a boolean type*/
BOOLEAN checkBoolean(EXPR exp);

/*Array Access*/
EXPR make_array_access_expr(EXPR arrayExpr,EXPR_LIST indexList);

/*Fenner's functions*/
VAL_LIST new_case_value(TYPETAG type, long lo, long hi);
BOOLEAN check_case_values(TYPETAG type, VAL_LIST vals, VAL_LIST prev_vals);
void case_value_list_free(VAL_LIST vals);
BOOLEAN get_case_value(EXPR expr, long * val, TYPETAG * type);
BOOLEAN check_for_preamble(EXPR var, EXPR init, EXPR limit);

#endif



