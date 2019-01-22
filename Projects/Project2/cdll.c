#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdll.h"

/* just a convenience */
void fatal( char * msg )
{
	printf("%s\n",msg);
	exit(EXIT_FAILURE);
}


/* ----------------------------------------------------------------------------
	initList:

*/
void initList(CDLL *list, int (*compare)(void*, void*), void (*print)(void*, int),
			  void (*freeData)(void *))
{
	/* Y O U R   C O D E    H E R E */
	/*1) set the head pointer in the CDLL struct to NULL
	2) assign each of the incoming function pointers into their respective fields in the CDLL struct*/
	list->head = NULL;
	list->compare = compare;
	list->print = print;
	list->freeData = freeData;
}


/* ----------------------------------------------------------------------------
*/
void insertAtTail(CDLL *list, void *data)
{
	/* Y O U R   C O D E    H E R E */
	if (list->head == NULL)
	{
		list->head = malloc(sizeof(CDLL_NODE));
		list->head->data = data;
		list->head->next = list->head;
		list->head->prev = list->head;
		return;
	}
	CDLL_NODE * tail = (list->head)->prev;
	CDLL_NODE * newNode = malloc(sizeof(CDLL_NODE));
	newNode->data = data;
	newNode->next = list->head;
	newNode->prev = tail;
	(list->head)->prev = newNode;
	tail->next = newNode;
}



/* ----------------------------------------------------------------------------
	deleteNode:

	You have  passed in the pointer to the node to be deleted.
	No need to iterate or search. Of course a check for a NULL pointer passed in
	would not hurt.
	Delete the deadNode then return the pointer to it's successor (if CW) or
	if you are going CCW then return pointer to the deadNode's predecessor.
	If deadnode was the last node then return NULL since there is no succ or pred.
*/
CDLL_NODE * deleteNode(CDLL *list, CDLL_NODE *deadNode, int direction )
{
	/* Y O U R   C O D E    H E R E */
	if (deadNode == NULL) {
		fatal("deadNode is NULL");
		return NULL;
	} else if (deadNode->next == list->head && deadNode == list->head) {
		list->freeData(deadNode->data);
		free(deadNode);
		list->head = NULL;
		return NULL;
	} else if (deadNode == list->head) {
		CDLL_NODE * temp = deadNode->prev;
		list->head = (list->head)->next;
		temp->next = list->head;
		(list->head)->prev = temp;
		list->freeData(deadNode->data);
		free(deadNode);
		if (direction == CLOCKWISE) {
			return list->head;
		} else {
			return (list->head)->prev;
		}
	} else if (deadNode->next == list->head) {
		CDLL_NODE * temp = deadNode->prev;
		(list->head)->prev = temp;
		temp->next = list->head;
		list->freeData(deadNode->data);
		free(deadNode);
		if (direction == CLOCKWISE) {
			return list->head;
		} else {
			return (list->head)->prev;
		}
	} else {
		CDLL_NODE * tempNext = deadNode->next;
		CDLL_NODE * tempPrev = deadNode->prev;
		tempPrev->next = tempNext;
		tempNext->prev = tempPrev;
		list->freeData(deadNode->data);
		free(deadNode);
		if (direction == CLOCKWISE) {
			return tempNext;
		} else {
			return tempPrev;
		}
	}

}



/* ----------------------------------------------------------------------------
	printList:
	Observe my solution executable to see how it should look
	You are really just writing the loop and calling the printData on each node
*/

void printList( CDLL list, int direction, int mode )
{
	CDLL_NODE * curr = list.head;
	if (direction == CLOCKWISE) {
		do {
			list.print(curr->data,mode);
			curr = curr->next;
		} while (curr != list.head);
	} else {
		do {
			list.print(curr->data,mode);
			curr = curr->prev;
		} while (curr != list.head);
	}
	printf("\n");
}



/* ----------------------------------------------------------------------------
	searchList:

	Scan list until you find a node that contains the data value passed in.
	If found return that pointer - otherwise return NULL
*/
CDLL_NODE * searchList( CDLL list, void * target )
{
	CDLL_NODE * curr = list.head;
	while (curr->next != list.head && list.compare(curr->data,target) != 0)
	{
		curr = curr->next;
	}
	if (list.compare(curr->data,target) == 0) {
		return curr;
	} else {
		return NULL;
	}
}
