#ifndef DS_H_
#define DS_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//  Bool
////////////////////////////////////////////////////////////////////////////////

typedef unsigned int Bool;
#define TRUE 1
#define FALSE 0

////////////////////////////////////////////////////////////////////////////////
//  Generic data boxed with builtin destructor
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

////////////////////////////////////////////////////////////////////////////////
//  List of Strings
////////////////////////////////////////////////////////////////////////////////

typedef struct ListStr {
    // Nil
    int              nil;
    // Cons
    struct ListStr * cons;
    String *         data;
} ListStr;

// smart constructors
ListStr * nil_str();
ListStr * cons_str(String * data, ListStr * xs);
ListStr * snoc_str(ListStr * xs, String * data);
ListStr * copy_list_str(ListStr * xs);
ListStr * append_str(ListStr * xs, ListStr * ys);
ListStr * reverse_str(ListStr * xs);
ListStr * tokenize(String * data, String * sep);
Bool null_str(ListStr *);
int length_str(ListStr *);
char * show_list_str(ListStr * xs);
void print_list_str(ListStr * xs);
void free_list_str(ListStr * xs);

////////////////////////////////////////////////////////////////////////////////
//  List of Ints
////////////////////////////////////////////////////////////////////////////////

typedef struct ListInt {
    // Nil
    int              nil;
    // Cons
    struct ListInt * cons;
    int              data;
} ListInt;

// smart constructors
ListInt * nil_int();
ListInt * cons_int(int data, ListInt * xs);
ListInt * snoc_int(ListInt * xs, int data);
ListInt * copy_list_int(ListInt * xs);
Bool null_int(ListInt *);
int length_int(ListInt *);
void print_list_int(ListInt * xs);
void free_list_int(ListInt * xs);

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

typedef struct Command {
    String *  name;
    ListStr * args;
} Command;

Command * command(String * name, ListStr * args);
Command * parse_command(String * str);
Command * copy_command(Command *);
void free_command(Command * node);
void print_command(Command * node);

////////////////////////////////////////////////////////////////////////////////
//  List of Commands
////////////////////////////////////////////////////////////////////////////////

typedef struct ListCmd {
    // Nil
    int              nil;
    // Cons
    struct ListCmd * cons;
    Command *        data;
} ListCmd;

// smart constructors
ListCmd * nil_cmd();
ListCmd * cons_cmd(Command * data, ListCmd * xs);
ListCmd * snoc_cmd(ListCmd * xs, Command * data);
ListCmd * copy_list_cmd(ListCmd * xs);
Bool null_cmd(ListCmd *);
int length_cmd(ListCmd *);
void print_list_cmd(ListCmd * xs);
void free_list_cmd(ListCmd * xs);

////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////

typedef struct Line {
    ListCmd * cmds;
    Bool redirect;   // >
    int out;        // |
    int err;        // !
} Line;

Line * line(ListCmd *, Bool, int, int);
Line * parse_line(String *);
Line * copy_line(Line *);
void print_line(Line *);
void free_line(Line *);


#endif // DS_H_
