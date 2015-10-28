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

String * trim(String * str)
{
    char * ref = str -> content;

    // Trim leading space
    while(isspace(*ref)) ref++;

    // All spaces?
    if(*ref == 0) {
        free_string(str);
        return string("");
    } else {
        // Trim trailing space
        char * end = ref + strlen(ref) - 1;
        while(end > ref && isspace(*end)) end--;

        // Write new null terminator
        *(end+1) = 0;
        String * new_str = string(ref);
        free_string(str);
        return new_str;
    }
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

Bool null_str(ListStr * xs)
{
    if (xs -> nil) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int length_str(ListStr * xs)
{
    if (xs -> nil) {
        return 0;
    } else {
        return 1 + length_str(xs -> cons);
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
    } else if (xs -> cons -> nil) {
        printf("%s", xs -> data -> content);
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

Bool null_int(ListInt * xs)
{
    if (xs -> nil) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int length_int(ListInt * xs)
{
    if (xs -> nil) {
        return 0;
    } else {
        return 1 + length_int(xs -> cons);
    }
}

void print_list_int(ListInt * xs)
{
    if (xs -> nil) {
    } else if (xs -> cons -> nil) {
        printf("%d", xs -> data);
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
    String * name;
    ListStr * args = nil_str();

    if (null(str)) {
        name = string("");
    } else {
        // name
        char * p = strtok(str -> content," ");
        name = string(p);
        // args
        while (p = strtok(NULL, " "), p != NULL) {
            args = snoc_str(args, string(p));
        }
    }
    free_string(str);
    return command(name, args);
}

void print_command(Command * node)
{
    if (node -> args -> nil) {
        printf("%s", node -> name -> content);
    } else {
        printf("%s ", node -> name -> content);
        print_list_str(node -> args);
    }
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

Bool null_cmd(ListCmd * xs)
{
    if (xs -> nil) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int length_cmd(ListCmd * xs)
{
    if (xs -> nil) {
        return 0;
    } else {
        return 1 + length_cmd(xs -> cons);
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
    } else if (xs -> cons -> nil) {
        printf("[");
        print_command(xs -> data);
        printf("]");
    } else {
        printf("[");
        print_command(xs -> data);
        printf("]");
        printf(" | ");
        print_list_cmd(xs -> cons);
    }
}

////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////

Line * line(ListCmd * cmds, Bool redirect, int out, int err)
{
    Line * node = malloc(sizeof(Line));
    node -> cmds = cmds;
    node -> redirect = redirect;
    node -> out = out;
    node -> err = err;
    return node;
}

Line * parse_line(String * raw_str)
{
    String * str = trim(raw_str);
    ListCmd * cmds = nil_cmd();
    int numbered_pipe = 0;
    Bool redirect = FALSE;
    int out = -1;
    int err = -1;

    if (string_length(str) == 0) {
        free_string(str);
        return line(cmds, redirect, out, err);
    } else if (string_length(str) == 1) {
        cmds = cons_cmd(parse_command(str), cmds);
        return line(cmds, redirect, out, err);
    }

    // has numbered pipe at the end?
    char * end = str -> content + string_length(str) - 1;
    if (isdigit(*end)) {
        while (end > str -> content && isdigit(*end)) end--;
        if (*(end - 1) == ' ' && *end == '|') {
            out = atoi(end + 1);
            numbered_pipe++;
        } else if (*(end - 1) == ' ' && *end == '!') {
            err = atoi(end + 1);
            numbered_pipe++;
        }
    }


    // upon discovery, fill the parsed part with spaces
    if (numbered_pipe == 1) {
        while(end < str -> content + string_length(str)) {
            *end = ' ';
            end++;
        }
        str = trim(str);
    }

    // has numbered pipe at the end?
    end = str -> content + string_length(str) - 1;
    if (isdigit(*end)) {
        while (end > str -> content && isdigit(*end)) end--;
        if (*(end - 1) == ' ' && *end == '|' && out == -1) {
            out = atoi(end + 1);
            numbered_pipe++;
        } else if (*(end - 1) == ' ' && *end == '!' && err == -1) {
            err = atoi(end + 1);
            numbered_pipe++;
        }
    }

    // upon discovery, fill the parsed part with spaces
    if (numbered_pipe == 2) {
        while(end < str -> content + string_length(str)) {
            *end = ' ';
            end++;
        }
        str = trim(str);
    }

    // seperating input with " | " and cons the substring before it
    int offset = 0;
    int i = 0;
    char * ref = str -> content;
    while (i + 2 < string_length(str)) {
        if (ref[i] == ' ' && ref[i + 1] == '|' && ref[i + 2] == ' ') {
            // found!
            Command * cmd = parse_command(substring(str, offset, i));
            cmds = snoc_cmd(cmds, cmd);
            offset = i = i + 3;
        } else {
            i++;
        }
    }

    // see if there's redirection ">" in the last 2 commands
    Command * cmd = parse_command(substring(str, offset, i + 2));
    cmds = snoc_cmd(cmds, cmd);
    free_string(str);
    return line(cmds, redirect, out, err);
}

Line * copy_line(Line * node)
{
    return line(copy_list_cmd(node -> cmds), node -> redirect, node -> out, node -> err);
}

void print_line(Line * node)
{
    print_list_cmd(node -> cmds);
    if (node -> redirect) {
        printf(" >");
    } else {
        if (node -> out != -1)
            printf(" |%d", node -> out);
        if (node -> err != -1)
            printf(" !%d", node -> err);
    }
    printf("\n");
}

void free_line(Line * node)
{
    free_list_cmd(node -> cmds);
    free(node);
}
