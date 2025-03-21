#include "hashmap.h"
#include "../token.h"
#include <stdio.h>
#include <string.h>

hashmap *create_hashmap(const size_t size, arena *arena) {
    hashmap *self = arena_alloc(arena, sizeof(hashmap));
    self->entries = arena_alloc(arena, sizeof(hashmap_entry*) * size);
    for(int i = 0; i < size; i++) {
        self->entries[i] = NULL;
    }

    self->size = size;
    self->arena = arena;

    return self;
}

static size_t hash(const char *key, const size_t table_size) {
    unsigned long hash = 0;
    size_t len = strlen(key);
    for (int i = 0; i < len; i++) {
        hash = hash * 31 + key[i];
    }
    hash = hash % table_size;
    return hash;
}

static hashmap_entry *set_key_value(const char *key, const size_t value, arena* arena) {
    hashmap_entry* entry = arena_alloc(arena, sizeof(hashmap_entry));
    
    entry->key = arena_alloc(arena, strlen(key) + 1);
    strcpy(entry->key, key);

    entry->value = value;

    return entry;
}

// value: enum value
// size_t: enum size
void hashmap_put(hashmap *self, const char *key, const size_t value) {
    size_t slot = hash(key, self->size);

    hashmap_entry *entry = self->entries[slot];

    // entry is not yet occupied, meaning it is safe to insert
    if (entry == NULL) {
        self->entries[slot] = set_key_value(key, value, self->arena);
        // printf("Added %s to %d\n", key, slot);
        return;
    }

    // else, look for other empty slots, Open Addressing
    size_t traversed_slot = slot;


    do {
        // printf("Aak I need to move\n");
        // if the current entry has the exact same key as our input
        if (entry != NULL && strcmp(entry->key, key) == 0) {
            // replace the value of the entry
            entry->value = value;
            // printf("Added %s to %d\n", key, traversed_slot);
            return;
        }

        // else move to the next slot
        traversed_slot = (traversed_slot+1) % self->size;
        entry = self->entries[traversed_slot];
    } while (entry != NULL && traversed_slot != slot);
    
    // If entry is null, it means we found a free slot
    if (entry == NULL) {
        self->entries[traversed_slot] = set_key_value(key, value, self->arena);
        // printf("Added %s to %d\n", key, traversed_slot);
    }
    // if traversed slot returned to initial slot, it means there are no free entry to be occupied
    else if (traversed_slot == slot) {
        printf("Hashtable is already full");
        return;
    }

}

size_t hashmap_get(hashmap* self, const char* key) {
    size_t slot = hash(key, self->size);

    hashmap_entry* entry = self->entries[slot];

    // if the entry is null, return NULL because this key has not been set yet
    if (entry == NULL) {
        return (size_t)IDENTIFIER;
    }

    // else move to the next slot until we found a match
    size_t traversed_slot = slot;
    do {
        // if the current entry has the exact same key as our input
        if (entry && strcmp(entry->key, key) == 0) {
            // return the value
            return entry->value;
        }

        // else move to the next slot, modulo to return at start if last element
        entry = self->entries[++traversed_slot % self->size];
    } while (entry != NULL && traversed_slot != slot);
    
    // if we get back from the start slot when traversing, it means we did not set that key and the hashmap is full
    // if the entry is NULL then we traverse all possible slots but none matched
    // so it is an identifier
    return (size_t)IDENTIFIER;
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
        char *key = self->entries[i]->key;
        size_t value = self->entries[i]->value;
        printf("%d: Key: %s, Value: %d\n", i, key != NULL ? key : "NULL" , value);
    }
}