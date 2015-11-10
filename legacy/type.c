#include "type.h"


////////////////////////////////////////////////////////////////////////////////
//  Generic boxed data
////////////////////////////////////////////////////////////////////////////////

Box_ * box(void * content, void (*destruct_or)(void *), void * (*copier)(void *), void (*print_er)(void *))
{
    Box_ * node = malloc(sizeof(Box_));
    node -> content = content;
    node -> destruct_or = destruct_or;
    node -> copier = copier;
    node -> print_er = print_er;
    return node;
}

void * unbox(Box_ * node)
{
    void * result = node -> content;
    free(node);
    return result;
}

Box_ * copy_(Box_ * node)
{
    void * new_content = (node -> copier)(node -> content);
    return box(new_content, node -> destruct_or, node -> copier, node -> print_er);
}

void destruct_(Box_ * node)
{
    (node -> destruct_or)(node -> content);
    free(node);
}

void print_(Box_ * node)
{
    (node -> print_er)(node -> content);
}

////////////////////////////////////////////////////////////////////////////////
//  Pair
////////////////////////////////////////////////////////////////////////////////

Pair * pair(Box_ * a, Box_ * b)
{
    Pair * node = malloc(sizeof(Pair));
    node -> Fst = a;
    node -> Snd = b;
    return node;
}

void * fst(Pair * node) {
    return unbox(copy_(node -> Fst));
}

void * snd(Pair * node) {
    return unbox(copy_(node -> Snd));
}

Pair * copy__pair(Pair * old)
{
    return pair(copy_(old -> Fst), copy_(old -> Snd));
}

void free_pair(Pair * node)
{
    destruct_(node -> Fst);
    destruct_(node -> Snd);
    free(node);
}

void print__pair(Pair * node)
{
    printf("( ");
    print_(node -> Fst);
    printf(" , ");
    print_(node -> Snd);
    printf(" )");
}

Box_ * box_pair(Pair * node)
{
    return box(node, (void (*)(void *))free_pair, (void * (*)(void *))copy__pair, (void (*)(void *))print__pair);
}

////////////////////////////////////////////////////////////////////////////////
//  Either
////////////////////////////////////////////////////////////////////////////////

Either * left(Box_ * x)
{
    Either * node = malloc(sizeof(Either));
    node -> Left = x;
    node -> Right = NULL;
    return node;
}
Either * right(Box_ * x)
{
    Either * node = malloc(sizeof(Either));
    node -> Left = NULL;
    node -> Right = x;
    return node;
}

Either * copy__either(Either * node)
{
    if (node -> Left) {
        return left(copy_(node -> Left));
    } else {
        return right(copy_(node -> Right));
    }
}

void free_either(Either * node)
{
    if (node -> Left) {
        destruct_(node -> Left);
    } else {
        destruct_(node -> Right);
    }
}

void print__either(Either * node)
{
    if (node -> Left) {
        printf("Left ");
        print_(node -> Left);
    } else {
        printf("Right ");
        print_(node -> Right);
    }
}

Box_ * box_either(Either * node)
{
    return box(node, (void (*)(void *))free_either, (void * (*)(void *))copy__either, (void (*)(void *))print__either);
}
////////////////////////////////////////////////////////////////////////////////
//  Maybe
////////////////////////////////////////////////////////////////////////////////

// typedef struct Maybe {
//     Bool Nothing;   // TRUE if Nothing, FALSE, if Just
//     Box_ * Just;
// } Maybe;

Maybe * nothing()
{
    Maybe * node = malloc(sizeof(Maybe));
    node -> Nothing = TRUE;
    node -> Just = NULL;
    return node;
}

Maybe * just(Box_ * b)
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
        return unbox(copy_(node -> Just));
    }
}

Maybe * copy__maybe(Maybe * node)
{
    if (node -> Nothing) {
        return nothing();
    } else {
        return just(copy_(node -> Just));
    }
}

void print__maybe(Maybe * node)
{
    if (node -> Nothing) {
        printf("Nothing ");
    } else {
        printf("Just ");
        print_(node -> Just);
    }
}

void free_maybe(Maybe * node)
{
    if (node -> Nothing) {
        free(node);
    } else {
        destruct_(node -> Just);
        free(node);
    }
}

Box_ * box_maybe(Maybe * node)
{
    return box(node, (void (*)(void *))free_maybe, (void * (*)(void *))copy__maybe, (void (*)(void *))print__maybe);
}


