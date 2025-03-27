#include <string.h>
#include "error.h"
#include "hashmap.h"

hashmap *hm_errortype_to_string;

static void initialize_error_hashmap(arena *arena);

error_list create_error_list(arena* arena) {
    error_list self;
    self.arena = arena;
    self.errors = arena_alloc(arena, sizeof(error) * INITIAL_ERROR_CAPACITY);
    self.error_count = 0;
    self.max_count = INITIAL_ERROR_CAPACITY;

    initialize_error_hashmap(arena);
    return self;
}

void add_error(error_list* self, ErrorType type, int line, char* message) {
    if (self->error_count + 1 >= self->max_count) {
        size_t new_max = self->max_count * 2;
        error *new_errors = arena_alloc(self->arena, sizeof(error) * new_max);
        memcpy(new_errors, self->errors, sizeof(error) * self->error_count);
        self->errors = new_errors;
        self->max_count = new_max;
    }

    error *new_error = &self->errors[self->error_count++];
    new_error->type = type;
    new_error->line = line;
    new_error->message = message;
}

void print_all_errors(error_list* self) {
    error *curr;
    for (int i = 0; i < self->error_count; i++) { 
        curr = &self->errors[i];
        printf("%s at Line [%d]: %s",
            hashmap_get_string(hm_errortype_to_string, curr->type),
            curr->line,
            curr->message
        );
    }
}

static void initialize_error_hashmap(arena *arena) {
    hm_errortype_to_string = create_hashmap(3, arena, ENUM_TO_STRING);

    hashmap_put_string(hm_errortype_to_string, LEXICAL_ERROR, "Lexical Error");
    hashmap_put_string(hm_errortype_to_string, SYNTAX_ERROR, "Syntax Error");
    hashmap_put_string(hm_errortype_to_string, SEMANTIC_ERROR, "Semantic Error");
}