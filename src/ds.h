#ifndef DS_H_
#define DS_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//  String
////////////////////////////////////////////////////////////////////////////////

typedef struct String {
    char * content;
} String;

String * string(char * chars);
String * string_n(char * chars, size_t n);
String * copy_string(String * str);
size_t string_size(String * str);
void free_string(String * str);

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
void print_list_str(ListStr * xs);
void free_list_str(ListStr * xs);

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

typedef struct Command {
    String *  name;
    ListStr * args;
} Command;

Command * command(String * name, ListStr * args);
Command * parse_command(String * str);
void free_command(Command * node);
void print_command(Command * node);

#endif // DS_H_
