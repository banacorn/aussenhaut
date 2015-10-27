#include "ds.h"

#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////
//  String
////////////////////////////////////////////////////////////////////////////////


String * string_n(char * chars, int len)
{
    String * new_str = malloc(sizeof(String));
    new_str -> content = malloc(len + 1);
    char * buf = new_str -> content;
    strncpy(buf, chars, len);
    buf[len] = 0;
    return new_str;
}

String * string(char * chars)
{
    return string_n(chars, strlen(chars));
}

String * copy_string(String * str)
{
    return string(str -> content);
}

String * substring(String * str, int from, int to)
{
    if (from > to) {
        return string("");
    } else if (0 > from) {
        return string("");
    } else if (to > string_length(str)) {
        return string("");
    } else {
        char * ref = str -> content;
        return string_n(ref + from, to - from);
    }
}

size_t string_length(String * str)
{
    return strlen(str -> content);
}
size_t string_size(String * str)
{
    return strlen(str -> content) + 1;
}

Bool null(String * str)
{
    if (strlen(str -> content) == 0)
        return TRUE;
    else
        return FALSE;
}

char * show_string(String * str)
{
    return str -> content;
}

void print_string(String * str)
{
    char * ref = str -> content;
    printf("%s\n", ref);
}

void free_string(String * str)
{
    free(str -> content);
    free(str);
}

////////////////////////////////////////////////////////////////////////////////
//  List of Strings
////////////////////////////////////////////////////////////////////////////////

ListStr * nil_str()
{
    ListStr * node = malloc(sizeof(ListStr));
    node -> nil = 1;
    node -> cons = NULL;
    node -> data = NULL;
    return node;
}

ListStr * cons_str(String * data, ListStr * xs)
{
    ListStr * node = malloc(sizeof(ListStr));
    node -> nil = 0;
    node -> cons = xs;
    node -> data = data;
    return node;
}

// O(n)
ListStr * snoc_str(ListStr * xs, String * data)
{
    if (xs -> nil) {
        return cons_str(data, xs);
    } else {
        ListStr * result = cons_str(xs -> data, snoc_str(xs -> cons, data));
        free(xs);
        return result;
    }
}

ListStr * append_str(ListStr * xs, ListStr * ys)
{
    if (xs -> nil) {
        free(xs);
        return ys;
    } else {
        ListStr * result = cons_str(xs -> data, append_str(xs -> cons, ys));
        free(xs);
        return result;
    }
}


// O(n ^ 2), bad
ListStr * reverse_str(ListStr * xs)
{
    if (xs -> nil) {
        return xs;
    } else {
        ListStr * result = snoc_str(reverse_str(xs -> cons), xs -> data);
        free(xs);
        return result;
    }
}

void free_list_str(ListStr * xs)
{
    if (xs -> nil) {
        free(xs);
    } else {
        free_list_str(xs -> cons);
        free_string(xs -> data);
        free(xs);
    }
}

ListStr * copy_list_str(ListStr * xs)
{
    if (xs -> nil) {
        return nil_str();
    } else {
        ListStr * new_xs = copy_list_str(xs -> cons);
        String * new_data = copy_string(xs -> data);
        return cons_str(new_data, new_xs);
    }
}

void print_list_str(ListStr * xs)
{
    if (xs -> nil) {
        // printf("\n");
    } else {
        printf("%s ", xs -> data -> content);
        print_list_str(xs -> cons);
    }
}

////////////////////////////////////////////////////////////////////////////////
//  List of Ints
////////////////////////////////////////////////////////////////////////////////

ListInt * nil_int()
{
    ListInt * node = malloc(sizeof(ListInt));
    node -> nil = 1;
    node -> cons = NULL;
    node -> data = 0;
    return node;
}

ListInt * cons_int(int data, ListInt * xs)
{
    ListInt * node = malloc(sizeof(ListInt));
    node -> nil = 0;
    node -> cons = xs;
    node -> data = data;
    return node;
}

// O(n)
ListInt * snoc_int(ListInt * xs, int data)
{
    if (xs -> nil) {
        return cons_int(data, xs);
    } else {
        ListInt * result = cons_int(xs -> data, snoc_int(xs -> cons, data));
        free(xs);
        return result;
    }
}

ListInt * copy_list_int(ListInt * xs)
{
    if (xs -> nil) {
        return nil_int();
    } else {
        ListInt * new_xs = copy_list_int(xs -> cons);
        return cons_int(xs -> data, new_xs);
    }
}
void print_list_int(ListInt * xs)
{
    if (xs -> nil) {
        // printf("\n");
    } else {
        printf("%d ", xs -> data);
        print_list_int(xs -> cons);
    }
}

