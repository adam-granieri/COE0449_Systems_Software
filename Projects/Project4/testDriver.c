#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

//Adam Granieri
//COE 0449 Systems Software
//Fall 2018

#define myFree my_free
#define myMalloc my_worstfit_malloc

void first_test(void);
void second_test(void);

int main() {
	printf("FIRST TEST BEGIN\n");
	first_test();
	printf("FIRST TEST END\n");
	printf("\nSECOND TEST BEGIN\n");
	second_test();
	printf("SECOND TEST END\n");
	return 0;
}

void first_test() {
	//Malloc a bunch a data
	printf("brk before mallocs: %p\n", sbrk(0));
	void* m1 = myMalloc(300);
	void* m2 = myMalloc(4);
	void* m3 = myMalloc(78);
	void* m4 = myMalloc(100);
	printf("brk after mallocs: %p\n", sbrk(0));

	//Free a few
	myFree(m3);
	printf("brk after free of m3 at %p : %p\n", m3, sbrk(0));
	myFree(m4);
	printf("brk after free of m4 at %p : %p\n", m4, sbrk(0));

	//Malloc some more data
	void* m5 = myMalloc(30);
	printf("brk after malloc of m5 at %p : %p\n", m5, sbrk(0));
	void* m6 = myMalloc(230);
	printf("brk after malloc of m6 at %p : %p\n", m6, sbrk(0));

	//Free everything
	myFree(m6);
	myFree(m5);
	myFree(m1);
	myFree(m2);
	printf("brk after free everything: %p\n", sbrk(0));

	//malloc one last time
	void* oneLastTime = myMalloc(350);
	printf("brk after allocating on empty list: %p\n", sbrk(0));

	//And free one last time
	myFree(oneLastTime);
	printf("brk after freeing one last time: %p\n", sbrk(0));
}

void second_test() {
	//Malloc some data part 2
	printf("brk before mallocs: %p\n", sbrk(0));
	void* n1 = myMalloc(20);
	void* n2 = myMalloc(3);
	void* n3 = myMalloc(100);
	printf("brk after mallocs: %p\n", sbrk(0));

	//Free a few part 2 (hey that rhymes)
	myFree(n2);
	myFree(n3);
	printf("brk after 2 frees: %p\n", sbrk(0));

	//Malloc some more data part 2
	void* n4 = myMalloc(200);
	printf("brk after malloc: %p\n", sbrk(0));

	//Free everything part 2
	myFree(n1);
	myFree(n4);
	printf("brk after final free: %p\n", sbrk(0));

}