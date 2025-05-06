#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "util.h"
#include "parser.h"
#include "interpreter.h"
#include "util/arena.h"


lexer s;
parser p;
interpreter *i;
arena a;

void run(const char *data, size_t total) {
    a = arena_create(INITIAL_ARENA_SIZE);

    s = lexer_create(&(lexer_src) {
        .len = total, .data = data
    });
    lexer_gen_tokens(&s);

    p = parser_create(&s.tokens, &a);

    ast_node *ast = parser_parse(&p);

    // AST printing disabled; only interpreter output follows

    i = interp_create(&a);
    interp_execute(i, ast);

    arena_free(&a);
}

void run_file(const char *file_name) {

    FILE *f;
    f = fopen(file_name, "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR: File not found!\n");
        exit(1);
    }

    // https://www.youtube.com/watch?v=Oa_ji3dDFew
    size_t allocated = INCREASE;
    size_t total = 0;

    char *data = malloc(allocated);
    while (!feof(f) && !ferror(f)) {
        data[total] = fgetc(f);
        total++;
        if (total >= allocated) {
            allocated += INCREASE;
            data = realloc(data, allocated);
        }
    }

    data = realloc(data, total);
    data[total - 1] = '\0';

    run(data, total);

    fclose(f);
    free(data);
}

int main(int argc, char *argv[]) {

    if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "ERROR: Invalid arguments!\n");
        return 1;
    }

    return 0;
}
