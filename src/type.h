#ifndef TYPE_H_
#define TYPE_H_

#include <stdlib.h>
#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////
//  Generic boxed data
////////////////////////////////////////////////////////////////////////////////

typedef struct Box_ {
    void * content;
    void (*destruct_or)(void *);
    void * (*copier)(void *);
    void (*print_er)(void *);
} Box_;

Box_ * box(void *, void (*)(void *), void * (*)(void *), void (*)(void *));
void * unbox(Box_ *);
Box_ * copy_(Box_ *);
void destruct_(Box_ *);
void print_(Box_ *);

////////////////////////////////////////////////////////////////////////////////
//  Pair
////////////////////////////////////////////////////////////////////////////////

typedef struct Pair {
    Box_ * Fst;
    Box_ * Snd;
} Pair;

Pair * pair(Box_ *, Box_ *);
void * fst(Pair *);
void * snd(Pair *);
Pair * copy__pair(Pair *);
void free_pair(Pair *);
void print__pair(Pair *);
Box_ * box_pair(Pair *);

////////////////////////////////////////////////////////////////////////////////
//  Either
////////////////////////////////////////////////////////////////////////////////

typedef struct Either {
    Box_ * Left;
    Box_ * Right;
} Either;

Either * left(Box_ *);
Either * right(Box_ *);
Either * copy__either(Either *);
void free_either(Either *);
void print__either(Either *);
Box_ * box_either(Either *);


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
    Box_ * Just;
} Maybe;

Maybe * nothing();
Maybe * just(Box_ *);
void * from_just(Maybe *);

Maybe * copy__maybe(Maybe *);
void print__maybe(Maybe *);
void free_maybe(Maybe *);
Box_ * box_maybe(Maybe *);

////////////////////////////////////////////////////////////////////////////////
//  List_
////////////////////////////////////////////////////////////////////////////////

typedef struct List_ {
    Bool Nil;               //  Nil
    struct List_ * Cons;     //  Cons
    Box_ * data;             //  Carrier
} List_;

List_ * nil();
List_ * cons(Box_ *, List_ *);
List_ * snoc(List_ *, Box_ *);
List_ * copy__list(List_ *);

void * elemAt(List_ *, int);

void * head(List_ *);
List_ * tail(List_ *);
List_ * init(List_ *);
void * last(List_ *);

List_ * append_(List_ *, List_ *);
List_ * reverse_(List_ *);

List_ * map_(Box_ *(*)(Box_ *), List_ *);
List_ * filter_(Bool (*)(Box_ *), List_ *);

Bool null(List_ *);
int length(List_ *);

void free_list(List_ *);
void print__list(List_ *);
Box_ * box_list(List_ *);

#endif // TYPE_H_
