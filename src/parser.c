#include "parser.h"
#include "ast.h"
#include "token.h"
#include "util/arena.h"
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
static ast_node *parse_binary(parser *self , int precedence);
static ast_node *parse_unary(parser *self);
static ast_node *parse_primary(parser *self);

static int get_precedence(TokenType type);

static bool is_at_end(parser *self);
static token *peek(parser *self);

static void synchronize(parser *self);
static void parser_curr(parser *self);

static ast_node *parse_switch_stmt(parser *self);
static ast_node *parse_case_stmt(parser *self);

parser parser_create(token_list *tokens, arena *arena) {
    parser self;
    self.tokens = tokens;
    self.arena = arena;
    self.current = 0;
    self.head = &tokens->list[0];
    self.token_count = tokens->size;
    self.error_list = create_error_list(arena);
    return self;
}

static void parser_error(parser *self, const char *message) {
    token *current = &self->tokens->list[self->current];
    add_error(self->error_list, SYNTAX_ERROR, current->line, message);
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
    } else if (match(self, PULI)) { // Add PULI token check for switch statements
        res = parse_switch_stmt(self);
    } else if (match(self, KATAPUSAN)) {
        while (!is_at_end(self))
            advance(self);
        return NULL;
    } else {
        char* buf = arena_alloc(self->arena, sizeof(char) * 128);
        snprintf(buf, 128, "Unexpected token '%s' in statement",
                 peek(self)->lexeme);
        parser_error(self, buf);
    }

    if (!res) {
        synchronize(self);
        return NULL;
    }
    return res;
}

static ast_node *parse_var_decl(parser *self) {
    advance(self);
    token* datatype = advance(self);
    const char* decl_datatype = datatype->lexeme;

    if (strcmp(decl_datatype, "NUMERO") != 0 && strcmp(decl_datatype, "LETRA") != 0 && 
        strcmp(decl_datatype, "TINUOD") != 0 && strcmp(decl_datatype, "TIPIK") != 0) {
        parser_error(self, "(MUGNA) Expected data type after 'MUGNA' during variable declaration");
        return NULL;
    }
    TokenType d_type = datatype->type;

    int temp_size = 10;
    token *names = arena_alloc(self->arena, sizeof(token) * temp_size);
    ast_node **inits = arena_alloc(self->arena, sizeof(ast_node *) * temp_size);
    int name_count = 0;

    while (!match(self, NEWLINE)) {
        token *name = advance(self);
        if (!name || name->type != IDENTIFIER) {
            char* buf = arena_alloc(self->arena, sizeof(char) * 128);
            sprintf(buf, "(MUGNA) Expected variable name after data type during variable declaration but instead got [%s]", 
                get_string_from_token_type(name->type));
            parser_error(self, buf);
            return NULL;
        }

        names[name_count] = *name;

        ast_node *init = NULL;
        if (match(self, EQUAL)) {
            advance(self);
            init = parse_expression(self);
            if (!init) {
                parser_error(self, "(MUGNA) Expected expression after '='");
                return NULL;
            }
        }

        inits[name_count] = init;
        name_count++;

        if (name_count == temp_size) {
            temp_size += 10;
            token *new_names =
                arena_alloc(self->arena, sizeof(token) * temp_size);
            ast_node **new_inits =
                arena_alloc(self->arena, sizeof(ast_node *) * temp_size);
            memcpy(new_names, names, name_count * sizeof(token));
            memcpy(new_inits, inits, name_count * sizeof(ast_node *));
            names = new_names;
            inits = new_inits;
        }

        if (match(self, COMMA)) {
            if (peek_next(self)->type != IDENTIFIER) {
                parser_error(self, "(MUGNA) Expected a variable name after ','");
                return NULL;
            }
            advance(self);
        }
    }

    if (name_count == 0) {
        parser_error(self, "(MUGNA) Expected: Variable name");
        return NULL;
    }

    expect(self, NEWLINE, "(MUGNA) Expected: statement in new line");
    ast_node *decl = ast_new_node(self->arena, AST_VAR_DECL);
    decl->var_decl.names = names;
    decl->var_decl.inits = inits;
    decl->var_decl.name_count = name_count;
    decl->var_decl.d_type = d_type;
    return decl;
}

