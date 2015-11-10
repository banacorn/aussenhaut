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

Env * cons_env(String_ * k , String_ * v, Env * xs)
{
    Env * node = malloc(sizeof(Env));
    node -> Nil = FALSE;
    node -> key = k;
    node -> val = v;
    node -> Cons = xs;
    return node;
}

Env * copy_env(Env * xs)
{
    if (xs -> Nil) {
        return nil_env();
    } else {
        return cons_env(copy_string(xs -> key), copy_string(xs -> val), copy_env(xs -> Cons));
    }
}

Env * insert(Env * xs, String_ * k, String_ * v)
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
Env * remove_env(Env * xs, String_ * s)
{
    if (xs -> Nil) {
        free_string(s);
        return xs;
    } else {
        if (compare_string(xs -> key, s)) {        // found
            Env * rest = xs -> Cons;
            free_string(xs -> key);
            free_string(xs -> val);
            free(xs);
            free_string(s);
            return rest;
        } else {
            xs -> Cons = remove_env(xs -> Cons, s);
            return xs;
        }
    }

}

// returns NULL
String_ * search(Env * xs, String_ * s)
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

String_ * show_all_env(Env * xs)
{
    if (xs -> Nil) {
        return string("");
    } else {
        String_ * result = show_all_env(xs -> Cons);
        String_ * new_str = append_string(copy_string(xs -> key), string("="));
        new_str = append_string(new_str, copy_string(xs -> val));
        new_str = append_string(new_str, string("\n"));
        new_str = append_string(new_str, result);
        return new_str;
    }
}

// List_ * append_to_all(List_ * xs, String_ * ys)
// {
//     if (xs -> Nil) {
//         free_string(ys);
//         return xs;
//     } else {
//         xs -> data = box_str(append_string(unbox(xs -> data), copy_string(ys)));
//         xs -> Cons = append_to_all(xs -> Cons, ys);
//         return xs;
//     }
// }

String_ * augment_path(String_ * path)
{
    return append_string(path, string("/"));
    // String_ * here = string(".");
    // if (compare_string(path, here)) {
    //     free_string(path);
    //     free_string(here);
    //     return string("./");
    //     // return string("ras/");
    // } else {
    //     free_string(here);
    //     return append_string(path, string("/"));
    // }
}

List_ * get_path(Env * xs)
{
    String_ * path_str = search(xs, string("PATH"));
    if (path_str) {
        return map_string(augment_path, tokenize(path_str, string(":")));
    } else {
        return nil();
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
