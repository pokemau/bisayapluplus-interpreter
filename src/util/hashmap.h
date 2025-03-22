#ifndef HASHMAP
#define HASHMAP

#include "arena.h"
#define DEFAULT_SIZE 100;

// s_tt: string to tokentype
// tt_s: tokentype to string
typedef struct hashmap_entry {
    union {
        struct {
            char* key;
            size_t value;
        } string_to_enum;
        struct {
            size_t key;
            char* value;
        } enum_to_string;
    };
} hashmap_entry;

typedef enum Hashmap_Type {
    STRING_TO_ENUM,
    ENUM_TO_STRING,
} Hashmap_Type;

typedef struct hashmap {
    size_t size;
    hashmap_entry **entries;
    arena* arena;
    Hashmap_Type type;
} hashmap;

hashmap *create_hashmap(const size_t size, arena *arena, Hashmap_Type type);
void hashmap_put_enum(hashmap *self, const char *key, const size_t value);
void hashmap_put_string(hashmap *self, const size_t key, const char *value);
size_t hashmap_get_enum(hashmap *self, const char *key);
char *hashmap_get_string(hashmap *self, const size_t key);
void print_hashmap(hashmap *self);

#endif