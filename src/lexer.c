#include "lexer.h"
#include "token.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


void lexer_error(int line, const char *msg) {
    fprintf(stderr, "Line [%d]: %s\n", line, msg);
    exit(1);
}
bool is_alpha(const char c) {
    return isalpha(c) || c == '_';
}

static bool is_at_end(lexer *self) {
    return self->current >= self->source.len;
}

static char advance(lexer *self) {
    return self->source.data[self->current++];
}

static char peek(lexer *self) {
    if (is_at_end(self)) { return '\0'; }
    return self->source.data[self->current];
}

static char peek_next(lexer *self) {
    if (self->current + 1 >= self->source.len)
        return '\0';
    return self->source.data[self->current + 1];
}

static bool match(lexer *self, char expected) {
    if (is_at_end(self)) { return false; }
    if (self->source.data[self->current] != expected) {
        return false;
    }
    self->current++;
    return true;
}

static const char *get_token_substring(lexer *self, int start, int end) {
    int text_len = end - start;
    char *text = malloc(text_len);
    if (text == NULL) {
        lexer_error(self->line, "Failed to allocate mem");
    }
    strncpy(text, self->source.data + start, text_len);

    // remove '\n' from string
    int l = strlen(text);
    if (l > 0 && text[l - 1] == '\n') {
        text[l - 1] = '\0';
    }

    return text;
}

static void add_token(lexer *self, TokenType type, void *literal) {

    if (type == STRING || type == TRUE || type == FALSE || type == CHAR)
        self->start++;

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

static void scan_string(lexer *self) {
    // shit code, refactor
    while(peek(self) != '"' && !is_at_end(self)) {
        advance(self);
    }

    if (is_at_end(self)) {
        lexer_error(self->line, "Unterminated String");
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

static void scan_char(lexer *self) {
    if (isalnum(peek(self)) && (peek_next(self) == '\'')) {
        advance(self);
        advance(self);
        const char *text = get_token_substring(self,
                                               self->start+1,
                                               self->current-1);
        self->current--;
        add_token(self, CHAR, (void*)text);
        advance(self);
        return;
    }
    lexer_error(self->line, "Unterminated or Invalid Char!");
}

static void scan_comment(lexer *self) {
    if (match(self, '-')) {
        while (peek(self) != '\n' && !is_at_end(self))
            advance(self);
    } else {
        add_token(self, MINUS, NULL);
    }
}


static void scan_number(lexer *self) {

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
        lexer_error(self->line, "Failed to allocate memory to double");
    }
    *val = atof(text);
    add_token(self, NUMBER, val);
}

static void scan_identifier(lexer *self) {
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

static token *previous(lexer *self) {
    if (self->tokens.size == 0)
        return NULL;
    return &self->tokens.list[self->tokens.size-1];
}

static void lexer_scan_token(lexer *self) {
    char c = advance(self);

    switch(c) {
        case ' ':
        case '\t':
        case '\r':
            break;
        case '\n':
            if (previous(self) && previous(self)->type != NEWLINE)
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
        case '%': add_token(self, MODULO, NULL);          break;
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
                lexer_error(self->line, "Unexpected character!");
            }
            break;
    }
}

lexer lexer_create(struct lexer_src *source) {

    lexer self;
    self.source = *source;
    self.tokens.list = malloc(sizeof(token_list) * INCREASE);

    self.tokens.size = 0;
    self.tokens.max = sizeof(token_list) * INCREASE;

    self.start = 0;
    self.current = 0;
    self.line = 1;

    return self;
}

static void print_tokens(lexer *self) {
    printf("==========================\n");
    printf("========= TOKENS =========\n");
    printf("==========================\n");

    for (int i = 0 ; i < self->tokens.size; i++) {
        token *curr = &self->tokens.list[i];
        print_token(curr);
    }
    printf("\n");
}

void lexer_gen_tokens(lexer *self) {
    while (!is_at_end(self)) {
        lexer_scan_token(self);
        self->start = self->current;
    }

    print_tokens(self);
}
