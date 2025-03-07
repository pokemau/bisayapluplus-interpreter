#include "parser.h"
#include "ast.h"
#include "token.h"
#include "util.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

static ast_node *parse_for(parser *self);
static ast_node *parse_if(parser *self);
static ast_node *parse_block(parser *self);
static ast_node *parse_input(parser *self);
static ast_node *parse_var_decl(parser *self);
static ast_node *parse_binary(parser *self, int precedence);
static ast_node *parse_unary(parser *self);
static ast_node *parse_print(parser *self);
static ast_node *parse_primary(parser *self);
static ast_node *parse_assignment(parser *self);
static int get_precedence(TokenType type);

static ast_node *parse_statement(parser *self);
static ast_node *parse_expression(parser *self);

parser parser_create(token_list *tokens) {
    parser self;
    self.tokens = tokens;
    self.current = 0;
    return self;
}

static void parser_error(parser *self, const char *message) {
    token *current = &self->tokens->list[self->current];
    fprintf(stderr, "Syntax Error at line %d: %s\n", current->line, message);
    exit(1);
}

static token *peek(parser *self) {
    if (self->current >= self->tokens->size)
        return NULL;
    return &self->tokens->list[self->current];
}
static token *peek_next(parser *self) {
    if (self->current + 1 >= self->tokens->size)
        return NULL;
    return &self->tokens->list[self->current + 1];
}

static token *advance(parser *self) {
    if (self->current >= self->tokens->size)
        return NULL;
    return &self->tokens->list[self->current++];
}
static token *expect(parser *self, TokenType type, const char *message) {
    token *t = peek(self);
    if (t && t->type == type) {
        return advance(self);
    }
    parser_error(self, message);
    return NULL;
}

static bool match(parser *self, TokenType type) {
    token *t = peek(self);
    return t && t->type == type;
}

static ast_node *new_ast_node(ast_node_type type) {
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    if (!node)
        bpp_error(0, "Failed to allocate AST node");
    node->type = type;
    return node;
}

ast_node *parser_parse(parser *self) {
    ast_node *program = new_ast_node(AST_PROGRAM);
    expect(self, SUGOD, "Expected 'SUGOD' at start of program");

    ast_node *stmts = NULL;
    int temp_size = 10;
    stmts = malloc(sizeof(ast_node) * temp_size);
    if (!stmts) {
        fprintf(stderr, "STMTS ERROR\n");
        exit(1);
    }

    int stmt_count = 0;
    while (!match(self, KATAPUSAN)) {

        if (match(self, NEWLINE)) {
            advance(self);
            continue;
        }

        ast_node *stmt = parse_statement(self);
        //        ast_node *stmt = parse_declaration(self);
        stmts[stmt_count] = *stmt;
        stmt_count++;

        if (stmt_count == temp_size) {
            stmts = realloc(stmts, sizeof(ast_node) * (stmt_count + 10));
            temp_size += 10;
        }
    }

    expect(self, KATAPUSAN, "Expected 'KATAPUSAN' at the end of program");

    program->program.statements = realloc(stmts, sizeof(ast_node) * stmt_count);
    program->program.stmt_count = stmt_count;
    program->program.statements = stmts;

    return program;
}

static ast_node *parse_statement(parser *self) {
    token *t = peek(self);

    printf("CURR TOKEN: ");
    print_token(t);

    if (match(self, MUGNA)) {
        ast_node *t = parse_var_decl(self);
        printf("%d\n", t->type);
        return t;
    } else if (match(self, IPAKITA)) {
        return parse_print(self);
    } else if (match(self, IDENTIFIER)) {
        return parse_assignment(self);
    } else if (match(self, ALANG_SA)) {
        return parse_for(self);
    } else if (match(self, DAWAT)) {
        return parse_input(self);
    }

    // } else if (match(self, DAWAT)) {
    //     return parse_input(self);
    // } else if (match(self, KUNG)) {
    //     return parse_if(self);
    // } else if (match(self, MINUS) && peek_next(self)->type == MINUS) {
    //     advance(self); // Skip -- comment
    //     advance(self);
    //     while (!match(self, NEWLINE) && peek(self))
    //         advance(self);
    //     return parse_statement(self); // Recursive call for next statement
    // }
    parser_error(self, "Unexpected token in statement");
    return NULL;
}

