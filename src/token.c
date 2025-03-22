#include "token.h"

#include "util/hashmap.h"
#include <stdio.h>
#include <string.h>

hashmap* hm_string_to_tokentype;
hashmap* hm_tokentype_to_string;

void initialize_token_hashmap(arena* arena) {
    // size 19 * 4, more space for the table, less collisions
    hm_string_to_tokentype = create_hashmap(19*4, arena, STRING_TO_ENUM);
    hashmap_put_enum(hm_string_to_tokentype, "IDENTIFIER", (size_t)IDENTIFIER);
    hashmap_put_enum(hm_string_to_tokentype, "UG", (size_t)UG);
    hashmap_put_enum(hm_string_to_tokentype, "O", (size_t)O);
    hashmap_put_enum(hm_string_to_tokentype, "DILI", (size_t)DILI);
    hashmap_put_enum(hm_string_to_tokentype, "NUMERO", (size_t)NUMERO);
    hashmap_put_enum(hm_string_to_tokentype, "LETRA", (size_t)LETRA);
    hashmap_put_enum(hm_string_to_tokentype, "TIPIK", (size_t)TIPIK);
    hashmap_put_enum(hm_string_to_tokentype, "TINUOD", (size_t)TINUOD);
    hashmap_put_enum(hm_string_to_tokentype, "SUGOD", (size_t)SUGOD);
    hashmap_put_enum(hm_string_to_tokentype, "KATAPUSAN", (size_t)KATAPUSAN);
    hashmap_put_enum(hm_string_to_tokentype, "MUGNA", (size_t)MUGNA);
    hashmap_put_enum(hm_string_to_tokentype, "IPAKITA", (size_t)IPAKITA);
    hashmap_put_enum(hm_string_to_tokentype, "DAWAT", (size_t)DAWAT);
    hashmap_put_enum(hm_string_to_tokentype, "KUNG", (size_t)KUNG);
    hashmap_put_enum(hm_string_to_tokentype, "WALA", (size_t)WALA);
    hashmap_put_enum(hm_string_to_tokentype, "KUNG_WALA", (size_t)KUNG_WALA);
    hashmap_put_enum(hm_string_to_tokentype, "KUNG_DILI", (size_t)KUNG_DILI);
    hashmap_put_enum(hm_string_to_tokentype, "PUNDOK", (size_t)PUNDOK);
    hashmap_put_enum(hm_string_to_tokentype, "ALANG_SA", (size_t)ALANG_SA);
    hashmap_put_enum(hm_string_to_tokentype, "UNKOWN", (size_t)UNKNOWN);

    // print_hashmap(hm_string_to_tokentype);

    // unlike above, the hash function of this uses the enum int values
    // meaning they never collides if array has enough space
    hm_tokentype_to_string = create_hashmap(55, arena, ENUM_TO_STRING);
    hashmap_put_string(hm_tokentype_to_string, (size_t)TIPIK, "TIPIK");
    hashmap_put_string(hm_tokentype_to_string, (size_t)LETRA, "LETRA");
    hashmap_put_string(hm_tokentype_to_string, (size_t)NUMERO, "NUMERO");
    hashmap_put_string(hm_tokentype_to_string, (size_t)TINUOD, "TINUOD");
    hashmap_put_string(hm_tokentype_to_string, (size_t)UNKNOWN, "UNKNOWN");

    hashmap_put_string(hm_tokentype_to_string, (size_t)LEFT_BRACE, "LEFT_BRACE");
    hashmap_put_string(hm_tokentype_to_string, (size_t)RIGHT_BRACE, "RIGHT_BRACE");
    hashmap_put_string(hm_tokentype_to_string, (size_t)LEFT_BRACKET, "LEFT_BRACKET");
    hashmap_put_string(hm_tokentype_to_string, (size_t)RIGHT_BRACKET, "RIGHT_BRACKET");
    hashmap_put_string(hm_tokentype_to_string, (size_t)COMMA, "COMMA");
    hashmap_put_string(hm_tokentype_to_string, (size_t)DOT, "DOT");
    hashmap_put_string(hm_tokentype_to_string, (size_t)SEMICOLON, "SEMICOLON");
    hashmap_put_string(hm_tokentype_to_string, (size_t)COLON, "COLON");
    hashmap_put_string(hm_tokentype_to_string, (size_t)EQUAL, "EQUAL");
    hashmap_put_string(hm_tokentype_to_string, (size_t)AMPERSAND, "AMPERSAND");
    hashmap_put_string(hm_tokentype_to_string, (size_t)DOLLAR, "DOLLAR");
    hashmap_put_string(hm_tokentype_to_string, (size_t)HASH, "HASH");
    hashmap_put_string(hm_tokentype_to_string, (size_t)NEWLINE, "NEWLINE");
    hashmap_put_string(hm_tokentype_to_string, (size_t)LEFT_PAREN, "LEFT_PAREN");
    hashmap_put_string(hm_tokentype_to_string, (size_t)RIGHT_PAREN, "RIGHT_PAREN");
    hashmap_put_string(hm_tokentype_to_string, (size_t)STAR, "STAR");
    hashmap_put_string(hm_tokentype_to_string, (size_t)SLASH, "SLASH");
    hashmap_put_string(hm_tokentype_to_string, (size_t)MODULO, "MODULO");
    hashmap_put_string(hm_tokentype_to_string, (size_t)PLUS, "PLUS");
    hashmap_put_string(hm_tokentype_to_string, (size_t)MINUS, "MINUS");
    hashmap_put_string(hm_tokentype_to_string, (size_t)GREATER, "GREATER");
    hashmap_put_string(hm_tokentype_to_string, (size_t)LESS, "LESS");
    hashmap_put_string(hm_tokentype_to_string, (size_t)GREATER_EQUAL, "GREATER_EQUAL");
    hashmap_put_string(hm_tokentype_to_string, (size_t)LESS_EQUAL, "LESS_EQUAL");
    hashmap_put_string(hm_tokentype_to_string, (size_t)EQUAL_EQUAL, "EQUAL_EQUAL");
    hashmap_put_string(hm_tokentype_to_string, (size_t)NOT_EQUAL, "NOT_EQUAL");
    hashmap_put_string(hm_tokentype_to_string, (size_t)UG, "UG");
    hashmap_put_string(hm_tokentype_to_string, (size_t)O, "O");
    hashmap_put_string(hm_tokentype_to_string, (size_t)DILI, "DILI");
    hashmap_put_string(hm_tokentype_to_string, (size_t)TRUE, "TRUE");
    hashmap_put_string(hm_tokentype_to_string, (size_t)FALSE, "FALSE");
    hashmap_put_string(hm_tokentype_to_string, (size_t)IDENTIFIER, "IDENTIFIER");
    hashmap_put_string(hm_tokentype_to_string, (size_t)STRING, "STRING");
    hashmap_put_string(hm_tokentype_to_string, (size_t)CHAR, "CHAR");
    hashmap_put_string(hm_tokentype_to_string, (size_t)NUMBER, "NUMBER");
    hashmap_put_string(hm_tokentype_to_string, (size_t)SUGOD, "SUGOD");
    hashmap_put_string(hm_tokentype_to_string, (size_t)KATAPUSAN, "KATAPUSAN");
    hashmap_put_string(hm_tokentype_to_string, (size_t)MUGNA, "MUGNA");
    hashmap_put_string(hm_tokentype_to_string, (size_t)IPAKITA, "IPAKITA");
    hashmap_put_string(hm_tokentype_to_string, (size_t)DAWAT, "DAWAT");
    hashmap_put_string(hm_tokentype_to_string, (size_t)KUNG, "KUNG");
    hashmap_put_string(hm_tokentype_to_string, (size_t)WALA, "WALA");
    hashmap_put_string(hm_tokentype_to_string, (size_t)KUNG_WALA, "KUNG_WALA");
    hashmap_put_string(hm_tokentype_to_string, (size_t)KUNG_DILI, "KUNG_DILI");
    hashmap_put_string(hm_tokentype_to_string, (size_t)PUNDOK, "PUNDOK");
    hashmap_put_string(hm_tokentype_to_string, (size_t)ALANG_SA, "ALANG_SA");
    hashmap_put_string(hm_tokentype_to_string, (size_t)EOFILE, "EOFILE");
    // print_hashmap(hm_tokentype_to_string);
}

void print_token(token *t) {
    printf("[%d] %-20s %s\n", t->line, token_val[t->type], t->lexeme);
}

TokenType get_token_type(const char *s) {
    return (TokenType)hashmap_get_enum(hm_string_to_tokentype, s);
}

char* get_string_from_token_type(TokenType type) {
    return hashmap_get_string(hm_tokentype_to_string, type);
}