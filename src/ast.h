#ifndef _AST_H_
#define _AST_H_

#include "token.h"
#include <stdlib.h>
#include "util.h"

typedef enum ast_node_type {
    AST_PROGRAM,
    AST_VAR_DECL,   // MUGNA var declaration
    AST_ASSIGNMENT, // var assignments
    AST_PRINT,      // IPAKITA statements
    AST_INPUT,      // DAWAT statements
    AST_IF,         // KUNG
    AST_ELSE_IF,    // KUNG DILI
    AST_ELSE,       // KUNG WALA
    AST_FOR,        // ALAG SA
    AST_BLOCK,      // PUNDOK
    AST_BINARY,     // Binary operations (+, -, /, *)
    AST_UNARY,      // Unary operatins (+, -)
    AST_LITERAL,    // Literals (NUMERO, LETRA, TINUOD, PITIK)
    AST_VARIABLE,   // Variable reference
} ast_node_type;

static char *ast_node_type_val[] = {
    "AST_PROGRAM",
    "AST_VAR_DECL",
    "AST_ASSIGNMENT",
    "AST_PRINT",
    "AST_INPUT",
    "AST_IF",
    "AST_ELSE_IF",
    "AST_ELSE",
    "AST_FOR",
    "AST_BLOCK",
    "AST_BINARY",
    "AST_UNARY",
    "AST_LITERAL",
    "AST_VARIABLE",
};

typedef struct ast_node ast_node;
struct ast_node {
    ast_node_type type;
    union {

        struct {
            ast_node *statements;
            int stmt_count;
        } Program;

        struct {
            TokenType d_type;
            Token **names;
            ast_node **inits;
            int name_count;
        } Var_decl;

        struct {
            Token *var;
            ast_node *expr;
        } Assignment;

        struct {
            ast_node **exprs;
            int expr_count;
        } Print_stmt;

        struct {
            Token *vars;
            int var_count;
        } Input;

        struct {
            ast_node *condition;
            ast_node *then_block;
            ast_node *else_block;
        } If_stmt;

        struct {
            ast_node *init;
            ast_node *condition;
            ast_node *update;
            ast_node *body;
        } For_stmt;

        struct {
            ast_node *statements;
            int stmt_count;
        } Block;

        struct {
            ast_node *left;
            Token *op;
            ast_node *right;
        } Binary;

        struct {
            Token *op;
            ast_node *expr;
        } Unary;

        struct {
            Token *value;
        } Literal;

        struct {
            Token *name;
        } Variable;
    };
};

ast_node *ast_new_node(ast_node_type type);

#endif