static ast_node *parse_var_decl(parser *self) {
    advance(self);

    TokenType d_type = advance(self)->type;
    if (d_type != NUMERO && d_type != LETRA && d_type != TINUOD &&
        d_type != TIPIK) {
        parser_error(self, "Expected data type after 'MUGNA'");
    }

    ast_node *decl = new_ast_node(AST_VAR_DECL);

    // TODO: implement allocator functions
    int temp_size = 10;

    token *names = malloc(sizeof(token) * temp_size);
    ast_node **inits = malloc(sizeof(ast_node *) * temp_size);
    int name_count = 0;

    while (!match(self, NEWLINE)) {
        token *name = expect(self, IDENTIFIER, "Expected: variable name");

        names[name_count] = *name;

        ast_node *init = NULL;
        if (match(self, EQUAL)) {
            advance(self);
            init = parse_expression(self);
        }

        inits[name_count] = init;
        name_count++;

        if (name_count == temp_size) {
            names = realloc(names, sizeof(token) * (name_count + 10));
            inits = realloc(inits, sizeof(ast_node *) * (name_count + 10));
            temp_size += 10;
        }

        if (match(self, COMMA)) {
            advance(self);
        }
    }

    names = realloc(names, sizeof(token) * name_count);
    inits = realloc(inits, sizeof(ast_node *) * name_count);

    decl->var_decl.names = names;
    decl->var_decl.inits = inits;
    decl->var_decl.name_count = name_count;
    decl->var_decl.d_type = d_type;
    return decl;
}

static ast_node *parse_assignment(parser *self) {
    token *var_name = advance(self);
    expect(self, EQUAL, "Expected '=' after variable name");

    ast_node *expr = NULL;

    if (match(self, IDENTIFIER) && peek_next(self)->type == EQUAL) {
        expr = parse_assignment(self);
    } else {
        expr = parse_expression(self);
    }

    ast_node *assign = new_ast_node(AST_ASSIGNMENT);
    assign->assignment.expr = expr;
    assign->assignment.var = var_name;
    return assign;
}

static ast_node *parse_print(parser *self) {
    advance(self);
    expect(self, COLON, "Expected ':' after 'IPAKITA'");

    ast_node *print = new_ast_node(AST_PRINT);
    ast_node **exprs = NULL;
    int temp_size = 10;
    exprs = malloc(sizeof(ast_node *) * temp_size);

    int expr_count = 0;

    while (!match(self, NEWLINE)) {
        ast_node *expr = parse_expression(self);
        exprs[expr_count++] = expr;

        if (expr_count == temp_size) {
            exprs = realloc(exprs, sizeof(ast_node *) * (expr_count * 10));
            if (!exprs) {
                fprintf(stderr, "ERROR:>Failed to resize (parse_print)\n");
                exit(1);
            }
            temp_size += 10;
        }

        if (match(self, AMPERSAND)) {
            advance(self);
        }
    }

    print->print.exprs = exprs;
    print->print.expr_count = expr_count;
    return print;
}

static ast_node *parse_input(parser *self) {
    advance(self);
    expect(self, COLON, "Expected ':' after 'DAWAT'");

    ast_node *input = new_ast_node(AST_INPUT);
    token *vars = NULL;

    int temp_size = 10;
    vars = malloc(sizeof(token) * temp_size);
    if (!vars) {
        fprintf(stderr, "Failed to allocate vars [parse_input]");
        exit(1);
    }
    int var_count = 0;

    while (true) {
        token *var = expect(self, IDENTIFIER, "Expected variable name");
        vars[var_count++] = *var;
        if (var_count == temp_size) {
            vars = realloc(vars, sizeof(token) * (var_count + 10));
            temp_size += 10;
        }
        if (match(self, NEWLINE))
            break;
        expect(self, COMMA, "EXPECT COMMA");
    }

    input->input.vars = vars;
    input->input.var_count = var_count;
    return input;
}

static ast_node *parse_block(parser *self) {
    expect(self, PUNDOK, "Expected 'PUNDOK'");
    expect(self, LEFT_BRACE, "Expected '{' after 'PUNDOK'");

    ast_node *block = new_ast_node(AST_BLOCK);
    ast_node **stmts = NULL;
    int stmt_count = 0;

    while (!match(self, RIGHT_BRACE) && peek(self)) {
        if (match(self, NEWLINE)) {
            advance(self);
            continue;
        }
        ast_node *stmt = parse_statement(self);
        stmts = realloc(stmts, sizeof(ast_node *) * (stmt_count + 1));
        stmts[stmt_count++] = stmt;
    }

    expect(self, RIGHT_BRACE, "Expected '}' after block");
    block->block.statements = *stmts;
    block->block.stmt_count = stmt_count;
    return block;
}

static ast_node *parse_if(parser *self) {
    advance(self); // Skip KUNG
    expect(self, LEFT_PAREN, "Expected '(' after 'KUNG'");
    ast_node *condition = parse_expression(self);
    expect(self, RIGHT_PAREN, "Expected ')' after condition");

    ast_node *then_block = parse_block(self);
    ast_node *else_block = NULL;

    if (match(self, KUNG_WALA)) {
        advance(self);
        else_block = parse_block(self);
    } else if (match(self, KUNG_DILI)) {
        advance(self);
        expect(self, LEFT_PAREN, "Expected '(' after 'KUNG DILI'");
        ast_node *elif_cond = parse_expression(self);
        expect(self, RIGHT_PAREN, "Expected ')' after condition");
        ast_node *elif_block = parse_block(self);

        ast_node *if_node = new_ast_node(AST_IF);
        if_node->if_stmt.condition = elif_cond;
        if_node->if_stmt.then_block = elif_block;

        if (match(self, KUNG_WALA)) {
            advance(self);
            if_node->if_stmt.else_block = parse_block(self);
        }
        else_block = if_node;
    }

    ast_node *if_stmt = new_ast_node(AST_IF);
    if_stmt->if_stmt.condition = condition;
    if_stmt->if_stmt.then_block = then_block;
    if_stmt->if_stmt.else_block = else_block;
    return if_stmt;
}

