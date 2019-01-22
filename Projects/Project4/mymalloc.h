//Adam Granieri
//COE 0449 Systems Software
//Fall 2018

#include <stdio.h>

//struct for indicating memory blocks
typedef struct Node {
	int freeSpace;
	int chunkSize;
	struct Node* prev;
	struct Node* next;
} Node;

//Data for linkedList of memory locations
Node *head;
Node *curr;
Node *tail;

//Function references
void coalese(void*);
void *maxFreeSpace(int);
void *my_worstfit_malloc(int);
void my_free(void*);

//function definitions
void* my_worstfit_malloc(int size) {
	void* oldBrk = (void*)sbrk(0);

	if (head == 0) { //new head location
		
		sbrk(size + sizeof(Node));
		//initalize head node
		head = oldBrk;
		head->freeSpace = 0;
		head->next = 0;
		head->prev = 0;
		head->chunkSize = size;
		//set other list data to head since there is only one
		curr = head;
		tail = head;

	} else { //find another place with worstfit
		
		//New node on list for space
		Node* newSpace = maxFreeSpace(size);
		
		if (newSpace == 0) { //no free space

			sbrk(size + sizeof(Node));
			// create new node at tail
			tail = (Node*) oldBrk;
			tail->freeSpace = 0;
			tail->next = 0;
			tail->prev = curr;
			tail->chunkSize = size;
			//set other list data
			curr->next = tail;
			curr = tail;

		} else {

			newSpace->freeSpace = 0;

			//Check for extra memory between mallocs
			int newLocation = newSpace->chunkSize - sizeof(Node);
			if (newLocation > (size + sizeof(Node))) {

				//New node into the linkedList
				Node *temp = ((void*) newSpace) + sizeof(Node) + size;
				temp->freeSpace = 1;
				temp->next = newSpace->next;
				temp->prev = newSpace;
				temp->chunkSize = newLocation - size;
				//set other list data
				newSpace->next = temp;
				newSpace->chunkSize = size;

			}

			return ((void*) newSpace) + sizeof(Node);
		}

	}

	return oldBrk + sizeof(Node);
}

void my_free(void *ptr) {
	Node* temp = ptr - sizeof(Node);
	temp->freeSpace = 1;

	while (temp != 0 && temp == tail) {
		if (temp->freeSpace != 1) { //check if memory is not free
			break;
		}
		tail->next = 0;
		tail = tail->prev;
		sbrk( -(((void*) sbrk(0)) - ((void*) temp)) ); // move _brk to resize heap
		temp = tail;
		curr = tail;
		//check if list is empty and adjust pointers and exit loop
		if (curr == 0) {
			head = 0;
			tail = 0;
			break;
		}
	}
	coalese(curr);
}

void coalese(void *ptr) {
	//check for empty pointer
	if (ptr == 0) {
		return;
	}

	Node* tempCurr = (Node*) ptr;
	Node* tempPrev = tempCurr->prev;
	Node* tempNext = tempCurr->next;

	//loop for iterating through list prior
	while (tempPrev != 0 && tempPrev->freeSpace == 1) {
		//check if two adjacent nodes are free
		if (tempPrev->prev != 0 && tempPrev->prev->freeSpace == 1) {
			tempPrev = tempPrev->prev;
		} else { //otherwise break;
			break;
		}
	}

	//loop for interating through list after ptr
	while (tempNext != 0 && tempNext->freeSpace == 1) {
		//check if two adjacent nodes are free
		if (tempNext->next != 0 && tempNext->next->freeSpace == 1) {
			tempNext = tempNext->next;
		} else { //otherwise break;
			break;
		}
	}

	//Check if two chunks left can be coalesed
	if (tempPrev != 0 && tempPrev->freeSpace == 1) {
		//check if next chunk can join prev
		if (tempNext != 0 && tempNext->freeSpace == 1) {
			tempPrev->next = tempNext->next;
			tempPrev->chunkSize += tempCurr->chunkSize + tempNext->chunkSize + 2*sizeof(Node);
		} else { //otherwise only prev can be joined
			tempPrev->next = tempNext;
			tempPrev->chunkSize += tempCurr->chunkSize + sizeof(Node);
		}
	} else if (tempNext != 0 && tempNext->freeSpace == 1) { //join only next
		tempCurr->next = tempNext->next;
		tempCurr->chunkSize += tempNext->chunkSize + sizeof(Node);
	}

	//update curr node
	if (curr > tail) {
		curr = tail;
	}

}

void* maxFreeSpace(int amount) {
	Node* temp = head;
	Node* maxSizeChunk = 0;

	//Search for largest block
	while (temp != tail && temp != 0) {
		if (maxSizeChunk == 0) { //set max chunk to first block that meets criteria
			//check if free space that fits
			if (temp->freeSpace == 1 && temp->chunkSize >= amount) {
				maxSizeChunk = temp;
			}
		} else if (temp->freeSpace == 1 && temp->chunkSize > maxSizeChunk->chunkSize) {
			maxSizeChunk = temp;
		}
		temp = temp->next;
	}

	//case for tail node
	if (temp->freeSpace == 1 && temp != 0 && temp->chunkSize >= amount) {
		if (maxSizeChunk == 0 || (maxSizeChunk->chunkSize < temp->chunkSize)) { //If we never found a maxSizeChunk
			maxSizeChunk = temp;
		}
	}
	return maxSizeChunk;
}