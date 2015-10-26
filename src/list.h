#ifndef LIST_H_
#define LIST_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct List {
    // Nil
    int             nil;
    // Cons
    struct List *   cons;
    void *          data;
} List;

// smart constructors
List * nil();
List * cons(void * data, size_t data_size, List * xs);
// deconstructor
void free_list(List * xs);
// print
void print_list(List * xs);

#endif // LIST_H_
