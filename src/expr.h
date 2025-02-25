#ifndef _EXPR_H_
#define _EXPR_H_

#include <stdbool.h>

#include "token.h"


typedef enum expr_type {
    EXPR_BOOL,
    EXPR_NUMBER,
    EXPR_STRING,
    EXPR_UNARY,
    EXPR_GROUPING,
} expr_type;

typedef struct expr expr;
struct expr {
    expr_type type;

    union {
        struct {
            token *op;
            expr *left;
            expr *right;
        } binary;

        struct {
            token *token;
            expr *right;
        } unary;

        struct {
            token *token;
            union {
                bool boolean;
                double number;
                char *string;
            };
        } literal;

        expr *grouping;
    };
};

#endif
