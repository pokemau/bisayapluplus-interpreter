#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "token.h"

typedef struct scanner_source {
    long len;
    const char *data;
} scanner_source;

typedef struct Scanner {
    scanner_source *source;
    enum TokenType *tokens;
    int start;
    int current;
    int line;
} Scanner;

Scanner scanner_create(scanner_source *source);

void scanner_scan_tokens(Scanner *self);

#endif
