/* Tree building functions */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "types.h"
#include "tree.h"
#include "symtab.h"
#include "message.h"
#include "encode.h"

/* Function that inserts an ST_ID into a linked list */
ID_LIST id_prepend(ID_LIST list, ST_ID newid)
{
  ID_LIST new;
  new = (ID_LIST) malloc(sizeof(ID_NODE));
  
  /*Inserts the element and returns the list*/
  new->id = newid;
  new->next = list;
  return new;
  
} // end id-prepend

/* Function that inserts an index into the index list */
INDEX_LIST index_append(INDEX_LIST list, TYPE newtype)
{
  /*Works by using the prev field to link back to the current list*/
  /*Creates the index list*/
  INDEX_LIST new, p;

  /*Checks to see if the new type exists*/
  if(!newtype)
  {
    /*Error and returns*/
    error("Illegal index type (ignored)");	
    return list;
  }
  else
  {
    new = (INDEX_LIST) malloc(sizeof(INDEX));
  
    /* initialize new */	
    new->type = newtype;
    new->next = NULL;

    p=list;
    if (p) {	
	if (debug) printf("List not empty\n");	/* if list exists */
	while(p->next) p=p->next; 	/* go to end of list */
    	p->next=new;			/* append new */
	return list;			/* return current list */
    }
    else return new;			/* otherwise just return new */
  }
} // end index_append

/*Function that looks up the type given from a symbol table id*/
TYPE check_typename(ST_ID id)
{
  /*Data record from the symbol table and block number*/
  ST_DR data_rec;
  int block;

  /*Gets the data record from the symbol table*/
  data_rec = st_lookup(id, &block); 
  
  /*If no record is found*/
  if (!data_rec) 
  {
    /*Undeclared type error, return null*/
    error("Undeclared type name: \"%s\"",st_get_id_str(id));
    return NULL;
  }
  
  /*Debugging information*/
  else if(debug) 
  { 
    /*Print the debugging information*/
    printf("Denoter typename: %s\n", st_get_id_str(id) );
    printf("Reference TYPE:\n");
    ty_print_type(data_rec->u.typename.type); 
    printf("\n");
  }
  /*Returns the type of the data record*/
  return data_rec->u.typename.type; 
} // end check_typename 

/*Function that creates an integer constant expression node*/
EXPR make_intconst_expr(long val, TYPE type)
{
  /*Creates the node and allocates memory*/
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /*Sets the values of the node*/
  eNode->tag = INTCONST;
  eNode->u.intval = val;
  eNode->type = type;
 
  /*Returns the node*/
  return eNode;
}

/*Function thata creates an real constant expression node*/
EXPR make_realconst_expr(double val)
{
  /*Creates the node and allocates memory*/
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /*Sets the values of the node*/
  eNode->tag = REALCONST;
  eNode->u.realval = val;
  eNode->type = ty_build_basic(TYDOUBLE);

  /*Returns the node*/
  return eNode;
}

/*Function that creates a subrange*/
TYPE make_subrange(long a, long b)
{
  /*If subrange is invalid, error*/
  if(a>b)
  {
    /*Error, return null*/
    error("Empty subrange in array index");
    return NULL;
  }
  /*Else build the subrange*/
  else
  {
    /*Debugging information*/
    if(debug) printf("Building subrange of INT from %d to %d\n", (int)a, (int)b);

    return ty_build_subrange(ty_build_basic(TYSIGNEDLONGINT), a, b);
  }
} // end make_subrange

/*Function that creates an array*/
TYPE make_array(INDEX_LIST list, TYPE newtype)
{
  /*If no type, error*/
  if(!newtype)
  {
    /*Error, return null*/
    error("Data type expected for array elements"); 
    return NULL;
  }
  /*Else return the array*/
  else 
  {
    /*Debugging information*/
    if(debug) 
    {
      /*Prints debugging information*/
      printf("Build array of TYPE:\n");
      ty_print_type(newtype); 
      printf("\n");
    }

    /*Gets the type tag of the type of the elements of the array*/
    TYPETAG tag = ty_query(newtype);

    /*Checks the type*/
    if(tag == TYUNION || tag == TYENUM || tag == TYSTRUCT || tag == TYARRAY || tag == TYSET || tag == TYFUNC || tag == TYBITFIELD || tag == TYSUBRANGE || tag == TYERROR)
    {
      /*Data type expected for array elements, returns null*/
      error("Data type expected for array elements");
      return NULL;
    }
    else
    {
      /*Returns the array TYPE*/
      return ty_build_array(newtype, list);
    }
  }
} // end make_array

