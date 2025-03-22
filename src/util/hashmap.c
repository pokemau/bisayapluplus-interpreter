#include "hashmap.h"
#include "../token.h"
#include <stdio.h>
#include <string.h>

hashmap *create_hashmap(const size_t size, arena *arena, Hashmap_Type type) {
    hashmap *self = arena_alloc(arena, sizeof(hashmap));
    self->entries = arena_alloc(arena, sizeof(hashmap_entry*) * size);
    for(int i = 0; i < size; i++) {
        self->entries[i] = NULL;
    }

    self->size = size;
    self->arena = arena;
    self->type = type;

    return self;
}

static size_t hash_string_to_enum(const char *key, const size_t table_size) {
    unsigned long hash = 0;
    size_t len = strlen(key);
    for (int i = 0; i < len; i++) {
        hash = hash * 31 + key[i];
    }
    hash = hash % table_size;
    return hash;
}

static size_t hash_enum_to_string(const size_t key, const size_t table_size) {
    // use enum value as key directly
    return ((int) key % table_size);
}

static hashmap_entry *set_key_value_string_to_enum(const char *key, const size_t value, arena* arena) {
    hashmap_entry* entry = arena_alloc(arena, sizeof(hashmap_entry));
    
    entry->string_to_enum.key = arena_alloc(arena, strlen(key) + 1);
    strcpy(entry->string_to_enum.key, key);

    entry->string_to_enum.value = value;

    return entry;
}

static hashmap_entry *set_key_value_enum_to_string(const size_t key, const char* value, arena* arena) {
    hashmap_entry* entry = arena_alloc(arena, sizeof(hashmap_entry));
    
    entry->enum_to_string.key = key;

    entry->enum_to_string.value = arena_alloc(arena, strlen(value) + 1);
    strcpy(entry->enum_to_string.value, value);

    return entry;
}

void hashmap_put_enum(hashmap *self, const char *key, const size_t value) {
    size_t slot = hash_string_to_enum(key, self->size);
    hashmap_entry *entry = self->entries[slot];

    // entry is not yet occupied, meaning it is safe to insert
    if (entry == NULL) {
        self->entries[slot] = set_key_value_string_to_enum(key, value, self->arena);
        // printf("Added %s to %d\n", key, slot);
        return;
    }
    // else, look for other empty slots, Open Addressing
    size_t traversed_slot = slot;

    do {
        // printf("Aak I need to move\n");
        // if the current entry has the exact same key as our input
        if (entry != NULL && strcmp(entry->string_to_enum.key, key) == 0) {
            // replace the value of the entry
            entry->string_to_enum.value = value;
            // printf("Added %s to %d\n", key, traversed_slot);
            return;
        }

        // else move to the next slot
        traversed_slot = (traversed_slot+1) % self->size;
        entry = self->entries[traversed_slot];
    } while (entry != NULL && traversed_slot != slot);
    
    // If entry is null, it means we found a free slot
    if (entry == NULL) {
        self->entries[traversed_slot] = set_key_value_string_to_enum(key, value, self->arena);
        // printf("Added %s to %d\n", key, traversed_slot);
    }
    // if traversed slot returned to initial slot, it means there are no free entry to be occupied
    else if (traversed_slot == slot) {
        printf("Hashtable is already full");
        return;
    }
}

void hashmap_put_string(hashmap *self, const size_t key, const char *value) {
    size_t slot = hash_enum_to_string(key, self->size);
    hashmap_entry *entry = self->entries[slot];

    // entry is not yet occupied, meaning it is safe to insert
    if (entry == NULL) {
        self->entries[slot] = set_key_value_enum_to_string(key, value, self->arena);
        // printf("Added %d to %d\n", key, slot);
        return;
    }
    // else, look for other empty slots, Open Addressing
    size_t traversed_slot = slot;

    do {
        // if the current entry has the exact same key as our input
        if (entry != NULL && entry->enum_to_string.key == key) {
            // replace the value of the entry
            entry->enum_to_string.value = arena_alloc(self->arena, strlen(value) + 1);
            strcpy(entry->enum_to_string.value, value);
            // printf("Added %d to %d\n", key, traversed_slot);
            return;
        }

        // else move to the next slot
        traversed_slot = (traversed_slot+1) % self->size;
        entry = self->entries[traversed_slot];
    } while (entry != NULL && traversed_slot != slot);
    
    // If entry is null, it means we found a free slot
    if (entry == NULL) {
        self->entries[traversed_slot] = set_key_value_enum_to_string(key, value, self->arena);
        // printf("Added %d to %d\n", key, traversed_slot);
    }
    // if traversed slot returned to initial slot, it means there are no free entry to be occupied
    else if (traversed_slot == slot) {
        printf("Hashtable is already full");
        return;
    }
}

size_t hashmap_get_enum(hashmap* self, const char* key) {
    size_t slot = hash_string_to_enum(key, self->size);

    hashmap_entry* entry = self->entries[slot];

    // if the entry is null, return NULL because this key has not been set yet
    if (entry == NULL) {
        return (size_t)IDENTIFIER;
    }

    // else move to the next slot until we found a match
    size_t traversed_slot = slot;
    do {
        // if the current entry has the exact same key as our input
        if (entry && strcmp(entry->string_to_enum.key, key) == 0) {
            // return the value
            return entry->string_to_enum.value;
        }

        // else move to the next slot, modulo to return at start if last element
        traversed_slot = (traversed_slot+1) % self->size;
        entry = self->entries[traversed_slot];
    } while (entry != NULL && traversed_slot != slot);
    
    // if we get back from the start slot when traversing, it means we did not set that key and the hashmap is full
    // if the entry is NULL then we traverse all possible slots but none matched
    // so it is an identifier
    return (size_t)IDENTIFIER;
}

char *hashmap_get_string(hashmap* self, const size_t key) {
    size_t slot = hash_enum_to_string(key, self->size);

    hashmap_entry* entry = self->entries[slot];

    // if the entry is null, return NULL because this key has not been set yet
    if (entry == NULL) {
        return "NOT FOUND";
    }

    // else move to the next slot until we found a match
    size_t traversed_slot = slot;
    do {
        // if the current entry has the exact same key as our input
        if (entry && entry->enum_to_string.key == key) {
            // return the value
            return entry->enum_to_string.value;
        }

        // else move to the next slot, modulo to return at start if last element
        traversed_slot = (traversed_slot+1) % self->size;
        entry = self->entries[traversed_slot];
    } while (entry != NULL && traversed_slot != slot);
    
    // if we get back from the start slot when traversing, it means we did not set that key and the hashmap is full
    // if the entry is NULL then we traverse all possible slots but none matched
    return "NOT FOUND";
}

void print_hashmap(hashmap *self) {
    if (self == NULL || self->entries == NULL) {
        printf("Hashmap is not initialized.\n");
        return;
    }
    for(int i=0; i<self->size; i++){
        if (self->entries[i] == NULL){
            printf("%d: NULL\n", i);
            continue;
        }
        if (self->type == STRING_TO_ENUM) {
            char *key = self->entries[i]->string_to_enum.key;
            size_t value = self->entries[i]->string_to_enum.value;
            printf("%d: Key: %s, Value: %d\n", i, key != NULL ? key : "NULL" , value);
        } else if (self->type == ENUM_TO_STRING) {
            size_t key = self->entries[i]->enum_to_string.key;
            char *value = self->entries[i]->enum_to_string.value;
            printf("%d: Key: %d, Value: %s\n", i, key , value != NULL ? value : "NULL");
        }
    }
}