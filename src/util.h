#ifndef UTIL_H_
#define UTIL_H_

#include "type.h"
#include "string.h"

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

typedef struct Command {
    String * name;
    List * args;
} Command;

Command * command(String * name, List * args);
Command * parse_command(String * str);
Command * copy_command(Command *);
void free_command(Command * node);
void print_command(Command * node);

////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////

typedef struct Line {
    List * cmds;
    Bool redirect;  // >
    int out;        // |
    int err;        // !
} Line;

// Line * line(ListCmd *, Bool, int, int);
// Line * parse_line(String *);
// Line * copy_line(Line *);
void print_line(Line *);
void free_line(Line *);

#endif // UTIL_H_