/* Function that makes a type data record and installs it in the symbol table */
void make_type(ST_ID iden, TYPE newtype)
{
  /* Creates the data record and allocates memory */
  ST_DR p;
  p = stdr_alloc();
  BOOLEAN resolved;	

  /*Return if the type does not exist*/
  if(!newtype) 
  {
    /*Debugging information*/
    if(debug) 
      printf("Type does not exist\n");
	
    /*Return*/
    return;
  }

  /* Sets the data record attributes and installs the type in the symbol table */
  p->tag = TYPENAME;
  p->u.typename.type = newtype;

  /*Checks to see if the type has been resolved*/
  resolved = st_install(iden, p);

  /*If the type has not been resolved error*/
  if(!resolved) 
    error("Duplicate definition of \"%s\" in block %d", st_get_id_str(iden), st_get_cur_block());
  /*Debugging*/
  else if(debug) 
    printf("TYPE name %s installed\n", st_get_id_str(iden));
} // end make_type

/* Function that makes a variable data record and installs it in the symbol table */
void make_var(ID_LIST list, TYPE newtype)
{
  /*Symbol table data record and boolean variable*/
  ST_DR p;  
  BOOLEAN resolved;
  
  /*If the member list is empty, error*/
  if(!list) 
    bug("Empty list passed to make_var");

  /*If undefined type, error*/
  if(!newtype) 
  {
    /*Error, return*/
    error("Variable(s) must be of data type"); 
    return;
  }

    /*Gets the type tag of the type of the elements of the array*/
    TYPETAG tag = ty_query(newtype);

    /*Checks the type*/
    if(tag == TYFUNC || tag == TYERROR)
    {
      /*Data type expected for array elements, returns null*/
      error("Variable(s) must be of data type");
      return;
    }
	
  /*While the list is not null*/
  while(list) 
  {
    /*Allocates memory for the data record, sets the tag, sets the type*/
    p = stdr_alloc();	  
    p->tag = GDECL;
    p->u.decl.type = newtype;

    /*Installs the variable in the symbol table*/
    resolved = st_install(list->id, p); 

    /*If the type is not resolved error*/
    if(!resolved) error("Duplicate variable declaration: \"%s\"", st_get_id_str(list->id));
    else {

    	/*Calls the encoding function*/
    	declareVariable(list->id, newtype);
	
	if(debug)
    	{
      	  /*Print debugging statements*/
      	  printf("GDECL created with type:\n");
	  ty_print_type(newtype);
      	  printf("\n");
	}
    }	/* end else */

    /*Move on to the next item in the member list*/
    list=list->next;
  }
} // end make_var

/*Function that inserts an ID into a member list*/
MEMBER_LIST insertMember(MEMBER_LIST mList, ST_ID id)
{
  /*Member list pointers*/
  MEMBER_LIST previous = NULL;
  MEMBER_LIST toReturn = mList;

  /*While loop to determine where the node should be placed*/
  while(mList != NULL)
  {
    /*Goes to the next node in the list*/
    previous = mList;
    mList = mList->next;
    
  }

  /*If the previous node is equal to null, insert at front*/
  if(previous == NULL)
  {
    /*Create the node and insert it*/
    toReturn = (MEMBER_LIST)malloc(sizeof(MEMBER));
    toReturn->id = id;
    toReturn->next = mList;

    /*Returns the list*/
    return toReturn;
  }

  /*Insert somewhere in the middle of the list*/
  previous->next = (MEMBER_LIST)malloc(sizeof(MEMBER));
  previous = previous->next;
  previous->id = id;
  previous->next = mList;

  /*Returns the list*/
  return toReturn; 
} // end insertMember

/*Function that creates a member list from the linked list of ST_ID's*/
MEMBER_LIST createMemberListFromID(ID_LIST list)
{
  /*Linked List*/
  ID_LIST copy = list;

  /*Creates the member lists and allocates memory*/
  MEMBER_LIST memList = NULL;

  /*While there are still elements in the list*/
  while(copy != NULL)
  {
    /*Calls the function to insert the ID into the member list*/
    memList = insertMember(memList, copy->id);

    /*Moves on to the next element*/
    copy = copy->next;

  }

  /*Returns the member list*/
  return memList;
} // end 

