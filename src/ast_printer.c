#include "parser.h"
#include <stdio.h>
#include <string.h>

// Helper function to print indentation
static void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  "); // Two spaces per depth level
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
        printf("PROGRAM (stmt_count: %d)\n", node->Program.stmt_count);
        for (int i = 0; i < node->Program.stmt_count; i++) {
            ast_print(&node->Program.statements[i], depth + 1);
        }
        break;

    case AST_VAR_DECL:
        printf("[VAR_DECL] ");
        switch (node->Var_decl.d_type) {
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
        printf(", names: %d)\n", node->Var_decl.name_count);
        for (int i = 0; i < node->Var_decl.name_count; i++) {
            print_indent(depth + 1);
            printf("Name: %s", node->Var_decl.names[i]->lexeme);
            if (node->Var_decl.inits[i]) {
                printf(" = ");
                ast_print(node->Var_decl.inits[i], 0); // Inline expression
            } else {
                printf("\n");
            }
        }
        break;

    case AST_ASSIGNMENT:
        printf("[ASSIGN] (var: %s)\n", node->Assignment.var->lexeme);
        print_indent(depth + 1);
        printf("Value:\n");
        ast_print(node->Assignment.expr, depth + 2);
        break;

    case AST_PRINT:
        printf("[PRINT] (expr_count: %d)\n", node->Print_stmt.expr_count);
        for (int i = 0; i < node->Print_stmt.expr_count; i++) {
            print_indent(depth + 1);
            printf("Expr %d:\n", i + 1);
            ast_print(node->Print_stmt.exprs[i], depth + 2);
        }
        break;

    case AST_INPUT:
        printf("[INPUT] (var_count: %d)\n", node->Input.var_count);
        for (int i = 0; i < node->Input.var_count; i++) {
            print_indent(depth + 1);
            printf("Var: %s\n", node->Input.vars[i].lexeme);
        }
        break;

    case AST_IF:
        printf("[IF]\n");
        print_indent(depth + 1);
        printf("Condition:\n");
        ast_print(node->If_stmt.condition, depth + 2);
        print_indent(depth + 1);
        printf("Then:\n");
        ast_print(node->If_stmt.then_block, depth + 2);
        if (node->If_stmt.else_block) {
            print_indent(depth + 1);
            printf("Else:\n");
            ast_print(node->If_stmt.else_block, depth + 2);
        }
        break;

    case AST_ELSE_IF:
        printf("[ELSE_IF]\n");
        print_indent(depth + 1);
        printf("Condition:\n");
        ast_print(node->If_stmt.condition, depth + 2);
        print_indent(depth + 1);
        printf("Then:\n");
        ast_print(node->If_stmt.then_block, depth + 2);
        if (node->If_stmt.else_block) {
            print_indent(depth + 1);
            printf("Else:\n");
            ast_print(node->If_stmt.else_block, depth + 2);
        }
        break;

    case AST_ELSE:
        printf("[ELSE]\n");
        print_indent(depth + 1);
        printf("Then:\n");
        ast_print(node->If_stmt.then_block, depth + 2);
        // No condition or else_block for AST_ELSE
        break;

    case AST_FOR:
        printf("[FOR]\n");
        print_indent(depth + 1);
        printf("Init:\n");
        ast_print(node->For_stmt.init, depth + 2);
        print_indent(depth + 1);
        printf("Condition:\n");
        ast_print(node->For_stmt.condition, depth + 2);
        print_indent(depth + 1);
        printf("Update:\n");
        ast_print(node->For_stmt.update, depth + 2);
        print_indent(depth + 1);
        printf("Body:\n");
        ast_print(node->For_stmt.body, depth + 2);
        break;

    case AST_BLOCK:
        printf("[BLOCK] (stmt_count: %d)\n", node->Block.stmt_count);
        for (int i = 0; i < node->Block.stmt_count; i++) {
            ast_print(&node->Block.statements[i], depth + 1);
        }
        break;

    case AST_BINARY:
        printf("[BINARY] (op: %s)\n", node->Binary.op->lexeme);
        print_indent(depth + 1);
        printf("Left:\n");
        ast_print(node->Binary.left, depth + 2);
        print_indent(depth + 1);
        printf("Right:\n");
        ast_print(node->Binary.right, depth + 2);
        break;

    case AST_UNARY:
        printf("[UNARY] (op: %s)\n", node->Unary.op->lexeme);
        print_indent(depth + 1);
        printf("Expr:\n");
        ast_print(node->Unary.expr, depth + 2);
        break;

    case AST_LITERAL:
        printf("[LITERAL] (type: %s, value: %s)\n",
               token_val[node->Literal.value->type], // Assumes token_val array exists
               node->Literal.value->lexeme);
        break;

    case AST_VARIABLE:
        printf("[VARIABLE] (name: %s)\n", node->Variable.name->lexeme);
        break;

    default:
        printf("[UNKNOWN] (type: %d)\n", node->type);
        break;
    }
}
