/*  Tree building functions */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "defs.h"
#include "types.h"
#include "tree.h"
#include "symtab.h"
#include "message.h"
#include "encode.h"
#include "backend.h"

/*   Inserts an ST_ID into a linked list */
ID_LIST id_prepend(ID_LIST list, ST_ID newid) 
{
  ID_LIST new;
  new = (ID_LIST) malloc(sizeof(ID_NODE));
  
  /*  Inserts the element and returns the list */
  new->id = newid;
  new->next = list;
  return new;
  
}/* End id_prepend */

/* Inserts an index into the index list */
INDEX_LIST index_append(INDEX_LIST list, TYPE newtype)
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
}/* End index_append */

/* Looks up the type given from a symbol table id */
TYPE check_typename(ST_ID id)  
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
}/* End check_typename */ 

/* Creates a subrange */
TYPE make_subrange(EXPR a, EXPR b)  
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
}/* End make_subrange */

/* Creates an array */
TYPE make_array(INDEX_LIST list, TYPE newtype)  
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
}/* End make_array */

/* Makes a type data record and installs it in the symbol table */
void make_type(ST_ID iden, TYPE newtype)  
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
}/* End make_type */

/* Makes a variable data record and installs it in the symbol table */
void make_var(ID_LIST list, TYPE newtype)  
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
      
      p->u.decl.type = newtype;
      p->u.decl.sc = NO_SC;
      p->tag = GDECL;

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
    }/* End else */

    /* Move on to the next item in the member list */
    list=list->next;
  }
}/* End make_var */

/* Resolves pointers */
void resolve_ptr_types()  
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
}/* End resolve_ptrs */

/*  Inserts an ID into a member list */
MEMBER_LIST insertMember(MEMBER_LIST mList, ST_ID id)
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
}/* End insertMember */

/*  Creates a member list from the linked list of ST_ID's */
MEMBER_LIST createMemberListFromID(ID_LIST list)
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
}/* End */ 

/* Takes a member list and assigns a type to each member */
MEMBER_LIST make_members(ID_LIST list, TYPE newtype)  
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
}/* End make_members */

/* Adds two member lists together */
MEMBER_LIST member_concat(MEMBER_LIST list1, MEMBER_LIST list2)  
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
}/* End member_concat */

/* Inserts an ID into a parameter list */
PARAM_LIST insertParam(PARAM_LIST pList, ST_ID id, BOOLEAN isRef)
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
}/* End instertParam */

/* Converts a linked list to a list of parameters */
PARAM_LIST createParamListFromID(ID_LIST list, BOOLEAN isRef)
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
}/* End */

/* Sets the type for all of the elements in the parameter list */
PARAM_LIST make_params(ID_LIST list, TYPE newtype, BOOLEAN isRef)  
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
PARAM_LIST param_concat(PARAM_LIST list1, PARAM_LIST list2)  
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
}/* End check_params */

/* Creates a function type */
TYPE make_func(PARAM_LIST list, TYPE return_type)  
{
   /* Checks to see if the parameter list exists */
   if(!list)
   {
      /* Not an error: function with no params */
      if (debug) printf("Empty parameter list for function\n");
   }

  /* Gets the type tag of the return type */
  TYPETAG tag = ty_query(return_type);

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
	/* Parameter not simple type error */
	error("Parameter type must be a simple type");
      }

      /* check for duplicate params */
      check_params(copy);

      /* Gets the next parameter */
      copy = copy->next;
    }

    /* Create the function and return it */
    return ty_build_func(return_type, list, TRUE);
  }
}/* End make_function */

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
} /* End expr_list_reverse */

/* Inserts an ST_ID into a linked list */
EXPR_LIST expr_prepend(EXPR_LIST list, EXPR expr)  
{
  EXPR_LIST new;
  new = (EXPR_LIST) malloc(sizeof(EXPR_LIST_NODE));
  
  /* Inserts the element and returns the list */
  new->expr = expr;
  new->next = list;
  return new;
}/* End expr_prepend */

