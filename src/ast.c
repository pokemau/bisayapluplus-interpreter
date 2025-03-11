#include "ast.h"

ast_node *ast_new_node(ast_node_type type) {
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    if (!node)
        bpp_error(0, "Failed to allocate AST node");
    node->type = type;
    return node;
}
