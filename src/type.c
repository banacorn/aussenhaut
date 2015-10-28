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

void * unbox(Box * node)
{
    void * result = node -> content;
    free(node);
    return result;
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

void * fst(Pair * node) {
    return unbox(copy(node -> fst));
}

void * snd(Pair * node) {
    return unbox(copy(node -> snd));
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
    printf("( ");
    print(node -> fst);
    printf(" , ");
    print(node -> snd);
    printf(" )");
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
//  Maybe
////////////////////////////////////////////////////////////////////////////////

// typedef struct Maybe {
//     Bool Nothing;   // TRUE if Nothing, FALSE, if Just
//     Box * Just;
// } Maybe;

Maybe * nothing()
{
    Maybe * node = malloc(sizeof(Maybe));
    node -> Nothing = TRUE;
    node -> Just = NULL;
    return node;
}

Maybe * just(Box * b)
{
    Maybe * node = malloc(sizeof(Maybe));
    node -> Nothing = FALSE;
    node -> Just = b;
    return node;
}

void * from_just(Maybe * node)
{
    if (node -> Nothing) {
        perror("from Nothing");
        return NULL;
    } else {
        return unbox(copy(node -> Just));
    }
}

void print_maybe(Maybe * node)
{
    if (node -> Nothing) {
        printf("Nothing ");
    } else {
        printf("Just ");
        print(node -> Just);
    }
}

void free_maybe(Maybe * node)
{
    if (node -> Nothing) {
        free(node);
    } else {
        destruct(node -> Just);
        free(node);
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

Box * head(List * xs)
{
    if (xs -> nil) {
        perror("head on empty list");
        return NULL;
    } else {
        Box * result = copy(xs -> data);
        return result;
    }
}

List * tail(List * xs)
{
    if (xs -> nil) {
        perror("tail on empty list");
        return NULL;
    } else {
        List * result = copy_list(xs -> cons);
        return result;
    }
}

List * init(List * xs)
{
    if (xs -> nil) {                    // 0
        perror("init on empty list");
        return NULL;
    } else if (xs -> cons -> nil) {     // 1
        return nil();
    } else {
        List * result = cons(copy(xs -> data), init(copy_list(xs -> cons)));
        return result;
    }
}

Box * last(List * xs)
{
    if (xs -> nil) {                    // 0
        perror("last on empty list");
        return NULL;
    } else if (xs -> cons -> nil) {     // 1
        Box * result = copy(xs -> data);
        return result;
    } else {
        Box * result = last(copy_list(xs -> cons));
        return result;
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

void print_list_aux(List * xs)
{
    if (xs -> nil) {
        // prints nothing
    } else if (xs -> cons -> nil) {
        print(xs -> data);
    } else {
        print(xs -> data);
        printf(", ");
        print_list_aux(xs -> cons);
    }
}

void print_list(List * xs)
{
    printf("[");
    print_list_aux(xs);
    printf("]");
}
