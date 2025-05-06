#include "ast.h"
#include "util.h"
#include "util/dyn_arr.h"
#include <stdlib.h>
#include <stdio.h>

ast_node *ast_new_node(arena *arena, ast_node_type type) {
    /*ast_node *node = (ast_node *)malloc(sizeof(ast_node));*/
    ast_node *node = arena_alloc(arena, sizeof(ast_node));
    node->type = type;
    return node;
}

void ast_free_node(ast_node *node) {
    if (!node) {
        fprintf(stderr, "Node is NULL [ast_free_node]");
        return;
    }

    switch (node->type) {

    case AST_PROGRAM:
        for (int i = 0; i < node->program.stmt_count; i++) {
            ast_free_node(&node->program.statements[i]);
        }
        dynarray_destroy(node->program.statements);
        break;
    case AST_VAR_DECL:
        free(node->var_decl.names);
        for (int i = 0; i < node->var_decl.name_count; i++) {
            if (node->var_decl.inits[i])
                ast_free_node(node->var_decl.inits[i]);
        }
        free(node->var_decl.inits);
        break;
    case AST_ASSIGNMENT:
    case AST_PRINT:
        for (int i = 0; i < node->print.expr_count; i++) {
            if (node->print.exprs[i])
                ast_free_node(node->print.exprs[i]);
        }
        free(node->print.exprs);
        break;
    case AST_INPUT:
        free(node->input.vars);
        break;
    case AST_IF:
    case AST_ELSE_IF:
        if (node->if_stmt.condition) {
            ast_free_node(node->if_stmt.condition);
        }
        if (node->if_stmt.then_block) {
            ast_free_node(node->if_stmt.then_block);
        }
        if (node->if_stmt.else_block) {
            ast_free_node(node->if_stmt.else_block);
        }
        break;
    case AST_ELSE:
        if (node->if_stmt.then_block) {
            ast_free_node(node->if_stmt.then_block);
        }
        break;
    case AST_FOR:
        if (node->for_stmt.init) {
            ast_free_node(node->for_stmt.init);
        }
        if (node->for_stmt.condition) {
            ast_free_node(node->for_stmt.condition);
        }
        if (node->for_stmt.update) {
            ast_free_node(node->for_stmt.update);
        }
        if (node->for_stmt.body) {
            ast_free_node(node->for_stmt.body);
        }
        break;
    case AST_BLOCK:
        for (int i = 0; i < node->block.stmt_count; i++) {
            ast_free_node(&node->block.statements[i]);
        }
        free(node->block.statements);
        break;
    case AST_BINARY:
        if (node->binary.left) {
            ast_free_node(node->binary.left);
        }
        if (node->binary.right) {
            ast_free_node(node->binary.right);
        }
        break;
    case AST_UNARY:
        if (node->unary.expr) {
            ast_free_node(node->unary.expr);
        };
        break;
    case AST_LITERAL:
        break;
    case AST_SWITCH:
        ast_free_node(node->switch_stmt.expression);
        for (int i = 0; i < node->switch_stmt.case_count; i++) {
            ast_free_node(node->switch_stmt.cases[i]);
        }
        free(node->switch_stmt.cases);
        break;

    case AST_CASE:
        ast_free_node(node->case_stmt.value);
        ast_free_node(node->case_stmt.block);
        break;
    case AST_VARIABLE:
        break;
    default:
        fprintf(stderr, "Unknown AST node type %d [ast_free_node]\n",
                node->type);
        break;
    }
    free(node);
}
