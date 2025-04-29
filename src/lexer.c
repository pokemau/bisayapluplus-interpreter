#include "lexer.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


static void lexer_error(lexer *self, const char *msg) {
    // fprintf(stderr, "Lexical Error at Line [%d]: %s\n", line, msg);
    add_error(self->error_list, LEXICAL_ERROR, self->line, msg);
}
static bool is_alpha(const char c) {
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

static char *get_token_substring(lexer *self, int start, int end) {
    int text_len = end - start;
    char *text = arena_alloc(&self->arena, text_len + 1);
    strncpy(text, self->source.data + start, text_len);
    text[text_len] = '\0';

    // remove '\n' from string
    int l = strlen(text);
    if (l > 0 && text[l - 1] == '\n') {
        text[l - 1] = '\0';
    }

    return text;
}

static void add_token(lexer *self, TokenType type, void *literal) {
    if (self->tokens.size >= self->tokens.max) {
        size_t new_max = self->tokens.max * 2;
        token *new_list = arena_alloc(&self->arena,
                                      sizeof(token) * new_max);
        memcpy(new_list, self->tokens.list,
               self->tokens.size * sizeof(token));
        self->tokens.list = new_list;
        self->tokens.max = new_max;
        /*printf("Reallocated tokens");*/
    }

    if (type == STRING || type == TRUE || type == FALSE || type == CHAR)
        self->start++;

    const char *text = get_token_substring(self, self->start, self->current);

    self->tokens.list[self->tokens.size++] = (token){
        .line = self->line,
        .lexeme = text,
        .type = type,
        .literal = literal
    };

}

static void scan_string(lexer *self) {
    while(peek(self) != '"' && !is_at_end(self) && peek_next(self) != '\n') {
        advance(self);
    }

    if (peek_next(self) == '\n' || is_at_end(self)) {
        lexer_error(self, "Unterminated String!");
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
    if (isalnum(peek(self)) && (peek_next(self) == '\'') && peek_next(self) != '\n') {
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
    lexer_error(self, "Unterminated or Invalid Char!");
}

static void scan_input_string(lexer *self) {
    while (peek(self) != ',' && !is_at_end(self)){
        advance(self);
    }
    char *text = get_token_substring(self, self->start, self->current);

    // remove trailing spaces
    int i = strlen(text) - 1;
    while (i > 0) {
        if (text[i] == ' ' || text[i] == '\n' || text[i] == '\t')
            i--;
        else
            break;
    }
    text[i+1] = '\0';

    // might remove later if the feature of user input TINUOD is not a thing
    self->start--;
    printf("Text: %s\n", text);
    if (strcmp(text, "OO") == 0) {
        add_token(self, TRUE, NULL);
    } else if (strcmp(text, "DILI") == 0) {
        add_token(self, FALSE, NULL);
    } else if (strlen(text) > 1){
        lexer_error(self, "User input for LETRA variable was given a string!");
    } else{
        add_token(self, CHAR, (void*)text);
    }
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
    double *val = arena_alloc(&self->arena, sizeof(double));
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
        case '+': add_token(self, PLUS, NULL);            break;
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
                char buf[128];
                sprintf(buf, "Unknown character '%c'!" , c);
                lexer_error(self, buf);
            }
            break;
    }
}

lexer lexer_create(struct lexer_src *source) {
    lexer self;
    self.source = *source;
    self.arena = arena_create(INITIAL_ARENA_SIZE);
    self.error_list = create_error_list(&self.arena);
    self.tokens.size = 0;
    self.tokens.max = INITIAL_TOKEN_CAPACITY;
    self.tokens.list = arena_alloc(&self.arena,
                                   sizeof(token) * self.tokens.max);

    self.start = 0;
    self.current = 0;
    self.line = 1;

    initialize_token_hashmap(&(self.arena));

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

    // print_tokens(self);
}

void lexer_gen_input_tokens(lexer *self) {
    char c;
    while (!is_at_end(self)) {
        c = advance(self);
        self->start = self->current-1;
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
            case '+': add_token(self, PLUS, NULL);            break;
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

            default:
                if (isdigit(c)) {
                    scan_number(self);
                } else{
                    scan_input_string(self);
                }
        }
    }
    // print_tokens(self);
}
