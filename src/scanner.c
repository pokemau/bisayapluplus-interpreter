#include "scanner.h"
#include "token.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char get_char(Scanner *self) {
    return self->source.data[self->current++];
}
bool is_at_end(Scanner *self) {
    return self->current >= self->source.len;
}
bool match(Scanner *self, char expected) {
    if (is_at_end(self)) { return false; }
    if (self->source.data[self->current] != expected) {
        return false;
    }
    self->current++;
    return true;
}

void add_token(Scanner *self, TokenType type) {


    // debug print
    printf("%d ", self->current-1); // char count
    if (self->source.data[self->current-1] == '\n')
        printf("nl\n");
    else if (type == EQUAL_EQUAL) printf("['%s']", "==");
    else if (type == GREATER_EQUAL) printf("['%s']", ">=");
    else if (type == LESS_EQUAL) printf("['%s']", "<=");
    else if (type == NOT_EQUAL) printf("['%s']", "<>");
    else
        printf("['%c']", self->source.data[self->current-1]);
    printf(" token: %d\n", type);
    // end

    self->tokens.list[self->tokens.size++] = type;

    // resize tokens list
    if (self->tokens.size >= self->tokens.max) {

        self->tokens.max *= 1.5;
        self->tokens.list = realloc(self->tokens.list,
                                     sizeof(token_list) * self->tokens.max);

        if (self->tokens.list == NULL) {
            fprintf(stderr, "ERROR: Failed to resize tokens list\n");
            exit(1);
        }
    }
}

void scanner_scan_token(Scanner *self) {
    char c = get_char(self);

    switch(c) {
        case '(': add_token(self, LEFT_PAREN);    break;
        case ')': add_token(self, RIGHT_PAREN);   break;
        case '{': add_token(self, LEFT_BRACE);    break;
        case '}': add_token(self, RIGHT_BRACE);   break;
        case '[': add_token(self, LEFT_BRACKET);  break;
        case ']': add_token(self, RIGHT_BRACKET); break;
        case ',': add_token(self, COMMA);         break;
        case '.': add_token(self, DOT);           break;
        case '-': add_token(self, MINUS);         break;
        case '+': add_token(self, PLUS);          break;
        case ';': add_token(self, SEMICOLON);     break;
        case '*': add_token(self, STAR);          break;
        case '=':
            add_token(self, match(self, '=') ? EQUAL_EQUAL: EQUAL);
            break;
        case '>':
            add_token(self, match(self, '=') ? GREATER_EQUAL : GREATER);
            break;
        case '<':
            if (match(self, '=')) {
                add_token(self, LESS_EQUAL);
            } else if (match(self, '>')) {
                add_token(self, NOT_EQUAL);
            } else {
                add_token(self, LESS);
            }
            break;
        case '\n': self->line++; break;

        // ignore whitespace
        case ' ':
        case '\t':
        case '\r':
            break;

        // EOF
        case '\0': break;

        default:
            bpp_error(self->line, "Unexpected character!");
            break;
    }
}

struct Scanner scanner_create(scanner_source *source) {

    struct Scanner self;
    self.source = *source;

    self.tokens.list = malloc(sizeof(token_list) * INCREASE);
    self.tokens.size = 0;
    self.tokens.max = sizeof(token_list) * INCREASE;

    self.start = 0;
    self.current = 0;
    self.line = 1;

    return self;
}

void scanner_scan_tokens(Scanner *self) {

    while (!is_at_end(self)) {
        scanner_scan_token(self);
    }

    printf("tokens\n");

    for (int i = 0 ; i < self->tokens.size; i++) {

        printf("%d ", self->tokens.list[i]);
    }
    printf("\n");

}


