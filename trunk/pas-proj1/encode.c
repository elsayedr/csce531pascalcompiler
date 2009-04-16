#include "defs.h"
#include "encode.h"
#include "backend.h"
#include "types.h"
#include <stdio.h>

/* Function that outputs the necessary code for a global declaration */
void declareVariable(ST_ID id, TYPE type)
{
  /* Variables for the alignment and skip sizes */
  int align, skip;

  /* Gets the alignment value and skip value */
  align = getAlignSize(type);
  skip = getSkipSize(type);

  /* Calls the backend functions */
  b_global_decl(st_get_id_str(id), align);
  b_skip(skip);
}

/* Function that gets the required alignment value */
int getAlignSize(TYPE type)
{
  /* Gets the type tag from the type */
  TYPETAG tag = ty_query(type);

  /* Represents the index list */
  INDEX_LIST list;

  /* Low and high for subrange type */
  long low;
  long high;

  /* Switch based on typetag */
  switch(tag)
  {
    /* Array case */
    case TYARRAY:
      /* Recursive call to handle array */
      return getAlignSize(ty_query_array(type, &list));
      break;
    /* Pointer case */
    case TYPTR:
      /* Returns the size */
      return sizeof(char *);
      break;
    /* Subrange case */
    case TYSUBRANGE:
      return getAlignSize(ty_query_subrange(type, &low, &high));
      break;
    /* Double case */
    case TYDOUBLE:
      /* Returns the size of a double */
      return sizeof(double);
      break;
   /* Float */
   case TYFLOAT:
    /* Returns the size */
    return sizeof(float);
    break;
   /* Long double */
   case TYLONGDOUBLE:
    /* Returns the size */
    return sizeof(long double);
    break;
   /* Signed long int */
   case TYSIGNEDLONGINT:
    /* Returns the size */
    return sizeof(signed long int);
    break;
   /* Signed short int */
   case TYSIGNEDSHORTINT:
    /* Returns the size */
    return sizeof(signed short int);
    break;
   /* Signed int */
   case TYSIGNEDINT:
    /* Returns the size */
    return sizeof(signed int);
    break;
   /* Unsigned long int */
   case TYUNSIGNEDLONGINT:
    /* Returns the size */
    return sizeof(unsigned long int);
    break;
   /* Unsigned short int */
   case TYUNSIGNEDSHORTINT:
    /* Returns the size */
    return sizeof(unsigned short int);
    break;
   /* Unsigned integer */
   case TYUNSIGNEDINT:
    /* Returns the size */
    return sizeof(unsigned int);
    break;
   /* Unsigned char */
   case TYUNSIGNEDCHAR:
    /* Returns the size */
    return sizeof(unsigned char);
    break;
   /* Signed char */
   case TYSIGNEDCHAR:
    /* Returns the size */
    return sizeof(signed char);
    break;
  }
}

/* Function that gets the required skip value */
int getSkipSize(TYPE type)
{
  /* Gets the type tag from the type */
  TYPETAG tag = ty_query(type);

  /* Represents the index list */
  INDEX_LIST list;

  /* Low and high for subrange type */
  long low, high, size;

  /* Switch based on typetag */
  switch(tag)
  {
    /* Array case */
    case TYARRAY:
      /* Recursive call to handle array */
      size = getSkipSize(ty_query_array(type, &list));
      while (list) {
	ty_query_subrange(list->type, &low, &high); 
	size *= high - low + 1;           
	list = list->next;
      }
      return size;
      break;
    /* Pointer case */
    case TYPTR:
      /* Returns the size */
      return sizeof(char *);
      break;
    /* Subrange case */
    case TYSUBRANGE:
      return getSkipSize(ty_query_subrange(type, &low, &high));
      break;
    /* Double case */
    case TYDOUBLE:
      /* Returns the size of a double */
      return sizeof(double);
      break;
   /* Float */
   case TYFLOAT:
    /* Returns the size */
    return sizeof(float);
    break;
   /* Long double */
   case TYLONGDOUBLE:
    /* Returns the size */
    return sizeof(long double);
    break;
   /* Signed long int */
   case TYSIGNEDLONGINT:
    /* Returns the size */
    return sizeof(signed long int);
    break;
   /* Signed short int */
   case TYSIGNEDSHORTINT:
    /* Returns the size */
    return sizeof(signed short int);
    break;
   /* Signed int */
   case TYSIGNEDINT:
    /* Returns the size */
    return sizeof(signed int);
    break;
   /* Unsigned long int */
   case TYUNSIGNEDLONGINT:
    /* Returns the size */
    return sizeof(unsigned long int);
    break;
   /* Unsigned short int */
   case TYUNSIGNEDSHORTINT:
    /* Returns the size */
    return sizeof(unsigned short int);
    break;
   /* Unsigned integer */
   case TYUNSIGNEDINT:
    /* Returns the size */
    return sizeof(unsigned int);
    break;
   /* Unsigned char */
   case TYUNSIGNEDCHAR:
    /* Returns the size */
    return sizeof(unsigned char);
    break;
   /* Signed char */
   case TYSIGNEDCHAR:
    /* Returns the size */
    return sizeof(signed char);
    break;
  }
}

