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

/* Inserts an ST_ID into a linked list */
ID_LIST id_prepend(ID_LIST list, ST_ID newid)  // exported
{
  ID_LIST new;
  new = (ID_LIST) malloc(sizeof(ID_NODE));
  
  /* Inserts the element and returns the list */
  new->id = newid;
  new->next = list;
  return new;
  
} // end id_prepend

/* Inserts an index into the index list */
INDEX_LIST index_append(INDEX_LIST list, TYPE newtype)  // exported
{
  /* Works by using the prev field to link back to the current list */
  /* Creates the index list */
  INDEX_LIST new, p;

  /* Checks to see if the new type exists */
  if(!newtype)
  {
    /* Error and returns */
    error("Illegal index type (ignored)");	
    return list;
  }
  else
  {
    new = (INDEX_LIST) malloc(sizeof(INDEX));
  
    /* Initialize new */	
    new->type = newtype;
    new->next = NULL;

    p=list;
    if (p) {	
	if (debug) printf("List not empty\n");	/* If list exists */
	while(p->next) p=p->next; 	/* Go to end of list */
    	p->next=new;			/* Append new */
	return list;			/* Return current list */
    }
    else return new;			/* Otherwise just return new */
  }
} // end index_append

/* Looks up the type given from a symbol table id */
TYPE check_typename(ST_ID id)  // exported
{
  /* Data record from the symbol table and block number */
  ST_DR data_rec;
  int block;

  /* Gets the data record from the symbol table */
  data_rec = st_lookup(id, &block); 
  
  /* If no record is found */
  if (!data_rec) 
  {
    /* Undeclared type error, return error type */
    error("Undeclared type name: \"%s\"",st_get_id_str(id));
    return ty_build_basic(TYERROR);
  }
  
  /* Debugging information */
  else if(debug) 
  { 
    /* Print the debugging information */
    printf("Denoter typename: %s\n", st_get_id_str(id) );
    printf("Reference TYPE:\n");
    ty_print_type(data_rec->u.typename.type); 
    printf("\n");
  }
  /* Returns the type of the data record */
  return data_rec->u.typename.type; 
} // end check_typename 

/* Creates a subrange */
TYPE make_subrange(EXPR a, EXPR b)  // exported
{
  long low, high;

  if (a->tag!=INTCONST) {
	error("Subrange lower index is not Integer");
	return NULL;
  }
  if (b->tag!=INTCONST) {
	error("Subrange upper index is not Integer");
	return NULL;
  }

  low=a->u.intval;
  high=b->u.intval;

  /* If subrange is invalid, error */
  if(low>high)
  {
    /* Error, return null */
    error("Empty subrange in array index");
    return NULL;
  }
  /* Else build the subrange */
  else
  {
    /* Debugging information */
    if(debug) printf("Building subrange of INT from %d to %d\n", (int)low, (int)high);

    return ty_build_subrange(ty_build_basic(TYSIGNEDLONGINT), low, high);
  }
} // end make_subrange

/* Creates an array */
TYPE make_array(INDEX_LIST list, TYPE newtype)  // exported
{
  /* If no type, error */
  if(!newtype)
  {
    /* Error, return error type */
    error("Data type expected for array elements"); 
    return ty_build_basic(TYERROR);
  }
  /* Else return the array */
  else 
  {
    /* Debugging information */
    if(debug) 
    {
      /* Prints debugging information */
      printf("Build array of TYPE:\n");
      ty_print_type(newtype); 
      printf("\n");
    }

    /* Gets the type tag of the type of the elements of the array */
    TYPETAG tag = ty_query(newtype);

    /* Checks the type */
    if(tag == TYUNION || tag == TYENUM || tag == TYSTRUCT || tag == TYSET || tag == TYFUNC || tag == TYBITFIELD || tag == TYSUBRANGE || tag == TYERROR)
    {
      /* Data type expected for array elements, returns error type */
      error("Data type expected for array elements");
      return ty_build_basic(TYERROR);
    }
    else
    {
      /* Returns the array TYPE */
      return ty_build_array(newtype, list);
    }
  }
} // end make_array

