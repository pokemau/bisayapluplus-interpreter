#include "scanner.h"
#include "token.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool is_digit(const char c) {
    printf("%c:", c);
    return true;
}

bool is_at_end(Scanner *self) {
    return self->current >= self->source.len;
}
char get_next_char(Scanner *self) {
    return self->source.data[self->current++];
}
char peek(Scanner *self) {
    if (is_at_end(self)) { return '\0'; }
    return self->source.data[self->current];
}
bool match(Scanner *self, char expected) {
    if (is_at_end(self)) { return false; }
    if (self->source.data[self->current] != expected) {
        return false;
    }
    self->current++;
    return true;
}

const char *get_token_substring(Scanner *self, int start, int end) {
    int text_len = end - start + 1;
    char *text = malloc(text_len);
    if (text == NULL) {
        bpp_error(self->line, "Failed to allocate mem");
    }
    strncpy(text, self->source.data + start, text_len-1);
    return text;
}

void add_token(Scanner *self, TokenType type, void *literal) {

    // [ DEBUG PRINT ]
//    printf("%d ", self->current-1); // char count
//    else if (type == EQUAL_EQUAL) printf("['%s']", "==");
//    else if (type == GREATER_EQUAL) printf("['%s']", ">=");
//    else if (type == LESS_EQUAL) printf("['%s']", "<=");
//    else if (type == NOT_EQUAL) printf("['%s']", "<>");
//    else
//        printf("['%c']", self->source.data[self->current-1]);
//    printf(" token: %d\n", type);
//
//    printf("%d %d\n", self->start, self->current);
    // [ END DEBUG ]


    if (type == STRING)
        self->start += 1;

    const char *text = get_token_substring(self, self->start, self->current);

    self->tokens.list[self->tokens.size++] = (Token){
        .line = self->line,
        .lexeme = text,
        .type = type,
        .literal = literal != NULL ? literal : NULL
    };

    // resize tokens list
    if (self->tokens.size >= self->tokens.max) {

        self->tokens.max *= 1.5;
        self->tokens.list = realloc(self->tokens.list,
                                     sizeof(token_list) * self->tokens.max);

        if (self->tokens.list == NULL) {
            fprintf(stderr, "ERROR:>Failed to resize tokens list\n");
            exit(1);
        } else {
            printf("Reallocated [TOKENS]\n");
        }
    }
}

void scanner_scan_token(Scanner *self) {
    char c = get_next_char(self);

    switch(c) {
        case '(': add_token(self, LEFT_PAREN, NULL);      break;
        case ')': add_token(self, RIGHT_PAREN, NULL);     break;
        case '{': add_token(self, LEFT_BRACE, NULL);      break;
        case '}': add_token(self, RIGHT_BRACE, NULL);     break;
        case '[': add_token(self, LEFT_BRACKET, NULL);    break;
        case ']': add_token(self, RIGHT_BRACKET, NULL);   break;
        case ',': add_token(self, COMMA, NULL);           break;
        case '.': add_token(self, DOT, NULL);             break;
        case '/': add_token(self, SLASH, NULL);           break;
        case '-':
            // comment
            if (match(self, '-')) {
                while (peek(self) != '\n' && !is_at_end(self))
                    get_next_char(self);
            } else {
                add_token(self, MINUS, NULL);
            }
            break;
        case '+': add_token(self, PLUS, NULL);            break;
        case ';': add_token(self, SEMICOLON, NULL);       break;
        case '*': add_token(self, STAR, NULL);            break;
        case '=':
            add_token(self, match(self, '=') ? EQUAL_EQUAL: EQUAL, NULL);
            break;
        case '>':
            add_token(self, match(self, '=') ? GREATER_EQUAL : GREATER, NULL);
            break;
        case '<':
            if (match(self, '=')) {
                add_token(self, LESS_EQUAL, NULL);
            } else if (match(self, '>')) {
                add_token(self, NOT_EQUAL, NULL);
            } else {
                add_token(self, LESS, NULL);
            }
            break;
        case '\n': self->line++; break;

        case '"':
            while(peek(self) != '"' && !is_at_end(self)) {
                get_next_char(self);
            }

            if (is_at_end(self)) {
                bpp_error(self->line, "Unterminated String");
            }


            const char *text = get_token_substring(self,
                                                   self->start+1,
                                                   self->current);
            add_token(self, STRING, (void*) text);

            get_next_char(self);

            break;


        // ignore whitespace
        case ' ':
        case '\t':
        case '\r':
            break;

        // EOF
        case '\0': break;

        default:
            if (is_digit(c)) {
            }
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
        self->start = self->current;
    }

    printf("tokens\n");

    for (int i = 0 ; i < self->tokens.size; i++) {

        Token *curr = &self->tokens.list[i];
        printf("[%d] %s %d\n", curr->line, curr->lexeme, curr->type);
    }
    printf("\n");
}


