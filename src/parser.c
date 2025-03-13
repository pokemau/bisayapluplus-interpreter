#include "parser.h"
#include "expr.h"
#include "stmt.h"
#include "token.h"
#include "util/dyn_arr.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static int get_precedence(TokenType type);
static void parser_curr(Parser *self);
static void synchronize(Parser *self);
static void parser_error(Parser *self, const char *message);
static bool match(Parser *self, TokenType type);
static bool is_at_end(Parser *self);
static Token *peek_prev(Parser *self, int count);
static Token *peek(Parser *self);
static Token *peek_next(Parser *self);
static Token *advance(Parser *self);
static Token *previous(Parser *self);
static Token *expect(Parser *self, TokenType type, const char *message);
// static ast_node *parse_for(Parser *self);
// static ast_node *parse_if(Parser *self);
// static ast_node *parse_block(Parser *self);
// static ast_node *parse_input(Parser *self);
// static ast_node *parse_var_decl(Parser *self);
// static ast_node *parse_binary(Parser *self, int precedence);
// static ast_node *parse_unary(Parser *self);
// static ast_node *parse_print(Parser *self);
// static ast_node *parse_primary(Parser *self);
// static ast_node *parse_assignment(Parser *self);
// static ast_node *parse_statement(Parser *self);
// static ast_node *parse_expression(Parser *self);

static Expr *parse_expression(Parser *self);
static Expr *parse_assignment(Parser *self);
static Expr *parse_unary(Parser *self);
static Expr *parse_primary(Parser *self);
static Expr *parse_factor(Parser *self);
static Expr *parse_equality(Parser *self);
static Expr *parse_term(Parser *self);
static Expr *parse_comparison(Parser *self);

static Stmt *parse_decl(Parser *self);
static Stmt *parse_var_decl(Parser *self);
static Stmt *parse_statement(Parser *self);

static Stmt *parse_for_stmt(Parser *self);
static Stmt *parse_expr_stmt(Parser *self);

Parser parser_create(token_list *tokens) {
    Parser self;
    self.tokens = tokens;
    self.current = 0;
    self.head = &tokens->list[0];
    return self;
}

Stmt *parser_parse(Parser *self) {
    expect(self, SUGOD, "Expected: 'SUGOD' at the start of program.");
    expect(self, NEWLINE, "Expected: Statement in new line");

    Stmt *statements = dynarray_create(Stmt);

    bool end = false;

    while (!is_at_end(self)) {

        if (match(self, NEWLINE))
            advance(self);

        Stmt *stmt = parse_decl(self);
        if (stmt)
            dynarray_push(statements, stmt);
    }

    // TODO: check for KATAPUSAN
    return statements;
}

// ast_node *parser_parse(Parser *self) {
//     ast_node *program = ast_new_node(AST_PROGRAM);
//     expect(self, SUGOD, "Expected 'SUGOD' at start of program");
//
//     ast_node *stmts = dynarray_create(ast_node);
//
//     Stmt *statements = dynarray_create(Stmt);
//
//     while (peek(self)) {
//     }
//
//     while (peek(self)) {
//
//         if (match(self, NEWLINE)) {
//             advance(self);
//             continue;
//         }
//
//         if (match(self, KATAPUSAN))
//             break;
//
//         ast_node *stmt = parse_statement(self);
//         if (stmt)
//             dynarray_push(stmts, *stmt);
//     }
//
//     expect(self, KATAPUSAN, "Expected 'KATAPUSAN' at the end of program");
//
//     program->Program.stmt_count = dynarray_length(stmts);
//     program->Program.statements = stmts;
//
//     return program;
// }

static Expr *parse_expression(Parser *self) {
    return parse_assignment(self);
}

static Expr *parse_assignment(Parser *self) {
    Expr *e;
    if (match(self, EQUAL)) {
        Token *op = advance(self);
        Expr *val = parse_assignment(self);
    }
    return e;
}

static Expr *parse_unary(Parser *self) {
    if (match(self, MINUS) || match(self, DILI)) {
        Token *op = peek_prev(self, 1);
        Expr *right = parse_unary(self);
        return make_unary(op, right);
    }
    return parse_primary(self);
}

