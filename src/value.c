#include "value.h"
#include "ast.h"
#include "token.h"

value value_create_numero(int num) {
    return (value){
        .type = VAL_NUMERO,
        .as.numero = num,
    };
}

value value_create_letra(char c) {
    return (value){
        .type = VAL_LETRA,
        .as.letra = c,
    };
}

value value_create_tipik(double num) {
    return (value){
        .type = VAL_TIPIK,
        .as.tipik = num,
    };
}

value value_create_tinuod(bool b) {
    return (value){
        .type = VAL_TINUOD,
        .as.tinuod = b,
    };
}

value value_create_null() { return (value){.type = VAL_NULL}; }

value_type value_get_d_type(TokenType d_type) {
    switch (d_type) {
    case NUMERO:
        return VAL_NUMERO;
    case LETRA:
        return VAL_LETRA;
    case TIPIK:
        return VAL_TIPIK;
    case TINUOD:
        return VAL_TINUOD;
    default:
        return VAL_NULL;
    }
}

void value_free(value val) {}
