/* Tree building functions */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "types.h"
#include "tree.h"
#include "symtab.h"
#include "message.h"

/* Function that inserts an ST_ID into a member list */
linkedList insert_id(linkedList list, ST_ID newid)
{
  linkedList new;
  new = (linkedList) malloc(sizeof(linkedListNode));
  
  /*Inserts the element and returns the list*/
  new->id = newid;
  new->next = list;
  return new;
}

/* Function that inserts an index into the index list */
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

/*Function that looks up the type given from a symbol table id*/
TYPE lookup_type(ST_ID id)
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
}

/*Function that creates an array*/
TYPE make_array(INDEX_LIST list, TYPE newtype)
{
  /*If no type, error*/
  if(!newtype)
  {
    /*Error, return null*/
    error("Data type expected for array elements\n"); 
    return NULL;
  }
  /*Else return the array*/
  else 
    return ty_build_array(newtype, list); 
}

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
    error("Duplicate definition of \"%s\" in block %d\n", st_get_id_str(iden), st_get_cur_block());
  /*Debugging*/
  else if(debug) 
    printf("TYPE name %s installed\n", st_get_id_str(iden));
}

/* Function that makes a variable data record and installs it in the symbol table */
void make_var(linkedList list, TYPE newtype)
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
    error("Variable(s) must be of data type\n"); 
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
    if(!resolved) 
      error("Duplicate variable declaration: \"%s\"\n", st_get_id_str(list->id));
    /*Debugging information*/
    else if(debug)
    {
      /*Print debugging statements*/
      printf("GDECL created with type:\n");
      ty_print_type(newtype);
      printf("\n");
    }
    /*Move on to the next item in the member list*/
    list=list->next;
  }
}

/* Function that takes a member list and assigns a type to each member */
MEMBER_LIST type_members(linkedList list, TYPE newtype)
{
  /*Member list*/
  MEMBER_LIST memList, p;
  memList = createMemberListFromID(list);
  p = memList;

  /*If empty list, bug*/
  if(!p) 
    bug("Empty list passed to add members");

  /*While there are still elements in the list*/
  while(p) 
  {
    /*Sets the type of the element, moves on to the next element*/
    p->type = newtype;
    p=p->next;
  }

  /*Returns the member list*/
  return memList;
}

/*Function that adds two member lists together*/
MEMBER_LIST combine_members(MEMBER_LIST list1, MEMBER_LIST list2)
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
}

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
}

/* Function that evaluates the value of an identifier */
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

  /*Debugging information*/
  if(debug)
  {
    /*Prints Debugging information*/
    printf("INTCONST %d created\n",(int)n);
  }

  /*Returns the node*/
  return p;
}

/*Function that makes a real node*/
ST make_real(double n)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));
	
  /*Sets the value of the node attributes*/
  p->tag = REALCONST;
  p->u.realconst = n;

  /*Debugging information*/
  if(debug) 
  {
    /*Print debugging information*/
    printf("REALCONST %f created\n",n);
  }

  /*Returns the real node*/
  return p;
}

/*Function that makes an identifier node*/
ST make_id(ST_ID iden)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node attributes*/
  p->tag = ID_NODE;
  p->u.id_node.id = iden;

  /*Debugging information*/
  if(debug) 
  {
    /*Print debugging information*/
    printf("ID NODE %s created\n", st_get_id_str(iden) );
  }

  /*Returns the node*/
  return p;
}

/*Function that makes a unary operator node*/
ST make_unop(char c, ST a)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));
  
  /*Sets the value of the node attributes*/
  p->tag = UNOP;
  p->u.unop.op = c;
  p->u.unop.arg = a;

  /*Debugging information*/
  if(debug) 
  {
    /*Prints Debugging information*/
    printf("UNOP %c created\n",c);
  }

  /*Returns the node*/
  return p;
}

/*Function that makes a binary operator node*/
ST make_binop(ST a1, char c, ST a2)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node attributes*/
  p->tag = BINOP;
  p->u.binop.op = c;
  p->u.binop.arg1 = a1;
  p->u.binop.arg2 = a2;

  /*Debugging information*/
  if(debug) 
  {
    /*Prints Debugging information*/
    printf("BINOP %c created\n",c);
  }

  /*Returns the node*/
  return p;
}

