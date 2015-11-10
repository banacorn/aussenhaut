#ifndef ENV_H_
#define ENV_H_

#include "type.h"
#include "string.h"

////////////////////////////////////////////////////////////////////////////////
//  Env
////////////////////////////////////////////////////////////////////////////////

typedef struct Env {
    Bool Nil;
    String_ * key;
    String_ * val;
    struct Env * Cons;
} Env;

// constructor
Env * nil_env();
Env * cons_env(String_ *, String_ *, Env *);
Env * copy_env(Env *);

// operation
Env * insert(Env *, String_ *, String_ *);
Env * remove_env(Env *, String_ *);
String_ * search(Env *, String_ *);
String_ * show_all_env(Env *);
List_ * get_path(Env *);

//
void free_env(Env *);
void print_env(Env *);

#endif // ENV_H_
