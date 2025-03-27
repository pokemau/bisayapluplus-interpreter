#ifndef ERROR
#define ERROR

#include "arena.h"

#define INITIAL_ERROR_CAPACITY 16

typedef enum ErrorType{
    LEXICAL_ERROR,
    SYNTAX_ERROR,
    SEMANTIC_ERROR,
} ErrorType;

typedef struct {
    ErrorType type;
    int line;
    char *message;
} error;

typedef struct {
    arena* arena;
    error *errors;
    size_t error_count;
    size_t max_count;
} error_list;

error_list create_error_list(arena* arena);
void add_error(error_list* self, ErrorType type, int line, char* message);
void print_all_errors(error_list* self);

#endif