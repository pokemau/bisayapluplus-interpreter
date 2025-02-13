#include "scanner.h"
#include "token.h"

#include <stdio.h>

char get_char(Scanner *self) {
    return self->source->data[self->current++];
}

void add_token(TokenType type) {

}

struct Scanner scanner_create(scanner_source *source) {

    struct Scanner self;
    self.source = source;
    // TODO:
    // allocate memory for tokens(?)

    self.start = 0;
    self.current = 0;
    self.line = 1;

    return self;
}

void scanner_scan_tokens(Scanner *self) {

    while (self->current <= self->source->len) {
        printf("%c\n", get_char(self));
    }
}

void scanner_scan_token(Scanner *self) {
    char c = get_char(self);

    switch(c) {
        case '(': add_token(LEFT_PAREN);    break;
        case ')': add_token(RIGHT_PAREN);   break;
        case '{': add_token(LEFT_BRACE);    break;
        case '}': add_token(RIGHT_BRACE);   break;
        case ',': add_token(COMMA);         break;
        case '.': add_token(DOT);           break;
        case '-': add_token(MINUS);         break;
        case '+': add_token(PLUS);          break;
        case ';': add_token(SEMICOLON);     break;
        case '*': add_token(STAR);          break;
    }

}

