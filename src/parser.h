#ifndef _PARSER_H_
#define _PARSER_H_


#include "lexer.h"

typedef struct Parser {
    token_list *tokens;
    int curr;
} Parser;

struct Parser parser_create(token_list *tokens) {
    Parser self;
    self.tokens = tokens;
    self.curr = 0;
    return self;
}

#endif