/* Function that takes a member list and assigns a type to each member */
MEMBER_LIST make_members(ID_LIST list, TYPE newtype)
{
  /*Member list*/
  MEMBER_LIST memList, p;
  memList = createMemberListFromID(list);
  p = memList;

  /*If empty list, bug*/
  if(!p) 
    bug("Empty list passed to add members");

  if (debug) {
    printf("Typing members with TYPE:\n");
    ty_print_type(newtype);
    printf("\n");
  }

  /*While there are still elements in the list*/
  while(p) 
  {
    /*Sets the type of the element, moves on to the next element*/
    p->type = newtype;
    p=p->next;
  }

  /*Returns the member list*/
  return memList;
} // end make_members

/*Function that adds two member lists together*/
MEMBER_LIST member_concat(MEMBER_LIST list1, MEMBER_LIST list2)
{
  /*Member list*/
  MEMBER_LIST p = list1;

  /*If no member list, bug*/
  if(!p) 
    bug("Empty list passed to combine members");

  /*While there are still elements in the first member list, add them to the beginning of the second*/
  while(p->next) 
    p=p->next;

  /*Links end of the first list and the beginning of the second*/
  p->next = list2;	
  
  /*Returns the list*/
  return list1;
} // end member_concat

/*Function that resolves pointers*/
void resolve_ptrs()
{
  /*Symbol table data record, type variables, symbol table id, block number, and boolean variable*/
  ST_DR data_rec;
  TYPE list, ptr, next;
  ST_ID id;
  int block;
  BOOLEAN resolved;

  /*Debugging information*/
  if(debug) 
    printf("Attempting to resolve pointers.\n");

  /*Gets the list of unresolved pointers*/
  list = ty_get_unresolved();

  /*If the list is empty no unresolved pointers*/
  if(!list) 
  {
    /*Debugging information*/
    if(debug) 
      printf("No unresolved pointers\n");
    
    /*Return*/
    return;
  }

  /*While there are still elements in the list*/
  while(list) 
  {
    /*Gets the pointer, id, and next*/  
    ptr = ty_query_ptr(list, &id, &next);	
	  
    /*Gets the data record for the id*/
    data_rec = st_lookup(id, &block); 
		
    /*If the data record does not exist, error*/
    if(!data_rec) 
      error("Unresolved type name: \"%s\"",st_get_id_str(id));
				
    /*Else attempt to resolve the pointer*/
    else
    {
      /*Try to resolve the type of the pointer*/
      resolved = ty_resolve_ptr(list, data_rec->u.typename.type);

      /*If the pointer is not resolved, error*/
      if(!resolved) 
	error("Unresolved pointer");
      /*Debugging information*/
      else if(debug) 
      {
	/*Debugging information*/
	printf("Resolved %s ptr with type: \n",st_get_id_str(id));
	ty_print_type(data_rec->u.typename.type);
	printf("\n");
      }
    }

    /*Move on to the next item in the list*/
    list = next;
  } 	
} // end resolve_ptrs

/*Function that inserts an ID into a parameter list*/
PARAM_LIST insertParam(PARAM_LIST pList, ST_ID id, BOOLEAN isRef)
{
  /*Parameter list pointers*/
  PARAM_LIST previous = NULL;
  PARAM_LIST toReturn = pList;

  /*While loop to determine where the node should be placed*/
  while(pList != NULL)
  {
    /*Goes to the next node in the list*/
    previous = pList;
    pList = pList->next;
    
  }

  /*If the previous node is equal to null, insert at front*/
  if(previous == NULL)
  {
    /*Create the node and insert it*/
    toReturn = (PARAM_LIST)malloc(sizeof(PARAM));
    toReturn->id = id;
    toReturn->is_ref = isRef;
    toReturn->next = pList;

    /*Returns the list*/
    return toReturn;
  }

  /*Insert somewhere in the middle of the list*/
  previous->next = (PARAM_LIST)malloc(sizeof(PARAM));
  previous = previous->next;
  previous->id = id;
  previous->is_ref = isRef;
  previous->next = pList;

  /*Returns the list*/
  return toReturn;
} // end insertParam

