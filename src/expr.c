#include "expr.h"
#include <stdlib.h>

Expr *make_unary(Token *op, Expr *right) {
    Expr *self = malloc(sizeof(Expr));
    self->type = EXPR_UNARY;
    self->Unary.op = op;
    self->Unary.right = right; return self;
}

Expr *make_literal(void *value, TokenType type) {
    Expr *self = malloc(sizeof(Expr));
    self->type = EXPR_LITERAL;
    self->Literal.value = value;
    self->Literal.type = type;
    return self;
}

Expr *make_variable(Token *token) {
    Expr *self = malloc(sizeof(Expr));
    self->type = EXPR_VARIABLE;
    self->Variable.name = token;
    return self;
}

Expr *make_binary(Expr *left, Token *op, Expr *right) {
    Expr *self = malloc(sizeof(Expr));
    self->type = EXPR_BINARY;
    self->Binary.left = left;
    self->Binary.op = op;
    self->Binary.right = right;
    return self;
}
