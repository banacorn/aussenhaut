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

// operation
Env * insert(Env *, String *, String *);
String * search(Env *, String *);

//
void free_env(Env *);
void print_env(Env *);

#endif // ENV_H_
