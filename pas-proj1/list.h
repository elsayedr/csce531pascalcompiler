#ifndef LIST_H
#define LIST_H

/*Defines a linked list*/

/*Imports headers*/
#include "symtab.h"

/*Defines a structure that represents a node in a linked list*/
typedef struct listNode
{
  /*Symbol table data record*/
  ST_ID id;
  
  /*Link to next element in the list*/
  struct listNode *next;

}linkedListNode, *linkedList;

/*Function definitions*/
linkedList insert(linkedList list, ST_ID id);
void initialize();

#endif