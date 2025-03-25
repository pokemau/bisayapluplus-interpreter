#ifndef _LEXER_H_
#define _LEXER_H_

#include "token.h"
#include "util/arena.h"

#define INITIAL_TOKEN_CAPACITY 16

typedef struct lexer_src {
    long len;
    const char *data;
} lexer_src;

typedef struct token_list {
    long size;
    long max;
    token *list;
} token_list;

typedef struct lexer {
    lexer_src source;
    token_list tokens;
    int start;
    int current;
    int line;
    arena arena;
} lexer;

lexer lexer_create(lexer_src *source);
void lexer_gen_tokens(lexer *self);
void lexer_gen_input_tokens(lexer *self);

#endif
