#include "arena.h"
#include <stdio.h>

arena arena_create(size_t size) {
    arena self;
    self.mem = malloc(size);
    if (!self.mem) {
        fprintf(stderr, "Failed to allocate mem [arena]\n");
        exit(1);
    }
    self.size = size;
    self.offset = 0;
    return self;
}
void *arena_alloc(arena *self, size_t size) {
    size_t alignment = 8;
    size_t offset = (self->offset + alignment - 1) & ~(alignment - 1);
    size_t new_offset = offset + size;

    if (new_offset >= self->size) {
        size_t new_size = self->size * 2;

        while (new_offset > new_size) {
            new_size *= 2;
        }

        self->mem = realloc(self->mem, new_size);
        if (!self->mem) {
            fprintf(stderr, "Failed to alloc mem\n");
            exit(1);
        }
        self->size =new_size;
    }

    void *ptr = self->mem + offset;
    self->offset = new_offset;
    return ptr;
}

void arena_free(arena *self) {
    free(self->mem);
    self->mem = NULL;
    self->size = 0;
    self->offset = 0;
}
