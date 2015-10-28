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
void * unbox(Box *);
Box * copy(Box *);
void destruct(Box *);
void print(Box *);

////////////////////////////////////////////////////////////////////////////////
//  Pair
////////////////////////////////////////////////////////////////////////////////

typedef struct Pair {
    Box * Fst;
    Box * Snd;
} Pair;

Pair * pair(Box *, Box *);
void * fst(Pair *);
void * snd(Pair *);
Pair * copy_pair(Pair *);
void free_pair(Pair *);
void print_pair(Pair *);
Box * box_pair(Pair *);

////////////////////////////////////////////////////////////////////////////////
//  Either
////////////////////////////////////////////////////////////////////////////////

typedef struct Either {
    Box * Left;
    Box * Right;
} Either;

Either * left(Box *);
Either * right(Box *);
Either * copy_either(Either *);
void free_either(Either *);
void print_either(Either *);
Box * box_either(Either *);


////////////////////////////////////////////////////////////////////////////////
//  Bool
////////////////////////////////////////////////////////////////////////////////

typedef unsigned int Bool;
#define TRUE 1
#define FALSE 0

////////////////////////////////////////////////////////////////////////////////
//  Maybe
////////////////////////////////////////////////////////////////////////////////

typedef struct Maybe {
    Bool Nothing;   // TRUE if Nothing, FALSE, if Just
    Box * Just;
} Maybe;

Maybe * nothing();
Maybe * just(Box *);
void * from_just(Maybe *);

Maybe * copy_maybe(Maybe *);
void print_maybe(Maybe *);
void free_maybe(Maybe *);
Box * box_maybe(Maybe *);

////////////////////////////////////////////////////////////////////////////////
//  List
////////////////////////////////////////////////////////////////////////////////

typedef struct List {
    Bool Nil;               //  Nil
    struct List * Cons;     //  Cons
    Box * data;             //  Carrier
} List;

List * nil();
List * cons(Box *, List *);
List * snoc(List *, Box *);
List * copy_list(List *);

void * head(List *);
List * tail(List *);
List * init(List *);
void * last(List *);

List * append(List *, List *);
List * reverse(List *);

List * map(Box *(*)(Box *), List *);

Bool null(List *);
int length(List *);

void free_list(List *);
void print_list(List *);
Box * box_list(List *);

#endif // TYPE_H_
