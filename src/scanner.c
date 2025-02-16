#include "scanner.h"
#include "token.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


bool is_alpha(const char c) {

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

char peek_next(Scanner *self) {
    if (self->current +1 >= self->source.len)
        return '\0';
    return self->source.data[self->current+1];
}

bool match(Scanner *self, char expected) {
    if (is_at_end(self)) { return false; }
    if (self->source.data[self->current] != expected) {
        return false;
    }
    self->current++;
    return true;
}

void get_number(Scanner *self) {

    while (isdigit(peek(self)))
        get_next_char(self);


    if (peek(self) == '.' && isdigit(peek_next(self))) {
        get_next_char(self);
        while(isdigit(peek(self)))
            get_next_char(self);
    }

    const char *text = get_token_substring(self, self->start, self->current);
    double *val = malloc(sizeof(double));
    if (val == NULL) {
        bpp_error(self->line, "Failed to allocate memory to double");
    }

    *val = atof(text);

    add_token(self, NUMBER, val);
}

void is_identifier(Scanner *self) {
    char c = get_next_char(self);
    while (isalnum(c) || c == '_') {
        c = get_next_char(self);
    }

    const char *text = get_token_substring(self, self->start, self->current-1);

    // check for ALANG SA
    if (strcmp(text, "ALANG") == 0) {
        if (peek(self) == 'S' && peek_next(self) == 'A') {
            get_next_char(self);
            c = get_next_char(self);
            text = get_token_substring(self, self->start, self->current-1);
            add_token(self, ALANG_SA, NULL);
            if (c == '\n') self->line++;
            return;
        }
    }

    add_token(self, get_token_type(text), NULL);
    if (c == '\n') self->line++;
}

static const char *get_token_substring(Scanner *self, int start, int end) {
    int text_len = end - start + 1;
    char *text = malloc(text_len);
    if (text == NULL) {
        bpp_error(self->line, "Failed to allocate mem");
    }
    strncpy(text, self->source.data + start, text_len-1);

    // remove '\n' from string
    int l = strlen(text);
    if (l > 0 && text[l-1] == '\n') {
        text[l-1] = '\0';
    }

    return text;
}

static void add_token(Scanner *self, TokenType type, void *literal) {

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


    if (type == STRING || type == TRUE || type == FALSE)
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

            // check if string is OO or DILI
            if (strlen(text) == 2 && strcmp("OO", text) == 0) {
                add_token(self, TRUE, NULL);
            } else if (strlen(text) == 4 && strcmp("DILI", text) == 0) {
                add_token(self, FALSE, NULL);
            } else {
                add_token(self, STRING, (void*) text);
            }

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
            if (isdigit(c)) {
                get_number(self);
            } else if (c == '_' || is_alpha(c)) {
                is_identifier(self);
            } else {
                bpp_error(self->line, "Unexpected character! %d");
            }

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

    printf("==========================\n");
    printf("==========TOKENS==========\n");
    printf("==========================\n");
    for (int i = 0 ; i < self->tokens.size; i++) {
        Token *curr = &self->tokens.list[i];
        print_token(curr);
    }
    printf("\n");
}


