#include "lexer.h"
#include "token.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


bool is_alpha(const char c) {
    return isalpha(c) || c == '_';
}

static bool is_at_end(Lexer *self) {
    return self->current >= strlen(self->source);
}

static char advance(Lexer *self) {
    return self->source[self->current++];
}

static char peek(Lexer *self) {
    if (is_at_end(self)) { return '\0'; }
    return self->source[self->current];
}

static char peek_next(Lexer *self) {
    if (self->current + 1 >= strlen(self->source))
        return '\0';
    return self->source[self->current + 1];
}

static bool match(Lexer *self, char expected) {
    if (is_at_end(self)) { return false; }
    if (self->source[self->current] != expected) {
        return false;
    }
    self->current++;
    return true;
}

static const char *get_token_substring(Lexer *self, int start, int end) {
    int text_len = end - start;
    char *text = malloc(text_len);
    if (text == NULL) {
        bpp_error(self->line, "Failed to allocate mem");
    }
    strncpy(text, self->source + start, text_len);

    // remove '\n' from string
    int l = strlen(text);
    if (l > 0 && text[l - 1] == '\n') {
        text[l - 1] = '\0';
    }

    return text;
}

static void add_token(Lexer *self, TokenType type, void *literal) {

    if (type == STRING || type == TRUE || type == FALSE)
        self->start += 1;

    const char *text = get_token_substring(self, self->start, self->current);

    self->tokens.list[self->tokens.size++] = (token){
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

static void scan_string(Lexer *self) {
    // shit code, refactor
    while(peek(self) != '"' && !is_at_end(self)) {
        advance(self);
    }

    if (is_at_end(self)) {
        bpp_error(self->line, "Unterminated String");
    }

    const char *text = get_token_substring(self,
                                           self->start + 1,
                                           self->current);
    // check if string is OO or DILI
    if (strlen(text) == 2 && strcmp("OO", text) == 0) {
        add_token(self, TRUE, NULL);
    } else if (strlen(text) == 4 && strcmp("DILI", text) == 0) {
        add_token(self, FALSE, NULL);
    } else {
        add_token(self, STRING, (void*) text);
    }

    advance(self);
}

static void scan_char(Lexer *self) {
    if (isalnum(peek(self)) && peek_next(self) == '\'') {
        advance(self);
        advance(self);

        const char *text = get_token_substring(self,
                                               self->start + 1,
                                               self->current - 1);
        self->start++;
        self->current--;
        add_token(self, IDENTIFIER, (void*) text);
    }
}

static void scan_comment(Lexer *self) {
    if (match(self, '-')) {
        while (peek(self) != '\n' && !is_at_end(self))
            advance(self);
    } else {
        add_token(self, MINUS, NULL);
    }
}


static void scan_number(Lexer *self) {

    while (isdigit(peek(self)))
        advance(self);

    if (peek(self) == '.' && isdigit(peek_next(self))) {
        advance(self);
        while(isdigit(peek(self)))
            advance(self);
    }

    const char *text = get_token_substring(self, self->start, self->current);
    double *val = malloc(sizeof(double));
    if (val == NULL) {
        bpp_error(self->line, "Failed to allocate memory to double");
    }
    *val = atof(text);
    add_token(self, NUMBER, val);
}

static void scan_identifier(Lexer *self) {
    char c = advance(self);
    while (isalnum(c) || c == '_') {
        c = advance(self);
    }
    const char *text = get_token_substring(self, self->start, self->current-1);

    // check for ALANG SA
    if (strcmp(text, "ALANG") == 0) {
        if (peek(self) == 'S' && peek_next(self) == 'A') {
            advance(self);
            c = advance(self);
            text = get_token_substring(self, self->start, self->current-1);
            add_token(self, ALANG_SA, NULL);
            return;
        }
    }

    // check for KUNG DILI, KUNG WALA
    if (strcmp(text, "KUNG") == 0) {
        const char *kung_text = get_token_substring(self, self->current,
                                                    self->current+4);
        if(strcmp(kung_text, "DILI") == 0) {
            advance(self);
            advance(self);
            advance(self);
            advance(self);
            add_token(self, KUNG_DILI, NULL);
        } else if(strcmp(kung_text, "WALA") == 0) {
            advance(self);
            advance(self);
            advance(self);
            advance(self);
            add_token(self, KUNG_WALA, NULL);
        } else {
            add_token(self, KUNG, NULL);
        }
        return;
    }

    self->current--;
    add_token(self, get_token_type(text), NULL);
}

static void lexer_scan_token(Lexer *self) {
    char c = advance(self);

    switch(c) {
        case ' ':
        case '\t':
        case '\r':
            break;
        case '\n':
           add_token(self, NEWLINE, NULL);
            self->line++;
            break;
        case '\0': add_token(self, EOFILE, NULL);         break;
        case '(': add_token(self, LEFT_PAREN, NULL);      break;
        case ')': add_token(self, RIGHT_PAREN, NULL);     break;
        case '{': add_token(self, LEFT_BRACE, NULL);      break;
        case '}': add_token(self, RIGHT_BRACE, NULL);     break;
        case '[': add_token(self, LEFT_BRACKET, NULL);    break;
        case ']': add_token(self, RIGHT_BRACKET, NULL);   break;
        case ',': add_token(self, COMMA, NULL);           break;
        case '.': add_token(self, DOT, NULL);             break;
        case '/': add_token(self, SLASH, NULL);           break;
        case '&': add_token(self, AMPERSAND, NULL);       break;
        case '$': add_token(self, DOLLAR, NULL);          break;
        case '#': add_token(self, HASH, NULL);            break;
        case '+': 
//            if (match(self, '+')) {
//                add_token(self, INCREMENT, NULL);
//            } else {
                add_token(self, PLUS, NULL);
//            }
            break;
        case ';': add_token(self, SEMICOLON, NULL);       break;
        case ':': add_token(self, COLON, NULL);           break;
        case '*': add_token(self, STAR, NULL);            break;
        case '-': scan_comment(self);                     break;
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

        case '\'': scan_char(self);                       break;
        case '"': scan_string(self);                      break;

        default:
            if (isdigit(c)) {
                scan_number(self);
            } else if (is_alpha(c)) {
                scan_identifier(self);
            } else {
                printf("{ %c }\n", c);
                bpp_error(self->line, "Unexpected character!");
            }
            break;
    }
}

Lexer lexer_create(struct lexer_src *source) {

    Lexer self;
    self.source = source->data;
    self.tokens.list = malloc(sizeof(token_list) * INCREASE);

    self.tokens.size = 0;
    self.tokens.max = sizeof(token_list) * INCREASE;

    self.start = 0;
    self.current = 0;
    self.line = 1;

    return self;
}

static void print_tokens(Lexer *self) {
    printf("==========================\n");
    printf("========= TOKENS =========\n");
    printf("==========================\n");

    for (int i = 0 ; i < self->tokens.size; i++) {
        token *curr = &self->tokens.list[i];
        print_token(curr);
    }
    printf("\n");
}

void lexer_gen_tokens(Lexer *self) {
    while (!is_at_end(self)) {
        self->start = self->current;
        lexer_scan_token(self);
    }

    add_token(self, EOFILE, NULL);

//    print_tokens(self);
}


