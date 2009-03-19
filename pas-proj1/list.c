/*
list.c
This file contains all of the list functions
*/

/*Imports headers*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/*Initialization function*/
void initialize(linkedList list)
{
    /*Sets the list initially to be null*/
    list = NULL;
}

/*Inserts the word into the list*/
linkedList insert(linkedList list, ST_ID id)
{
  /*Pointers for the linked list*/
  linkedList previous = NULL;
  linkedList toReturn = list;

  /*While loop to determine where the node should be placed*/
  while(list != NULL)
  {
    /*Goes to the next node in the list*/
    previous = list;
    list = list->next;
    
  }

  /*If the previous node is equal to null, insert at front*/
  if(previous == NULL)
  {
    /*Create the node and insert it*/
    toReturn = (linkedList)malloc(sizeof(linkedListNode));
    toReturn->id = id;
    toReturn->next = list;

    /*Returns the list*/
    return toReturn;
  }

  /*Insert somewhere in the middle of the list*/
  previous->next = (linkedList)malloc(sizeof(linkedListNode));
  previous = previous->next;
  previous->id = id;
  previous->next = list;

  /*Returns the list*/
  return toReturn;
}