static void synchronize(parser *self) {
    if (peek(self)->type == NEWLINE) {
        advance(self);
        return;
    }

    if (peek_next(self))
        advance(self);
    else
        return;

    while (!is_at_end(self) && peek_prev(self)->type != NEWLINE) {
        token *curr = peek(self);
        switch (curr->type) {
        case MUGNA:
        case IPAKITA:
        case DAWAT:
        case KUNG:
        case ALANG_SA:
        case PULI:
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
    token *var_name = advance(self);

    ast_node *expr = NULL;

    if (peek(self)->type == PLUS || peek(self)->type == MINUS) {
        token *signToken = advance(self);
        ast_node *left = ast_new_node(self->arena, AST_VARIABLE);
        left->variable.name = var_name;
        ast_node *right;

        if (match(self, EQUAL)) {
            advance(self);
            right = parse_expression(self);
            if (!right) {
                char* buf = arena_alloc(self->arena, sizeof(char) * 128);
                sprintf(buf, "(Var Assign) Expected expression after '%s %s='", var_name->lexeme, signToken->lexeme);
                parser_error(self, buf);
                return NULL;
            }
        }
        else if (match(self, PLUS) || match(self, MINUS)) {
            if (!(signToken->type == PLUS && match(self, PLUS)) &&
                !(signToken->type == MINUS && match(self, MINUS))) {
                    char* buf = arena_alloc(self->arena, sizeof(char) * 128);
                    sprintf(buf, "(Var Assign) Unexpected '-+' or '+-' after '%s'", var_name->lexeme);
                    parser_error(self, buf);
                    return NULL;
            }
            right = ast_new_node(self->arena, AST_LITERAL);

            token *literal_token = arena_alloc(self->arena, sizeof(token));
            literal_token->line = self->head->line;
            literal_token->lexeme = "1";
            literal_token->type = NUMBER;
            literal_token->literal = arena_alloc(self->arena, sizeof(double));
            *(double *)literal_token->literal = 1.0;

            right->literal.value = literal_token;
            advance(self);
        } else {
            char* buf = arena_alloc(self->arena, sizeof(char) * 128);
            sprintf(buf, "(Var Assign) Incomplete operation after '%s %s'", var_name->lexeme, signToken->lexeme);
            parser_error(self, buf);
            return NULL;
        }

        ast_node *binary = ast_new_node(self->arena, AST_BINARY);
        binary->binary.op = signToken;
        binary->binary.left = left;
        binary->binary.right = right;
        expr = binary;
    } else {
        token* current_token_before_expect = peek(self);
        if (current_token_before_expect != NULL) {
            fprintf(stderr, "DEBUG: parse_assignment for var '%s' (line %d), expecting '=', saw token type %s ('%s') at line %d\n",
                    var_name->lexeme, var_name->line,
                    get_string_from_token_type(current_token_before_expect->type),
                    current_token_before_expect->lexeme,
                    current_token_before_expect->line);
        } else {
            fprintf(stderr, "DEBUG: parse_assignment for var '%s' (line %d), expecting '=', saw NULL token\n",
                    var_name->lexeme, var_name->line);
        }

        if (expect(self, EQUAL, "(Var Assign) Expected '=' after variable name")) {
            if (peek(self)->type == IDENTIFIER) {
                if (peek_next(self)->type == EQUAL) {
                    expr = parse_assignment(self);
                } else {
                    expr = parse_expression(self);
                }
            } else {
                expr = parse_expression(self);
            }
        } else {
            return NULL;
        }
    }

    if (!expr) {
        return NULL;
    }

    ast_node *assign = ast_new_node(self->arena, AST_ASSIGNMENT);
    assign->assignment.expr = expr;
    assign->assignment.var = var_name;

    return assign;
}

static ast_node *parse_print_stmt(parser *self) {
    advance(self);
    if (!expect(self, COLON, "(IPAKITA) Expected ':' after 'IPAKITA'"))
        return NULL;

    int temp_size = 10;
    ast_node **exprs = arena_alloc(self->arena, sizeof(ast_node *) * temp_size);
    int expr_count = 0;

    while (!match(self, NEWLINE)) {
        ast_node *expr = parse_expression(self);
        if (!expr) {
            return NULL;
        }

        exprs[expr_count++] = expr;

        if (expr_count >= temp_size) {
            temp_size += 10;
            ast_node **new_exprs =
                arena_alloc(self->arena, sizeof(ast_node *) * temp_size);
            memcpy(new_exprs, exprs, sizeof(ast_node *) * expr_count);
            exprs = new_exprs;
        }

        if (match(self, AMPERSAND)) {
            advance(self);
        } else if (match(self, NEWLINE)){}
        else {
            parser_error(self, "(IPAKITA) Expected '&' or new line after expression");
            while(!is_at_end(self) && peek(self)->type != NEWLINE) {
                advance(self);
            }
        }
    }

    if (peek_prev(self)->type == AMPERSAND) {
        parser_error(self, "(IPAKITA) Expected expression after '&'");
        return NULL;
    }

    if (!expect(self, NEWLINE, "(IPAKITA) Expected: Statement in new line")) {
        advance(self);
        return NULL;
    }

    ast_node *print = ast_new_node(self->arena, AST_PRINT);
    print->print.exprs = exprs;
    print->print.expr_count = expr_count;
    return print;
}

static ast_node *parse_input_stmt(parser *self) {
    advance(self);
    expect(self, COLON, "(DAWAT) Expected ':' after 'DAWAT'");

    int temp_size = 10;
    token *vars = arena_alloc(self->arena, sizeof(token) * temp_size);
    if (!vars) {
        return NULL;
    }
    int var_count = 0;

    while (true) {
        token *var = expect(self, IDENTIFIER, "(DAWAT) Expected variable name");
        vars[var_count++] = *var;
        if (var_count >= temp_size) {
            temp_size += 10;
            token *new_vars =
                arena_alloc(self->arena, sizeof(token) * temp_size);
            memcpy(new_vars, vars, sizeof(token) * var_count);
            vars = new_vars;
        }
        if (match(self, NEWLINE))
            break;
        expect(self, COMMA, "(DAWAT) Expected comma");
    }

    ast_node *input = ast_new_node(self->arena, AST_INPUT);
    input->input.vars = vars;
    input->input.var_count = var_count;
    return input;
}

static ast_node *parse_block(parser *self) {
    if (!expect(self, PUNDOK, "Expected: 'PUNDOK'"))
        return NULL;
    if (!expect(self, LEFT_BRACE, "Expected: '{' after 'PUNDOK'"))
        return NULL;

    while (match(self, NEWLINE)) {
        advance(self);
    }

    int temp_size = 10;
    ast_node **stmts = arena_alloc(self->arena,
                                   sizeof(ast_node *) * temp_size);
    int stmt_count = 0;

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

        if (stmt_count >= temp_size) {
            temp_size += 10;
            ast_node **new_stmts = arena_alloc(self->arena,
                                               sizeof(ast_node *) * temp_size);
            memcpy(new_stmts, stmts, sizeof(ast_node *) * stmt_count);
            stmts = new_stmts;
        }
    }


    if (!found) {
        parser_error(self, "(PUNDOK) Expected: '}' to close 'PUNDOK'");
        return NULL;
    }

    advance(self);
    while (match(self, NEWLINE)) {
        advance(self);
    }

    ast_node *block = ast_new_node(self->arena, AST_BLOCK);
    if (stmts) {
        block->block.statements = arena_alloc(self->arena, sizeof(ast_node *) * stmt_count);
        for (int i = 0; i < stmt_count; i++) {
            block->block.statements[i] = stmts[i];
        }
    }
    block->block.stmt_count = stmt_count;
    return block;
}

static ast_node *parse_if_stmt(parser *self) {
    advance(self);

    if (!expect(self, LEFT_PAREN, "Expected '(' after 'KUNG'"))
        return NULL;

    ast_node *condition = parse_expression(self);
    if (!condition) {
        return NULL;
    }

    if (!expect(self, RIGHT_PAREN, "Expected ')' after 'KUNG'"))
        return NULL;

    while (match(self, NEWLINE)) {
        advance(self);
    }

    ast_node *then_block = parse_block(self);
    if (!then_block) {
        return NULL;
    }

    ast_node *if_stmt = ast_new_node(self->arena, AST_IF);
    if_stmt->if_stmt.condition = condition;
    if_stmt->if_stmt.then_block = then_block;

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

        while (match(self, NEWLINE)) {
            advance(self);
        }

        ast_node *then_block = parse_block(self);
        if (!then_block)
            return NULL;

        ast_node *stmt = ast_new_node(self->arena, AST_ELSE_IF);
        stmt->if_stmt.condition = condition;
        stmt->if_stmt.then_block = then_block;

        if (match(self, KUNG_DILI) || match(self, KUNG_WALA)) {
            stmt->if_stmt.else_block = parse_else_stmt(self);
        }
        return stmt;

    } else if (match(self, KUNG_WALA)) {
        advance(self);
        while (match(self, NEWLINE)) {
            advance(self);
        }

        ast_node *block = parse_block(self);
        if (!block) {
            return NULL;
        }

        ast_node *stmt = ast_new_node(self->arena, AST_ELSE);
        stmt->if_stmt.then_block = block;
        return stmt;
    }
    return NULL;
}

