#include "ds.h"
#include "type.h"

#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

Command * command(String * name, List * args)
{
    Command * node = malloc(sizeof(Command));
    node -> name = name;
    node -> args = args;
    return node;
}

Command * copy_command(Command * node)
{
    return command(copy_string(node -> name), copy_list(node -> args));
}

Command * parse_command(String * str)
{
    String * name;
    List * args = nil();

    if (null_string(str)) {
        name = string("");
    } else {
        // name
        char * p = strtok(str -> content," ");
        name = string(p);
        // args
        while (p = strtok(NULL, " "), p != NULL) {
            args = snoc(args, box_str(p));
        }
    }
    free_string(str);
    return command(name, args);
}

void free_command(Command * node)
{
    free_string(node -> name);
    free_list(node -> args);
    free(node);
}


void print_command(Command * node)
{
    if (node -> args -> nil) {
        printf("%s", node -> name -> content);
    } else {
        printf("%s ", node -> name -> content);
        print_list(node -> args);
    }
}

Box * box_cmd(String * name, List * args)
{
    return box(command(name, args), (void (*)(void *))free_command, (void * (*)(void *))copy_command, (void (*)(void *))print_command);
}

////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////

Line * line(List * cmds, Bool redirect, int out, int err)
{
    Line * node = malloc(sizeof(Line));
    node -> cmds = cmds;
    node -> redirect = redirect;
    node -> out = out;
    node -> err = err;
    return node;
}

// Line * parse_line(String * raw_str)
// {
//     String * str = trim(raw_str);
//     ListCmd * cmds = nil_cmd();
//     int numbered_pipe = 0;
//     Bool redirect = FALSE;
//     int out = -1;
//     int err = -1;
//
//     if (string_length(str) == 0) {
//         free_string(str);
//         return line(cmds, redirect, out, err);
//     } else if (string_length(str) == 1) {
//         cmds = cons_cmd(parse_command(str), cmds);
//         return line(cmds, redirect, out, err);
//     }
//
//     // has numbered pipe at the end?
//     char * end = str -> content + string_length(str) - 1;
//     if (isdigit(*end)) {
//         while (end > str -> content && isdigit(*end)) end--;
//         if (*(end - 1) == ' ' && *end == '|') {
//             out = atoi(end + 1);
//             numbered_pipe++;
//         } else if (*(end - 1) == ' ' && *end == '!') {
//             err = atoi(end + 1);
//             numbered_pipe++;
//         }
//     }
//
//
//     // upon discovery, fill the parsed part with spaces
//     if (numbered_pipe == 1) {
//         while(end < str -> content + string_length(str)) {
//             *end = ' ';
//             end++;
//         }
//         str = trim(str);
//     }
//
//     // has numbered pipe at the end?
//     end = str -> content + string_length(str) - 1;
//     if (isdigit(*end)) {
//         while (end > str -> content && isdigit(*end)) end--;
//         if (*(end - 1) == ' ' && *end == '|' && out == -1) {
//             out = atoi(end + 1);
//             numbered_pipe++;
//         } else if (*(end - 1) == ' ' && *end == '!' && err == -1) {
//             err = atoi(end + 1);
//             numbered_pipe++;
//         }
//     }
//
//     // upon discovery, fill the parsed part with spaces
//     if (numbered_pipe == 2) {
//         while(end < str -> content + string_length(str)) {
//             *end = ' ';
//             end++;
//         }
//         str = trim(str);
//     }
//
//     // seperating input with " | " and cons the substring before it
//     int offset = 0;
//     int i = 0;
//     char * ref = str -> content;
//     while (i + 2 < string_length(str)) {
//         if (ref[i] == ' ' && ref[i + 1] == '|' && ref[i + 2] == ' ') {
//             // found!
//             Command * cmd = parse_command(substring(str, offset, i));
//             cmds = snoc_cmd(cmds, cmd);
//             offset = i = i + 3;
//         } else {
//             i++;
//         }
//     }
//
//     // see if there's redirection ">" in the last 2 commands
//     Command * cmd = parse_command(substring(str, offset, i + 2));
//     cmds = snoc_cmd(cmds, cmd);
//     free_string(str);
//     return line(cmds, redirect, out, err);
// }
//
// Line * copy_line(Line * node)
// {
//     return line(copy_list_cmd(node -> cmds), node -> redirect, node -> out, node -> err);
// }
//
// void print_line(Line * node)
// {
//     print_list_cmd(node -> cmds);
//     if (node -> redirect) {
//         printf(" >");
//     } else {
//         if (node -> out != -1)
//             printf(" |%d", node -> out);
//         if (node -> err != -1)
//             printf(" !%d", node -> err);
//     }
//     printf("\n");
// }
//
// void free_line(Line * node)
// {
//     free_list_cmd(node -> cmds);
//     free(node);
// }