/* Processes variable declarations */
int process_var_decl(ID_LIST ids, TYPE type, int cur_offset)
{
  /* Symbol table data record and boolean variable */
  ST_DR p;  
  BOOLEAN resolved;
  
  /* If the member list is empty, error */
  if(!ids) 
    bug("Empty list passed to process_var_decl");

  /* If undefined type, error */
  if(!type) 
  {
    /* Error, return */
    error("Variable(s) must be of data type"); 
    return;
  }

  /* Gets the type tag of the type of the elements of the array */
  TYPETAG tag = ty_query(type);

  /* Checks the type */
  if(tag == TYFUNC || tag == TYERROR)
  {
      /* Data type expected for array elements, returns null */
      error("Variable(s) must be of data type");

      /* Return */
      return;
  }

  /*Checks the tag and aligns if necessar*/
  /*If characters don't align*/
  if(tag == TYSIGNEDCHAR || tag == TYUNSIGNEDCHAR)
  {
  }
  /*Else align the stack*/
  else
  {
    /*Aligns the offset*/
    base_offset_stack[bo_top] = base_offset_stack[bo_top] - getAlignSize(ty_build_basic(tag));
  }
	
  /* While the list is not null */
  while(ids) 
  {
    /*Decreases the offset by the size of the data type*/
    base_offset_stack[bo_top] = base_offset_stack[bo_top] - getSkipSize(type);

    /* Allocates memory for the data record, sets the tag, sets the type */
    p = stdr_alloc();	  
    p->tag = LDECL;
    p->u.decl.type = type;
    p->u.decl.v.offset = base_offset_stack[bo_top];

    /* Installs the variable in the symbol table */
    resolved = st_install(ids->id, p); 

    /* If the type is not resolved error */
    if(!resolved) 
      error("Duplicate variable declaration: \"%s\"", st_get_id_str(ids->id));
    /* Else variable resolved */
    else 
    {
      /* Calls the encoding function */
      /*declareVariable(list->id, newtype); Not sure how to declare a local*/
	
      /* Debugging */
      if(debug)
      {
	/* Print debugging statements */
	printf("LDECL created with type:\n");
	ty_print_type(type);
	printf("\n");
      }
    }/* End else */

    /* Move on to the next item in the member list */
    ids=ids->next;
  } 

  /*I think this is the value that should be returned*/
  return base_offset_stack[bo_top];
}

/* Builds function declarations */
void build_func_decl(ST_ID id, TYPE type, DIRECTIVE dir)
{
  /* Creates the data record */
  ST_DR p;
  BOOLEAN resolved;
  PARAM_LIST params;
  TYPE returntype;
  BOOLEAN check;
    
  /* Bug if the type does not exist */
  if(!type) bug("Type for function declaration does not exist");
  
  p = stdr_alloc();	  
  p->tag = GDECL;
  p->u.decl.is_ref = FALSE;
  p->u.decl.v.global_func_name = get_global_func_name(id);

  /* If directive is external, set storage class to SC_EXTERN */
  if(dir == DIR_EXTERNAL)
  {  p->u.decl.sc = EXTERN_SC;
     returntype = ty_query_func(type, &params, &check);		// change check_args to FALSE
     p->u.decl.type = ty_build_func(returntype, params, FALSE);	// rebuild new function type
  }
  /* Else if directive is forward set storage class to NO_SC */
  else if(dir == DIR_FORWARD)
  {
    p->u.decl.sc = NO_SC;
    p->u.decl.type = type;	// type is unaltered 
  }
  /* Not external or forward, error */
  else
    error("Invalid directive: \"%s\"", dir);
    
  /* Installs the function in the symbol table */
  resolved = st_install(id, p); 

  /* If the function is not resolved error */
  if(!resolved) 
    error("Duplicate forward or external function declaration");
  /* Else function resolved */
  else 
  {	
    /* Debugging */
    if(debug)
    {
      /* Print debugging statements */
      printf("GDECL created with type:\n");
      ty_print_type(p->u.decl.type);
      printf("\n");
    }
  } // end else

  /* Return */
  return;  
}/* End build_func_decl */

