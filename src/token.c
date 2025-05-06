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
    hashmap_put_enum(hm_string_to_tokentype, "PULI", (size_t)PULI);
    hashmap_put_enum(hm_string_to_tokentype, "KASO", (size_t)KASO);

    // print_hashmap(hm_string_to_tokentype);

    // unlike above, the hash function of this uses the enum int values
    // meaning they never collides if array has enough space
    hm_tokentype_to_string = create_hashmap(55, arena, ENUM_TO_STRING);
    hashmap_put_string(hm_tokentype_to_string, (size_t)TIPIK, "TIPIK");
    hashmap_put_string(hm_tokentype_to_string, (size_t)LETRA, "LETRA");
    hashmap_put_string(hm_tokentype_to_string, (size_t)NUMERO, "NUMERO");
    hashmap_put_string(hm_tokentype_to_string, (size_t)TINUOD, "TINUOD");

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
    hashmap_put_string(hm_tokentype_to_string, (size_t)PULI, "PULI");
    hashmap_put_string(hm_tokentype_to_string, (size_t)KASO, "KASO");
    hashmap_put_string(hm_tokentype_to_string, (size_t)EOFILE, "EOFILE");
    // print_hashmap(hm_tokentype_to_string);
}

void print_token(token *t) {
    printf("[%d] %-20s %s\n", t->line, token_val[t->type], t->lexeme);
}

TokenType get_token_type(const char *s) {
    // Keywords
    if (strcmp(s, "SUGOD") == 0) return SUGOD;
    if (strcmp(s, "KATAPUSAN") == 0) return KATAPUSAN;
    if (strcmp(s, "MUGNA") == 0) return MUGNA;
    if (strcmp(s, "IPAKITA") == 0) return IPAKITA;
    if (strcmp(s, "DAWAT") == 0) return DAWAT;
    if (strcmp(s, "KUNG") == 0) return KUNG;
    if (strcmp(s, "KUNG_DILI") == 0 || strcmp(s, "KUNGDILI") == 0) return KUNG_DILI; // Allow with or without underscore
    if (strcmp(s, "KUNG_WALA") == 0 || strcmp(s, "KUNGWALA") == 0) return KUNG_WALA; // Allow with or without underscore
    if (strcmp(s, "PUNDOK") == 0) return PUNDOK;
    if (strcmp(s, "ALANG_SA") == 0 || strcmp(s, "ALANGSA") == 0) return ALANG_SA; // Allow with or without underscore
    if (strcmp(s, "PULI") == 0) return PULI;
    if (strcmp(s, "KASO") == 0) return KASO;
    // Data types
    if (strcmp(s, "NUMERO") == 0) return NUMERO;
    if (strcmp(s, "LETRA") == 0) return LETRA;
    if (strcmp(s, "TIPIK") == 0) return TIPIK;
    if (strcmp(s, "TINUOD") == 0) return TINUOD;
    // Boolean literals
    if (strcmp(s, "OO") == 0) return TRUE; // Bisaya for TRUE
    if (strcmp(s, "DILI") == 0 && strlen(s) == 4) return FALSE; // Bisaya for FALSE/NO (ensure it's not part of KUNG_DILI)
                                                              // DILI is also a logical NOT operator, lexer handles that by context if needed or parser disambiguates
                                                              // For simple keyword mapping, this is okay.
    // Default identifier
    return IDENTIFIER;
}

char* get_string_from_token_type(TokenType type) {
    return hashmap_get_string(hm_tokentype_to_string, type);
}