/* Function that is called when a function block is entered */
void enter_func_body(char * global_func_name, TYPE type, int loc_var_offset)
{
  /* Type, parameter list, boolean, and tag variables for querying functions */
  TYPE fType;
  PARAM_LIST fParams;
  BOOLEAN checkArgs;
  TYPETAG tag;

  /*Offset value for each variable, block number, data record*/
  int lVarOffset;
  int blockNum;
  ST_DR dataRec;

  /* Queries the function, gets the tag */
  fType = ty_query_func(type, &fParams, &checkArgs);
  tag = ty_query(fType);

  /* Calls encoding function */
  b_func_prologue(global_func_name);

  /* While there are still elements in the list */
  while(fParams != NULL)
  {
    /* Gets the type tag */
    TYPETAG tag;
    tag = ty_query(fParams->type);

    /* Stores the formal parameter */
    lVarOffset = b_store_formal_param(tag);

    /*Gets the data record for the formal parameter*/
    dataRec = st_lookup(fParams->id, &blockNum);

    /*Compares the offset values*/
    if(lVarOffset != dataRec->u.decl.v.offset)
      bug("Local variable offset does not match");

    /* Moves to the next element in the list */
    fParams = fParams->next;
  }
  
  /*If it is a function, allocate space for the return value*/
  if(tag != TYVOID)
    b_alloc_return_value();

  /*Allocates space for the local variables*/
  b_alloc_local_vars(loc_var_offset);		// this has to be loc_var_offset or it doesn't work right

}/* End enter_func_body */

/* Function that is called when a function block is exited */
void exit_func_body(char * global_func_name, TYPE type)
{
  /* Type, parameter list, boolean, tag variables for querying functions */
  TYPE fType;
  PARAM_LIST fParams;
  BOOLEAN checkArgs;
  TYPETAG tag;

  /*Pops the id off the id stack*/
  fi_top--;

  /*Queries the function, gets the tag*/
  fType = ty_query_func(type, &fParams, &checkArgs);
  tag = ty_query(fType);

  /*Checks the return type and allocates space as needed*/
  if(tag != TYVOID)
    b_prepare_return(tag);

  /* Calls the encoding function */
  b_func_epilogue(global_func_name);

  /* Exits the block */
  st_exit_block();

  /*Decrements the stack top*/
  bo_top--;
}/* End exit_func body */

/* Function that calls backend routines to enter the main program body */
void enter_main_body()
{
  /* Calls the backend routine */
  b_func_prologue("main");
}/* End enter_main_body */

/* Function that calls backend routines to exit the main program body */
void exit_main_body()
{
  /* Calls the backend routine */
  b_func_epilogue("main");
}/* End exit_main_body */

/*Returns the local variable offset*/
int get_local_var_offset()
{
  /*Returns the local variable offset*/
  return b_get_local_var_offset();
}

/*Gets the formal parameter offset value for a certian type*/
int getFormalParameterOffset(TYPETAG tag)
{
  /*Returns the offset value*/
  return b_get_formal_param_offset(tag);
}