/* Called when a function is entered */
int enter_function(ST_ID id, TYPE type, char * global_func_name)
{
  /* Variable that represents the current block number */
  int block;

  /* Symbol table data record */
  ST_DR datRec;

  /*Variables needed for function checking*/
  PARAM_LIST p1, p2;
  BOOLEAN b1, b2;
  TYPE t1, t2;

  t1 = ty_query_func(type, &p1, &b1);

  if (debug) printf ("Entering function %s\n", global_func_name);
  
  /* Looks for the data record in the symbol table */
  datRec = st_lookup(id, &block);

  /* If data record is not found install it in the symbol table */
  if(datRec == NULL)
  {
    if (debug) printf("Function %s not previously declared.\n",global_func_name);

    /* Allocates memory for the new data record */
    datRec = stdr_alloc();

    /* Sets the attributes of the data record */
    datRec->tag = FDECL;
    datRec->u.decl.type = type;
    datRec->u.decl.sc = NO_SC;
    datRec->u.decl.is_ref = FALSE;
    datRec->u.decl.v.global_func_name = global_func_name;

    /* Installs the data record */
    BOOLEAN install = st_install(id, datRec);

    /*Debugging*/
    if(debug)
    {
      if(install == TRUE)
	printf("Installed data record for ID: %s\n",st_get_id_str(id) );

      int block;
      ST_DR rec = st_lookup(id, &block);

      if(rec == NULL)
	printf("...Data Record not found\n");
    }
  } // end new function

  /* If data record is found check the record */
  else	// prior decl
  {
    if (debug) printf("Function %s previously declared.\n",global_func_name);

    /* Checks the tag and storage class */
    if(datRec->tag != GDECL || datRec->u.decl.sc != NO_SC)
    {
      /* Error, duplicate delcaration */
      error("Duplicate function declaration");

      /* Return */
      return;
    }
    /* Else, check the return type */
    else
    {
      /*Gets the return type of the function*/
      t2 = ty_query_func(datRec->u.decl.type, &p2, &b2);

      /*Checks the return type*/
      if(ty_test_equality(t1, t2) == TRUE)
      {
	if (debug) printf("Function %s current and previous DECL match.\n",global_func_name);

	/* Changes the tag, sets func name */
	datRec->tag = FDECL;
	datRec->u.decl.v.global_func_name = global_func_name;
      }
      /*Else error*/
      else
      {
	/*Error, return*/
	error("Incorrect return type");
	return;
      }
    }
  }	// end prior decl stuff

  /*Pushes the id onto the stack*/
  fi_top++;
  if (debug) printf("Incremented function ID top to: %d\n",fi_top);
  func_id_stack[fi_top] = id;

  /* Function checks out, so we enter a new block */
  st_enter_block();
  if (debug) printf("Entering block: %d\n",st_get_cur_block() );

  /*Increments the stack pointer*/
  bo_top++;
  if (debug) printf("Incremented base offset top to: %d\n",bo_top);

  /* initializes formal parameter offset calculation */
  b_init_formal_param_offset();

  /*Checks to see if the function is local by checking the block number*/
  if(st_get_cur_block() > 2)		// functions declared in block 1 are global
  {
    b_store_formal_param(TYPTR);	// shadow param
    if (debug) printf("Shadow param - local block: %d\n", st_get_cur_block() );
  }

  /* Installs the parameters */
  install_local_params(p1);

  /*Gets the initial offset*/
  base_offset_stack[bo_top] = get_local_var_offset();

  if (debug) printf("Current offset on stack is: %d\n",base_offset_stack[bo_top]);

  /*Returns the offset*/
  if (t1=TYVOID) return base_offset_stack[bo_top];
  else return (base_offset_stack[bo_top] -= 8); 		// save space for return value
  
}/* End enter_function */

