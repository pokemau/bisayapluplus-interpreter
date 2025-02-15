#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "token.h"

typedef struct scanner_source {
    long len;
    const char *data;
} scanner_source;

typedef struct token_list {
    long size;
    long max;
    Token *list;
} token_list;

typedef struct Scanner {
    scanner_source source;
    token_list tokens;
    int start;
    int current;
    int line;
} Scanner;

Scanner scanner_create(scanner_source *source);

static const char *get_token_substring(Scanner *self, int start, int end);
static void add_token(Scanner *self, TokenType type, void *literal);
void scanner_scan_tokens(Scanner *self);

#endif
