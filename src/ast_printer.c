#include "parser.h"
#include <stdio.h>

// Helper function to print indentation
static void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

// Function to print the AST
void ast_print(ast_node *node, int depth) {
    if (!node) {
        print_indent(depth);
        printf("<null>\n");
        return;
    }

    print_indent(depth);
    switch (node->type) {
    case AST_PROGRAM:
        printf("PROGRAM (stmt_count: %d)\n", node->program.stmt_count);
        for (int i = 0; i < node->program.stmt_count; i++) {
            ast_print(&node->program.statements[i], depth + 1);
        }
        break;

    case AST_VAR_DECL:
        printf("[VAR_DECL] ");
        switch (node->var_decl.d_type) {
        case NUMERO:
            printf("NUMERO");
            break;
        case LETRA:
            printf("LETRA");
            break;
        case TINUOD:
            printf("TINUOD");
            break;
        case TIPIK:
            printf("TIPIK");
            break;
        default:
            printf("UNKNOWN");
            break;
        }
        printf(", names: %d)\n", node->var_decl.name_count);
        for (int i = 0; i < node->var_decl.name_count; i++) {
            print_indent(depth + 1);
            printf("Name: %s", node->var_decl.names[i].lexeme);
            if (node->var_decl.inits[i]) {
                printf(" = ");
                ast_print(node->var_decl.inits[i], 0);
            } else {
                printf("\n");
            }
        }
        break;

    case AST_ASSIGNMENT:
        printf("[ASSIGN] (var: %s)\n", node->assignment.var->lexeme);
        print_indent(depth + 1);
        printf("Value:\n");
        ast_print(node->assignment.expr, depth + 2);
        break;

    case AST_PRINT:
        printf("[PRINT] (expr_count: %d)\n", node->print.expr_count);
        for (int i = 0; i < node->print.expr_count; i++) {
            print_indent(depth + 1);
            printf("Expr %d:\n", i + 1);
            ast_print(node->print.exprs[i], depth + 2);
        }
        break;

    case AST_INPUT:
        printf("[INPUT] (var_count: %d)\n", node->input.var_count);
        for (int i = 0; i < node->input.var_count; i++) {
            print_indent(depth + 1);
            printf("Var: %s\n", node->input.vars[i].lexeme);
        }
        break;

    case AST_IF:
        printf("[IF]\n");
        print_indent(depth + 1);
        printf("Condition:\n");
        ast_print(node->if_stmt.condition, depth + 2);
        print_indent(depth + 1);
        printf("Then:\n");
        ast_print(node->if_stmt.then_block, depth + 2);
        if (node->if_stmt.else_block) {
            print_indent(depth + 1);
            printf("Else:\n");
            ast_print(node->if_stmt.else_block, depth + 2);
        }
        break;

    case AST_ELSE_IF:
        printf("[ELSE_IF]\n");
        print_indent(depth + 1);
        printf("Condition:\n");
        ast_print(node->if_stmt.condition, depth + 2);
        print_indent(depth + 1);
        printf("Then:\n");
        ast_print(node->if_stmt.then_block, depth + 2);
        if (node->if_stmt.else_block) {
            print_indent(depth + 1);
            printf("Else:\n");
            ast_print(node->if_stmt.else_block, depth + 2);
        }
        break;

    case AST_ELSE:
        printf("[ELSE]\n");
        print_indent(depth + 1);
        printf("Then:\n");
        ast_print(node->if_stmt.then_block, depth + 2);
        break;

    case AST_FOR:
        printf("[FOR]\n");
        print_indent(depth + 1);
        printf("Init:\n");
        ast_print(node->for_stmt.init, depth + 2);
        print_indent(depth + 1);
        printf("Condition:\n");
        ast_print(node->for_stmt.condition, depth + 2);
        print_indent(depth + 1);
        printf("Update:\n");
        ast_print(node->for_stmt.update, depth + 2);
        print_indent(depth + 1);
        printf("Body:\n");
        ast_print(node->for_stmt.body, depth + 2);
        break;

    case AST_BLOCK:
        printf("[BLOCK] (stmt_count: %d)\n", node->block.stmt_count);
        for (int i = 0; i < node->block.stmt_count; i++) {
            ast_print(&node->block.statements[i], depth + 1);
        }
        break;

    case AST_BINARY:
        printf("[BINARY] (op: %s)\n", node->binary.op->lexeme);
        print_indent(depth + 1);
        printf("Left:\n");
        ast_print(node->binary.left, depth + 2);
        print_indent(depth + 1);
        printf("Right:\n");
        ast_print(node->binary.right, depth + 2);
        break;

    case AST_UNARY:
        printf("[UNARY] (op: %s)\n", node->unary.op->lexeme);
        print_indent(depth + 1);
        printf("Expr:\n");
        ast_print(node->unary.expr, depth + 2);
        break;

    case AST_LITERAL:
        printf("[LITERAL] (type: %s, value: %s)\n",
               token_val[node->literal.value->type],
               node->literal.value->lexeme);
        break;

    case AST_VARIABLE:
        printf("[VARIABLE] (name: %s)\n", node->variable.name->lexeme);
        break;

    default:
        printf("[UNKNOWN] (type: %d)\n", node->type);
        break;
    }
}
