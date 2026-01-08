#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "util/arena.h"

static void run(const char* data, size_t total) {
    arena a = arena_create(INITIAL_ARENA_SIZE);

    lexer s = lexer_create(&(lexer_src){
        .len = total,
        .data = data,
    });

    lexer_gen_tokens(&s);
    if (s.error_list->error_count > 0) {
        print_all_errors(s.error_list);
        arena_free(&s.arena);
        arena_free(&a);
        return;
    }

    parser p = parser_create(&s.tokens, &a);
    ast_node* ast = parser_parse(&p);
    if (p.error_list->error_count > 0) {
        print_all_errors(p.error_list);
        arena_free(&s.arena);
        arena_free(&a);
        return;
    }

    interpreter* i = interp_create(&a);
    interp_execute(i, ast);

    arena_free(&s.arena);
    arena_free(&a);
}

static char* read_file_content(const char* file_name, size_t* out_size) {
    FILE* f = fopen(file_name, "rb");
    if (f == NULL) {
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }

    long fsize = ftell(f);
    if (fsize < 0) {
        fclose(f);
        return NULL;
    }

    rewind(f);

    char* string = malloc(fsize + 1);
    if (!string) {
        fclose(f);
        return NULL;
    }

    size_t read_size = fread(string, 1, fsize, f);
    fclose(f);

    if (read_size < (size_t)fsize) {
        free(string);
        return NULL;
    }

    string[fsize] = '\0';
    // Return size including null terminator, so lexer processes the EOF/Null
    if (out_size)
        *out_size = (size_t)fsize + 1;

    return string;
}

static void run_file(const char* file_name) {
    size_t total = 0;
    char* data = read_file_content(file_name, &total);

    if (data == NULL) {
        fprintf(stderr, "ERROR: Could not read file '%s'\n", file_name);
        exit(1);
    }

    run(data, total);
    free(data);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    run_file(argv[1]);

    return 0;
}
