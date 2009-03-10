#ifndef TREE_H
#define TREE_H

#include "defs.h"
#include "types.h"

typedef enum {INTCONST,REALCONST,ID_NODE,TYPE_NODE,UNOP,BINOP} st_tagtype;

typedef struct st_node {
	st_tagtype tag;
	union {
		long intconst;

		double realconst;

		struct {
			ST_ID id;
		} id_node;

		struct {
			ST_ID id;
			TYPE type;
		} type_node;
		
		struct {
			char op;	
			struct st_node *arg;
		} unop;

		struct {
			char op;	
			struct st_node *arg1;
			struct st_node *arg2;
		} binop;

	} u;
} ST_NODE, *ST;

ST make_int(long);
ST make_real(double);
ST make_type(ST_ID, TYPE);
ST make_unop(char, ST);
ST make_binop(ST, char, ST);
long eval_id(ST);

#endif



