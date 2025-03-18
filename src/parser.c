#include "parser.h"
#include "ast.h"
#include "token.h"
#include "util/dyn_arr.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static ast_node *parse_statement(parser *self);
static ast_node *parse_if_stmt(parser *self);
static ast_node *parse_else_stmt(parser *self);
static ast_node *parse_for_stmt(parser *self);
static ast_node *parse_print_stmt(parser *self);
static ast_node *parse_input_stmt(parser *self);

static ast_node *parse_var_decl(parser *self);
static ast_node *parse_assignment(parser *self);

static ast_node *parse_block(parser *self);

static ast_node *parse_expression(parser *self);
static ast_node *parse_binary(parser *self, int precedence);
static ast_node *parse_unary(parser *self);
static ast_node *parse_primary(parser *self);

static int get_precedence(TokenType type);

static bool is_at_end(parser *self);
static token *peek(parser *self);

static void synchronize(parser *self);
static void parser_curr(parser *self);

parser parser_create(token_list *tokens) {
    parser self;
    self.tokens = tokens;
    self.current = 0;
    self.head = &tokens->list[0];
    return self;
}

static void parser_error(parser *self, const char *message) {
    token *current = &self->tokens->list[self->current];
    fprintf(stderr, "Syntax Error at line %d: %s\n", current->line, message);
    //    exit(1);
}

static bool is_at_end(parser *self) { return peek(self)->type == EOFILE; }

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
static token *peek_prev(parser *self) {
    if (self->current == 0)
        return NULL;
    return &self->tokens->list[self->current - 1];
}

