#include "type.h"
#include "env.h"

////////////////////////////////////////////////////////////////////////////////
//  Env
////////////////////////////////////////////////////////////////////////////////

Env * nil_env()
{
    Env * node = malloc(sizeof(Env));
    node -> Nil = TRUE;
    node -> key = NULL;
    node -> val = NULL;
    node -> Cons = NULL;
    return node;
}

Env * cons_env(String * k , String * v, Env * xs)
{
    Env * node = malloc(sizeof(Env));
    node -> Nil = FALSE;
    node -> key = k;
    node -> val = v;
    node -> Cons = xs;
    return node;
}

Env * insert(Env * xs, String * k, String * v)
{
    if (xs -> Nil) {                                // not found, insert
        return cons_env(k, v, xs);
    } else {
        if (compare_string(xs -> key, k)) {        // found, update
            free_string(xs -> val); // release old value
            free_string(k);
            xs -> val = v;
            return xs;
        } else {
            xs -> Cons = insert(xs -> Cons, k, v);
            return xs;
        }
    }
}

// returns NULL
String * search(Env * xs, String * s)
{
    if (xs -> Nil) {
        free_string(s);
        return NULL;
    } else {
        if (compare_string(xs -> key, s)) {        // found
            free_string(s);
            return copy_string(xs -> val);
        } else {
            return search(xs -> Cons, s);
        }
    }
}


void free_env(Env * node)
{
    if (node -> Nil) {
        free(node);
    } else {
        free_env(node -> Cons);
        free_string(node -> key);
        free_string(node -> val);
        free(node);
    }
}

void print_env_aux(Env * xs)
{
    if (xs -> Nil) {
        // prints nothing
    } else if (xs -> Cons -> Nil) {
        printf("    ");
        print_string(xs -> key);
        printf(": ");
        print_string(xs -> val);
    } else {
        printf("    ");
        print_string(xs -> key);
        printf(": ");
        print_string(xs -> val);
        printf("\n");
        print_env_aux(xs -> Cons);
    }
}

void print_env(Env * xs)
{
    printf("{\n");
    print_env_aux(xs);
    printf("\n}");
}