/* Makes a type data record and installs it in the symbol table */
void make_type(ST_ID iden, TYPE newtype)  // exported
{
  /* Creates the data record and allocates memory */
  ST_DR p;
  p = stdr_alloc();
  BOOLEAN resolved;	

  /* Return if the type does not exist */
  if(!newtype) 
  {
    /* Debugging information */
    if(debug) 
      printf("Type does not exist\n");
	
    /* Return */
    return;
  }

  /* Sets the data record attributes and installs the type in the symbol table */
  p->tag = TYPENAME;
  p->u.typename.type = newtype;

  /* Checks to see if the type has been resolved */
  resolved = st_install(iden, p);

  /* If the type has not been resolved error */
  if(!resolved) 
    error("Duplicate definition of \"%s\" in block %d", st_get_id_str(iden), st_get_cur_block());
  /* Debugging */
  else if(debug) 
    printf("TYPE name %s installed\n", st_get_id_str(iden));
} // end make_type

/* Makes a variable data record and installs it in the symbol table */
void make_var(ID_LIST list, TYPE newtype)  // exported
{
  /* Symbol table data record and boolean variable */
  ST_DR p;  
  BOOLEAN resolved;
  
  /* If the member list is empty, error */
  if(!list) 
    bug("Empty list passed to make_var");

  /* If undefined type, error */
  if(!newtype) 
  {
    /* Error, return */
    error("Variable(s) must be of data type"); 
    return;
  }

  /* Gets the type tag of the type of the elements of the array */
  TYPETAG tag = ty_query(newtype);

  /* Checks the type */
  if(tag == TYFUNC || tag == TYERROR)
  {
      /* Data type expected for array elements, returns null */
      error("Variable(s) must be of data type");

      /* Even when there is an error, Fenner installs the variable in the symbol table */
      p = stdr_alloc();	  
      p->tag = GDECL;
      p->u.decl.type = newtype;
      p->u.decl.sc = NO_SC;

      /* If the type is error, set the declaration error to true */
      if(tag == TYERROR)
	p->u.decl.err = TRUE;

      /* Installs the variable in the symbol table */
      resolved = st_install(list->id, p);

      /* Return */
      return;
  }
	
  /* While the list is not null */
  while(list) 
  {
    /* Allocates memory for the data record, sets the tag, sets the type */
    p = stdr_alloc();	  
    p->tag = GDECL;
    p->u.decl.type = newtype;

    /* Installs the variable in the symbol table */
    resolved = st_install(list->id, p); 

    /* If the type is not resolved error */
    if(!resolved) 
      error("Duplicate variable declaration: \"%s\"", st_get_id_str(list->id));
    /* Else variable resolved */
    else 
    {
      /* Calls the encoding function */
      declareVariable(list->id, newtype);
	
      /* Debugging */
      if(debug)
      {
	/* Print debugging statements */
	printf("GDECL created with type:\n");
	ty_print_type(newtype);
	printf("\n");
      }
    } // end else

    /* Move on to the next item in the member list */
    list=list->next;
  }
} // end make_var

/* Resolves pointers */
void resolve_ptr_types()  // exported
{
  /* Symbol table data record, type variables, symbol table id, block number, and boolean variable */
  ST_DR data_rec;
  TYPE list, ptr, next;
  ST_ID id;
  int block;
  BOOLEAN resolved;

  /* Debugging information */
  if(debug) 
    printf("Attempting to resolve pointers.\n");

  /* Gets the list of unresolved pointers */
  list = ty_get_unresolved();

  /* If the list is empty no unresolved pointers */
  if(!list) 
  {
    /* Debugging information */
    if(debug) 
      printf("No unresolved pointers\n");
    
    /* Return */
    return;
  }

  /* While there are still elements in the list */
  while(list) 
  {
    /* Gets the pointer, id, and next */  
    ptr = ty_query_ptr(list, &id, &next);	
	  
    /* Gets the data record for the id */
    data_rec = st_lookup(id, &block); 
		
    /* If the data record does not exist, error */
    if(!data_rec) 
      error("Unresolved type name: \"%s\"",st_get_id_str(id));
				
    /* Else attempt to resolve the pointer */
    else
    {
      /* Try to resolve the type of the pointer */
      resolved = ty_resolve_ptr(list, data_rec->u.typename.type);

      /* If the pointer is not resolved, error */
      if(!resolved) 
	error("Unresolved pointer");
      /* Debugging information */
      else if(debug) 
      {
	/* Debugging information */
	printf("Resolved %s ptr with type: \n",st_get_id_str(id));
	ty_print_type(data_rec->u.typename.type);
	printf("\n");
      }
    }

    /* Move on to the next item in the list */
    list = next;
  } 	
} // end resolve_ptrs

