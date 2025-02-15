#include "token.h"

#include <stdio.h>

void print_token(Token *t) {

    const char *text;

    switch (t->type) {
    case LEFT_BRACE:
        text = "LEFT_BRACE";
        break;
    case RIGHT_BRACE:
        text = "RIGHT_BRACE";
        break;
    case LEFT_BRACKET:
        text = "LEFT_BRACKET";
        break;
    case RIGHT_BRACKET:
        text = "RIGHT_BRACKET";
        break;
    case COMMA:
        text = "COMMA";
        break;
    case DOT:
        text = "DOT";
        break;
    case SEMICOLON:
        text = "SEMICOLON";
        break;
    case EQUAL:
        text = "EQUAL";
        break;
    case LEFT_PAREN:
        text = "LEFT_PAREN";
        break;
    case RIGHT_PAREN:
        text = "RIGHT_PAREN";
        break;
    case STAR:
        text = "STAR";
        break;
    case SLASH:
        text = "SLASH";
        break;
    case MODULO:
        text = "MODULO";
        break;
    case PLUS:
        text = "PLUS";
        break;
    case MINUS:
        text = "MINUS";
        break;
    case GREATER:
        text = "GREATER";
        break;
    case LESS:
        text = "LESS";
        break;
    case GREATER_EQUAL:
        text = "GREATER_EQUAL";
        break;
    case LESS_EQUAL:
        text = "LESS_EQUAL";
        break;
    case EQUAL_EQUAL:
        text = "EQUAL_EQUAL";
        break;
    case NOT_EQUAL:
        text = "NOT_EQUAL";
        break;
    case UG:
        text = "UG";
        break;
    case O:
        text = "O:";
        break;
    case DILI:
        text = "DILI";
        break;
    case TRUE:
        text = "TRUE";
        break;
    case FALSE:
        text = "FALSE";
        break;
    case IDENTIFIER:
        text = "IDENTIFIER";
        break;
    case STRING:
        text = "STRING";
        break;
    case NUMBER:
        text = "NUMBER";
        break;
    case NUMERO:
        text = "NUMERO";
        break;
    case LETRA:
        text = "LETRA";
        break;
    case TIPIK:
        text = "TIPIK";
        break;
    case TINUOD:
        text = "TINUOD";
        break;
    case SUGOD:
        text = "SUGOD";
        break;
    case KATAPUSAN:
        text = "KATAPUSAN";
        break;
    case MUGNA:
        text = "MUGNA";
        break;
    case IPAKITA:
        text = "IPAKITA";
        break;
    case DAWAT:
        text = "DAWAT";
        break;
    case KUNG:
        text = "KUNG";
        break;
    case KUNG_KUNG:
        text = "KUNG_KUNG";
        break;
    case PUNDOK:
        text = "PUNDOK";
        break;
    case ALANG_SA:
        text = "ALANG_SA";
        break;
    case EOFILE:
        text = "EOFILE";
        break;
    }

    printf("[%d] %s \t\t\t%s\n", t->line, t->lexeme, text);
}
#include "token.h"