/* Creates an integer constant expression node */
EXPR make_intconst_expr(long val, TYPE type)  
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the values of the node */
  eNode->tag = INTCONST;
  eNode->u.intval = val;
  eNode->type = type;

  if (debug) printf("Created expr node for INTCONST: %d\n",(int)val);
 
  /* Returns the node */
  return eNode;
}/* End make_intconst_expr */

/* Creates a real constant expression node */
EXPR make_realconst_expr(double val)  
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the values of the node */
  eNode->tag = REALCONST;
  eNode->u.realval = val;
  eNode->type = ty_build_basic(TYDOUBLE);

  if (debug) printf("Created expr node for REALCONST: %f\n",val);

  /* Returns the node */
  return eNode;
}/* End make_realconst_expr */

/* Creates a string constant expression node */
EXPR make_strconst_expr(char * str)  
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the values of the node */
  eNode->tag = STRCONST;
  eNode->u.strval = str;
  eNode->type = ty_build_ptr(NULL, ty_build_basic(TYUNSIGNEDCHAR));

  if (debug) printf("Created expr node for STRCONST: '%s'\n",str);

  /* Returns the node */
  return eNode;
}/* End make_strconst_expr */

/* Creates an id expression node */
EXPR make_id_expr(ST_ID id)  
{
  /* Symbol table data record and block number */
  ST_DR record;
  int blockNum;

  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

    /* Gets the ST_DR for the type */
  record = st_lookup(id, &blockNum);

  if (!record) {
	error("Undeclared identifier \"%s\" in expression", st_get_id_str(id) );
	return make_error_expr();
  }

  if (debug) {
	printf("Looked up data record for ID: %s\n", st_get_id_str(id) );
	printf("Block number is: %d\n",blockNum);
	printf("STDR tag is: %d\n",record->tag);
  }

  /* check if ID is installed as a Typename */
  if (record->tag==TYPENAME) {
    error("Identifier installed as a Typename");
    return make_error_expr();
  }

  /* Sets the values of the node */
  eNode->type = record->u.decl.type;
 
  if (record->tag==GDECL)
  {
    eNode->tag = GID;
    eNode->u.gid = id;
  }

  else if ( (record->tag==LDECL) || (record->tag==LDECL) )
  {
    eNode->tag = LVAR;
    eNode->u.lvar.is_ref = record->u.decl.is_ref;
    eNode->u.lvar.link_count = st_get_cur_block() - blockNum;
    eNode->u.lvar.offset = record->u.decl.v.offset;
  }

  else if (record->tag==FDECL) 
  {
    if (blockNum<=1) {
    	eNode->tag = GID;
    	eNode->u.gid = id;
    }
    else {
	eNode->tag = LFUN;
	eNode->u.lfun.global_name = record->u.decl.v.global_func_name;
        eNode->u.lfun.link_count = st_get_cur_block() - blockNum;
    }
  }

  if (debug) printf("Created expr node for ID: '%s'\n", st_get_id_str(id) );

  /* Returns the node */
  return eNode;
}/* End make_id_expr */

/* Makes a null operation expression node */
EXPR make_null_expr(EXPR_NULLOP op)  
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = NULLOP;
  eNode->u.nullop.op = op;

  if (op==NIL_OP) eNode->type = ty_build_basic(TYVOID);
  else eNode->type = ty_build_basic(TYSIGNEDCHAR);
 
  if (debug) printf("Created expr node for NULLOP: %d\n", op);

  /* Returns the node */
  return eNode;
}/* End make_null_expr */

