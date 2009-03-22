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
  skip = getSkipSize(type);

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
  long low;
  long high;

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
      return getAlignSize(ty_query_subrange(type, &low, &high));
      break;
    /*Double case*/
    case TYDOUBLE:
      /*Returns the size of a double*/
      return sizeof(double);
      break;
   /*Float*/
   case TYFLOAT:
    /*Returns the size*/
    return sizeof(float);
    break;
   /*Long double*/
   case TYLONGDOUBLE:
    /*Returns the size*/
    return sizeof(long double);
    break;
   /*Signed long int*/
   case TYSIGNEDLONGINT:
    /*Returns the size*/
    return sizeof(signed long int);
    break;
   /*Signed short int*/
   case TYSIGNEDSHORTINT:
    /*Returns the size*/
    return sizeof(signed short int);
    break;
   /*Signed int*/
   case TYSIGNEDINT:
    /*Returns the size*/
    return sizeof(signed int);
    break;
   /*Unsigned long int*/
   case TYUNSIGNEDLONGINT:
    /*Returns the size*/
    return sizeof(unsigned long int);
    break;
   /*Unsigned short int*/
   case TYUNSIGNEDSHORTINT:
    /*Returns the size*/
    return sizeof(unsigned short int);
    break;
   /*Unsigned integer*/
   case TYUNSIGNEDINT:
    /*Returns the size*/
    return sizeof(unsigned int);
    break;
   /*Unsigned char*/
   case TYUNSIGNEDCHAR:
    /*Returns the size*/
    return sizeof(unsigned char);
    break;
   /*Signed char*/
   case TYSIGNEDCHAR:
    /*Returns the size*/
    return sizeof(signed char);
    break;
  }
}

/*Function that gets the required skip value*/
int getSkipSize(TYPE type)
{
  /*Gets the type tag from the type*/
  TYPETAG tag = ty_query(type);

  /*Represents the index list*/
  INDEX_LIST list;

  /*Low and high for subrange type*/
  long low;
  long high;

  /*Switch based on typetag*/
  switch(tag)
  {
    /*Array case*/
    case TYARRAY:
      /*Recursive call to handle array*/
      return (high - low) * getSkipSize(ty_query_array(type, &list));
      break;
    /*Pointer case*/
    case TYPTR:
      /*Returns the size*/
      return sizeof(char *);
      break;
    /*Subrange case*/
    case TYSUBRANGE:
      return getSkipSize(ty_query_subrange(type, &low, &high));
      break;
    /*Double case*/
    case TYDOUBLE:
      /*Returns the size of a double*/
      return sizeof(double);
      break;
   /*Float*/
   case TYFLOAT:
    /*Returns the size*/
    return sizeof(float);
    break;
   /*Long double*/
   case TYLONGDOUBLE:
    /*Returns the size*/
    return sizeof(long double);
    break;
   /*Signed long int*/
   case TYSIGNEDLONGINT:
    /*Returns the size*/
    return sizeof(signed long int);
    break;
   /*Signed short int*/
   case TYSIGNEDSHORTINT:
    /*Returns the size*/
    return sizeof(signed short int);
    break;
   /*Signed int*/
   case TYSIGNEDINT:
    /*Returns the size*/
    return sizeof(signed int);
    break;
   /*Unsigned long int*/
   case TYUNSIGNEDLONGINT:
    /*Returns the size*/
    return sizeof(unsigned long int);
    break;
   /*Unsigned short int*/
   case TYUNSIGNEDSHORTINT:
    /*Returns the size*/
    return sizeof(unsigned short int);
    break;
   /*Unsigned integer*/
   case TYUNSIGNEDINT:
    /*Returns the size*/
    return sizeof(unsigned int);
    break;
   /*Unsigned char*/
   case TYUNSIGNEDCHAR:
    /*Returns the size*/
    return sizeof(unsigned char);
    break;
   /*Signed char*/
   case TYSIGNEDCHAR:
    /*Returns the size*/
    return sizeof(signed char);
    break;
  }
}
