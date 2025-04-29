#include "value.h"
#include "token.h"
#include "util/hashmap.h"

hashmap* hm_value_type_to_string;

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

value value_create_null(value_type var_type) { 
    return (value){
        .type = VAL_NULL,
        .as.var_type = var_type,
    }; 
}

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

void initialize_value_hashmap(arena* arena) {
    hm_value_type_to_string = create_hashmap(6, arena, ENUM_TO_STRING);

    hashmap_put_string(hm_value_type_to_string, (size_t)VAL_TIPIK, "TIPIK");
    hashmap_put_string(hm_value_type_to_string, (size_t)VAL_LETRA, "LETRA");
    hashmap_put_string(hm_value_type_to_string, (size_t)VAL_NUMERO, "NUMERO");
    hashmap_put_string(hm_value_type_to_string, (size_t)VAL_TINUOD, "TINUOD");
    hashmap_put_string(hm_value_type_to_string, (size_t)VAL_NULL, "NULL");
    hashmap_put_string(hm_value_type_to_string, (size_t)VAL_NOT_EXIST, "NOT EXIST");

}

char* get_string_from_value_type(value_type type) {
    return hashmap_get_string(hm_value_type_to_string, type);
}

value_type get_variable_type(value val) {
    if (val.type == VAL_NULL) {
        return val.as.var_type;
    }
    return val.type;
}

// TIPIK > LETRA > NUMERO > TINUOD
void value_precedence_convert(value* x, value* y) {
    value tempx, tempy;
    value_type final_type;
    if (x->type == VAL_TIPIK || y->type == VAL_TIPIK) final_type = VAL_TIPIK;
    else if (x->type == VAL_LETRA || y->type == VAL_LETRA) final_type = VAL_LETRA;
    else if (x->type == VAL_NUMERO || y->type == VAL_NUMERO) final_type = VAL_NUMERO;
    else final_type = VAL_TINUOD;

    switch (final_type) {
    case VAL_TIPIK:
        tempx = value_create_tipik(x->type == VAL_TIPIK ? x->as.tipik :
            x->type == VAL_LETRA ? (double)x->as.letra :
            x->type == VAL_NUMERO ? (double)x->as.numero :
            (double)x->as.tinuod);
        tempy = value_create_tipik(y->type == VAL_TIPIK ? y->as.tipik :
            y->type == VAL_LETRA ? (double)y->as.letra :
            y->type == VAL_NUMERO ? (double)y->as.numero :
            (double)y->as.tinuod);

    case VAL_LETRA:
        tempx = value_create_letra(x->type == VAL_LETRA ? x->as.letra :
            x->type == VAL_NUMERO ? (char)x->as.numero :
            (char)x->as.tinuod);
        tempy = value_create_letra(y->type == VAL_LETRA ? y->as.letra :
            y->type == VAL_NUMERO ? (char)y->as.numero :
            (char)y->as.tinuod);
    case VAL_NUMERO:
        tempx = value_create_numero(x->type == VAL_NUMERO ? x->as.numero :
            (long)x->as.tinuod);
        tempy = value_create_numero(y->type == VAL_NUMERO ? y->as.numero :
            (long)y->as.tinuod);
    case VAL_TINUOD:
        break;  //if they reach this, both values are already booleans (tinuods)
    default:
        break;// VAL_NULL or logic error happened
    }

    x = &tempx;
    y = &tempy;
}