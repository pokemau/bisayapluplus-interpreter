#include "token.h"

#include "util/hashmap.h"
#include <stdio.h>
#include <string.h>

hashmap* hm_string_to_tokentype;

void initialize_hashmap(arena* arena) {
    // size 19 * 4, more space for the table, less collisions
    hm_string_to_tokentype = create_hashmap(19*4, arena);
    hashmap_put(hm_string_to_tokentype, "IDENTIFIER", (size_t)IDENTIFIER);
    hashmap_put(hm_string_to_tokentype, "UG", (size_t)UG);
    hashmap_put(hm_string_to_tokentype, "O", (size_t)O);
    hashmap_put(hm_string_to_tokentype, "DILI", (size_t)DILI);
    hashmap_put(hm_string_to_tokentype, "NUMERO", (size_t)NUMERO);
    hashmap_put(hm_string_to_tokentype, "LETRA", (size_t)LETRA);
    hashmap_put(hm_string_to_tokentype, "TIPIK", (size_t)TIPIK);
    hashmap_put(hm_string_to_tokentype, "TINUOD", (size_t)TINUOD);
    hashmap_put(hm_string_to_tokentype, "SUGOD", (size_t)SUGOD);
    hashmap_put(hm_string_to_tokentype, "KATAPUSAN", (size_t)KATAPUSAN);
    hashmap_put(hm_string_to_tokentype, "MUGNA", (size_t)MUGNA);
    hashmap_put(hm_string_to_tokentype, "IPAKITA", (size_t)IPAKITA);
    hashmap_put(hm_string_to_tokentype, "DAWAT", (size_t)DAWAT);
    hashmap_put(hm_string_to_tokentype, "KUNG", (size_t)KUNG);
    hashmap_put(hm_string_to_tokentype, "WALA", (size_t)WALA);
    hashmap_put(hm_string_to_tokentype, "KUNG_WALA", (size_t)KUNG_WALA);
    hashmap_put(hm_string_to_tokentype, "KUNG_DILI", (size_t)KUNG_DILI);
    hashmap_put(hm_string_to_tokentype, "PUNDOK", (size_t)PUNDOK);
    hashmap_put(hm_string_to_tokentype, "ALANG_SA", (size_t)ALANG_SA);

    // print_hashmap(hm_string_to_tokentype);
}

void print_token(token *t) {
    printf("[%d] %-20s %s\n", t->line, token_val[t->type], t->lexeme);
}

TokenType get_token_type(const char *s) {
    return (TokenType)hashmap_get(hm_string_to_tokentype, s);
}
