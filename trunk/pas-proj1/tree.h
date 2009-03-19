/* Definitions to support tree.c */

#ifndef TREE_H
#define TREE_H

#include "defs.h"
#include "types.h"
#include "list.h"

/*Enumerates the possible node types*/
typedef enum 
{
  /*Possible node types*/
  INTCONST,REALCONST,ID_NODE,TYPE_NODE,UNOP,BINOP
} st_tagtype;

/*Structure for the syntax tree node*/
typedef struct st_node 
{
  /*Tag that specifies the node type*/
  st_tagtype tag;

  /*Union*/
  union 
  {
    /*Value if the node is an intconst*/
    long intconst;
    
    /*Value if the node is a real constant*/
    double realconst;

    /*Structure if the node is an id node*/
    struct 
    {
      /*Pointer to the id in the symbol table*/
      ST_ID id;
    } id_node;

    /*Structure if the node is a type node*/
    struct 
    {
      /*Pointer to the id in the symbol table*/
      ST_ID id;

      /*Specifies the type*/
      TYPE type;
    } type_node;
	
    /*Structure if the node is a unary operator*/
    struct 
    {
      /*Character representing the operator*/
      char op;

      /*Structure representing the argument for the node*/
      struct st_node *arg;
    } unop;

    /*Structure if the node is a binary operator*/
    struct 
    {
      /*Character represnting the operator */
      char op;	

      /*Pointers to the arguments*/
      struct st_node *arg1;
      struct st_node *arg2;
    } binop;

  } u; /*End of the union*/
} ST_NODE, *ST;


/*Function definitions*/

TYPE lookup_type(ST_ID);
TYPE make_subrange(long, long);
TYPE make_array(INDEX_LIST, TYPE);
void make_type(ST_ID, TYPE);
void make_var(linkedList, TYPE);
INDEX_LIST insert_index(INDEX_LIST, TYPE);
linkedList insert_id(linkedList, ST_ID);
MEMBER_LIST type_members(linkedList, TYPE);
MEMBER_LIST combine_members(MEMBER_LIST, MEMBER_LIST);
MEMBER_LIST createMemberListFromID(linkedList);
MEMBER_LIST insertMember(MEMBER_LIST, ST_ID);
void resolve_ptrs();
PARAM_LIST insert_id_into_param_list(PARAM_LIST, ST_ID, BOOLEAN);
PARAM_LIST convertMemberListToParams(MEMBER_LIST, BOOLEAN);
PARAM_LIST type_params(PARAM_LIST, TYPE, BOOLEAN);
PARAM_LIST combine_params(PARAM_LIST, PARAM_LIST);
TYPE make_func(PARAM_LIST, TYPE);
TYPE make_proc(PARAM_LIST);

ST make_id(ST_ID);
ST make_int(long);
ST make_real(double);
ST make_unop(char, ST);
ST make_binop(ST, char, ST);
long eval_id(ST);

#endif



