#ifndef _LEXER_H_
#define _LEXER_H_

#include "token.h"

typedef struct lexer_src {
    long len;
    const char *data;
} lexer_src;

typedef struct token_list {
    long size;
    long max;
    Token *list;
} token_list;

typedef struct Lexer {
//    lexer_src source;
    const char *source;
    token_list tokens;
    int start;
    int current;
    int line;
} Lexer;

Lexer lexer_create(lexer_src *source);
void lexer_gen_tokens(Lexer *self);

#endif