static ast_node *parse_for(parser *self) {
    advance(self);
    expect(self, LEFT_PAREN, "Expected '(' after ALANG SA");

    ast_node *init = NULL;

    if (match(self, MUGNA)) {
        init = parse_var_decl(self);
    } else if (match(self, IDENTIFIER)) {
        init = parse_assignment(self);
    }

    expect(self, COMMA, "Expected ',' after initialization");
    ast_node *condition = parse_expression(self);

    expect(self, COMMA, "Expected ',' after condition");

    ast_node *update = parse_assignment(self);

    // handle increment operator (ctr++)
//    if (peek(self)->type == IDENTIFIER && peek_next(self)->type == INCREMENT) {
//        token *var_name = advance(self);
//        token *op = expect(self, INCREMENT, "Expected '++'");
//        update = new_ast_node(AST_UNARY);
//        update->unary.op = op;
//        update->unary.expr = new_ast_node(AST_VARIABLE);
//        update->unary.expr->variable.name = var_name;
//    } else {
//        update = parse_expression(self);
//    }

    expect(self, RIGHT_PAREN, "Expected ')' after update");
    expect(self, NEWLINE, "Expected 'PUNDOK' in new line");

    ast_node *body = parse_block(self);

    ast_node *for_stmt = new_ast_node(AST_FOR);
    for_stmt->for_stmt.init = init;
    for_stmt->for_stmt.condition = condition;
    for_stmt->for_stmt.update = update;
    for_stmt->for_stmt.body = body;
    return for_stmt;
}

static ast_node *parse_expression(parser *self) {
    return parse_binary(self, 0);
}

static ast_node *parse_binary(parser *self, int precedence) {
    ast_node *left = parse_unary(self);

    while (peek(self)) {
        token *op = peek(self);
        int op_precedence = get_precedence(op->type);
        if (op_precedence <= precedence)
            break;

        advance(self);
        ast_node *right = parse_binary(self, op_precedence);

        ast_node *binary = new_ast_node(AST_BINARY);
        binary->binary.op = op;
        binary->binary.left = left;
        binary->binary.right = right;
        left = binary;
    }
    return left;
}

static int get_precedence(TokenType type) {
    switch (type) {
    case STAR:
    case SLASH:
    case MODULO:
        return 3;
    case PLUS:
    case MINUS:
        return 2;
    case LESS:
    case GREATER:
    case LESS_EQUAL:
    case GREATER_EQUAL:
    case EQUAL_EQUAL:
    case NOT_EQUAL:
        return 1;
    case UG:
    case O:
        return 0;
    default:
        return -1;
    }
}

static ast_node *parse_unary(parser *self) {
    if (match(self, PLUS) || match(self, MINUS) || match(self, DILI)) {
        token *op = advance(self);
        printf("TOKE: ");
        print_token(op);
        ast_node *expr = parse_unary(self);
        ast_node *unary = new_ast_node(AST_UNARY);
        unary->unary.op = op;
        unary->unary.expr = expr;
        return unary;
    }
    return parse_primary(self);
}

static ast_node *parse_primary(parser *self) {
    token *t = peek(self);
    if (!t)
        parser_error(self, "Unexpected end of input");

    if (match(self, NUMBER) || match(self, STRING) || match(self, TRUE) ||
        match(self, FALSE)) {
        ast_node *literal = new_ast_node(AST_LITERAL);
        literal->literal.value = advance(self);
        return literal;
    } else if (match(self, IDENTIFIER)) {
        ast_node *var = new_ast_node(AST_VARIABLE);
        var->variable.name = advance(self);
        return var;
    } else if (match(self, LEFT_PAREN)) {
        advance(self);
        ast_node *expr = parse_expression(self);
        expect(self, RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    } else if (match(self, LEFT_BRACKET)) { // Escape code [#]
        advance(self);                      // Consume [
        expect(self, HASH, "Expected '#' after '[' in escape code");
        expect(self, RIGHT_BRACKET, "Expected ']' after '#'");
        ast_node *literal = new_ast_node(AST_LITERAL);
        literal->literal.value = malloc(sizeof(token));
        literal->literal.value->type = HASH;
        literal->literal.value->lexeme = "#";
        literal->literal.value->line = t->line;
        return literal;
    } else if (match(self, DOLLAR)) { // Newline ($)
        advance(self);
        ast_node *literal = new_ast_node(AST_LITERAL);
        literal->literal.value = malloc(sizeof(token));
        literal->literal.value->type = DOLLAR;
        literal->literal.value->lexeme = "$";
        literal->literal.value->line = t->line;
        return literal;
    }

    parser_error(self, "Expected expression");
    return NULL;
}