static Expr *parse_primary(Parser *self) {
    if (match(self, FALSE)) {
        return make_literal(NULL, FALSE);
    }
    if (match(self, TRUE)) {
        return make_literal(NULL, TRUE);
    }
    if (match(self, NUMBER) || match(self, STRING)) {
        return make_literal(peek(self)->literal, peek(self)->type);
    }
    if (match(self, IDENTIFIER)) {
        return make_variable(peek(self));
    }
    return NULL;
}

static Expr *parse_factor(Parser *self) {
    Expr *e = parse_unary(self);
    while (match(self, SLASH) || match(self, STAR) || match(self, MODULO)) {
        Token *op = peek(self);
        Expr *right = parse_unary(self);
        e = make_binary(e, op, right);
    }
    return e;
}

static Expr *parse_equality(Parser *self) {
    Expr *e = parse_comparison(self);
    while (match(self, NOT_EQUAL) || match(self, EQUAL_EQUAL)) {
        Token *op = advance(self);
        Expr *right = parse_comparison(self);
        e = make_binary(e, op, right);
    }
    return e;
}

static Expr *parse_term(Parser *self) {
    Expr *e = parse_factor(self);
    while (match(self, MINUS) || match(self, PLUS) || match(self, AMPERSAND)) {
        Token *op = advance(self);
        Expr *right = parse_factor(self);
        e = make_binary(e, op, right);
    }
    return e;
}

static Expr *parse_comparison(Parser *self) {
    Expr *e = parse_term(self);
    while (match(self, GREATER) || match(self, LESS) ||
           match(self, GREATER_EQUAL || match(self, LESS_EQUAL))) {
        Token *op = advance(self);
        Expr *right = parse_comparison(self);
        e = make_binary(e, op, right);
    }
    return e;
}

static Stmt *parse_decl(Parser *self) {
    Stmt *res = NULL;

    if (match(self, MUGNA)) {
        res = parse_var_decl(self);
    } else {
        res = parse_statement(self);
    }
    // else {
    //     printf("ELSE CURR::::::");
    //     parser_curr(self);
    //     parser_error(self, "Invalid Token");
    //     exit(1);
    // }

    if (!res) {
        printf("SYNC CURR:::::::::");
        parser_curr(self);
        printf("SYNCHRONIZE\n");
        synchronize(self);
        return NULL;
    }

    return res;
}

static Stmt *parse_var_decl(Parser *self) {
    advance(self);
    Token *curr = peek(self);
    TokenType d_type = curr->type;
    if (d_type != NUMERO && d_type != TIPIK && d_type != TINUOD &&
        d_type != LETRA) {
        parser_error(self, "Expected: Data type after 'MUGNA'");
        return NULL;
    }
    advance(self);

    int size = 10;
    Stmt *var_decls = malloc(sizeof(Stmt) * size);
    int count = 0;

    while (!match(self, NEWLINE)) {
        Token *name = expect(self, IDENTIFIER, "Expected: variable name");
        if (!name) {
            return NULL;
        }

        Expr *initializer = NULL;
        if (match(self, EQUAL)) {
            initializer = parse_expression(self);
        }

        Stmt *var_decl = stmt_make_var_decl(name, initializer);
        var_decls[count++] = *var_decl;
        if (count == size) {
            var_decls = realloc(var_decls, sizeof(Stmt) * (count+10));
            size += 10;
        }

        if (match(self, COMMA)) {
            advance(self);
        }
    }
    return var_decls;
    //     while (!match(self, NEWLINE)) {
    //         Token *name = expect(self, IDENTIFIER, "Expected: variable
    //         name"); print_token(name); if (!name) {
    //             return NULL;
    //         }
    //
    //         names[name_count] = name;
    //
    //         ast_node *init = NULL;
    //         if (match(self, EQUAL)) {
    //             advance(self);
    //             init = parse_expression(self);
    //         }
    //
    //         inits[name_count] = init;
    //         name_count++;
    //
    //         if (name_count == temp_size) {
    //             names = realloc(names, sizeof(Token *) * (name_count + 10));
    //             inits = realloc(inits, sizeof(ast_node *) * (name_count +
    //             10)); temp_size += 10;
    //         }
    //
    //         if (match(self, COMMA)) {
    //             if (peek_next(self) && (peek_next(self)->type != IDENTIFIER))
    //             {
    //                 parser_error(self, "Expected: variable name after ','");
    //                 return NULL;
    //             }
    //             advance(self);
    //         }
    //     }
}

static Stmt *parse_statement(Parser *self) {
    if (match(self, ALANG_SA)) {
        return parse_for_stmt(self);
    }

    return parse_expr_stmt(self);
}

