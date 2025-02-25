#ifndef _DYN_ARR_H_
#define _DYN_ARR_H_

#define INITIAL_SIZE 512
#define MULTIPLIER 1.5

#include <stdlib.h>

typedef struct dyn_arr {
    int len;
    int max;
    size_t elem_size;
    void *arr;
} dyn_arr;

dyn_arr dyn_arr_create(size_t elem_size);
void dyn_arr_insert(dyn_arr *arr, void *elem);


#endif
