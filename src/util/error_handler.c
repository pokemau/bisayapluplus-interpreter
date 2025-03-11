#include "error_handler.h"

bool had_error = false;
bool had_runtime_error = false;


void report(const char *text, int line, const char *file) {
    fprintf(stderr, "ERROR: %s:%d: %s\n", file, line, text);
    had_error = true;
}

void error(token *tok, const char *msg) {
    char file_buffer[256];

    if (tok->type == EOFILE)
        report(msg, tok->line, "at end");
    else {
        snprintf(file_buffer, sizeof(file_buffer), "at '%s'", tok->lexeme);
        report(msg, tok->line, file_buffer);
    }
}
