#ifndef EXEC_H_
#define EXEC_H_

#include "Cello.h"
#include "parser.h"

var env_path(var);
var search_exec(var, var);

// for execv
char ** clone_char_array(var, var);
void free_command_char_array(var, char **);

int exec_command(var env, var command, var socket);
void exec_line(var env, struct Line* line, struct Socket* socket);

#endif // EXEC_H_