////////////////////////////////////////////////////////////////////////////////
//  List_
////////////////////////////////////////////////////////////////////////////////

List_ * nil()
{
    List_ * node = malloc(sizeof(List_));
    node -> Nil = TRUE;
    node -> Cons = NULL;
    node -> data = NULL;
    return node;
}

List_ * cons(Box_ * box, List_ * xs)
{
    List_ * node = malloc(sizeof(List_));
    node -> Nil = FALSE;
    node -> data = box;
    node -> Cons = xs;
    return node;
}

// O(n)
List_ * snoc(List_ * xs, Box_ * box)
{
    if (xs -> Nil) {
        return cons(box, xs);
    } else {
        List_ * result = cons(xs -> data, snoc(xs -> Cons, box));
        free(xs);
        return result;
    }
}

List_ * copy__list(List_ * xs)
{
    if (xs -> Nil) {
        return nil();
    } else {
        return cons(copy_(xs -> data), copy__list(xs -> Cons));
    }
}

void * elemAt(List_ * xs, int n)
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

void * head(List_ * xs)
{
    if (xs -> Nil) {
        perror("head on empty list");
        return NULL;
    } else {
        return unbox(copy_(xs -> data));
    }
}

List_ * tail(List_ * xs)
{
    if (xs -> Nil) {
        perror("tail on empty list");
        return NULL;
    } else {
        List_ * result = copy__list(xs -> Cons);
        return result;
    }
}

List_ * init(List_ * xs)
{
    if (xs -> Nil) {                    // 0
        perror("init on empty list");
        return NULL;
    } else if (xs -> Cons -> Nil) {     // 1
        return nil();
    } else {
        List_ * result = cons(copy_(xs -> data), init(xs -> Cons));
        return result;
    }
}

void * last(List_ * xs)
{
    if (xs -> Nil) {                    // 0
        perror("last on empty list");
        return NULL;
    } else if (xs -> Cons -> Nil) {     // 1
        void * result = unbox(copy_(xs -> data));
        return result;
    } else {
        return last(xs -> Cons);
    }
}


List_ * append_(List_ * xs, List_ * ys)
{
    if (xs -> Nil) {
        free(xs);
        return ys;
    } else {
        List_ * result = cons(xs -> data, append_(xs -> Cons, ys));
        free(xs);
        return result;
    }
}

List_ * reverse_(List_ * xs)
{
    if (xs -> Nil) {
        return xs;
    } else {
        List_ * result = snoc(reverse_(xs -> Cons), xs -> data);
        free(xs);
        return result;
    }
}

List_ * map_(Box_ *(*f)(Box_ *), List_ * xs)
{
    if (xs -> Nil) {
        return xs;
    } else {
        List_ * result = map_(f, xs -> Cons);
        Box_ * val = f(xs -> data);
        free(xs);
        return cons(val, result);
    }
}

List_ * filter_(Bool (*f)(Box_ *), List_ * xs)
{
    if (xs -> Nil) {
        return xs;
    } else {
        List_ * result = filter_(f, xs -> Cons);
        if (f(xs -> data)) {
            Box_ * val = copy_(xs -> data);
            destruct_(xs -> data);
            free(xs);
            return cons(val, result);
        } else {
            destruct_(xs -> data);
            free(xs);
            return result;
        }
    }
}

Bool null(List_ * xs)
{
    return xs -> Nil;
}

int length(List_ * xs)
{
    if (xs -> Nil) {
        return 0;
    } else {
        return 1 + length(xs -> Cons);
    }
}

void free_list(List_ * xs)
{
    if (xs -> Nil) {
        free(xs);
    } else {
        free_list(xs -> Cons);
        destruct_(xs -> data);
        free(xs);
    }
}

void print__list_aux(List_ * xs)
{
    if (xs -> Nil) {
        // print_s nothing
    } else if (xs -> Cons -> Nil) {
        print_(xs -> data);
    } else {
        print_(xs -> data);
        printf(", ");
        print__list_aux(xs -> Cons);
    }
}

void print__list(List_ * xs)
{
    printf("[");
    print__list_aux(xs);
    printf("]");
}

Box_ * box_list(List_ * xs)
{
    return box(xs, (void (*)(void *))free_list, (void * (*)(void *))copy__list, (void (*)(void *))print__list);
}
