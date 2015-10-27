#include "ds.h"

////////////////////////////////////////////////////////////////////////////////
//  String
////////////////////////////////////////////////////////////////////////////////

String * string(char * chars)
{
    String * new_str = malloc(sizeof(String));
    new_str -> content = malloc(strlen(chars) + 1);
    strcpy(new_str -> content, chars);
    return new_str;
}

String * copy_string(String * str)
{
    return string(str -> content);
}

size_t string_size(String * str)
{
    return strlen(str -> content) + 1;
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
        printf("\n");
    } else {
        printf("%s ", xs -> data -> content);
        print_list_str(xs -> cons);
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
