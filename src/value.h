#ifndef _VALUE_H_
#define _VALUE_H_

#include "token.h"
#include <stdbool.h>

typedef enum {
    VAL_NUMERO,
    VAL_LETRA,
    VAL_TIPIK,
    VAL_TINUOD,
    VAL_NULL,
} value_type;

typedef struct value {
    value_type type;
    union {
        long numero;
        double tipik;
        bool tinuod;
        char letra;
    } as;
} value;

value_type value_get_d_type(TokenType d_type);

value value_create_null();
value value_create_numero(int num);
value value_create_letra(char c);
value value_create_tipik(double num);
value value_create_tinuod(bool b);
void value_free(value self);


#endif
