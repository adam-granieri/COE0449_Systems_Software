/*
   LAB#4

   FILL IN AND TEST ALL THESE FUNCTIONS PROTOTYPED BELOW
   SEVERAL ARE ALREADY FILED IN such as insertAtFront() and printList(), fatal, etc.

RUBRIK:

worth 15% searchNode
worth 20% insertAtTail  - MUST RE_USE insertAtFront
worth 10% removeAtFront
worth 40% removeNode    - MUST RE_USE removeAtFront
worth 15% freeList      - MUST RE_USE removeAtFront

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Node
{
  char  *word;
  struct Node  *next;
} Node;


Node *searchNode( Node * head, char * key );
void insertAtFront( Node **head, char * key );  // ALREADY WRITTEN FOR YOU
void insertAtTail( Node **head, char * key );
void removeAtFront( Node ** head );
void removeNode( Node **head, char * key );
void freeList( Node **head );
void printList( Node * head ); // ALREADY WRITTEN FOR YOU
void fatal( char * msg ); // ALREADY WRITTEN FOR YOU

#define BUFFER_CAP 20

int main()
{
  Node *head = NULL;

  while (1)
  {
		char option;
		printf("\nChoose 'H'ead Insert, 'T'ail insert, 'R'emove, 'S'earch, F'ree, 'Q'uit " );
		fflush( stdout );
		int result = scanf(" %c%*[^\n]", &option); getchar();  // MAGIC BULLET TO CORRECTLY READ A SINGLE CHAR FROM STDIN
		if (result <1) fatal("failure reading from stdin\n");

		if (option == 'H' )
		{
			char * word=malloc(BUFFER_CAP);  // DONT ENTER ANY LONG WORDS!
			printf("Enter a word to insertAtFront: " );
			fflush( stdout );
			char * result = fgets( word, BUFFER_CAP, stdin );
			if (result==NULL) fatal("failure reading from stdin\n");
			strtok(word,"\n"); // overwrites '\n' with  '\0'
			insertAtFront( &head, word ); /* shallow copy string into list   */
			printList( head );
		}
		if (option == 'T' )
		{
			char * word=malloc(BUFFER_CAP);  // DONT ENTER ANY LONG WORDS!
			printf("Enter a word to insertAtTail: " );
			fflush( stdout );
			char * result = fgets( word, BUFFER_CAP, stdin );
			if (result==NULL) fatal("failure reading from stdin\n");
			strtok(word,"\n"); // overwrites '\n' with  '\0'
			insertAtTail( &head, word ); /* shallow copy string into list   */
			printList( head );
		}
    		if (option == 'R' )
		{
			char * word=malloc(BUFFER_CAP);  // DONT ENTER ANY LONG WORDS!
			printf("Enter a word to remove: " );
			fflush( stdout );
			char * result = fgets( word, BUFFER_CAP, stdin );
			if (result==NULL) fatal("failure reading from stdin\n");
			strtok(word,"\n"); // overwrites '\n' with  '\0'
			removeNode( &head, word );
			printList( head );
			free( word ); // we were just using it for matching
		}
		if (option == 'S' )
		{
			char * word=malloc(BUFFER_CAP);  // DONT ENTER ANY LONG WORDS!
			printf("Enter a word to find: " );
			fflush( stdout );
			char * result = fgets( word, BUFFER_CAP, stdin );
			if (result==NULL) fatal("failure reading from stdin\n");
			strtok(word,"\n"); // overwrites '\n' with  '\0'
			if (searchNode( head, word ))
				fprintf(stderr, "%s FOUND\n",word );
			else
				fprintf(stderr, "%s NOT FOUND\n",word );
			printList( head );
			free( word ); // we were just using it for matching
		}
		if (option == 'F' ) // free the entire list (remember to set head to NULL)
		{
			freeList( &head );
			printList( head );
		}
		else if (option == 'Q' )
			exit( 0 );
	} // END WHILE

  	return 0;
}

//  # # # # # # # # # # # # # # # # # # # # # # #
//  # # # # # # # # # # # # # # # # # # # # # # #

// GIVEN AS IS - DO NOT MODIFY
void printList( Node * head )
{
	fprintf(stderr, "\nLIST: ");
	while (head)
	{
		fprintf(stderr, "%s ", head->word );
		head=head->next;
	}
	int result =  fprintf(stderr, "\n");
	if (result<1) fatal("failure writing to stdout\n");
}


// GIVEN AS IS - DO NOT MODIFY
void fatal( char * msg )
{
	printf("\nFatal Error: %s\n\n", msg );
	exit(0);
}

// GIVEN AS IS - DO NOT MODIFY
void insertAtFront( Node **head, char * key )
{
	Node *new =  malloc( sizeof(Node) );
	if (!new)  fatal("Malloc of new Node failed");
	new->word = key;
	new->next = *head;
	*head = new;
}

void insertAtTail( Node **head, char * word )
{
  	Node *new = *head;
  	if (!new) 
  	{
  		insertAtFront(head, word);
  	} 
	else 
  	{
  		while(new->next != NULL)
  		{
  			new = new->next;
  		}
  		insertAtFront(&new->next, word);
  	}
}

void removeAtFront( Node ** head )
{
	Node *tmp = *head;
	if (tmp==NULL)
	{
		printf("List is empty or has no head ref\n");
		return;
	}
	else 
	{
		*head = tmp->next;
		free(tmp);
	}
}

void removeNode( Node ** head, char * key )
{
	Node *tmp = *head;
	if (tmp == NULL || (tmp != NULL && strcmp(tmp->word, key) == 0 ))
	{
		removeAtFront(head);
		return;
	}
	else
	{
		Node *prev = tmp;
		while (tmp != NULL && strcmp(tmp->word, key) != 0)
		{
			prev = tmp;
			tmp = tmp->next;
		}
		if (tmp == NULL)
		{
			printf("Node not found\n");
			return;
		}
		prev->next = tmp->next;
		free(tmp);
	}
}

Node * searchNode ( Node * head, char * key )
{
	Node *curr = head;
	while (curr != NULL)
	{
		if (strcmp(curr->word, key) == 0){
			return curr;
		} else {
			curr = curr->next;
		}
	}
	return NULL;
}

void freeList(  Node ** head )
{
	if (*head == NULL)
	{
		removeAtFront(head);
		return;
	}
	else
	{
		while (*head != NULL)
		{
			removeAtFront(head);
		}
	}
}
