#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "environment.h"
#include "ast.h"
#include "util/arena.h"

typedef struct interpreter {
    environment *env;
    arena *arena;
} interpreter;

interpreter *interp_create(arena *arena);

void interp_free(interpreter *self);
void interp_execute(interpreter *self, ast_node *node);

#endif