void free_list_int(ListInt * xs)
{
    if (xs -> nil) {
        free(xs);
    } else {
        free_list_int(xs -> cons);
        free(xs);
    }
}

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

Command * command(String * name, ListStr * args)
{
    Command * node = malloc(sizeof(Command));
    node -> name = name;
    node -> args = args;
    return node;
}

Command * copy_command(Command * node)
{
    return command(copy_string(node -> name), copy_list_str(node -> args));
}


void free_command(Command * node)
{
    free_string(node -> name);
    free_list_str(node -> args);
    free(node);
}

Command * parse_command(String * str)
{
    Command * node = malloc(sizeof(Command));
    // name
    char * p = strtok(str -> content," ");
    node -> name = string(p);
    // args
    node -> args = nil_str();
    while (p = strtok(NULL, " "), p != NULL) {
        node -> args = snoc_str(node -> args, string(p));
    }
    free_string(str);
    return node;
}

void print_command(Command * node)
{
    printf("%s ", node -> name -> content);
    print_list_str(node -> args);
}

////////////////////////////////////////////////////////////////////////////////
//  List of Strings
////////////////////////////////////////////////////////////////////////////////

ListCmd * nil_cmd()
{
    ListCmd * node = malloc(sizeof(ListCmd));
    node -> nil = 1;
    node -> cons = NULL;
    node -> data = NULL;
    return node;
}

ListCmd * cons_cmd(Command * data, ListCmd * xs)
{
    ListCmd * node = malloc(sizeof(ListCmd));
    node -> nil = 0;
    node -> cons = xs;
    node -> data = data;
    return node;
}

// O(n)
ListCmd * snoc_cmd(ListCmd * xs, Command * data)
{
    if (xs -> nil) {
        return cons_cmd(data, xs);
    } else {
        ListCmd * result = cons_cmd(xs -> data, snoc_cmd(xs -> cons, data));
        free(xs);
        return result;
    }
}

void free_list_cmd(ListCmd * xs)
{
    if (xs -> nil) {
        free(xs);
    } else {
        free_list_cmd(xs -> cons);
        free_command(xs -> data);
        free(xs);
    }
}

ListCmd * copy_list_cmd(ListCmd * xs)
{
    if (xs -> nil) {
        return nil_cmd();
    } else {
        ListCmd * new_xs = copy_list_cmd(xs -> cons);
        Command * new_data = copy_command(xs -> data);
        return cons_cmd(new_data, new_xs);
    }
}

void print_list_cmd(ListCmd * xs)
{
    if (xs -> nil) {
    } else {
        print_command(xs -> data);
        printf(" | ");
        print_list_cmd(xs -> cons);
    }
}

////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////
//
// typedef struct Line {
//     ListCmd * cmds;
//     int       pipeTo;
// } Line;

Line * line(ListCmd * cmds, int pipeTo)
{
    Line * node = malloc(sizeof(Line));
    node -> cmds = cmds;
    node -> pipeTo = pipeTo;
    return node;
}


// Line * parse_numbered_pipes(String * str)

Line * parse_line(String * str)
{
    Line * node = malloc(sizeof(Line));
    node -> cmds = nil_cmd();
    node -> pipeTo = 1;

    // seperating input with " | " and cons the substring before it
    int offset = 0;
    int i = 0;
    char * ref = str -> content;
    while (i < string_length(str) - 2) {
        if (ref[i] == ' ' && ref[i + 1] == '|' && ref[i + 2] == ' ') {
            // found!
            Command * cmd = parse_command(substring(str, offset, i));
            node -> cmds = cons_cmd(cmd, node -> cmds);
            offset = i;
            break;
        } else {
            // not found
            i++;
        }
    }
    if (i == string_length(str) - 2)
        i = -1;

    // //  skip the last " | "
    // offset += 3;
    // // dealing with the substring after " | "
    // i = offset;
    // while (i < string_length(str) - 2) {
    //     if (ref[i] == ' ' && ref[i + 1] == '|' && isdigit(ref[i + 2])) {
    //         // found!
    //         String * sub = substring(str, offset, i);
    //         print_string(sub);
    //         free_string(sub);
    //         // Command * cmd = parse_command(substring(str, offset, i));
    //         // node -> cmds = cons_cmd(cmd, node -> cmds);
    //         offset = i;
    //         break;
    //     } else {
    //         // not found
    //         i++;
    //     }
    // }

    free_string(str);
    return node;
}

Line * copy_line(Line * node)
{
    return line(copy_list_cmd(node -> cmds), node -> pipeTo);
}

void print_line(Line * node)
{
    print_list_cmd(node -> cmds);
    printf(" |%d\n", node -> pipeTo);
}

void free_line(Line * node)
{
    free_list_cmd(node -> cmds);
    free(node);
}
