#include "ds.h"

////////////////////////////////////////////////////////////////////////////////
//  Generic Data
////////////////////////////////////////////////////////////////////////////////

Data * data(void * content, size_t size)
{
    Data * node = malloc(sizeof(Data));
    node -> content = content;
    node -> size = size;
    return node;
}

void free_data(Data * node)
{
    free(node -> content);
    free(node);
}

Data * copy_data(Data * old_node)
{
    Data * new_node = malloc(sizeof(Data));

    // copy content
    void * new_content = malloc(old_node -> size);
    memcpy(new_content, old_node -> content, old_node -> size);

    // initialize
    new_node -> content = new_content;
    new_node -> size = old_node -> size;

    return new_node;
}

////////////////////////////////////////////////////////////////////////////////
//  String
////////////////////////////////////////////////////////////////////////////////

String string(String str)
{
    String new_str = malloc(strlen(str) + 1);
    strcpy(new_str, str);
    return new_str;
}

String copy_string(String str)
{
    return string(str);
}

size_t string_size(String str)
{
    return strlen(str) + 1;
}

void free_string(String str)
{
    free(str);
}

////////////////////////////////////////////////////////////////////////////////
//  List
////////////////////////////////////////////////////////////////////////////////

List * nil()
{
    List * node = malloc(sizeof(List));
    node -> nil = 1;
    node -> cons = NULL;
    node -> data = NULL;
    return node;
}

List * cons(Data * content, List * xs)
{
    List * node = malloc(sizeof(List));
    node -> nil = 0;
    node -> cons = xs;
    node -> data = content;
    return node;
}

void free_list(List * xs)
{
    if (xs -> nil) {
        free(xs);
    } else {
        free_list(xs -> cons);
        free_data(xs -> data);
        free(xs);
    }
}

List * copy_list(List * xs)
{
    if (xs -> nil) {
        return nil();
    } else {
        List * new_xs = copy_list(xs -> cons);
        Data * new_data = copy_data(xs -> data);
        return cons(new_data, new_xs);
    }
}

void print_list(List * xs)
{
    if (xs -> nil) {
        printf("\n");
    } else {
        print_list(xs -> cons);
    }
}

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

Command * command(String name, List * args)
{
    Command * node = malloc(sizeof(Command));
    node -> name = name;
    node -> args = args;
    return node;
}

void free_command(Command * node)
{
    free_string(node -> name);
    free_list(node -> args);
    free(node);
}

Command * parse_command(String str)
{
    Command * node = malloc(sizeof(Command));
    // name
    String p = strtok(str," ");
    node -> name = copy_string(p);

    // args
    node -> args = nil();
    while (p = strtok(NULL, " "), p != NULL) {
        String arg = copy_string(p);
        node -> args = cons(data(arg, string_size(arg)), node -> args);
    }

    free(str);
    return node;
}

void print_command(Command * node)
{

}
