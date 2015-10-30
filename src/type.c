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
    node -> Fst = a;
    node -> Snd = b;
    return node;
}

void * fst(Pair * node) {
    return unbox(copy(node -> Fst));
}

void * snd(Pair * node) {
    return unbox(copy(node -> Snd));
}

Pair * copy_pair(Pair * old)
{
    return pair(copy(old -> Fst), copy(old -> Snd));
}

void free_pair(Pair * node)
{
    destruct(node -> Fst);
    destruct(node -> Snd);
    free(node);
}

void print_pair(Pair * node)
{
    printf("( ");
    print(node -> Fst);
    printf(" , ");
    print(node -> Snd);
    printf(" )");
}

Box * box_pair(Pair * node)
{
    return box(node, (void (*)(void *))free_pair, (void * (*)(void *))copy_pair, (void (*)(void *))print_pair);
}

////////////////////////////////////////////////////////////////////////////////
//  Either
////////////////////////////////////////////////////////////////////////////////

Either * left(Box * x)
{
    Either * node = malloc(sizeof(Either));
    node -> Left = x;
    node -> Right = NULL;
    return node;
}
Either * right(Box * x)
{
    Either * node = malloc(sizeof(Either));
    node -> Left = NULL;
    node -> Right = x;
    return node;
}

Either * copy_either(Either * node)
{
    if (node -> Left) {
        return left(copy(node -> Left));
    } else {
        return right(copy(node -> Right));
    }
}

void free_either(Either * node)
{
    if (node -> Left) {
        destruct(node -> Left);
    } else {
        destruct(node -> Right);
    }
}

void print_either(Either * node)
{
    if (node -> Left) {
        printf("Left ");
        print(node -> Left);
    } else {
        printf("Right ");
        print(node -> Right);
    }
}

Box * box_either(Either * node)
{
    return box(node, (void (*)(void *))free_either, (void * (*)(void *))copy_either, (void (*)(void *))print_either);
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

Maybe * copy_maybe(Maybe * node)
{
    if (node -> Nothing) {
        return nothing();
    } else {
        return just(copy(node -> Just));
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

Box * box_maybe(Maybe * node)
{
    return box(node, (void (*)(void *))free_maybe, (void * (*)(void *))copy_maybe, (void (*)(void *))print_maybe);
}


////////////////////////////////////////////////////////////////////////////////
//  List
////////////////////////////////////////////////////////////////////////////////

List * nil()
{
    List * node = malloc(sizeof(List));
    node -> Nil = TRUE;
    node -> Cons = NULL;
    node -> data = NULL;
    return node;
}

List * cons(Box * box, List * xs)
{
    List * node = malloc(sizeof(List));
    node -> Nil = FALSE;
    node -> data = box;
    node -> Cons = xs;
    return node;
}

// O(n)
List * snoc(List * xs, Box * box)
{
    if (xs -> Nil) {
        return cons(box, xs);
    } else {
        List * result = cons(xs -> data, snoc(xs -> Cons, box));
        free(xs);
        return result;
    }
}

List * copy_list(List * xs)
{
    if (xs -> Nil) {
        return nil();
    } else {
        return cons(copy(xs -> data), copy_list(xs -> Cons));
    }
}

void * elemAt(List * xs, int n)
{
    if (xs -> Nil) {
        perror("indexing empty list");
        return NULL;
    } else {
        if (n == 0) {
            return head(xs);
        } else if (n > 0) {
            return elemAt(xs -> Cons, n - 1);
        } else {
            perror("invalid index number");
            return NULL;
        }
    }
}

void * head(List * xs)
{
    if (xs -> Nil) {
        perror("head on empty list");
        return NULL;
    } else {
        return unbox(copy(xs -> data));
    }
}

List * tail(List * xs)
{
    if (xs -> Nil) {
        perror("tail on empty list");
        return NULL;
    } else {
        List * result = copy_list(xs -> Cons);
        return result;
    }
}

List * init(List * xs)
{
    if (xs -> Nil) {                    // 0
        perror("init on empty list");
        return NULL;
    } else if (xs -> Cons -> Nil) {     // 1
        return nil();
    } else {
        List * result = cons(copy(xs -> data), init(xs -> Cons));
        return result;
    }
}

void * last(List * xs)
{
    if (xs -> Nil) {                    // 0
        perror("last on empty list");
        return NULL;
    } else if (xs -> Cons -> Nil) {     // 1
        void * result = unbox(copy(xs -> data));
        return result;
    } else {
        return last(xs -> Cons);
    }
}


List * append(List * xs, List * ys)
{
    if (xs -> Nil) {
        free(xs);
        return ys;
    } else {
        List * result = cons(xs -> data, append(xs -> Cons, ys));
        free(xs);
        return result;
    }
}

List * reverse(List * xs)
{
    if (xs -> Nil) {
        return xs;
    } else {
        List * result = snoc(reverse(xs -> Cons), xs -> data);
        free(xs);
        return result;
    }
}

List * map(Box *(*f)(Box *), List * xs)
{
    if (xs -> Nil) {
        return xs;
    } else {
        List * result = map(f, xs -> Cons);
        Box * val = f(xs -> data);
        free(xs);
        return cons(val, result);
    }
}

List * filter(Bool (*f)(Box *), List * xs)
{
    if (xs -> Nil) {
        return xs;
    } else {
        List * result = filter(f, xs -> Cons);
        if (f(xs -> data)) {
            Box * val = copy(xs -> data);
            destruct(xs -> data);
            free(xs);
            return cons(val, result);
        } else {
            destruct(xs -> data);
            free(xs);
            return result;
        }
    }
}

Bool null(List * xs)
{
    return xs -> Nil;
}

int length(List * xs)
{
    if (xs -> Nil) {
        return 0;
    } else {
        return 1 + length(xs -> Cons);
    }
}

void free_list(List * xs)
{
    if (xs -> Nil) {
        free(xs);
    } else {
        free_list(xs -> Cons);
        destruct(xs -> data);
        free(xs);
    }
}

void print_list_aux(List * xs)
{
    if (xs -> Nil) {
        // prints nothing
    } else if (xs -> Cons -> Nil) {
        print(xs -> data);
    } else {
        print(xs -> data);
        printf(", ");
        print_list_aux(xs -> Cons);
    }
}

void print_list(List * xs)
{
    printf("[");
    print_list_aux(xs);
    printf("]");
}

Box * box_list(List * xs)
{
    return box(xs, (void (*)(void *))free_list, (void * (*)(void *))copy_list, (void (*)(void *))print_list);
}
