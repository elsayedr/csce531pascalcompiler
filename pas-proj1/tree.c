/* Tree building functions */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "types.h"
#include "tree.h"
#include "symtab.h"
#include "message.h"

/*Function that inserts an index into the index list*/
INDEX_LIST insert_index(INDEX_LIST list, TYPE newtype)
{
  /*Works by using the prev field to link back to the current list*/
  /*Creates the index list and allocates memory*/
  INDEX_LIST new;
  new = (INDEX_LIST) malloc(sizeof(INDEX));
  
  /*Inserts the element and returns the list*/
  new->type = newtype;
  new->next = list;
  new->prev = NULL;
  return new;
}

/*Function that inserts an ST_ID into an ID list*/
ID_LIST insert_id(ID_LIST list, ST_ID newid)
{
  ID_LIST new;
  new = (ID_LIST) malloc(sizeof(ID_LIST_NODE));
  
  /*Inserts the element and returns the list*/
  new->id = newid;
  new->next = list;
  return new;
}

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

  /*Sets the value of the node attributes and returns it*/
  p->tag = INTCONST;
  p->u.intconst = n;
  if(debug)
  {
    printf("INTCONST %d created\n",(int)n);
  }
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
  if(debug) 
  {
    printf("REALCONST %f created\n",n);
  }
  return p;
}

/*Function that makes an identifier node*/
ST make_id(ST_ID iden)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node attributes and returns the node*/
  p->tag = ID_NODE;
  p->u.id_node.id = iden;
  if(debug) 
  {
    printf("ID NODE %s created\n", st_get_id_str(iden) );
  }
  return p;
}

/*Function that makes a type data record and installs it in the symbol table*/
void make_type(ST_ID iden, TYPE newtype)
{
  /*Creates the data record and allocates memory*/
  ST_DR p;
  p = stdr_alloc();

  /*Sets the data record attributes and installs the type in the symbol table*/
  p->tag = TYPENAME;
  p->u.typename.type = newtype;
  if(debug)
  {
    printf("TYPE name %s installed\n", st_get_id_str(iden) );
  }
  st_install(iden, p);
}

/*Function that makes a variable data record and installs it in the symbol table*/
void make_var(ID_LIST list, TYPE newtype)
{
  /*Creates the data record and allocates memory*/
  ST_DR p;
  
  if (!list) bug("Empty list passed to make_var");
  while (list) {
	p = stdr_alloc();	  
	p->tag = GDECL;
	p->u.decl.type = newtype;
	if(debug)
  	{
  	  printf("GDECL created with type: %d\n",ty_query(newtype));
 	}
  	st_install(list->id, p); /* installs the variable in the symbol table*/
	list=list->next;
  }
}

/*Function that makes a unary operator node*/
ST make_unop(char c, ST a)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));
  
  /*Sets the value of the node attributes and returns the node*/
  p->tag = UNOP;
  p->u.unop.op = c;
  p->u.unop.arg = a;
  if(debug) 
  {
    printf("UNOP %c created\n",c);
  }
  return p;
}

/*Function that makes a binary operator node*/
ST make_binop(ST a1, char c, ST a2)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node attributes and returns the node*/
  p->tag = BINOP;
  p->u.binop.op = c;
  p->u.binop.arg1 = a1;
  p->u.binop.arg2 = a2;
  if(debug) 
  {
    printf("BINOP %c created\n",c);
  }
  return p;
}

