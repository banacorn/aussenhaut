#ifndef ENV_H_
#define ENV_H_

#include "type.h"
#include "string.h"

////////////////////////////////////////////////////////////////////////////////
//  Env
////////////////////////////////////////////////////////////////////////////////

typedef struct Env {
    Bool Nil;
    String * key;
    String * val;
    struct Env * Cons;
} Env;

// constructor
Env * nil_env();
Env * cons_env(String *, String *, Env *);
Env * copy_env(Env *);

// operation
Env * insert(Env *, String *, String *);
Env * remove_env(Env *, String *);
String * search(Env *, String *);
String * show_all_env(Env *);
List * get_path(Env *);

//
void free_env(Env *);
void print_env(Env *);

#endif // ENV_H_
