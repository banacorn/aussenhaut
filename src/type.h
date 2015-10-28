#ifndef TYPE_H_
#define TYPE_H_

#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//  Generic boxed data
////////////////////////////////////////////////////////////////////////////////

typedef struct Box {
    void * content;
    void (*destructor)(void *);
    void * (*copier)(void *);
    void (*printer)(void *);
} Box;

Box * box(void *, void (*)(void *), void * (*)(void *), void (*)(void *));
Box * copy(Box *);
void destruct(Box *);
void print(Box *);

////////////////////////////////////////////////////////////////////////////////
//  Pair
////////////////////////////////////////////////////////////////////////////////

typedef struct Pair {
    Box * fst;
    Box * snd;
} Pair;

Pair * pair(Box *, Box *);
Pair * copy_pair(Pair *);
void free_pair(Pair *);
void print_pair(Pair *);


////////////////////////////////////////////////////////////////////////////////
//  Either
////////////////////////////////////////////////////////////////////////////////

typedef struct Either {
    Box * left;
    Box * right;
} Either;

Either * left(Box *);
Either * right(Box *);
Either * copy_either(Either *);
void free_either(Either *);
void print_either(Either *);




////////////////////////////////////////////////////////////////////////////////
//  Bool
////////////////////////////////////////////////////////////////////////////////

typedef unsigned int Bool;
#define TRUE 1
#define FALSE 0

////////////////////////////////////////////////////////////////////////////////
//  List
////////////////////////////////////////////////////////////////////////////////

typedef struct List {
    Bool nil;               //  Nil
    struct List * cons;     //  Cons
    Box * data;             //  Carrier
} List;

List * nil();
List * cons(Box *, List *);
List * snoc(List *, Box *);
List * copy_list(List *);

List * append(List *, List *);
List * reverse(List *);

Bool null(List *);
int length(List *);

void free_list(List *);
void print_list(List *);

#endif // TYPE_H_
