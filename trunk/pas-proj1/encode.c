#include "encode.h"
#include "backend.h"
#include "types.h"

/*Function that outputs the necessar code for a global delcaration*/
void declareVariable(ST_ID id, TYPE type)
{
  /*Variables for the alignment and skip sizes*/
  int align, skip;

  /*Gets the alignment value and skip value*/
  align = getAlignSize(type);
  skip = getSkipSize(type, align);

  /*Calls the backend functions*/
  b_global_decl(st_get_id_str(id), align);
  b_skip(skip);
}

/*Function that gets the required alignment value*/
int getAlignSize(TYPE type)
{
  /*Gets the type tag from the type*/
  TYPETAG tag = ty_query(type);

  /*Represents the index list*/
  INDEX_LIST list;

  /*Low and high for subrange type*/
  long* low;
  long* high;

  /*Switch based on typetag*/
  switch(tag)
  {
    /*Array case*/
    case TYARRAY:
      /*Recursive call to handle array*/
      return getAlignSize(ty_query_array(type, &list));
      break;
    /*Pointer case*/
    case TYPTR:
      /*Returns the size*/
      return sizeof(char *);
      break;
    /*Subrange case*/
    case TYSUBRANGE:
      return getAlignSize(ty_query_subrange(type, low, high));
      break;
    /*Double case*/
    case TYDOUBLE:
      /*Returns the size of a double*/
      return sizeof(double);
      break;
    /*Signed long int case*/
    case TYSIGNEDLONGINT:
      /*Returns the size*/
      return sizeof(long);
      break;
  }
}

/*Function that gets the required skip value*/
int getSkipSize(TYPE type, int align)
{
  /*Gets the type tag from the type*/
  TYPETAG tag = ty_query(type);

  /*Represents the index list*/
  INDEX_LIST list;

  /*Low and high for subrange type*/
  long *low; 
  long *high;
  TYPE subR;

  /*Switch based on typetag*/
  switch(tag)
  {
    /*Array case*/
    case TYARRAY:
      /*Recursive call to handle array*/
      return align * getSkipSize(ty_query_array(type, &list), align);
      break;
    /*Pointer case*/
    case TYPTR:
      /*Returns the size*/
      return sizeof(char *);
      break;
    /*Subrange case*/
    case TYSUBRANGE:
      subR = ty_query_subrange(type, low, high);
      return (high - low);
    /*Double case*/
    case TYDOUBLE:
      /*Returns the size of a double*/
      return sizeof(double);
      break;
    /*Signed long int case*/
    case TYSIGNEDLONGINT:
      /*Returns the size*/
      return sizeof(long);
      break;
  }
}