static token *previous(parser *self) {
    if (self->current == 0)
        return NULL;
    token *res = &self->tokens->list[self->current--];
    self->head = &self->tokens->list[self->current];
    return res;
}
static token *advance(parser *self) {
    if (self->current >= self->tokens->size)
        return NULL;
    token *res = &self->tokens->list[self->current++];
    self->head = &self->tokens->list[self->current];
    return res;
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

static ast_node *parse_statement(parser *self) {
    token *t = peek(self);
    //    printf("CURR TOKEN:::::");
    //    print_token(t);

    ast_node *res = NULL;

    if (match(self, MUGNA)) {
        res = parse_var_decl(self);
    } else if (match(self, IPAKITA)) {
        res = parse_print_stmt(self);
    } else if (match(self, IDENTIFIER)) {
        res = parse_assignment(self);
    } else if (match(self, ALANG_SA)) {
        res = parse_for_stmt(self);
    } else if (match(self, DAWAT)) {
        res = parse_input_stmt(self);
    } else if (match(self, KUNG)) {
        res = parse_if_stmt(self);
    } else if (match(self, KATAPUSAN)) {
        while (!is_at_end(self))
            advance(self);
        return NULL;
    } else {
        printf("ELSE::::::::");
        parser_curr(self);
        char buf[128];
        snprintf(buf, 128, "Unexpected token '%s' in statement",
                 peek(self)->lexeme);
        parser_error(self, buf);
    }

    if (!res) {
        printf("SYNCHRONIZE\n");
        synchronize(self);
        printf("AFTER SYNC:::::::");
        parser_curr(self);
        return NULL;
    }
    return res;
}

static ast_node *parse_var_decl(parser *self) {
    printf("=============PARSE VAR DECL=============\n");
    advance(self);

    TokenType d_type = advance(self)->type;
    if (d_type != NUMERO && d_type != LETRA && d_type != TINUOD &&
        d_type != TIPIK) {
        parser_error(self, "Expected data type after 'MUGNA'");
        return NULL;
    }

    // TODO: implement allocator functions
    int temp_size = 10;
    token *names = malloc(sizeof(token) * temp_size);
    ast_node **inits = malloc(sizeof(ast_node *) * temp_size);
    if (!names || !inits) {
        parser_error(self, "Failed to allocate [parse_var_decl]");
        return NULL;
    }
    int name_count = 0;

    while (!match(self, NEWLINE)) {
        token *name = advance(self);
        if (!name || name->type != IDENTIFIER) {
            parser_error(self, "Expected: variable name");
            free(names);
            free(inits);
            return NULL;
        }

        names[name_count] = *name;

        ast_node *init = NULL;
        if (match(self, EQUAL)) {
            advance(self);
            init = parse_expression(self);
            if (!init) {
                free(names);
                free(inits);
                return NULL;
            }
        }

        inits[name_count] = init;
        name_count++;

        if (name_count == temp_size) {
            names = realloc(names, sizeof(token) * (name_count + 10));
            inits = realloc(inits, sizeof(ast_node *) * (name_count + 10));
            temp_size += 10;
        }

        if (match(self, COMMA)) {
            if (peek_next(self)->type != IDENTIFIER) {
                parser_error(self, "Expected: Variable name after ','");
                free(names);
                free(inits);
                return NULL;
            }
            advance(self);
        }
    }

    if (name_count == 0) {
        parser_error(self, "Expected: Variable name");
        free(names);
        // TODO: free each ast_node
        free(inits);
        return NULL;
    }

    names = realloc(names, sizeof(token) * name_count);
    inits = realloc(inits, sizeof(ast_node *) * name_count);

    expect(self, NEWLINE, "Expected: statement in new line");
    ast_node *decl = ast_new_node(AST_VAR_DECL);
    decl->var_decl.names = names;
    decl->var_decl.inits = inits;
    decl->var_decl.name_count = name_count;
    decl->var_decl.d_type = d_type;
    return decl;
}

static void synchronize(parser *self) {
    if (peek_next(self))
        advance(self);
    else
        return;

    while (!is_at_end(self)) {
        token *curr = peek(self);
        switch (curr->type) {
            // case RIGHT_BRACE:
            // case KATAPUSAN:

        case MUGNA:
        case IPAKITA:
        case DAWAT:
        case KUNG:
        case ALANG_SA:
            return;
        default:
            advance(self);
        }
    }
}

static void parser_curr(parser *self) {
    printf("CURR:::::::");
    print_token(peek(self));
}

static ast_node *parse_assignment(parser *self) {
    printf("=============PARSE ASSIGNMENT=============\n");
    token *var_name = advance(self);
    if (!expect(self, EQUAL, "Expected '=' after variable name"))
        return NULL;
    ast_node *expr = NULL;

    if (match(self, IDENTIFIER) && peek_next(self)->type == EQUAL) {
        expr = parse_assignment(self);
        if (!expr) {
            return NULL;
        }
    } else {
        expr = parse_expression(self);
        if (!expr) {
            return NULL;
        }
//        printf("ASSSS:::::::::::::::::::::::::");
//        parser_curr(self);
//        if (peek_next(self)->type == NEWLINE)
//            advance(self);

//        char buf[128];
//        peek_next(self)->type != NEWLINE
//            ? snprintf(buf, 128, "'%s' is not assignable",
//                       peek_next(self)->lexeme)
//            : snprintf(buf, 128, "Expected: Expression after '%s'",
//                       peek(self)->lexeme);
//
//        if (!expect(self, NEWLINE, buf)) {
//            return NULL;
//        }
    }

    parser_curr(self);

    ast_node *assign = ast_new_node(AST_ASSIGNMENT);
    assign->assignment.expr = expr;
    assign->assignment.var = var_name;

    return assign;
}

static ast_node *parse_print_stmt(parser *self) {
    printf("=============PARSE IPAKITA=============\n");
    advance(self);
    if (!expect(self, COLON, "Expected ':' after 'IPAKITA'"))
        return NULL;

    ast_node **exprs = NULL;
    int temp_size = 10;
    exprs = malloc(sizeof(ast_node *) * temp_size);
    int expr_count = 0;

    while (!match(self, NEWLINE)) {
        ast_node *expr = parse_expression(self);
        if (!expr) {
            free(exprs);
            return NULL;
        }

        exprs[expr_count++] = expr;

        if (expr_count == temp_size) {
            exprs = realloc(exprs, sizeof(ast_node *) * (expr_count * 10));
            if (!exprs) {
                fprintf(stderr,
                        "ERROR: Failed to resize exprs [parse_print]\n");
                free(exprs);
                exit(1);
            }
            temp_size += 10;
        }

        // TODO: check if blank ang next sa &
        // eg. IPAKITA: a & b &
        if (match(self, AMPERSAND)) {
            advance(self);
        }
    }

    // printf("PRINT::::::::::::::");
    // parser_curr(self);

    if (peek_prev(self)->type == AMPERSAND) {
        parser_error(self, "Expected expression after '&'");
        free(exprs);
        return NULL;
    }

    if (!expect(self, NEWLINE, "Expected: Statement in new line")) {
        free(exprs);
        return NULL;
    }

    ast_node *print = ast_new_node(AST_PRINT);
    print->print.exprs = exprs;
    print->print.expr_count = expr_count;
    return print;
}

static ast_node *parse_input_stmt(parser *self) {
    advance(self);
    expect(self, COLON, "Expected ':' after 'DAWAT'");

    ast_node *input = ast_new_node(AST_INPUT);
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
    if (!expect(self, PUNDOK, "Expected: 'PUNDOK'"))
        return NULL;
    if (!expect(self, LEFT_BRACE, "Expected: '{' after 'PUNDOK'"))
        return NULL;

    if (!expect(self, NEWLINE, "Expected: Statement in new line [her]"))
        return NULL;

    ast_node *block = ast_new_node(AST_BLOCK);
    int temp_size = 10;
    ast_node **stmts = malloc(sizeof(ast_node *) * temp_size);
    if (!stmts) {
        fprintf(stderr, "Failed to alloc [parse_block]");
        exit(1);
    }
    int stmt_count = 0;

    int line = peek(self)->line;
    bool found = false;

    while (!is_at_end(self)) {
        if (match(self, NEWLINE)) {
            advance(self);
            continue;
        }

        if (match(self, RIGHT_BRACE)) {
            found = true;
            break;
        }

        ast_node *stmt = parse_statement(self);

        if (stmt)
            stmts[stmt_count++] = stmt;

        if (stmt_count == temp_size) {
            stmts = realloc(stmts, sizeof(ast_node *) * (stmt_count + 10));
            temp_size += 10;
        }
    }

    if (!found) {
        // TODO:
        // improve ang error message sa PUNDOK nga error
        // not necessary
        parser_error(self, "Expected: '}' to close 'PUNDOK'");
        return NULL;
    }

    advance(self);
    expect(self, NEWLINE, "Expected: Statement in new line");

    stmts = realloc(stmts, sizeof(ast_node *) * stmt_count);

    if (stmts) {
        block->block.statements = *stmts;
    }
    block->block.stmt_count = stmt_count;
    return block;
}

static ast_node *parse_if_stmt(parser *self) {
    printf("=============PARSE IF STMT=============\n");

    advance(self);

    if (!expect(self, LEFT_PAREN, "Expected '(' after 'KUNG'"))
        return NULL;

    ast_node *condition = parse_expression(self);
    if (!condition) {
        return NULL;
    }

    if (!expect(self, RIGHT_PAREN, "Expected ')' after 'KUNG'"))
        return NULL;

    if (!expect(self, NEWLINE, "Expected 'PUNDOK' in new line"))
        return NULL;

    ast_node *then_block = parse_block(self);
    if (!then_block) {
        return NULL;
    }

    ast_node *if_stmt = ast_new_node(AST_IF);
    if_stmt->if_stmt.condition = condition;
    if_stmt->if_stmt.then_block = then_block;

    //    if (!expect(self, NEWLINE, "Expected: Statement in new line[here]")) {
    //        // TODO: implement free node for this ast_node type
    //        ast_free_node(if_stmt);
    //        return NULL;
    //    }

    if (match(self, KUNG_DILI) || match(self, KUNG_WALA)) {
        ast_node *else_stmt = parse_else_stmt(self);
        if (!else_stmt) {
            return NULL;
        }
        if_stmt->if_stmt.else_block = else_stmt;
    }

    return if_stmt;
}

static ast_node *parse_else_stmt(parser *self) {
    if (match(self, KUNG_DILI)) {
        printf("=============PARSE ELSE IF STMT=============\n");
        advance(self);
        if (!expect(self, LEFT_PAREN, "Expected: '(' after 'KUNG DILI'"))
            return NULL;
        ast_node *condition = parse_expression(self);
        if (!condition) {
            return NULL;
        }
        if (!expect(self, RIGHT_PAREN,
                    "Expected: ')' after 'KUNG DILI' condition")) {
            return NULL;
        }

        if (!expect(self, NEWLINE, "Expected: statement in new line")) {
            return NULL;
        }

        ast_node *then_block = parse_block(self);
        if (!then_block)
            return NULL;

        ast_node *stmt = ast_new_node(AST_ELSE_IF);
        stmt->if_stmt.condition = condition;
        stmt->if_stmt.then_block = then_block;

        if (match(self, KUNG_DILI) || match(self, KUNG_WALA)) {
            stmt->if_stmt.else_block = parse_else_stmt(self);
        }
        return stmt;

    } else if (match(self, KUNG_WALA)) {
        printf("=============PARSE ELSE=============\n");
        advance(self);
        if (!expect(self, NEWLINE, "Expected 'PUNDOK' in new line"))
            return NULL;

        ast_node *block = parse_block(self);
        if (!block) {
            return NULL;
        }

        ast_node *stmt = ast_new_node(AST_ELSE);
        stmt->if_stmt.then_block = block;
        return stmt;
    }
    return NULL;
}

static ast_node *parse_for_stmt(parser *self) {
    advance(self);

    if (!expect(self, LEFT_PAREN, "Expected '(' after ALANG SA"))
        return NULL;

    ast_node *init = NULL;
    if (match(self, IDENTIFIER)) {
        init = parse_assignment(self);
        if (!init) {
            return NULL;
        }
    }
    else {
        parser_error(self, "Expected: assignment");
        return NULL;
    }

    if (!expect(self, COMMA, "Expected ',' after initialization"))
        return NULL;

    if (peek_next(self)->type != EQUAL_EQUAL &&
        peek_next(self)->type != NOT_EQUAL &&
        peek_next(self)->type != GREATER_EQUAL &&
        peek_next(self)->type != LESS_EQUAL &&
        peek_next(self)->type != LESS &&
        peek_next(self)->type != GREATER
    ) {
        parser_error(self, "Invalid condition");
        return NULL;
    }

    ast_node *condition = parse_expression(self);
    if (!condition) {
        return NULL;
    }

    if (!expect(self, COMMA, "Expected ',' after condition"))
        return NULL;

    ast_node *update = parse_assignment(self);
    if (!update) {
        return NULL;
    }

    if (!expect(self, RIGHT_PAREN, "Expected ')' after update"))
        return NULL;

    if (!expect(self, NEWLINE, "Expected 'PUNDOK' in new line"))
        return NULL;

    ast_node *body = parse_block(self);

    ast_node *for_stmt = ast_new_node(AST_FOR);
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

        ast_node *binary = ast_new_node(AST_BINARY);
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
        ast_node *expr = parse_unary(self);
        ast_node *unary = ast_new_node(AST_UNARY);
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

    if (match(self, CHAR) || match(self, NUMBER) || match(self, STRING) || match(self, TRUE) ||
        match(self, FALSE)) {
        ast_node *literal = ast_new_node(AST_LITERAL);
        literal->literal.value = advance(self);
        return literal;
    } else if (match(self, IDENTIFIER)) {
        ast_node *var = ast_new_node(AST_VARIABLE);
        var->variable.name = advance(self);
        return var;
    } else if (match(self, LEFT_PAREN)) {
        advance(self);
        ast_node *expr = parse_expression(self);
        expect(self, RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    } else if (match(self, LEFT_BRACKET)) {
        advance(self);

        while (!match(self, NEWLINE)) {
            if (match(self, RIGHT_BRACKET)) {
                ast_node *res = ast_new_node(AST_LITERAL);
                res->literal.value = peek_prev(self);
                advance(self);

                printf("AFTER:::::::::");
                parser_curr(self);
                return res;
            }
            // parser_curr(self);
            advance(self);
        }
        parser_error(self, "Expected: ']' after '['");
        return NULL;

        //        if (match(self, RIGHT_BRACKET)) {
        //            advance(self);
        //            ast_node *literal = ast_new_node(AST_LITERAL);
        //            literal->literal.value = malloc(sizeof(token));
        //            literal->literal.value->type = STRING;
        //            literal->literal.value->lexeme = "";
        //            literal->literal.value->line = t->line;
        //            return literal;
        //        } else {
        //            ast_node *expr = parse_expression(self);
        //            expect(self, RIGHT_BRACKET, "Expected: ']' after
        //            expression"); return expr;
        //        }
        //        expect(self, RIGHT_BRACKET, "Expected ']' after '#'");
        //        ast_node *literal = ast_new_node(AST_LITERAL);
        //        literal->literal.value = malloc(sizeof(token));
        //        literal->literal.value->type = HASH;
        //        literal->literal.value->lexeme = "#";
        //        literal->literal.value->line = t->line;
        //        return literal;
    } else if (match(self, DOLLAR)) {
        printf("DOLLAR\n");
        advance(self);
        ast_node *literal = ast_new_node(AST_LITERAL);
        literal->literal.value = malloc(sizeof(token));
        literal->literal.value->type = DOLLAR;
        literal->literal.value->lexeme = "$";
        literal->literal.value->line = t->line;
        return literal;
    }

    parser_error(self, "Expected expression");
    return NULL;
}

ast_node *parser_parse(parser *self) {
    ast_node *program = ast_new_node(AST_PROGRAM);
    expect(self, SUGOD, "Expected 'SUGOD' at start of program");
    expect(self, NEWLINE, "Expected statement in new line");

    ast_node *stmts = dynarray_create(ast_node);

    while (!is_at_end(self)) {

        if (match(self, NEWLINE))
            advance(self);

        ast_node *stmt = parse_statement(self);
        if (stmt)
            dynarray_push(stmts, *stmt);
    }

    // TODO:
    // parse ipakita
    // parse dawat
    // parse var_decl types LETRA, TINUOD

    previous(self);
    previous(self);
    expect(self, KATAPUSAN, "Expected 'KATAPUSAN' at the end of program");

    program->program.stmt_count = dynarray_length(stmts);
    program->program.statements = stmts;

    return program;
}