/*Helper function to encode a unary operator node*/
void encodeUnop(EXPR_UNOP op, EXPR arg)
{
  /*Debugging*/
  if (debug) printf("Encoding unary operator: %d\n",op);

  /*Recursive call on the argument to encode_Expr*/
  encode_expr(arg->u.unop.operand);

  /*Variables needed in the switch*/
  TYPETAG tag;
  ST_ID s1;
  TYPE t1;
  BOOLEAN isConverted = FALSE;

  /*Gets the tag of the argument*/
  tag = ty_query(arg->u.unop.operand->type);
  
  /*Switch based on the operator*/
  switch(op)
  {
    /*Convert operator*/
    case CONVERT_OP:
      /*Checks the type of the argument*/
      if(tag == TYSIGNEDLONGINT)
      {
	/*Single to Real*/
	b_convert(TYSIGNEDLONGINT, TYDOUBLE);
      }
      /*If subrange type, get base type*/
      else if(tag == TYSUBRANGE)
      {
	/*Gets the base type*/
	long low, high;
	TYPE baseT = ty_query_subrange(arg->u.unop.operand->type, &low, &high);
	
	/*Converts subrange to base type*/
	b_convert(TYSUBRANGE, ty_query(baseT));
      }
      /*Else, pointer to void converted to pointer not of void*/
      else if(tag == TYPTR)
      {
	/*Not implemented yet*/
      }
      break;
    /*Unary plus case*/
    case UPLUS_OP:
      break;
    /*Unary minus*/
    case NEG_OP:
      /*Calls the backend routine*/
      b_negate(tag);
      break;
    /*Ord operator*/
    case ORD_OP:
      /*If tag is unsigned char, conver to signed long int*/
      if(tag == TYUNSIGNEDCHAR)
      {
	b_convert(tag, TYSIGNEDLONGINT);
      }
      break;
    /*Chr operator*/
    case CHR_OP:
      /*If tag is signed long int, convert to unsigned char*/
      if(tag == TYSIGNEDLONGINT)
	b_convert(tag, TYUNSIGNEDCHAR);
      break;
    /*Succ operator*/
    case UN_SUCC_OP:
      /*If tag is not signed long int, convert*/
      if(tag != TYSIGNEDLONGINT)
      {
	/*Converts to integer*/
	b_convert(tag, TYSIGNEDLONGINT);
	isConverted = TRUE;
      }
      /*Pushes the constant integer, adds it*/
      b_push_const_int(1);
      b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);

      /*If the expression has been converted, convert back*/
      if(isConverted == TRUE)
	b_convert(TYSIGNEDLONGINT, tag);
      break;
    /*Pred operator*/
    case UN_PRED_OP:
      /*If tag is not signed long int, convert*/
      if(tag != TYSIGNEDLONGINT)
      {
	/*Converts to integer*/
	b_convert(tag, TYSIGNEDLONGINT);
	isConverted = TRUE;
      }
      /*Pushes the constant integer, adds it*/
      b_push_const_int(-1);
      b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);

      /*If the expression has been converted, convert back*/
      if(isConverted == TRUE)
	b_convert(TYSIGNEDLONGINT, tag);
      break;
    /*Indirection operator*/
    case INDIR_OP:
      break;
    /*New operator*/
    case NEW_OP:
      /*Allocates size for the argument list*/
      b_alloc_arglist(4);

      /*Pushes an intconst the size of the type the pointer points to*/
      b_push_const_int(getSkipSize(ty_query_ptr(arg->u.unop.operand->type, &s1, &t1)));

      /*Loads an argument*/
      b_load_arg(ty_query(ty_query_ptr(arg->u.unop.operand->type, &s1, &t1)));
      
      /*Calls the external C function malloc*/
      b_funcall_by_name("malloc", TYPTR);

      /*Assigns the pointer and pops the top value off the stack*/
      b_assign(TYPTR);
      b_pop();
      break;
    /*Dispose operator*/
    case DISPOSE_OP:
      /*Allocates size for the argument list*/
      b_alloc_arglist(4);

      /*Calls the external C function free*/
      b_funcall_by_name("free", TYPTR);
      break;
    /*Deref opeator*/
    case DEREF_OP:
      b_deref(tag);
      break;
  /* set return op */
  case SET_RETURN_OP:
   b_set_return(ty_query(arg->u.unop.operand->type));
   break;
  }

}