/* Makes a unary operator expression node */
EXPR make_un_expr(EXPR_UNOP op, EXPR sub)  
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = UNOP;	
  eNode->u.unop.op = op;
  eNode->type = sub->type;
  eNode->u.unop.operand = sub;
   
  /*Variables needed if indirection operator, subrange conversion*/
  ST_ID id;
  TYPE next;
  long low, high;

  /*Debugging*/
  if(debug) 
    printf("Created expr node for UNOP: %d\n", op);

  /* if deref prevent loop */
  if(op==DEREF_OP) 
    return eNode; 
  else if(op == ADDRESS_OP || op == NEW_OP)
  {
    if(is_lval(sub) == FALSE)
    {
      /*Error set node to error expression*/
      error("L value expected for unary operator");
      return make_error_expr();
    }
  }

  /* add deref as necessary */
  if(is_lval(sub)) 
    eNode->u.unop.operand = make_un_expr(DEREF_OP, sub);

  /*Queries the type of the subexpression*/
  TYPETAG subTag = ty_query(sub->type);

  /*If the subexpression type is float or subrange convert*/
  if(subTag == TYFLOAT || subTag == TYSUBRANGE)
  {
    /*Add conversion node, if not already creating conversion node*/
    if(op != CONVERT_OP)
      eNode->u.unop.operand = make_un_expr(CONVERT_OP, sub);
  }

  /*Switch based on the operation*/
  switch(op)
  {
    case CONVERT_OP:
      /*Set type to double if float*/
      if(subTag == TYFLOAT || subTag == TYSIGNEDLONGINT)
	eNode->type = ty_build_basic(TYDOUBLE);
      /*Set type to double if float*/
      else if(subTag == TYSUBRANGE)
	eNode->type = ty_query_subrange(sub->type, &low, &high);
      /*Else illegal conversion, return error expression*/
      else
	error("Illegal conversion");
      break;
    case DEREF_OP:
      break; 
    case NEG_OP:
      /*Type check, error if fails*/
      if(subTag != TYSIGNEDLONGINT && subTag != TYFLOAT && subTag != TYDOUBLE)
	error("Illegal type argument to unary minus");
      else if (subTag==TYSIGNEDLONGINT) 
      break; 
    case ORD_OP:
      /*Type check, error if fails*/

      /*If the string is length one*/
      if(sub->tag==STRCONST) 
      {
 	if (strlen(sub->u.strval)==1)
        {
          /* Sets the values of the node */
          eNode->tag = INTCONST;
          eNode->u.intval = sub->u.strval[0];
          eNode->type = ty_build_basic(TYSIGNEDLONGINT);
	}
	else error("Illegal string pointer for ORD operator");
      }

      else if(subTag != TYSIGNEDLONGINT && subTag != TYUNSIGNEDCHAR)
	error("Illegal type argument to Ord");

      /*Sets the type*/
      eNode->type = ty_build_basic(TYSIGNEDLONGINT);
      break; 
    case CHR_OP:
      /*Type check*/
      if(subTag != TYSIGNEDLONGINT)
	error("Illegal type argument to Chr");
      /*Set type*/
      eNode->type = ty_build_basic(TYUNSIGNEDCHAR);
      break; 
    case UN_SUCC_OP:
      /*Type check, error if fails*/
      if(subTag != TYSIGNEDLONGINT && subTag != TYUNSIGNEDCHAR)
	error("Nonordinal type argument to Succ or Pred");
      /*Else, check if constant folding can be done*/
      else
      {
	/*Check subexpression type*/
	if(sub->tag == INTCONST)
	  eNode = make_intconst_expr(sub->u.intval++, ty_build_basic(TYSIGNEDLONGINT));
	else if(sub->tag == STRCONST && strlen(sub->u.strval) == 1)
	{
	  /*Makes the string constant*/
	  char * str = malloc(sizeof(char));
	  *str = sub->u.strval[0]++;
	  eNode = make_strconst_expr(str);
	}
      }
      break;
    case UN_PRED_OP:
      /*Type check, error if fails*/
      if(subTag != TYSIGNEDLONGINT && subTag != TYUNSIGNEDCHAR)
	error("Nonordinal type argument to Succ or Pred");
      /*Else, check if constant folding can be done*/
      else
      {
	/*Check subexpression type*/
	if(sub->tag == INTCONST)
	  eNode = make_intconst_expr(sub->u.intval--, ty_build_basic(TYSIGNEDLONGINT));
	else if(sub->tag == STRCONST && strlen(sub->u.strval) == 1)
	{
	  /*Makes the string constant*/
	  char * str = malloc(sizeof(char));
	  *str = sub->u.strval[0]--;
	  eNode = make_strconst_expr(str);
	}
      }
      break;
    case UN_EOF_OP:
      break; 
    case UN_EOLN_OP:
      break; 
    case INDIR_OP:
      eNode->type = ty_query_ptr(sub->type, &id, &next);
      if (debug) 
	printf("Setting type for INDIR node\n");
      break; 
    case UPLUS_OP:
      /*Type check, error if fails*/
      if(subTag != TYSIGNEDLONGINT && subTag != TYFLOAT && subTag != TYDOUBLE)
	error("Illegal type argument to unary plus");
      break;
    case NEW_OP:
      break; 
    case DISPOSE_OP:
      break;
    case ADDRESS_OP:
      break;
    case SET_RETURN_OP:
      break;
  }

  /*Returns the node*/
  return eNode;;
} /* End make_un_expr */

