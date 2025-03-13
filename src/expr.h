#ifndef _EXPR_H_
#define _EXPR_H_

#include "token.h"

typedef enum Expr_type {
    EXPR_ASSIGN,
    EXPR_BINARY,
    EXPR_GROUPING,
    EXPR_LITERAL,
    EXPR_LOGICAL,
    EXPR_UNARY,
    EXPR_VARIABLE
} Expr_type;

typedef struct Expr Expr;
struct Expr {
    Expr_type type;

    union {

        struct {
            Token *name;
            Expr *value;
        } Assign;

        struct {
            Expr *left;
            Token *op;
            Expr *right;
        } Binary;

        struct {
            Expr *expr;
        } Grouping;

        struct {
            void *value;
            TokenType type;
        } Literal;

        struct {
            Expr *left;
            Token *op;
            Expr *right;
        } Logical;

        struct {
            Token *op;
            Expr *right;
        } Unary;

        struct {
            Token *name;
        } Variable;
    };
};

Expr *make_unary(Token *op, Expr *right);
Expr *make_literal(void *value, TokenType type);
Expr *make_variable(Token *token);
Expr *make_binary(Expr *left, Token *op, Expr *right);

#endif
