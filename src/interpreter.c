#include "interpreter.h"
#include "ast.h"
#include "environment.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

static void execute_statement(interpreter *self, ast_node *node);
static value evaluate(interpreter *self, ast_node *node);

interpreter *interpreter_create() {
    interpreter *self = malloc(sizeof(interpreter));
    self->env = env_create(NULL);
    return self;
}


void interpreter_execute(interpreter *self, ast_node *node) {
    if (!node)
        return;

    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->program.stmt_count; i++) {
                execute_statement(self, &node->program.statements[i]);
            }
            break;
        default:
            fprintf(stderr, "Invalid Node!");
            exit(1);
            break;
    }
}

static void execute_statement(interpreter *self, ast_node *node) {
    if (!node)
        return;

    switch (node->type) {
        case AST_PROGRAM:
            break;
        case AST_VAR_DECL:
            for (int i = 0; i < node->var_decl.name_count; i++) {
                value va
            }


        case AST_ASSIGNMENT:
        case AST_PRINT:
        case AST_INPUT:
        case AST_IF:
        case AST_ELSE_IF:
        case AST_ELSE:
        case AST_FOR:
        case AST_BLOCK:
        case AST_BINARY:
        case AST_UNARY:
        case AST_LITERAL:
        case AST_VARIABLE:
            break;
    }
}

static value evaluate(interpreter *self, ast_node *node) {
    
}
