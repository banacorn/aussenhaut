#include "type.h"

#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////
//  Generic boxed data
////////////////////////////////////////////////////////////////////////////////

Box * box(void * content, void (*destructor)(void *), void * (*copier)(void *), void (*printer)(void *))
{
    Box * node = malloc(sizeof(Box));
    node -> content = content;
    node -> destructor = destructor;
    node -> copier = copier;
    node -> printer = printer;
    return node;
}

Box * copy(Box * node)
{
    void * new_content = (node -> copier)(node -> content);
    return box(new_content, node -> destructor, node -> copier, node -> printer);
}

void destruct(Box * node)
{
    (node -> destructor)(node -> content);
    free(node);
}

void print(Box * node)
{
    (node -> printer)(node -> content);
}

////////////////////////////////////////////////////////////////////////////////
//  Pair
////////////////////////////////////////////////////////////////////////////////

Pair * pair(Box * a, Box * b)
{
    Pair * node = malloc(sizeof(Pair));
    node -> fst = a;
    node -> snd = b;
    return node;
}

Pair * copy_pair(Pair * old)
{
    return pair(copy(old -> fst), copy(old -> snd));
}

void free_pair(Pair * node)
{
    destruct(node -> fst);
    destruct(node -> snd);
    free(node);
}

void print_pair(Pair * node)
{
    print(node -> fst);
    print(node -> snd);
}


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

Bool null_string(String * str)
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

Box * box_str(char * chars)
{
    return box(string(chars), (void (*)(void *))free_string, (void * (*)(void *))copy_string, (void (*)(void *))print_string);
}


////////////////////////////////////////////////////////////////////////////////
//  List
////////////////////////////////////////////////////////////////////////////////

List * nil()
{
    List * node = malloc(sizeof(List));
    node -> nil = TRUE;
    node -> cons = NULL;
    node -> data = NULL;
    return node;
}

List * cons(Box * box, List * xs)
{
    List * node = malloc(sizeof(List));
    node -> nil = FALSE;
    node -> data = box;
    node -> cons = xs;
    return node;
}

// O(n)
List * snoc(List * xs, Box * box)
{
    if (xs -> nil) {
        return cons(box, xs);
    } else {
        List * result = cons(xs -> data, snoc(xs -> cons, box));
        free(xs);
        return result;
    }
}

List * copy_list(List * xs)
{
    if (xs -> nil) {
        return nil();
    } else {
        return cons(copy(xs -> data), copy_list(xs -> cons));
    }
}

List * append(List * xs, List * ys)
{
    if (xs -> nil) {
        free(xs);
        return ys;
    } else {
        List * result = cons(xs -> data, append(xs -> cons, ys));
        free(xs);
        return result;
    }
}

List * reverse(List * xs)
{
    if (xs -> nil) {
        return xs;
    } else {
        List * result = snoc(reverse(xs -> cons), xs -> data);
        free(xs);
        return result;
    }
}

Bool null(List * xs)
{
    return xs -> nil;
}

int length(List * xs)
{
    if (xs -> nil) {
        return 0;
    } else {
        return 1 + length(xs -> cons);
    }
}

void free_list(List * xs)
{
    if (xs -> nil) {
        free(xs);
    } else {
        free_list(xs -> cons);
        destruct(xs -> data);
        free(xs);
    }
}

void print_list(List * xs)
{
    if (xs -> nil) {
    } else {
        print(xs -> data);
        print_list(xs -> cons);
    }
}