static ast_node *parse_for_stmt(parser *self) {
    advance(self);

    if (!expect(self, LEFT_PAREN, "(ALANG SA) Expected '(' after ALANG SA"))
        return NULL;

    ast_node *init = NULL;
    if (match(self, IDENTIFIER)) {
        init = parse_assignment(self);
        if (!init) {
            return NULL;
        }
    } else {
        parser_error(self, "(ALANG SA) Expected: assignment");
        return NULL;
    }

    if (!expect(self, COMMA, "(ALANG SA) Expected ',' after initialization"))
        return NULL;

    if (peek_next(self)->type != EQUAL_EQUAL &&
        peek_next(self)->type != NOT_EQUAL &&
        peek_next(self)->type != GREATER_EQUAL &&
        peek_next(self)->type != LESS_EQUAL && peek_next(self)->type != LESS &&
        peek_next(self)->type != GREATER) {
        parser_error(self, "(ALANG SA) Invalid condition");
        return NULL;
    }

    ast_node *condition = parse_expression(self);
    if (!condition) {
        return NULL;
    }

    if (!expect(self, COMMA, "(ALANG SA) Expected ',' after condition"))
        return NULL;

    ast_node *update = parse_assignment(self);
    if (!update) {
        return NULL;
    }

    if (!expect(self, RIGHT_PAREN, "(ALANG SA) Expected ')' after update"))
        return NULL;

    while (match(self, NEWLINE)) {
        advance(self);
    }

    ast_node *body = parse_block(self);

    ast_node *for_stmt = ast_new_node(self->arena, AST_FOR);
    for_stmt->for_stmt.init = init;
    for_stmt->for_stmt.condition = condition;
    for_stmt->for_stmt.update = update;
    for_stmt->for_stmt.body = body;
    return for_stmt;
}

