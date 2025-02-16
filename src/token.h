#ifndef _TOKEN_H_
#define _TOKEN_H_

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
    DOLLAR,                         // 10
    HASH,

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
    NOT_EQUAL,                      // 23

    // Logical Operators (AND, OR, NOT)
    UG, O, DILI,                    // 26

    // Boolean ("OO", "DILI")
    TRUE, FALSE,                    // 28

    // Unary operator (+, -) // IDK

    // Literals
    IDENTIFIER, STRING, NUMBER,     // 31

    // Data Types
    NUMERO, LETRA, TIPIK, TINUOD,   // 35

    // Keywords
    SUGOD,
    KATAPUSAN,
    MUGNA,
    IPAKITA,
    DAWAT,
    KUNG,
    WALA,
    PUNDOK,
    ALANG_SA,                       // 43

    // EOF
    EOFILE
} TokenType;


typedef struct Token {
    int line;
    enum TokenType type;

    // text
    const char *lexeme;

    // ang value nga iya gi hold
    void *literal;
} Token;

TokenType get_token_type(const char *s);
void print_token(Token *t);

#endif
