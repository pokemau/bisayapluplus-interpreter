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
    EQUAL,                          // 7

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
    NOT_EQUAL,                      // 20

    // Logical Operators (AND, OR, NOT)
    UG, O, DILI,                    // 23

    // Boolean ("OO", "DILI")
    TRUE, FALSE,                    // 25

    // Unary operator (+, -) // IDK

    // Literals
    IDENTIFIER, STRING, NUMBER,     // 28

    // Data Types
    NUMERO, LETRA, TIPIK, TINUOD,   // 32

    // Keywords
    SUGOD,
    KATAPUSAN,
    MUGNA,
    IPAKITA,
    DAWAT,
    KUNG,
    WALA,
    PUNDOK,
    ALANG_SA,                       // 40

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
