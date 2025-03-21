#include "token.h"

#include "util/hashmap.h"
#include <stdio.h>
#include <string.h>

hashmap* hm_string_to_tokentype;

void initialize_hashmap(arena* arena) {
    hm_string_to_tokentype = create_hashmap(18*2, arena);
    hashmap_put(hm_string_to_tokentype, "UG", (void *)(uintptr_t)UG, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "O", (void *)(uintptr_t)O, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "DILI", (void *)(uintptr_t)DILI, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "NUMERO", (void *)(uintptr_t)NUMERO, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "LETRA", (void *)(uintptr_t)LETRA, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "TIPIK", (void *)(uintptr_t)TIPIK, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "TINUOD", (void *)(uintptr_t)TINUOD, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "SUGOD", (void *)(uintptr_t)SUGOD, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "KATAPUSAN", (void *)(uintptr_t)KATAPUSAN, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "MUGNA", (void *)(uintptr_t)MUGNA, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "IPAKITA", (void *)(uintptr_t)IPAKITA, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "DAWAT", (void *)(uintptr_t)DAWAT, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "KUNG", (void *)(uintptr_t)KUNG, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "WALA", (void *)(uintptr_t)WALA, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "KUNG_WALA", (void *)(uintptr_t)KUNG_WALA, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "KUNG_DILI", (void *)(uintptr_t)KUNG_DILI, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "PUNDOK", (void *)(uintptr_t)PUNDOK, sizeof(TokenType));
    hashmap_put(hm_string_to_tokentype, "ALANG_SA", (void *)(uintptr_t)ALANG_SA, sizeof(TokenType));

    print_hashmap(hm_string_to_tokentype);

    TokenType temp = (TokenType)(uintptr_t)hashmap_get(hm_string_to_tokentype, "PUNDOK");
    if (temp == PUNDOK) {
        printf("It Worked!");
    }else{
        printf("It did not work! It was %d\n", temp);
    }
}

void print_token(token *t) {
    printf("[%d] %-20s %s\n", t->line, token_val[t->type], t->lexeme);

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
        text = "O";
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
    case WALA:
        text = "WALA";
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
    case PUNDOK:
        text = "PUNDOK";
        break;
    case ALANG_SA:
        text = "ALANG_SA";
        break;
    case EOFILE:
        text = "EOFILE";
        break;
    case COLON:
        text = "COLON";
        break;
    case AMPERSAND:
        text = "AMPERSAND";
        break;
    case DOLLAR:
        text = "DOLLAR";
        break;
    case HASH:
        text = "HASH";
        break;
    case NEWLINE:
        text = "NEWLINE";
        break;
    case KUNG_WALA:
        text = "KUNG_WALA";
        break;
    case KUNG_DILI:
        text = "KUNG_DILI";
        break;
//    case INCREMENT:
//        text = "INCREMENT";
//        break;
//    case DECREMENT:
//        text = "DECREMENT";
//        break;
    }

//    printf("[%d] %-21s %s\n", t->line, text, t->lexeme);
}

TokenType get_token_type(const char *s) {

    // TODO: optimize, use better data structure

    // return (TokenType)hashmap_get(hm_string_to_tokentype, s);

    TokenType res = IDENTIFIER;

    if (strcmp(s, "UG") == 0) {
        res = UG;
    } else if (strcmp(s, "O") == 0) {
        res = O;
    } else if (strcmp(s, "DILI") == 0) {
        res = DILI;
    } else if (strcmp(s, "NUMERO") == 0) {
        res = NUMERO;
    } else if (strcmp(s, "LETRA") == 0) {
        res = LETRA;
    } else if (strcmp(s, "TIPIK") == 0) {
        res = TIPIK;
    } else if (strcmp(s, "TINUOD") == 0) {
        res = TINUOD;
    } else if (strcmp(s, "SUGOD") == 0) {
        res = SUGOD;
    } else if (strcmp(s, "KATAPUSAN") == 0) {
        res = KATAPUSAN;
    } else if (strcmp(s, "MUGNA") == 0) {
        res = MUGNA;
    } else if (strcmp(s, "IPAKITA") == 0) {
        res = IPAKITA;
    } else if (strcmp(s, "DAWAT") == 0) {
        res = DAWAT;
    } else if (strcmp(s, "KUNG") == 0) {
        res = KUNG;
    } else if (strcmp(s, "KUNG WALA") == 0) {
        res = KUNG_WALA;
    } else if (strcmp(s, "KUNG DILI") == 0) {
        res = KUNG_DILI;
    } else if (strcmp(s, "PUNDOK") == 0) {
        res = PUNDOK;
    } else if (strcmp(s, "ALANG SA") == 0) {
        res = ALANG_SA;
    }

    return res;
}