static ast_node *parse_switch_stmt(parser *self) {
    advance(self);

    if (!expect(self, LEFT_PAREN, "Expected '(' after PULI")) return NULL;

    ast_node *expression = parse_expression(self);
    if (!expression) return NULL;

    if (!expect(self, RIGHT_PAREN, "Expected ')' after switch expression (VAR)")) return NULL;
    if (!expect(self, NEWLINE, "Expected newline after PULI (VAR)")) return NULL;

    int temp_case_capacity = 5;
    ast_node **cases = arena_alloc(self->arena, sizeof(ast_node *) * temp_case_capacity);
    int case_count = 0;

    while (match(self, KASO)) {
        if (case_count >= temp_case_capacity) {
            temp_case_capacity *= 2;
            ast_node **new_cases = arena_alloc(self->arena, sizeof(ast_node *) * temp_case_capacity);
            memcpy(new_cases, cases, sizeof(ast_node *) * case_count);
            cases = new_cases;
        }
        ast_node *case_node = parse_case_stmt(self);
        if (!case_node) {
            return NULL;
        }
        cases[case_count++] = case_node;
    }

    if (case_count == 0) {
        parser_error(self, "Switch statement (PULI) must have at least one KASO block.");
        return NULL;
    }

    ast_node *switch_node = ast_new_node(self->arena, AST_SWITCH);
    switch_node->switch_stmt.expression = expression;
    switch_node->switch_stmt.cases = cases;
    switch_node->switch_stmt.case_count = case_count;

    return switch_node;
}

static ast_node *parse_case_stmt(parser *self) {
    advance(self);

    if (!expect(self, LEFT_BRACE, "Expected '{' after KASO")) return NULL;

    ast_node *value = parse_expression(self);
    if (!value) return NULL;

    if (!expect(self, RIGHT_BRACE, "Expected '}' after KASO value")) return NULL;
    if (!expect(self, COLON, "Expected ':' after KASO {value}")) return NULL;
    if (!expect(self, NEWLINE, "Expected newline after KASO {value}:")) return NULL;

    ast_node *block = parse_block(self);
    if (!block) return NULL;

    ast_node *case_node = ast_new_node(self->arena, AST_CASE);
    case_node->case_stmt.value = value;
    case_node->case_stmt.block = block;

    return case_node;
}

