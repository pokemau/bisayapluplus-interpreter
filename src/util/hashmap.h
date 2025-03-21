#ifndef HASHMAP
#define HASHMAP

#include "arena.h"
#define DEFAULT_SIZE 100;

typedef struct hashmap_entry {
    char* key;
    size_t value;
} hashmap_entry;

typedef struct hashmap {
    size_t size;
    hashmap_entry **entries;
    arena* arena;
} hashmap;

hashmap *create_hashmap(const size_t size, arena *arena);
void hashmap_put(hashmap *self, const char *key, const size_t value);
size_t hashmap_get(hashmap *self, const char *key);
void print_hashmap(hashmap *self);

#endif