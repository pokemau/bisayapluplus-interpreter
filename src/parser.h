#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "ast.h"
#include "util/arena.h"
#include "util/error.h"

typedef struct parser {
    token_list *tokens;
    int current;
    int token_count;
    token *head;
    arena *arena;
    error_list *error_list;
} parser;

parser parser_create(token_list *tokens, arena *arena);


void ast_print(ast_node *node, int depth);
ast_node **sub_parser_parse(parser* self, size_t expression_count, token* variables);
ast_node *parser_parse(parser *self);

// void parse(parser *self);

void ast_free(ast_node *node);

// expr *parse_expression(parser *self);
// expr *parse_equality(parser *self);


#endif
