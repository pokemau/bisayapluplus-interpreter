#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "environment.h"
#include "ast.h"

typedef struct interpreter {
    environment *env;
} interpreter;

interpreter *interpreter_create();
void interpreter_execute(interpreter *self, ast_node *node);

#endif
