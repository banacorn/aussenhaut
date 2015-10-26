#ifndef DS_H_
#define DS_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//  Generic Data
////////////////////////////////////////////////////////////////////////////////

typedef struct Data {
    void * content;
    size_t size;
} Data;

Data * data(void * content, size_t size);
void free_data(Data * node);
Data * copy_data(Data * node);

////////////////////////////////////////////////////////////////////////////////
//  String
////////////////////////////////////////////////////////////////////////////////

// typedef char * String;

typedef struct String {
    char * content;
} String;

String * string(char * chars);
String * string_n(char * chars, size_t n);
String * copy_string(String * str);
size_t string_size(String * str);
void free_string(String * str);

////////////////////////////////////////////////////////////////////////////////
//  List
////////////////////////////////////////////////////////////////////////////////

typedef struct List {
    // Nil
    int             nil;
    // Cons
    struct List *   cons;
    Data *          data;
} List;

// smart constructors
List * nil();
List * cons(Data * data, List * xs);

// deconstructor
void free_list(List * xs);

// copy
List * copy_list(List * xs);

// print
void print_list(List * xs);

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

typedef struct Command {
    String *    name;
    List *      args;
} Command;

Command * command(String * name, List * args);
Command * parse_command(String * str);
void free_command(Command * node);
void print_command(Command * node);

#endif // DS_H_