static Stmt *parse_for_stmt(Parser *self) {

}

static Stmt *parse_expr_stmt(Parser *self) {

}

// static Expr *parse_factor(Parser *self) {
//     Expr *e = parse_unary(self);
// }

// static ast_node *parse_statement(Parser *self) {
//     Token *t = peek(self);
//
//     ast_node *res = NULL;
//
//     if (match(self, MUGNA)) {
//         res = parse_var_decl(self);
//     } else if (match(self, IDENTIFIER)) {
//         res = parse_assignment(self);
//     } else if (match(self, KUNG)) {
//         res = parse_if(self);
//     } else {
//         char buf[256];
//         snprintf(buf, 256, "Unexpected token in statement '%s'",
//                  peek(self)->lexeme);
//         parser_error(self, buf);
//     }
//
//     if (!res) {
//         synchronize(self);
//         return NULL;
//     }
//
//     // parser_curr(self);
//     return res;
// }

// static ast_node *parse_var_decl(Parser *self) {
//     advance(self);
//
//     TokenType d_type = advance(self)->type;
//     if (d_type != NUMERO && d_type != LETRA && d_type != TINUOD &&
//         d_type != TIPIK) {
//         parser_error(self, "Expected data type after 'MUGNA'");
//         return NULL;
//     }
//
//     ast_node *decl = ast_new_node(AST_VAR_DECL);
//
//     // TODO: implement allocator functions
//     int temp_size = 10;
//     Token **names = malloc(sizeof(Token *) * temp_size);
//     ast_node **inits = malloc(sizeof(ast_node *) * temp_size);
//     int name_count = 0;
//
//     while (!match(self, NEWLINE)) {
//         Token *name = expect(self, IDENTIFIER, "Expected: variable name");
//         print_token(name);
//         if (!name) {
//             return NULL;
//         }
//
//         names[name_count] = name;
//
//         ast_node *init = NULL;
//         if (match(self, EQUAL)) {
//             advance(self);
//             init = parse_expression(self);
//         }
//
//         inits[name_count] = init;
//         name_count++;
//
//         if (name_count == temp_size) {
//             names = realloc(names, sizeof(Token *) * (name_count + 10));
//             inits = realloc(inits, sizeof(ast_node *) * (name_count + 10));
//             temp_size += 10;
//         }
//
//         if (match(self, COMMA)) {
//             if (peek_next(self) && (peek_next(self)->type != IDENTIFIER)) {
//                 parser_error(self, "Expected: variable name after ','");
//                 return NULL;
//             }
//             advance(self);
//         }
//     }
//
//     if (name_count == 0) {
//         parser_error(self, "Expected: variable name");
//         return NULL;
//     }
//
//     names = realloc(names, sizeof(Token *) * name_count);
//     inits = realloc(inits, sizeof(ast_node *) * name_count);
//
//     decl->Var_decl.names = names;
//     decl->Var_decl.inits = inits;
//     decl->Var_decl.name_count = name_count;
//     decl->Var_decl.d_type = d_type;
//     return decl;
// }

