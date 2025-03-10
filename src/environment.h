#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "value.h"
#include <stdbool.h>

typedef struct env_elem env_elem;
struct env_elem {
    char *name;
    value value;
    env_elem *next;
};

// lenked lest
typedef struct environment environment;
struct environment {
    env_elem *list;
    environment *parent;
};

environment *env_create(environment *parent);

void env_free(environment *self);
void env_define(environment *self, const char *name, value val);
bool env_get(environment *self, const char *name, value *out);
bool env_assign(environment *self, const char *name, value val);

#endif