/* Makes a binary operator expression node */
EXPR make_bin_expr(EXPR_BINOP op, EXPR left, EXPR right)  
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = BINOP;
  eNode->type = right->type;	// need to add implicit type conversion
  eNode->u.binop.op = op;
  eNode->u.binop.left = left;
  eNode->u.binop.right = right;

  if (debug) printf("Created expr node for BINOP: %d\n", op);

  /* check op to see if lval is needed */
  if (op==ASSIGN_OP) {
    if (is_lval(left)) {
      if (is_lval(right)) eNode->u.binop.right = make_un_expr(DEREF_OP, right);
      return eNode;    
    }
    else error("Assignment requires l-value on the left");
    return make_error_expr();
  }  

  /* add deref as necessary */
  else {
    if (is_lval(left)) eNode->u.binop.left = make_un_expr(DEREF_OP, left);
    if (is_lval(right)) eNode->u.binop.right = make_un_expr(DEREF_OP, right);
    return eNode;
  }

}/* End make_bin_expr */

/* Makes a function call expression node */
EXPR make_fcall_expr(EXPR func, EXPR_LIST args)
{
  /*Checks to determine if the expr is a function*/
  if(ty_query(func->type) != TYFUNC)
  {
     /*Bug, return*/
     bug("Not a function sent to make_fcall_expr");
     return make_error_expr();
  }

  /*Queries the function*/
  TYPE funcRetType;
  PARAM_LIST fParams;
  BOOLEAN checkArgs;
  funcRetType = ty_query_func(func->type, &fParams, &checkArgs);

  /*Copies the param list*/
  PARAM_LIST c1 = fParams;

  /*Copies the args list*/
  EXPR_LIST copy, copy2 = args;

  /*If check args is false, make r vals of all arguments and unary convert*/
  if(checkArgs == FALSE && args != NULL)
  {
    /*Cycles through the arguments and unary converts them*/
    while(copy != NULL)
    {
      /*Makes r values, Makes conversion nodes*/
      copy->expr = make_un_expr(DEREF_OP, copy->expr);
      copy->expr = make_un_expr(CONVERT_OP,copy->expr);

      /*Moves onto the next element in the list*/
      copy = copy->next;
    }
  }
  /*Else, check the arguments*/
  else if(args != NULL)
  {
     /*While loop to check all of the parameters*/
     while(copy2 != NULL & c1 != NULL)
     {
	/*If reference parameter, make sure lval, compatible type*/
	if(c1->is_ref == TRUE)
	{
	  /*If lval, error*/
	  if(is_lval(copy2->expr) == FALSE)
	  {
	    /*Error, return*/
	    error("Reference parameter is not an l-value");
	    return make_error_expr();
	  }

	  /*Type check*/
	  if(ty_test_equality(copy2->expr->type, c1->type) == FALSE)
	  {
	    /*Error, return*/
	    error("Reference argument has incompatible type");
	    return make_error_expr();
	  }
	}
	/*Else, deal with value parameters*/
	else
	{
	  /*Makes r values, Makes conversion nodes*/
	  copy2->expr = make_un_expr(DEREF_OP, copy2->expr);
	  copy2->expr = make_un_expr(CONVERT_OP,copy2->expr);
	}

	/*Moves on to the next argument*/
	copy2 = copy2->next;
	c1 = c1->next;

	/*If statements to check the number of arguments*/
	if(copy2 == NULL && c1 != NULL)
	{
	  /*Error, return*/
	  error("Wrong number of arguments to procedure or function call");
	  return make_error_expr();
	}
	else if(c1 == NULL && copy2 != NULL)
	{
	  /*Error, return*/
	  error("Wrong number of arguments to procedure or function call");
	  return make_error_expr();
	}
     }
  }

  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = FCALL;
  eNode->type = funcRetType;
  eNode->u.fcall.args = args;
  eNode->u.fcall.function = func;

  if (debug)
  {
    printf("Created expr node for function call with return type:\n");
    ty_print_type(funcRetType);
    printf("\n");
  }

  /* Returns the node */
  return eNode;
}/* end make_fcall_expr */