/*Helper function to encode a binary operator node*/
void encodeBinop(EXPR_BINOP op, EXPR exp)
{
  if (debug) printf("Encoding binary operator: %d\n",op);

  /*Recursive calls on the arguments to encode_Expr*/
  encode_expr(exp->u.binop.left);
  encode_expr(exp->u.binop.right);

  /*Switch based on the operator*/
  switch(op)
  {
    /*Addition*/
    case ADD_OP:
      /*Addition*/
      b_arith_rel_op(B_ADD, ty_query(exp->type));
      break;
    /*Subtraction*/
    case SUB_OP:
      /*Subtraction*/
      b_arith_rel_op(B_SUB, ty_query(exp->type));
      break;
    /*Multiplication*/
    case MUL_OP:
      /*Multiplication*/
      b_arith_rel_op(B_MULT, ty_query(exp->type));
      break;
    /*Division*/
    case DIV_OP:
      /*Division*/
      b_arith_rel_op(B_DIV, ty_query(exp->type));
      break;
    /*Modulus*/
    case MOD_OP:
      /*Modulus*/
      b_arith_rel_op(B_MOD, ty_query(exp->type));
      break;
    /*Real division*/
    case REALDIV_OP:
      /*Real division*/
      b_arith_rel_op(B_DIV, ty_query(exp->type));
      break;
    /*Is equal*/
    case EQ_OP:
      /*Tests equality*/
      b_arith_rel_op(B_EQ, ty_query(exp->type));
      break;
    /*Is less than*/
    case LESS_OP:
      /*Less than*/
      b_arith_rel_op(B_LT, ty_query(exp->type));
      break;
   /*Is less than or equal*/
   case LE_OP:
      /*Less than or equal to*/
      b_arith_rel_op(B_LE, ty_query(exp->type));
      break;
  /*Not equal*/
  case NE_OP:
      /*Not equal*/
      b_arith_rel_op(B_NE, ty_query(exp->type));
    break;
  /*Greater than or equal*/
  case GE_OP:
      /*Greater than or equal*/
      b_arith_rel_op(B_GE, ty_query(exp->type));
    break;
  /*Greater than*/
  case GREATER_OP:
      /*Greater than*/
      b_arith_rel_op(B_GT, ty_query(exp->type));
    break;
  /*Symbdiff*/
  case SYMDIFF_OP:
    break;
  /*Or*/
  case OR_OP:
    break;
  /*XOR*/
  case XOR_OP:
    break;
  /*And*/
  case AND_OP:
    break;
  /*Assignment*/
  case ASSIGN_OP:
     /*Checks tag to see if local variable*/
     if(exp->u.binop.left->tag == LVAR)
     {
	/*Pushes the address of the local variable*/
	b_push_loc_addr(exp->u.binop.left->u.lvar.offset);
     }
     
    /*Assigns, then pops*/
    b_assign(ty_query(exp->type));
    b_pop();
    break;
  }
}

