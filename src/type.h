#ifndef TYPE_H_
#define TYPE_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
//  Bool
////////////////////////////////////////////////////////////////////////////////

typedef unsigned int Bool;
#define TRUE 1
#define FALSE 0

////////////////////////////////////////////////////////////////////////////////
//  String
////////////////////////////////////////////////////////////////////////////////

typedef struct String {
    char * content;
} String;

String * string(char * chars);
String * string_n(char * chars, int len);
String * copy_string(String * str);
String * trim(String * str);
String * substring(String * str, int, int); // allocates new spaces
size_t string_length(String * str);
size_t string_size(String * str);   // memory size
Bool null_string(String * str);
char * show_string(String * str);
void print_string(String * str);
void free_string(String * str);
Box * box_str(char *);

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