/* Makes an error expression node */
EXPR make_error_expr()
{
  /* Creates the node and allocates memory */
  EXPR eNode;
  eNode = malloc(sizeof(EXPR_NODE));

  /* Sets the attributes of the node */
  eNode->tag = ERROR;
  eNode->type = NULL;

  if (debug) printf("Created expr node for error\n");

  /* Returns the node */
  return eNode;
}/* End make_err_expr */

/* Negates a number if its sign op is negative */
EXPR sign_number(EXPR_UNOP op, EXPR num)  
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
}/* End apply_sign_to_number */

/* Checks for assignment or procedure call */
EXPR check_assign_or_proc_call(EXPR lhs, ST_ID id, EXPR rhs)
{
	char * idstring = NULL;
	int block;

	if (debug) printf("Entering check assign or proc call\n");

	if (rhs) 
	{
		/* exception for function return value - check id with current function */
		if (id == func_id_stack[fi_top] && fi_top>=0)
		{
			if (debug) printf("Setting return value\n");

			/* if return type non-VOID return unop expr */
			if ( ty_query(lhs->type) != TYVOID ) {
				return make_un_expr(SET_RETURN_OP,rhs);
			}
			else {
				error("VOID return type");
				return make_error_expr();
			}
		}

		/* not a return value so return assign BINOP */
		else return make_bin_expr(ASSIGN_OP, lhs, rhs);
	}
	
	/* check for NULL pointer */
	if (!lhs) bug("NULL LHS expression");
	
	/* if New or Dispose then return LHS */
	if (lhs->tag==UNOP)
	{ 
		if ( (lhs->u.unop.op==NEW_OP) || (lhs->u.unop.op==DISPOSE_OP) ) {
			if (debug) printf("Returning New function call as LHS\n");
		}	return lhs;
	}

	if (debug) printf("LHS tag = %d\n", lhs->tag);

	/* lhs is already func call */
	if (lhs->tag==FCALL) {
		if ( ty_query(lhs->type) == TYVOID ) return lhs;
		else error("Procedure call to non-void function");
		return make_error_expr();
	}

	/* if tag = GID or LFUN check if LHS is procedure */
	if ( (lhs->tag==GID) || (lhs->tag==LFUN) )
	{
		/* check tagtype of data rec */
		if ( ty_query(lhs->type) == TYFUNC ) 

		/* return new FCALL node with no args */
		return make_fcall_expr(lhs, NULL);

		else error("Expected procedure name, saw data");
		return make_error_expr();
	}

	/* any other tag is an error */
	else if (lhs->tag!=10) error("Procedure call expected");

	return make_error_expr();

} /* end check_assign_or_proc_call */

