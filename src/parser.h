#ifndef PARSER_H_
#define PARSER_H_

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
int arg_length(Command *);
char ** clone_char_array(Command *, String *);
void free_command_char_array(Command *, char **);
void free_command(Command * node);
void print_command(Command * node);

////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////

typedef struct Line {
    List * cmds;
    // redirection
    Bool redirect;  // >
    String * target;
    // piping
    int out;        // |
    int err;        // !
} Line;

Line * line(List *, Bool, String *, int, int);
Line * parse_line(String *);
Line * copy_line(Line *);
void print_line(Line *);
void free_line(Line *);
Box * box_line(Line *);

#endif // PARSER_H_