/*Converts a linked list to a list of parameters*/
PARAM_LIST createParamListFromID(ID_LIST list, BOOLEAN isRef)
{
  /*Checks for empty member list*/
  if(!list)
  {
    /*Prints bug and returns*/
    bug("Empty linked list passed to convert to parameter list");
    return NULL;
  }

  /*Member list*/
  ID_LIST cList = list;

  /*Creates parameter lists*/
  PARAM_LIST retList = NULL;

  /*While there are still elements in the member list*/
  while(cList)
  {
    /*Inserts the element into the parameter list and moves on to the next element*/
    retList = insertParam(retList, cList->id, isRef);
    cList = cList->next;
  }

    /*Returns the parameter list*/
    return retList;
} // end

/*Function that sets the type for all of the elements in the parameter list*/
PARAM_LIST make_params(ID_LIST list, TYPE newtype, BOOLEAN isRef)
{
  /*Parameter list*/
  PARAM_LIST p, paramList = createParamListFromID(list, isRef);
  p = paramList;

  /*If empty list, bug*/
  if(!p) 
    bug("Empty list passed to add type parameters");

  /*While there are still elements in the list*/
  while(p) 
  {
    /*Sets the type of the element, moves on to the next element*/
    p->type = newtype;
    p->is_ref = isRef;
    p=p->next;
  }

  /*Returns the parameter list*/
  return paramList;
} // end make_params

/*Function that combines two parameter lists*/
PARAM_LIST param_concat(PARAM_LIST list1, PARAM_LIST list2)
{
  /*Parameter list*/
  PARAM_LIST p = list1;

  /*If no member list, bug*/
  if(!p) 
    bug("Empty list passed to combine parameters");

  /*While there are still elements in the first member list, add them to the beginning of the second*/
  while(p->next) 
    p=p->next;

  /*Links end of the first list and the beginning of the second*/
  p->next = list2;	
  
  /*Returns the list*/
  return list1;
} // end param_concat

/* Function that checks for duplicate params */
void check_params(PARAM_LIST list)
{
   PARAM_LIST p = list;
   ST_ID a,b;
   a = p->id;
   if(debug) printf("String ID: %s\n", st_get_id_str(a) );

   while (p->next) {
	p = p->next;
	b = p->id;
	if(debug) printf("Compare with string ID: %s\n", st_get_id_str(b) );
	if (a==b) error ("Duplicate parameter name: \"%s\"", st_get_id_str(a) );
   }
} // end check_params

/*Function that creates a function type*/
TYPE make_func(PARAM_LIST list, TYPE newtype)
{
   /*Checks to see if the parameter list exists*/
   if(!list)
   {
      /* Not an error: function with no params */
      if (debug) printf("Empty parameter list for function\n");
   }

  /*Gets the type tag of the return type*/
  TYPETAG tag = ty_query(newtype);

  /*Checks the return type*/
  if(tag == TYUNION || tag == TYENUM || tag == TYSTRUCT || tag == TYARRAY || tag == TYSET || tag == TYFUNC || tag == TYBITFIELD || tag == TYSUBRANGE || tag == TYERROR)
  {
    /*Error, function must reutrn simple type*/
    error("Function return type must be simple type");
  }
  /*Else check the parameters*/
  else
  {
    /*Copy of the parameter list*/
    PARAM_LIST copy = list;

    /*TYPETAG of the current parameter*/
    TYPETAG ptag;

    /*While loop to check the parameters*/
    while(copy != NULL)
    {
      /*Gets the type tag of the current parameter*/
      ptag = ty_query(copy->type);

      /*Checks the type of the parameter*/
      if(ptag == TYUNION || ptag == TYENUM || ptag == TYSTRUCT || ptag == TYARRAY || ptag == TYSET || ptag == TYFUNC || ptag == TYBITFIELD || ptag == TYSUBRANGE || ptag == TYERROR)
      {
	/*Parameter not simple type error, returns NULL*/
	error("Parameter type must be a simple type");
      }

      /* check for duplicate params */
      check_params(copy);

      /*Gets the next parameter*/
      copy = copy->next;
    }

    /*Create the function and return it*/
    return ty_build_func(newtype, list, TRUE);
  }
} // end make_function

