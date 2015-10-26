#include "list.h"

////////////////////////////////////////////////////////////////////////////////
//  List
////////////////////////////////////////////////////////////////////////////////

List * nil()
{
    List * node = malloc(sizeof(List));
    node -> nil = 1;
    node -> cons = NULL;
    node -> data = NULL;
    return node;
}

List * cons(void * data, size_t data_size, List * xs)
{
    List * node = malloc(sizeof(List));
    node -> nil = 0;
    node -> cons = xs;
    void * new_data = malloc(data_size);
    memcpy(new_data, data, data_size);
    node -> data = new_data;
    return node;
}

void free_list(List * xs)
{
    if (xs -> nil) {
        free(xs);
    } else {
        free_list(xs -> cons);
        free(xs -> data);
        free(xs);
    }
}

void print_list(List * xs)
{
    if (xs -> nil) {
        printf("\n");
    } else {
        printf(" # ");
        print_list(xs -> cons);
    }
}
