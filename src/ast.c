#include "ast.h"
#include "util.h"
#include "util/dyn_arr.h"
#include <stdlib.h>
#include <stdio.h>

ast_node *ast_new_node(ast_node_type type) {
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    if (!node)
        bpp_error(0, "Failed to allocate AST node");
    node->type = type;
    return node;
}

// TODO:
// free() allocated mem for other ast_node types
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