/* Returns whether an expr is an lval */
BOOLEAN is_lval(EXPR expr)
{
	/* If expr is empty, bug */
	if (!expr) bug("Empty expr sent to is_lval");
	
	/* Gets the expr_tag and type of the expr */
	EXPR_TAG eTag = expr->tag;
	TYPE eType = expr->type;
	
	/* Retrieves expr's typetag using its type */
	TYPETAG eTypeTag = ty_query(eType);
	
	/* If tag is LVAR, expr is an lval */
	if (eTag == LVAR)
		return TRUE;
	
	/* If tag is GID and typetag is a data type (not TYFUNC or TYERROR), expr is an lval */
	else if (eTag == GID && eTypeTag != TYFUNC && eTypeTag != TYERROR)
		return TRUE;
	
	/* If tag is UNOP, check the operator */
	else if (eTag == UNOP)
	{
		/* Retrieve unop operator */
		EXPR_UNOP eOp = expr->u.unop.op;
		
		/* If operator is the indirection operator (^), expr is an lval */
		if (eOp == INDIR_OP) return TRUE;
	}
	
	/* Expr is not an lval */
	return FALSE;
}/* End is_val */

/* Frees an expression */
void expr_free(EXPR expr)  
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
}/* End expr_free */

/* Frees up an expression list */
void expr_list_free(EXPR_LIST list)  
{
  /* Frees the expression, recursive call, frees the final list */
  if(list->expr)
    expr_free(list->expr);
  if(list->next)
    expr_list_free(list->next);
  free(list);
}/* End expr_list_free */

/* Frees up a linked list */
void id_list_free(ID_LIST list)  
{
  /* If the next element exists */
  if(list->next)
  {
    /* Frees the next node in the list */
    id_list_free(list->next);
  }

  /* Frees the list */
  free(list);
}/* End id_list_free */

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
  
    /*If the variable is a reference parameter*/
    if(copy->is_ref == TRUE)
      datRec->u.decl.v.offset = getFormalParameterOffset(TYPTR);
    /*Else get the offset based on the type*/
    else
    {
      /*If it is a subrange type, get the base type*/
      if(ty_query(copy->type) == TYSUBRANGE)
      {
	/*Gets the subrange base type and sets the offset accordingly*/
	long low, high;
	datRec->u.decl.v.offset = getFormalParameterOffset(ty_query(ty_query_subrange(copy->type, &low, &high)));
      }
      /*Else, get the offset by the type*/
      else
	datRec->u.decl.v.offset = getFormalParameterOffset(ty_query(copy->type));
    }

    /* Installs the parameter */
    st_install(copy->id, datRec);

    /* Moves on to the next element */
    copy = copy->next;
  }
}/* End install_local_params */

/*Function that gets the global function name of a function*/
char * get_global_func_name(ST_ID id)
{
  /*Looks up the function in the symbol table*/
  ST_DR funcRec;
  int block;
  funcRec = st_lookup(id, &block);

  /*If the function is undefined or global, just return the name*/
  if ( (!funcRec) || (block <= 1) ) return st_get_id_str(id);

  /*Else return the global assembly name*/
  else
  {
    if (debug) printf("Creating local function name in block %d\n",block);

    /*Gets the id of the function, ., block number*/
    char * idName = st_get_id_str(id);
    char * dot = malloc(2 * sizeof(char));
    *dot = '.';
    char * blockNum = malloc(sizeof(char));
    *blockNum = block + '0';
    

    /*Concats the  dot onto the block number, then that onto the end of the string*/
    char * globalFunc = malloc((strlen(idName) + 2) * sizeof(char));
    globalFunc = idName;
    strcat(dot, blockNum);
    strcat(globalFunc, dot);

    /*Returns the function name*/
    return globalFunc;
  }
}