static ast_node *parse_expression(parser *self) {
    return parse_binary(self, 0);
}

static ast_node *parse_binary(parser *self, int precedence) {
    ast_node *left = parse_unary(self);
    if (!left)
        return NULL;

    while (peek(self)) {
        token *op = peek(self);
        int op_precedence = get_precedence(op->type);
        if (op_precedence < precedence)
            break;

        advance(self);
        ast_node *right = parse_binary(self, op_precedence + 1);

        ast_node *binary = ast_new_node(self->arena, AST_BINARY);
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
        ast_node *unary = ast_new_node(self->arena, AST_UNARY);
        unary->unary.op = op;
        unary->unary.expr = expr;
        return unary;
    }
    return parse_primary(self);
}

static ast_node *parse_primary(parser *self) {
    token *t = peek(self);
    if (!t)
        parser_error(self, "(Expression) Unexpected end of input");

    if (match(self, CHAR) || match(self, NUMBER) || match(self, STRING) ||
        match(self, TRUE) || match(self, FALSE)) {
        ast_node *literal = ast_new_node(self->arena, AST_LITERAL);
        literal->literal.value = advance(self);
        return literal;
    } else if (match(self, IDENTIFIER)) {
        ast_node *var = ast_new_node(self->arena, AST_VARIABLE);
        var->variable.name = advance(self);
        return var;
    } else if (match(self, LEFT_PAREN)) {
        advance(self);
        ast_node *expr = parse_expression(self);
        expect(self, RIGHT_PAREN, "(Expression) Expected ')' after expression");
        return expr;
    } else if (match(self, LEFT_BRACKET)) {
        advance(self);

        while (!match(self, NEWLINE)) {
            if (match(self, RIGHT_BRACKET) && 
                (peek_next(self)->type == NEWLINE || peek_next(self)->type == AMPERSAND) || peek_next(self)->type == EOFILE) {
                ast_node *res = ast_new_node(self->arena, AST_LITERAL);
                res->literal.value = peek_prev(self);
                advance(self);
                return res;
            }
            advance(self);
        }
        parser_error(self, "(Expression) Expected: ']' after '['");
        return NULL;
    } else if (match(self, DOLLAR)) {
        advance(self);
        ast_node *literal = ast_new_node(self->arena, AST_LITERAL);
        literal->literal.value = malloc(sizeof(token));
        literal->literal.value->type = DOLLAR;
        literal->literal.value->lexeme = "$";
        literal->literal.value->line = t->line;
        return literal;
    }

    return NULL;
}

ast_node **sub_parser_parse(parser* self, size_t expression_count, token* variables) {
    ast_node **exprs = arena_alloc(self->arena, sizeof(ast_node*) * expression_count);
    int i = 0;
    while (i < expression_count) {
        if (match(self, COMMA)) {
            advance(self);
            continue;
        }
        ast_node *expression = parse_expression(self);
        exprs[i] = expression;
        i++;
    }
    return exprs;
}

ast_node *parser_parse(parser *self) {
    ast_node *program = ast_new_node(self->arena, AST_PROGRAM);
    expect(self, SUGOD, "Expected 'SUGOD' at start of program");
    expect(self, NEWLINE, "Expected statement in new line");

    int temp_size = 10;
    ast_node *stmts = arena_alloc(self->arena,
                                  sizeof(ast_node) * temp_size);
    int stmt_count = 0;

    while (!is_at_end(self)) {

        if (match(self, NEWLINE)) {
            advance(self);
            continue;
        }

        ast_node *stmt = parse_statement(self);
        if (stmt)
            stmts[stmt_count++] = *stmt;
        if (stmt_count >= temp_size) {
            temp_size += 10;
            ast_node *n = arena_alloc(self->arena,
                                      sizeof(ast_node) * temp_size);
            memcpy(n, stmts, sizeof(ast_node) * stmt_count);
            stmts = n;
        }
    }

    previous(self);
    if (!match(self, KATAPUSAN)) {
        previous(self);
        expect(self, KATAPUSAN, "Expected 'KATAPUSAN' at the end of program");
    }

    program->program.stmt_count = stmt_count;
    program->program.statements = stmts;

    return program;
}