#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "util/arena.h"
#include "value.h"
#include <stdbool.h>

typedef struct env_elem env_elem;
struct env_elem {
    char *name;
    value value;
    env_elem *next;
};

typedef struct environment environment;
struct environment {
    env_elem *list;
    environment *parent;
    arena *arena;
};

environment *env_create(environment *parent, arena *arena);

void env_error(int line, const char *msg);
void env_free(environment *self);
void env_define(environment *self, const char *name, value val);
bool env_get(environment *self, const char *name, value *out);
bool env_assign(environment *self, const char *name, value val);
value_type env_get_variable_type(environment *self, const char *name);

#endif