// static ast_node *parse_assignment(Parser *self) {
//     Token *var_name = advance(self);
//     expect(self, EQUAL, "Expected '=' after variable name");
//
//     ast_node *expr = NULL;
//
//     if (match(self, IDENTIFIER) && peek_next(self)->type == EQUAL) {
//         expr = parse_assignment(self);
//     } else {
//         expr = parse_expression(self);
//     }
//
//     ast_node *assign = ast_new_node(AST_ASSIGNMENT);
//     assign->Assignment.expr = expr;
//     assign->Assignment.var = var_name;
//
//     return assign;
// }
//
// static ast_node *parse_print(Parser *self) {
//     advance(self);
//     expect(self, COLON, "Expected ':' after 'IPAKITA'");
//
//     ast_node *print = ast_new_node(AST_PRINT);
//     ast_node **exprs = NULL;
//     int temp_size = 10;
//     exprs = malloc(sizeof(ast_node *) * temp_size);
//
//     int expr_count = 0;
//
//     while (!match(self, NEWLINE)) {
//         ast_node *expr = parse_expression(self);
//         exprs[expr_count++] = expr;
//
//         if (expr_count == temp_size) {
//             exprs = realloc(exprs, sizeof(ast_node *) * (expr_count * 10));
//             if (!exprs) {
//                 fprintf(stderr, "ERROR:>Failed to resize (parse_print)\n");
//                 exit(1);
//             }
//             temp_size += 10;
//         }
//
//         if (match(self, AMPERSAND)) {
//             advance(self);
//         }
//     }
//
//     print->Print_stmt.exprs = exprs;
//     print->Print_stmt.expr_count = expr_count;
//     return print;
// }
//
// static ast_node *parse_input(Parser *self) {
//     advance(self);
//     expect(self, COLON, "Expected ':' after 'DAWAT'");
//
//     ast_node *input = ast_new_node(AST_INPUT);
//     Token *vars = NULL;
//
//     int temp_size = 10;
//     vars = malloc(sizeof(Token) * temp_size);
//     if (!vars) {
//         fprintf(stderr, "Failed to allocate vars [parse_input]");
//         exit(1);
//     }
//     int var_count = 0;
//
//     while (true) {
//         Token *var = expect(self, IDENTIFIER, "Expected variable name");
//         vars[var_count++] = *var;
//         if (var_count == temp_size) {
//             vars = realloc(vars, sizeof(Token) * (var_count + 10));
//             temp_size += 10;
//         }
//         if (match(self, NEWLINE))
//             break;
//         expect(self, COMMA, "EXPECT COMMA");
//     }
//
//     input->Input.vars = vars;
//     input->Input.var_count = var_count;
//     return input;
// }
//
// static ast_node *parse_block(Parser *self) {
//     if (!expect(self, PUNDOK, "Expected 'PUNDOK'"))
//         return NULL;
//     if (!expect(self, LEFT_BRACE, "Expected '{' after 'PUNDOK'"))
//         return NULL;
//
//     ast_node *block = ast_new_node(AST_BLOCK);
//     ast_node **stmts = NULL;
//     int stmt_count = 0;
//
//     while (!match(self, RIGHT_BRACE) && peek(self)) {
//         if (match(self, NEWLINE)) {
//             advance(self);
//             continue;
//         }
//         ast_node *stmt = parse_statement(self);
//         stmts = realloc(stmts, sizeof(ast_node *) * (stmt_count + 1));
//         stmts[stmt_count++] = stmt;
//     }
//
//     expect(self, RIGHT_BRACE, "Expected '}' after block");
//     if (stmts) {
//         block->Block.statements = *stmts;
//     }
//     block->Block.stmt_count = stmt_count;
//     return block;
// }

static void parser_curr(Parser *self) {
    printf("CURR:::::::: ");
    print_token(peek(self));
}

static void synchronize(Parser *self) {
    advance(self);

    while (!is_at_end(self)) {
        Token *curr = peek(self);
        switch (curr->type) {
        case MUGNA:
        case KUNG:
        case ALANG_SA:
        case IPAKITA:
        case DAWAT:
            printf("END SYNC:::::");
            parser_curr(self);
            return;
        }
        advance(self);
    }
}

static Token *peek(Parser *self) {
    if (self->current >= self->tokens->size)
        return NULL;
    return &self->tokens->list[self->current];
}
static Token *peek_next(Parser *self) {
    if (self->current + 1 >= self->tokens->size)
        return NULL;
    return &self->tokens->list[self->current + 1];
}

static Token *advance(Parser *self) {
    if (self->current >= self->tokens->size)
        return NULL;
    Token *res = &self->tokens->list[self->current++];
    self->head = &self->tokens->list[self->current];
    return res;
}

static Token *previous(Parser *self) {
    if (self->current == 0)
        return NULL;
    Token *res = &self->tokens->list[self->current--];
    self->head = &self->tokens->list[self->current];
    return res;
}

static Token *expect(Parser *self, TokenType type, const char *message) {
    Token *t = peek(self);
    if (t && t->type == type) {
        return advance(self);
    }
    parser_error(self, message);
    return NULL;
}

static bool match(Parser *self, TokenType type) {
    Token *t = peek(self);
    return t && t->type == type;
}

static void parser_error(Parser *self, const char *message) {
    Token *current = &self->tokens->list[self->current];
    fprintf(stderr, "Syntax Error at line %d: %s\n", current->line, message);
    //    exit(1);
}

static bool is_at_end(Parser *self) {
    return peek(self) && peek(self)->type != EOFILE;
}

static Token *peek_prev(Parser *self, int count) {
    if (self->current - count < 0)
        return NULL;
    return &self->tokens->list[self->current - count];
}
