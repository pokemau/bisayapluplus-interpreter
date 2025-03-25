#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "util/arena.h"

typedef enum TokenType {
    // Characters
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA,
    DOT,
    SEMICOLON,
    COLON,
    EQUAL,
    AMPERSAND,
    DOLLAR,
    HASH,

    NEWLINE,

    // Arithmetic Operators
    LEFT_PAREN,
    RIGHT_PAREN,
    STAR,
    SLASH,
    MODULO,
    PLUS,
    MINUS,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EQUAL,
    EQUAL_EQUAL,
    NOT_EQUAL,

    // Logical Operators (AND, OR, NOT)
    UG, O, DILI,

    // Boolean ("OO", "DILI")
    TRUE, FALSE,

    // Unary operator (+, -) // IDK

    // Literals
    IDENTIFIER, STRING, CHAR, NUMBER,

    // Data Types (NUMBER, CHAR, FLOAT, BOOLEAN)
    NUMERO, LETRA, TIPIK, TINUOD,

    // Keywords
    SUGOD,
    KATAPUSAN,
    MUGNA,
    IPAKITA,
    DAWAT,
    KUNG,
    WALA,
    KUNG_WALA, KUNG_DILI,
    PUNDOK,
    ALANG_SA,

    // EOF
    EOFILE,

//    INCREMENT,
//    DECREMENT,
} TokenType;

static const char *token_val[] = {
    "LEFT_BRACE",
    "RIGHT_BRACE",
    "LEFT_BRACKET",
    "RIGHT_BRACKET",
    "COMMA",
    "DOT",
    "SEMICOLON",
    "COLON",
    "EQUAL",
    "AMPERSAND",
    "DOLLAR",
    "HASH",
    "NEWLINE",
    "LEFT_PAREN",
    "RIGHT_PAREN",
    "STAR",
    "SLASH",
    "MODULO",
    "PLUS",
    "MINUS",
    "GREATER",
    "LESS",
    "GREATER_EQUAL",
    "LESS_EQUAL",
    "EQUAL_EQUAL",
    "NOT_EQUAL",
    "UG",
    "O",
    "DILI",
    "TRUE",
    "FALSE",
    "IDENTIFIER",
    "STRING",
    "CHAR",
    "NUMBER",
    "NUMERO",
    "LETRA",
    "TIPIK",
    "TINUOD",
    "SUGOD",
    "KATAPUSAN",
    "MUGNA",
    "IPAKITA",
    "DAWAT",
    "KUNG",
    "WALA",
    "KUNG_WALA",
    "KUNG_DILI",
    "PUNDOK",
    "ALANG_SA",
    "EOFILE",
//    "INCREMENT",
//    "DECREMENT",
};


typedef struct token {
    int line;
    enum TokenType type;

    // text
    const char *lexeme;

    // ang value nga iya gi hold
    void *literal;
} token;

void initialize_token_hashmap(arena* arena);
const char *token_print_type(TokenType t);
TokenType get_token_type(const char *s);
char* get_string_from_token_type(TokenType type);
void print_token(token *t);

#endif
