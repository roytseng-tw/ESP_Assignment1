#include <stdlib.h>
#include "test2.h"

void add(test_struct *a) {
	a->a ++;
	return;
}

void create(test_struct **a) {
	(*a) = malloc(sizeof(test_struct));
	(*a)->str = malloc(sizeof(char)*10);
    (*a)->a = 10;
}