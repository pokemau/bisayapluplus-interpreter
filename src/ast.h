#ifndef _AST_H_
#define _AST_H_

#include "token.h"

typedef enum ast_node_type {
    AST_PROGRAM,
    AST_VAR_DECL, // MUGNA var declaration
    AST_ASSIGN,   // var assignments
    AST_PRINT,    // IPAKITA statements
    AST_INPUT,    // DAWAT statements
    AST_IF,       // KUNG, KUNG WALA, KUNG DILI
    AST_FOR,      // ALAG SA
    AST_BLOCK,    // PUNDOK
    AST_BINARY,   // Binary operations (+, -, /, *)
    AST_UNARY,    // Unary operatins (+, -)
    AST_LITERAL,  // Literals (NUMERO, LETRA, TINUOD, PITIK)
    AST_VARIABLE, // Variable reference
} ast_node_type;

typedef struct ast_node ast_node;
struct ast_node {
    ast_node_type type;
    union {

        struct {
            ast_node *statements;
            int stmt_count;
        } program;

        struct {
            TokenType d_type;
            token *names;
            ast_node **inits;
            int name_count;
        } var_decl;

        struct {
            token *var;
            ast_node *expr;
        } assign;

        struct {
            ast_node **exprs;
            int expr_count;
        } print;

        struct {
            token *vars;
            int var_count;
        } input;

        struct {
            ast_node *condition;
            ast_node *then_block;
            ast_node *else_block;
        } if_stmt;

        struct {
            ast_node *init;
            ast_node *condition;
            ast_node *update;
            ast_node *body;
        } for_stmt;

        struct {
            ast_node *statements;
            int stmt_count;
        } block;

        struct {
            ast_node *left;
            token *op;
            ast_node *right;
        } binary;

        struct {
            token *op;
            ast_node *expr;
        } unary;

        struct {
            token *value;
        } literal;

        struct {
            token *name;
        } variable;
    };
};

#endif
