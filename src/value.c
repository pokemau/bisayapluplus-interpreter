#include "value.h"

value value_create_number(double num) {
    return (value) {
        .type = VAL_NUMBER,
        .as.number = num,
    };
}


void value_free(value val) {

}
