#ifndef _TOKEN_H_
#define _TOKEN_H_


typedef enum TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET,
    RIGHT_BRACKET, COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, // 12


    // One or two character tokens.
    BANG, NOT_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,       // 20

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    SUGOD, KATAPUSAN, MUGNA, NUMERO, LETRA, TINUOD,
    TIPIK, UG, O, DILI, PUNDOK, KUNG, WALA,

    // not sure
    ALANG_SA,

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


#endif
