#include "interpreter.h"
#include "ast.h"
#include "environment.h"
#include "parser.h"
#include "value.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

bool interp_has_error = false;

static value evaluate(interpreter *self, ast_node *node);
static void execute_statement(interpreter *self, ast_node *node);

static void interp_error() {

}

interpreter interp_create() {
    interpreter self;
    self.env = env_create(NULL);
    return self;
}

void interp_free(interpreter *self) {
    environment *env = self->env;
    while (env) {
        environment *parent = env->parent;
        env_free(env);
        env = parent;
    }
    free(self);
}


void interp_execute(interpreter *self, ast_node *node) {
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

    value val;
    value_type d_type;

    switch (node->type) {
        case AST_PROGRAM:
            break;
        case AST_VAR_DECL:
            for (int i = 0; i < node->var_decl.name_count; i++) {
                d_type = value_get_d_type(node->var_decl.d_type);
                value val = node->var_decl.inits[i] ?
                                evaluate(self, node->var_decl.inits[i]) :
                                value_create_null();
                if (val.type != VAL_NULL && val.type != d_type) {
                    // TODO: Improve error message
                    // Type NUMERO cant be chuchu with type LETRA
                    env_error(node->var_decl.names[i].line, "Invalid type");
                }

                env_define(self->env, node->var_decl.names[i].lexeme, val);
            }
            break;

        case AST_ASSIGNMENT:
            val = evaluate(self, node->assignment.expr);
            if (!env_assign(self->env, node->assignment.var->lexeme, val)) {

                char buf[128];
                snprintf(buf, 128, "Undefined variable '%s'",
                         node->variable.name->lexeme);
                env_error(node->variable.name->line, buf);
            }
            break;
        case AST_PRINT:
            for (int i = 0; i < node->print.expr_count; i++) {
                value val = evaluate(self, node->print.exprs[i]);
                if (val.type == VAL_NUMERO) {
                    printf("%d", val.as.numero);
                }
                if (val.type == VAL_LETRA) {
                    printf("%c", val.as.letra);
                }
                if (val.type == VAL_TINUOD) {
                    printf("%s", val.as.tinuod ? "OO" : "DILI");
                }
                if (val.type == VAL_NULL) {
                    printf("NULL");
                }
            }
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
    if (!node) {
        return value_create_null();
    }

    value val;
    switch (node->type) {
        case AST_LITERAL:
            if (node->literal.value->type == NUMBER) {
                return value_create_numero(
                    *((double*)node->literal.value->literal));
            }
            if (node->literal.value->type == CHAR) {
                return value_create_letra(*(char*)node->literal.value->literal);
            }
            if (node->literal.value->type == TRUE) {
                return value_create_tinuod(true);
            }
            if (node->literal.value->type == FALSE) {
                return value_create_tinuod(false);
            }

        case AST_VARIABLE:
            if (!env_get(self->env, node->variable.name->lexeme, &val)) {
                char buf[128];
                snprintf(buf, 128, "Undefined variable '%s'",
                         node->variable.name->lexeme);
                env_error(node->variable.name->line, buf);
            }
            return val;
        case AST_ASSIGNMENT:
            val = evaluate(self, node->assignment.expr);
            env_assign(self->env, node->assignment.var->lexeme, val);
            return val;
    }

}