/*  Inserts an ID into a member list */
MEMBER_LIST insertMember(MEMBER_LIST mList, ST_ID id)  // internal
{
  /* Member list pointers */
  MEMBER_LIST previous = NULL;
  MEMBER_LIST toReturn = mList;

  /* While loop to determine where the node should be placed */
  while(mList != NULL)
  {
    /* Goes to the next node in the list */
    previous = mList;
    mList = mList->next;
    
  }

  /* If the previous node is equal to null, insert at front */
  if(previous == NULL)
  {
    /* Create the node and insert it */
    toReturn = (MEMBER_LIST)malloc(sizeof(MEMBER));
    toReturn->id = id;
    toReturn->next = mList;

    /* Returns the list */
    return toReturn;
  }

  /* Insert somewhere in the middle of the list */
  previous->next = (MEMBER_LIST)malloc(sizeof(MEMBER));
  previous = previous->next;
  previous->id = id;
  previous->next = mList;

  /* Returns the list */
  return toReturn; 
} // end insertMember

/*  Creates a member list from the linked list of ST_ID's */
MEMBER_LIST createMemberListFromID(ID_LIST list)  // internal
{
  /* Linked List */
  ID_LIST copy = list;

  /* Creates the member lists and allocates memory */
  MEMBER_LIST memList = NULL;

  /* While there are still elements in the list */
  while(copy != NULL)
  {
    /* Calls the function to insert the ID into the member list */
    memList = insertMember(memList, copy->id);

    /* Moves on to the next element */
    copy = copy->next;

  }

  /* Returns the member list */
  return memList;
} // end 

/* Takes a member list and assigns a type to each member */
MEMBER_LIST make_members(ID_LIST list, TYPE newtype)  // exported
{
  /* Member list */
  MEMBER_LIST memList, p;
  memList = createMemberListFromID(list);
  p = memList;

  /* If empty list, bug */
  if(!p) 
    bug("Empty list passed to add members");

  /* Debugging */
  if(debug) 	
  {
    /* Prints debugging statements */
    printf("Typing members with TYPE:\n");
    ty_print_type(newtype);
    printf("\n");
  }

  /* While there are still elements in the list */
  while(p) 
  {
    /* Sets the type of the element, moves on to the next element */
    p->type = newtype;
    p=p->next;
  }

  /* Returns the member list */
  return memList;
} // end make_members

/* Adds two member lists together */
MEMBER_LIST member_concat(MEMBER_LIST list1, MEMBER_LIST list2)  // exported
{
  /* Member list */
  MEMBER_LIST p = list1;

  /* If no member list, bug */
  if(!p) 
    bug("Empty list passed to combine members");

  /* While there are still elements in the first member list, add them to the beginning of the second */
  while(p->next) 
    p=p->next;

  /* Links end of the first list and the beginning of the second */
  p->next = list2;	
  
  /* Returns the list */
  return list1;
} // end member_concat

/* Inserts an ID into a parameter list */
PARAM_LIST insertParam(PARAM_LIST pList, ST_ID id, BOOLEAN isRef)  // internal
{
  /* Parameter list pointer, allocates memory */
  PARAM_LIST toReturn;
  toReturn = malloc(sizeof(PARAM));

  /* Sets attributes */
  toReturn->id = id;
  toReturn->is_ref = isRef;
  toReturn->sc = NO_SC;
  toReturn->next = pList;
  
  /* Returns the list */
  return toReturn;
} // end insertParam

