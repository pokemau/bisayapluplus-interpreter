#ifndef _VALUE_H_
#define _VALUE_H_


typedef enum {
    VAL_NUMBER,
} value_type;

typedef struct value {
    value_type type;
    union {
        double number;
    } as;

} value;


value value_create_number(double num);


void value_free(value self);


#endif
