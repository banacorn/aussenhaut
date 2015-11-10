#ifndef PARSER_H_
#define PARSER_H_

#include "type.h"
#include "string.h"

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

typedef struct Command {
    String_ * name;
    List_ * args;
} Command;

Command * command(String_ * name, List_ * args);
Command * parse_command(String_ * str);
Command * copy_command(Command *);
int arg_length(Command *);
char ** clone_char_array(Command *, String_ *);
void free_command_char_array(Command *, char **);
void free_command(Command * node);
void print_command(Command * node);

////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////

typedef struct Line {
    List_ * cmds;
    // redirection
    Bool redirect;  // >
    String_ * target;
    // piping
    int out;        // |
    int err;        // !
} Line;

Line * line(List_ *, Bool, String_ *, int, int);
Line * parse_line(String_ *);
Line * copy_line(Line *);
void print_line(Line *);
void free_line(Line *);
Box_ * box_line(Line *);

#endif // PARSER_H_
