#include "dyn_arr.h"

dyn_arr dyn_arr_create(size_t elem_size) {
    dyn_arr self;
    self.arr = malloc(sizeof(elem_size) * INITIAL_SIZE);
    self.len = 0;
    self.max = INITIAL_SIZE;
    self.elem_size = elem_size;
    return self;
}

void dyn_arr_insert(dyn_arr *self, void *elem) {
    self->arr[self->len] = elem;
}
