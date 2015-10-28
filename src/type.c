#include "type.h"

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
//  Either
////////////////////////////////////////////////////////////////////////////////

Either * left(Box * x)
{
    Either * node = malloc(sizeof(Either));
    node -> left = x;
    node -> right = NULL;
    return node;
}
Either * right(Box * x)
{
    Either * node = malloc(sizeof(Either));
    node -> left = NULL;
    node -> right = x;
    return node;
}

Either * copy_either(Either * node)
{
    if (node -> left) {
        return left(copy(node -> left));
    } else {
        return right(copy(node -> right));
    }
}

void free_either(Either * node)
{
    if (node -> left) {
        destruct(node -> left);
    } else {
        destruct(node -> right);
    }
}

void print_either(Either * node)
{
    if (node -> left) {
        printf("Left ");
        print(node -> left);
    } else {
        printf("Right ");
        print(node -> right);
    }
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
