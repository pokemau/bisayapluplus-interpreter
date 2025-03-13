#ifndef _STMT_H_
#define _STMT_H_

#include "expr.h"

typedef enum Stmt_type {
    STMT_BLOCK,
    STMT_EXPRESSION,
    STMT_IF,
    STMT_PRINT,
    STMT_VAR_DECL,
    STMT_INPUT
} Stmt_type;

typedef struct Stmt Stmt;
struct Stmt {
    Stmt_type type;
    union {
        struct {
            Stmt *statements;
        } Block;

        struct {
            Expr *expressions;
        } Expression;

        struct {
            Expr *condition;
            Stmt *then_block;
            Stmt *else_block;
        } If;

        struct {
            Expr *expression;
        } Print;

        struct {
            Token *name;
            Expr *initializer;
        } Var_decl;

        struct {
            Token *var_names;
        } Input;
    };
};

Stmt *stmt_make_var_decl(Token *name, Expr *initializer);

#endif
