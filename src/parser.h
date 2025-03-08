#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "ast.h"

typedef struct parser {
    token_list *tokens;
    int current;
    int token_count;
    token *head;
} parser;

parser parser_create(token_list *tokens);


void ast_print(ast_node *node, int depth);
ast_node *parser_parse(parser *self);

// void parse(parser *self);

void ast_free(ast_node *node);

// expr *parse_expression(parser *self);
// expr *parse_equality(parser *self);


#endif
