/* Tree building functions */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "types.h"
#include "tree.h"

ST make_type(ST_ID iden, TYPE newtype)
{}

long eval_id(ST iden)
{}

ST make_int(long n)
{
	ST p;
	p=(ST) malloc(sizeof(ST_NODE));
	p->tag=INTCONST;
	p->u.intconst=n;
	/*if (debug) printf("INTCONST %d\n",n);*/
	return p;
}


ST make_real(double n)
{
	ST p;
	p=(ST) malloc(sizeof(ST_NODE));
	p->tag=REALCONST;
	p->u.realconst=n;
	/*if (debug) printf("REALCONST %f\n",n);*/
	return p;
}

ST make_unop(char c, ST a)
{
	ST p;
	p=(ST) malloc(sizeof(ST_NODE));
	p->tag=UNOP;
	p->u.unop.op=c;
	p->u.unop.arg=a;
	/*if (debug) printf("UNOP %c\n",c);*/
	return p;
}

ST make_binop(ST a1, char c, ST a2)
{
	ST p;
	p=(ST) malloc(sizeof(ST_NODE));
	p->tag=BINOP;
	p->u.binop.op=c;
	p->u.binop.arg1=a1;
	p->u.binop.arg2=a2;
	/*if (debug) printf("BINOP %c\n",c);*/
	return p;
}


