#ifndef _ARENA_H_
#define _ARENA_H_

#include <stdlib.h>
#include <stdio.h>

#define INITIAL_ARENA_SIZE (1024 * 1024)

typedef struct arena {
    char *mem;
    size_t size;
    size_t offset;
} arena ;

arena arena_create(size_t size);
void *arena_alloc(arena *self, size_t size);
void arena_free(arena *self);



#endif
