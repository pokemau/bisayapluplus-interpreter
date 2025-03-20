#include "interpreter.h"
#include "ast.h"
#include "environment.h"
#include "parser.h"
#include "token.h"
#include "value.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

#define equality(a, b) ((a) > (b) ? 1 : (a) < (b) ? -1 : 0)

// TODO:
// fix evaluate AST_BINARY

bool interp_has_error = false;

static value evaluate(interpreter *self, ast_node *node);
static void execute_statement(interpreter *self, ast_node *node);

static void interp_error(int line, const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
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
            value val = node->var_decl.inits[i]
                            ? evaluate(self, node->var_decl.inits[i])
                            : value_create_null();
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
            ast_node *curr = node->print.exprs[i];
            if (curr->type == AST_LITERAL) {
                if (curr->literal.value->type == DOLLAR) {
                    printf("\n");
                } else {
                    printf("%s", curr->literal.value->lexeme);
                }
                continue;
            }
            value val = evaluate(self, curr);

            switch (val.type) {

            case VAL_NUMERO:
                printf("%ld", val.as.numero);
                break;
            case VAL_LETRA:
                printf("%c", val.as.letra);
                break;
            case VAL_TIPIK:
                printf("%f", val.as.tipik);
                break;
            case VAL_TINUOD:
                printf("%s", val.as.tinuod ? "OO" : "DILI");
                break;
            case VAL_NULL:
                printf("NULL");
                break;
            }
        }
        break;
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

    value val, left, right;
    switch (node->type) {
    case AST_LITERAL:

        switch (node->literal.value->type) {

        case NUMBER:
            return value_create_numero(
                *((double *)node->literal.value->literal));
        case CHAR:
            return value_create_letra(*(char *)node->literal.value->literal);
        case TRUE:
            return value_create_tinuod(true);
        case FALSE:
            return value_create_tinuod(false);
        default:
            interp_error(node->literal.value->line, "Invalid type");
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
    case AST_UNARY:
        // token *op;
        // ast_node *expr;
        val = evaluate(self, node->unary.expr);
        switch (node->unary.op->type) {
        case MINUS:
            switch (val.type) {
            case VAL_NUMERO:
                return value_create_numero(-val.as.numero);
            case VAL_TIPIK:
                return value_create_tipik(-val.as.tipik);
            default:
                interp_error(node->unary.op->line, "Operand NaN");
            }
            break;
        case DILI:
            switch (val.type) {
            case VAL_TINUOD:
                return value_create_tinuod(!val.as.tinuod);
            default:
                interp_error(node->unary.op->line,
                             "Operand of 'DILI' must be of type 'TINUOD'");
            }
        default:
            break;
        }
        break;
    case AST_BINARY:
        left = evaluate(self, node->binary.left);
        right = evaluate(self, node->binary.right);

        if (left.type == VAL_LETRA || right.type == VAL_LETRA) {
            char buf[128];
            snprintf(buf, 128, "Invalid operand '%c'",
                     left.type == VAL_LETRA ? left.as.letra : right.as.letra);
            interp_error(node->binary.op->line, buf);
            return value_create_null();
        }

        switch (node->binary.op->type) {
        case PLUS:
            return value_create_numero(left.as.numero + right.as.numero);
        case MINUS:
            return value_create_numero(left.as.numero - right.as.numero);
        case STAR:
            return value_create_numero(left.as.numero * right.as.numero);
        case SLASH:
            if (right.as.numero == 0) {
                interp_error(node->binary.op->line, "Division by zero");
            }
            return value_create_numero(left.as.numero / right.as.numero);
        case EQUAL_EQUAL:
            return value_create_tinuod(left.as.tinuod == right.as.tinuod);
        case NOT_EQUAL:
            return value_create_tinuod(left.as.tinuod != right.as.tinuod);
        case LESS:
            return value_create_tinuod(left.as.tinuod < right.as.tinuod);
        case LESS_EQUAL:
            return value_create_tinuod(left.as.tinuod <= right.as.tinuod);
        case GREATER:
            return value_create_tinuod(left.as.tinuod > right.as.tinuod);
        case GREATER_EQUAL:
            return value_create_tinuod(left.as.tinuod >= right.as.tinuod);
        case UG:
            return value_create_tinuod(left.as.tinuod && right.as.tinuod);
        case O:
            return value_create_tinuod(left.as.tinuod || right.as.tinuod);
        default:
            break;
        }
        break;
    default:
        break;
    }

    return value_create_null();
}