/*Helper function to encode a function call*/
void encodeFCall(EXPR func, EXPR_LIST args)
{
  /*Debugging*/
  if (debug) printf("Encoding function call\n");

  /*Size of the argument list*/
  int argListSize = 0;

  /*Global name of the function*/
  char * fGlobalName;

  /*Cheecks to see if the function is a gid or lfun*/
  if(func->tag == GID)
  {
    /*Gets the function name*/
    fGlobalName = st_get_id_str(func->u.gid);
  }
  /*Else if, local function*/
  else if(func->tag == LFUN)
  {
    /*Gets the function name, adds to size enough room for shadow pointer*/
    fGlobalName = func->u.lfun.global_name;
    argListSize = 4;
  }
  /*Else bug*/
  else
  {
    /*Bug, return*/
    bug("Function sent to encodeFCall not GID or LFUN");
    return;
  }

  /*Copies the arg list*/
  EXPR_LIST copy = args;
  EXPR_LIST copy2 = args;

  /*While loop that runs through the list*/
  while(copy != NULL)
  {
    /*If the argument is a double add eight*/
    if(ty_query(copy->expr->type) == TYDOUBLE)
      argListSize = argListSize + 8;
    /*Else, add four*/
    else
      argListSize = argListSize + 4;

    /*Moves on to the next item*/
    copy = copy->next;
  }

  /*Allocates the size for the argument list*/
  b_alloc_arglist(argListSize);

  /*Queries the function*/
  TYPE funcRetType;
  PARAM_LIST fParams;
  BOOLEAN checkArgs;
  funcRetType = ty_query_func(func->type, &fParams, &checkArgs);

  /*While loop that runs through the list to do encoding*/
  while(copy2 != NULL)
  {
    /*Calls encode_expr to encode the argument*/
    encode_expr(copy2->expr);

    /*If the paramater is a reference parameter*/
    if(fParams->is_ref == TRUE)
    {
      /*If not an lval, bug*/
      if(is_lval(copy2->expr) == FALSE)
	bug("Function argument expected to be L-val");
      /*Else, check types, load pointer*/
      else
      {
	/*If types are not compatible, error*/
	if(ty_test_equality(copy2->expr->type, fParams->type) == FALSE)
	{
	  /*Error*/
	  error("Parameter types do not match");
	}
	/*Loads the pointer*/
	b_load_arg(TYPTR);
      }
    }
    /*Else, value parameter*/
    else
    {
      /*Gets the tag*/
      TYPETAG tag;
      tag = ty_query(copy2->expr->type);

      /*If it is a lval, deref*/
      if(is_lval(copy2->expr) == TRUE)
	b_deref(tag);

      /*If chars, promote to longs, load arg*/
      if(tag == TYSIGNEDCHAR || tag == TYUNSIGNEDCHAR)
      {
	/*Convert, load arg*/
	b_convert(tag, TYSIGNEDLONGINT);
	b_load_arg(TYSIGNEDLONGINT);
      }
      /*If float, promote to double, load arg*/
      else if(tag == TYFLOAT)
      {
	/*Convert, load arg*/
	b_convert(tag, TYDOUBLE);
	b_load_arg(TYDOUBLE);
      }
      /*Else, load arg*/
      else
      {
	/*Load arg*/
	b_load_arg(tag);
      }
    }

    /*Moves on to the next item*/
    copy2 = copy2->next;
    fParams = fParams->next;
  }

  /*Calls the function*/
  b_funcall_by_name(fGlobalName, ty_query(funcRetType));
}

/*Function that encodes and expression*/
void encode_expr(EXPR expr)
{
  if (!expr) {
	bug("Encoded expression undefined");
	return;
  }

  if (debug) printf("Encoding expression tag: %d\n",expr->tag);

  /*Switch based on the tag of the expression*/
  switch(expr->tag)
  {
    /*Intconst case*/
    case INTCONST:
      /*Pushes the constant integer value on the stack*/
      b_push_const_int(expr->u.intval);
      break;
    /*Real constant case*/
    case REALCONST:
      /*Pushes the double constant value onto the stack*/
      b_push_const_double(expr->u.realval);
      break;
    /*String constant case*/
    case STRCONST:
      /*Pushes the string constant value onto the stack*/
      b_push_const_string(expr->u.strval);
      break;
    /*Global identifier case*/
    case GID:
      /*Pushes the address of the identifier onto the stack, dereferences*/
      b_push_ext_addr(st_get_id_str(expr->u.gid));
      break;
    /*Local varible case*/
    case LVAR:
      /*Pushes the address of the current frame pointer*/
      b_push_loc_addr(0);

      /*Count variable*/
      int i =0;

      /*For loop for the link count*/
      for(i =0; i<expr->u.lvar.link_count; i++)
      {
	/*Follows the reference link, so to speak*/
	b_offset(FUNC_LINK_OFFSET);
	b_deref(TYPTR);
      }

      /*Final offset*/
      b_offset(expr->u.lvar.offset);

      /*If it is a reference parameter, deref*/
      if(expr->u.lvar.is_ref == TRUE)
	b_deref(TYPTR);

      /*Break case*/
      break;
    /*Local function case*/
    case LFUN:
      break;
    /*Null operator case*/
    case NULLOP:
      /*Nil operation, pushes zero onto the stack*/
      b_push_const_int(0);
      break;
    /*Unary operator case*/
    case UNOP:
      /*Calls helper function*/
      encodeUnop(expr->u.unop.op, expr);
      break;
    /*Binary operator case*/
    case BINOP:
      /*Calls helper function*/
      encodeBinop(expr->u.binop.op, expr);
      break;
    /*Function call case*/
    case FCALL:
      /*Calls helper function*/
      encodeFCall(expr->u.fcall.function, expr->u.fcall.args);
      break;
    /*Error case*/
    case ERROR:
      break;
  }
}

