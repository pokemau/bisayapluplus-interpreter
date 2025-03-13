#ifndef _Parser_H_
#define _Parser_H_

#include "lexer.h"
#include "ast.h"
#include "stmt.h"

typedef struct Parser {
    token_list *tokens;
    int current;
    Token *head;
} Parser;

Parser parser_create(token_list *tokens);


void ast_print(ast_node *node, int depth);

Stmt *parser_parse(Parser *self);

// void parse(Parser *self);

void ast_free(ast_node *node);

// expr *parse_expression(Parser *self);
// expr *parse_equality(Parser *self);


#endif
