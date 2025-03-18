#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "token.h"
#include "util.h"
#include "parser.h"
#include "interpreter.h"


lexer s;
parser p;
interpreter i;

void run(const char *data, size_t total) {
    s = lexer_create(&(lexer_src){.len = total, .data = data});
    lexer_gen_tokens(&s);
    p = parser_create(&s.tokens);

    ast_node *ast = parser_parse(&p);

    printf("=========================\n");
    printf("========= AST ===========\n");
    printf("=========================\n");
    ast_print(ast, 0);

    // TODO:
    // Free allocated memory in parser

    i = interp_create();
    interp_execute(&i, ast);

    // free ast_nodes after program
//    for (int i = 0; i < ast->program.stmt_count; i++) {
//            ast_free_node(&ast[i]);
//    }

    // free allocated stuff
    free(s.tokens.list);
    for (int i = 0; i < s.tokens.size; i++) {
        token *curr = &s.tokens.list[i];
        if (curr->literal != NULL) {
            free(curr->literal);
        }
    }
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
