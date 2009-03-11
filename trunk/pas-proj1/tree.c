/* Tree building functions */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "types.h"
#include "tree.h"

/*Function that evaluates the value of an identifier*/
long eval_id(ST iden)
{
}

/*Function that makes an integer node*/
ST make_int(long n)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node and returns it*/
  p->tag = INTCONST;
  p->u.intconst = n;
  return p;
}

/*Function that makes a real node*/
ST make_real(double n)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));
	
  /*Sets the value of the node attributes and returns them*/
  p->tag = REALCONST;
  p->u.realconst = n;
  return p;
}

/*Make an identifier node*/
ST make_id(ST_ID iden)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node attributes and returns the node*/
  p->tag = ID_NODE;
  p->u.id_node.id = iden;
  return p;
}

/*Function that makes a type node*/
ST make_type(ST_ID iden, TYPE newtype)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the node attributes and installs the type*/
  p->tag = TYPE_NODE;
  p->u.type_node.id = iden;
  p->u.type_node.type = newtype;
  st_install(iden, p);
}

/*Function that makes a variable*/
void make_var(ST_ID iden, TYPE newtype)
{
  /*Creates the node and allocates memory*/
  ST_DR p;
  p = stdr_alloc();

  /*Sets the node attributes and installs the variable in the symbol table*/
  p->tag = GDECL;
  p->u.decl.type = newtype;
  st_install(iden, p);
}

/*Function that makes a unary operator node*/
ST make_unop(char c, ST a)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));
  
  /*Sets the node attributes and returns the node*/
  p->tag = UNOP;
  p->u.unop.op = c;
  p->u.unop.arg = a;
  return p;
}

/*Function that makes a binary operator node*/
ST make_binop(ST a1, char c, ST a2)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the node attributes and returns the node*/
  p->tag = BINOP;
  p->u.binop.op = c;
  p->u.binop.arg1 = a1;
  p->u.binop.arg2 = a2;
  return p;
}