/* Converts a linked list to a list of parameters */
PARAM_LIST createParamListFromID(ID_LIST list, BOOLEAN isRef)  // internal
{
  /* Checks for empty member list */
  if(!list)
  {
    /* Prints bug and returns */
    bug("Empty linked list passed to convert to parameter list");
    return NULL;
  }

  /* Member list */
  ID_LIST cList = list;

  /* Creates parameter lists */
  PARAM_LIST retList = NULL;

  /* While there are still elements in the member list */
  while(cList)
  {
    /* Inserts the element into the parameter list and moves on to the next element */
    retList = insertParam(retList, cList->id, isRef);
    cList = cList->next;
  }

    /* Returns the parameter list */
    return retList;
} // end

/* Sets the type for all of the elements in the parameter list */
PARAM_LIST make_params(ID_LIST list, TYPE newtype, BOOLEAN isRef)  // exported
{
  /* Parameter list */
  PARAM_LIST p, paramList = createParamListFromID(list, isRef);
  p = paramList;

  /* If empty list, bug */
  if(!p) 
    bug("Empty list passed to add type parameters");

  /* While there are still elements in the list */
  while(p) 
  {
    /* Sets the type of the element, moves on to the next element */
    p->type = newtype;
    p->is_ref = isRef;
    p=p->next;
  }

  /* Returns the parameter list */
  return paramList;
} // end make_params

/* Combines two parameter lists */
PARAM_LIST param_concat(PARAM_LIST list1, PARAM_LIST list2)  // exported
{
  /* Parameter list */
  PARAM_LIST p = list1;

  /* If no member list, bug */
  if(!p) 
    bug("Empty list passed to combine parameters");

  /* While there are still elements in the first member list, add them to the beginning of the second */
  while(p->next) 
    p=p->next;

  /* Links end of the first list and the beginning of the second */
  p->next = list2;	
  
  /* Returns the list */
  return list1;
} // end param_concat

/* Checks for duplicate params */
void check_params(PARAM_LIST list)  // internal
{
  /* Copies of the param list */
  PARAM_LIST p = list;

  /* ST id's */
  ST_ID a,b;

  /* Gets the first id off the list */
  a = p->id;

  /* Debugging */
  if(debug) 
    printf("String ID: %s\n", st_get_id_str(a) );
  
  /* While loop that goes through the list */
  while(p->next) 
  {
    /* Gets the next element in the list and the id */
    p = p->next;
    b = p->id;

    /* Debugging */
    if(debug) 
      printf("Compare with string ID: %s\n", st_get_id_str(b) );

    /* If duplicate parameters */
    if(a==b) 
      error("Duplicate parameter name: \"%s\"", st_get_id_str(a) );
   }
} // end check_params

/* Creates a function type */
TYPE make_func(PARAM_LIST list, TYPE newtype)  // exported
{
   /* Checks to see if the parameter list exists */
   if(!list)
   {
      /* Not an error: function with no params */
      if (debug) printf("Empty parameter list for function\n");
   }

  /* Gets the type tag of the return type */
  TYPETAG tag = ty_query(newtype);

  /* Checks the return type */
  if(tag == TYUNION || tag == TYENUM || tag == TYSTRUCT || tag == TYARRAY || tag == TYSET || tag == TYFUNC || tag == TYBITFIELD || tag == TYSUBRANGE || tag == TYERROR)
  {
    /* Error, function must reutrn simple type */
    error("Function return type must be simple type");
  }
  /* Else check the parameters */
  else
  {
    /* Copy of the parameter list */
    PARAM_LIST copy = list;

    /* TYPETAG of the current parameter */
    TYPETAG ptag;

    /* While loop to check the parameters */
    while(copy != NULL)
    {
      /* Gets the type tag of the current parameter */
      ptag = ty_query(copy->type);

      /* Checks the type of the parameter */
      if(ptag == TYUNION || ptag == TYENUM || ptag == TYSTRUCT || ptag == TYARRAY || ptag == TYSET || ptag == TYFUNC || ptag == TYBITFIELD || ptag == TYSUBRANGE || ptag == TYERROR)
      {
	/* Parameter not simple type error, returns NULL */
	error("Parameter type must be a simple type");
      }

      /* check for duplicate params */
      check_params(copy);

      /* Gets the next parameter */
      copy = copy->next;
    }

    /* Create the function and return it */
    return ty_build_func(newtype, list, TRUE);
  }
} // end make_function

