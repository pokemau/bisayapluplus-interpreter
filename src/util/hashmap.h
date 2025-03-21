#ifndef HASHMAP
#define HASHMAP

#include "arena.h"
#define DEFAULT_SIZE 100;

typedef struct enum_val {
    void *value;
    size_t size;
} enum_val;

typedef struct hashmap_entry {
    char* key;
    enum_val *val;
} hashmap_entry;

typedef struct hashmap {
    size_t size;
    hashmap_entry **entries;
    arena* arena;
} hashmap;

hashmap *create_hashmap(const size_t size, arena *arena);
void hashmap_put(hashmap *self, const char *key, const void *value, const size_t val_size);
void* hashmap_get(hashmap *self, const char *key);
void print_hashmap(hashmap *self);

#endif