/*Function that inserst an id into the parameter list*/
PARAM_LIST insert_id_into_param(PARAM_LIST list, ST_ID id, BOOLEAN isRef)
{
  /*Creates the list and allocates memory*/
  PARAM_LIST newList;
  newList = (PARAM_LIST) malloc(sizeof(PARAM));
  
  /*Inserts the element and returns the list*/
  newList->id = id;
  newList->next = list;
  newList->is_ref = isRef;
  return newList;
}

/*Converts a member list to a list of parameters*/
PARAM_LIST convertMemberListToParams(MEMBER_LIST list, BOOLEAN isRef)
{
  /*Checks for empty member list*/
  if(!list)
  {
    /*Prints bug and returns*/
    bug("Empty member list passed to convert to parameter list");
    return NULL;
  }

  /*Member list*/
  MEMBER_LIST mList = list;

  /*Creates parameter lists*/
  PARAM_LIST newList, retList = NULL;

  /*While there are still elements in the member list*/
  while(mList)
  {
    /*If null allocate membory for parameter list*/
    if(newList = NULL)
    {
      /*Allocate memory*/
      newList = (PARAM_LIST)malloc(sizeof(PARAM));

      /*Gets the first element of the member list*/
      newList->id = mList->id;
      newList->type = mList->type;
      newList->is_ref = isRef;

      /*Keep return pointer to the first element in the list*/
      retList = newList;
    }
    /*Else act normally*/
    else
    {
      /*Creates a new element for the parameter list and allocates memory*/
      PARAM_LIST newElement = (PARAM_LIST)malloc(sizeof(PARAM));
      
      /*Sets the information for the new element*/
      newElement->id = mList->id;
      newElement->type = mList->type;
      newElement->is_ref = isRef;
      newElement->prev = newList;

      /*Sets the parameter list pointers*/
      newList->next = newElement;
      newList = newElement;
    }

    /*Moves on to the next element in the members list*/
    mList = mList->next;
  }

    /*Returns the parameter list*/
    return retList;
}

/*Function that sets the type for all of the elements in the parameter list*/
PARAM_LIST type_params(PARAM_LIST list, TYPE newtype, BOOLEAN isRef)
{
  /*Parameter list*/
  PARAM_LIST p = list;

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
  return list;
}

/*Function that combines two parameter lists*/
PARAM_LIST combine_params(PARAM_LIST list1, PARAM_LIST list2)
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
}

/*Function that creates a function type*/
TYPE make_func(PARAM_LIST list, TYPE newtype)
{
   /*Checks to see if the parameter list exists*/
   if(!list)
   {
      /*Error, return NULL*/
      error("Parameter list expected to create function");
      return NULL;
   }
   /*Else create the function and return it*/
   else
   {
      /*Returns the built function*/
      return ty_build_func(newtype, list, TRUE);
   }
}

/*Function that creates a procedure type*/
TYPE make_proc(PARAM_LIST list)
{
   /*Checks to see if the parameter list exists*/
   if(!list)
   {
      /*Error, return NULL*/
      error("Parameter list expected to create function");
      return NULL;
   }
   /*Else create the function and return it*/
   else
   {
      /*Creates a type variable*/
      TYPE newtype = ty_build_basic(TYVOID);

      /*Returns the built function*/
      return ty_build_func(newtype, list, TRUE);
   }
}

/*Function that creates a member list from the linked list of ST_ID's*/
MEMBER_LIST createMemberListFromID(linkedList list)
{
  /*Linked List*/
  linkedList copy = list;

  /*Creates the member lists and allocates memory*/
  MEMBER_LIST memList;
  memList = malloc(sizeof(MEMBER));

  /*While there are still elements in the list*/
  while(copy != NULL)
  {
    /*Calls the function to insert the ID into the member list*/
    insertMember(memList, copy->id);

    /*Moves on to the next element*/
    copy = copy->next;
  }

  /*Returns the member list*/
  return memList;
}

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
    toReturn->prev = NULL;

    /*Returns the list*/
    return toReturn;
  }

  /*Insert somewhere in the middle of the list*/
  previous->next = (MEMBER_LIST)malloc(sizeof(MEMBER));
  previous = previous->next;
  previous->id = id;
  previous->next = mList;
  mList->prev = previous;
}