/* Reverses an expr list */
EXPR_LIST expr_list_reverse(EXPR_LIST list)
{
	EXPR_LIST revList, next;
	revList, next = (EXPR_LIST) malloc(sizeof(EXPR_LIST_NODE));
	
	revList = NULL;	

	/* Loops through list changing the order */
	while(list != NULL ) 
	{ 
		next = list->next;  	/* Get the next node */ 
		list->next = revList;  	/* Link current node to reversed list */
		revList = list; 		/* Set reversed list to current node */
		list = next; 			/* Update current node */
	} 

	/* Returns reversed list */
	return revList;
} // end expr_list_reverse

/* Inserts an ST_ID into a linked list */
EXPR_LIST expr_prepend(EXPR_LIST list, EXPR expr)  // exported
{
  EXPR_LIST new;
  new = (EXPR_LIST) malloc(sizeof(EXPR_LIST_NODE));
  
  /* Inserts the element and returns the list */
  new->expr = expr;
  new->next = list;
  return new;
} // end expr_prepend

/* Processes variable declarations */
int process_var_decl(ID_LIST ids, TYPE type, int cur_offset)
{
	// not implemented yet
}

/* Checks subranges */
TYPE check_subrange(EXPR lo, EXPR hi)
{
	// not implemented yet
} // end check_subranges 

/* Builds function declarations */
void build_func_decl(ST_ID id, TYPE type, DIRECTIVE dir)
{
  /* Creates the data record */
  ST_DR p;
  BOOLEAN resolved;
    
  /* Return if the type does not exist */
  if(!type) 
  {
    /* Debugging information */
    if(debug) 
      printf("Type for function declaration does not exist\n");
	
    /* Return */
    return;
  }
  
  p = stdr_alloc();	  
  p->tag = GDECL;
  p->u.decl.type = type;
  p->u.decl.is_ref = FALSE;
  p->u.decl.v.global_func_name = st_get_id_str(id);

  /* If directive is external, set storage class to SC_EXTERN */
  if(dir == DIR_EXTERNAL)
    p->u.decl.sc = EXTERN_SC;
  /* Else if directive is forward set storage class to NO_SC */
  else if(dir == DIR_FORWARD)
    p->u.decl.sc = NO_SC;
  /* Not external or forward, error */
  else
    error("Invalid directive: \"%s\"", dir);
    
  /* Installs the function in the symbol table */
  resolved = st_install(id, p); 

  /* If the function is not resolved error */
  if(!resolved) 
    error("Duplicate function declaration: \"%s\"", st_get_id_str(id));
  /* Else function resolved */
  else 
  {	
    /* Debugging */
    if(debug)
    {
      /* Print debugging statements */
      printf("GDECL created with type:\n");
      ty_print_type(type);
      printf("\n");
    }
  } // end else

  /* Return */
  return;  
} // end build_func_decl

/* Called when a function is entered */
char * enter_function(ST_ID id, TYPE type, int * local_var_offset)
{
	// not implemented yet
} // end enter_function

/* Creates an integer constant expression node */
EXPR make_intconst_expr(long val, TYPE type)  // exported
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the values of the node */
  eNode->tag = INTCONST;
  eNode->u.intval = val;
  eNode->type = type;
 
  /* Returns the node */
  return eNode;
} // end make_intconst_expr

/* Creates a real constant expression node */
EXPR make_realconst_expr(double val)  // exported
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the values of the node */
  eNode->tag = REALCONST;
  eNode->u.realval = val;
  eNode->type = ty_build_basic(TYDOUBLE);

  /* Returns the node */
  return eNode;
} // end make_realconst_expr

/* Creates a string constant expression node */
EXPR make_strconst_expr(char * str)  // exported
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the values of the node */
  eNode->tag = STRCONST;
  eNode->u.strval = str;
  eNode->type = ty_build_ptr(NULL, ty_build_basic(TYSIGNEDCHAR) );

  /* Returns the node */
  return eNode;
} // end make_strconst_expr

/* Creates an id expression node */
EXPR make_id_expr(ST_ID id)  // exported
{
  /* Symbol table data record and block number */
  ST_DR record;
  int blockNum;

  /* Gets the ST_DR for the type */
  record = st_lookup(id, &blockNum);

  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the values of the node */
  eNode->tag = GID;
  eNode->type = record->u.decl.type;
  eNode->u.gid = id;

  /* Returns the node */
  return eNode;
} // end make_id_expr

/* Makes a null operation expression node */
EXPR make_null_expr(EXPR_NULLOP op)  // exported
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = NULLOP;
  eNode->type = NULL;
  eNode->u.nullop.op = op;

  /* Returns the node */
  return eNode;
} // end make_null_expr

/* Makes a unary operator expression node */
EXPR make_un_expr(EXPR_UNOP op, EXPR sub)  // exported
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = UNOP;
  eNode->type = NULL;
  eNode->u.unop.op = op;
  eNode->u.unop.operand = sub;

  /* Returns the node */
  return eNode;
} // end make_un_expr

/* Makes a binary operator expression node */
EXPR make_bin_expr(EXPR_BINOP op, EXPR left, EXPR right)  // exported
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = BINOP;
  eNode->type = NULL;
  eNode->u.binop.op = op;
  eNode->u.binop.left = left;
  eNode->u.binop.right = right;

  /* Returns the node */
  return eNode;
} // end make_bin_expr

/* Makes a function call expression node */
EXPR make_fcall_expr(EXPR func, EXPR_LIST args)
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = FCALL;
  eNode->type = NULL;
  eNode->u.fcall.args = args;
  eNode->u.fcall.function = func;

  /* Returns the node */
  return eNode;
} // end make_fcall_expr

/* Makes an error expression node */
EXPR make_error_expr()
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = ERROR;
  eNode->type = NULL;

  /* Returns the node */
  return eNode;
} // end make_error_expr

/* Negates a number if its sign op is negative */
EXPR apply_sign_to_number(EXPR_UNOP op, EXPR num)  // exported
{
	/* Checks the tag, if intconst */
	if(num->tag == INTCONST)
	{
		/* If the sign is negative */
		if(op == NEG_OP)
			num->u.intval = num->u.intval * -1;
	}
	/* If realconst */
	else if(num->tag == REALCONST)
	{
		/* If the sign is negative */
		if(op == NEG_OP)
			num->u.realval = num->u.realval * -1;
	}
	/* If not intconst or realconst */
	else
		error("Failed to check sign, number is not INTCONST or REALCONST");
	
	/* Returns the node */
	return num;
} // end apply_sign_to_number

/* Checks for assignment or procedure call */
EXPR check_assign_or_proc_call(EXPR lhs, ST_ID id, EXPR rhs)
{
	// not implemented yet

	EXPR new = NULL;

	return new;
} // end check_assign_or_proc_call

/* Returns whether an expr is an lval */
BOOLEAN is_lval(EXPR expr)
{
	// not implemented yet
} // end is_val

/* Frees an expression */
void expr_free(EXPR expr)  // exported
{
  /* If the expression is an intconst, real, or string const */
  if(expr->tag == INTCONST || expr->tag == STRCONST || expr->tag == REALCONST || expr->tag == GID || expr->tag == LFUN || expr->tag == LVAR || expr->tag == NULLOP || expr->tag == ERROR)
  {
    /* Frees the expression */
    free(expr);
  }
  /* Else if unary operation */
  else if(expr->tag == UNOP)
  {
    /* Frees the expression of the operand, frees the expression */
    if(expr->u.unop.operand)
      expr_free(expr->u.unop.operand);
    free(expr);
  }
  /* Else if binary operation */
  else if(expr->tag == BINOP)
  {
    /* Frees the operands, frees the expression */
    if(expr->u.binop.left)
      expr_free(expr->u.binop.left);
    if(expr->u.binop.right)
      expr_free(expr->u.binop.right);
    free(expr);
  }
  /* Else if function call */
  else if(expr->tag == FCALL)
  {
    /* Frees the reference to the function, frees the arguments list, frees the expression */
    if(expr->u.fcall.function)
      expr_free(expr->u.fcall.function);
    if(expr->u.fcall.args)
      expr_list_free(expr->u.fcall.args);
    free(expr);
  }
} // end expr_free

/* Frees up an expression list */
void expr_list_free(EXPR_LIST list)  // exported
{
  /* Frees the expression, recursive call, frees the final list */
  if(list->expr)
    expr_free(list->expr);
  if(list->next)
    expr_list_free(list->next);
  free(list);
} // end expr_list_free

/* Frees up a linked list */
void id_list_free(ID_LIST list)  // exported
{
  /* If the next element exists */
  if(list->next)
  {
    /* Frees the next node in the list */
    id_list_free(list->next);
  }

  /* Frees the list */
  free(list);
} // end id_list_free

/* Function that checks the function declaration */
NAME_OFFSET check_func_decl(FUNC_HEAD fC)
{
  /* Variable that represents the current block number */
  int block;

  /* Symbol table data record */
  ST_DR datRec;
  
  /* Looks for the data record in the symbol table */
  datRec = st_lookup(fC.id, &block);

  /* If data record is not found install it in the symbol table */
  if(datRec == NULL)
  {
    /* Allocates memory for the new data record */
    datRec = stdr_alloc();

    /* Sets the attributes of the data record */
    datRec->tag = FDECL;
    datRec->u.decl.type = fC.type;
    datRec->u.decl.sc = NO_SC;
    datRec->u.decl.is_ref = FALSE;

    /* Installs the data record */
    st_install(datRec, fC.id);
  }
  /* If data record is found check the record */
  else
  {
    /* Checks the tag and storage class */
    if(datRec->tag != GDECL || datRec->u.decl.sc != NO_SC)
    {
      /* Error, duplicate delcaration */
      error("Duplicate variable declaration");

      /* Return */
      return;
    }
    /* Else change the tag */
    else
    {
      /* Changes the tag */
      datRec->tag = FDECL;
    }
  }

  /* Function checks out, so we enter a new block */
  st_enter_block();

  /* Parameter list, type, and boolean variable for function query */
  PARAM_LIST fParams;
  BOOLEAN checkArgs;
  TYPE funcRetType;

  /* Gets the ret type, param list, etc */
  funcRetType = ty_query_func(fC.type, &fParams, &checkArgs);

  /* Installs the parameters */
  install_local_params(fParams);

  /* Creates the name offset */
  NAME_OFFSET nOff;
  nOff.name = st_get_id_str(fC.id);
  /* Set the offset value here, but I do not know how */

  /* Return the name offset */
  return nOff;
} // end check_func_decl

/* Function that installs local parameters */
void install_local_params(PARAM_LIST pList)
{
  /* Copy of the parameter list */
  PARAM_LIST copy = pList;

  /* While there are still elements in the list */
  while(copy != NULL)
  {
    /* Creates the symbol table data record */
    ST_DR datRec;
    datRec = stdr_alloc();

    /* Sets the attributes of the symbol table data record */
    datRec->tag = PDECL;
    datRec->u.decl.type = copy->type;
    datRec->u.decl.sc = copy->sc;
    datRec->u.decl.is_ref = copy->is_ref;
    datRec->u.decl.err = copy->err;

    /* Installs the parameter */
    st_install(datRec, copy->id);

    /* Moves on to the next element */
    copy = copy->next;
  }
} // end install_local_params
