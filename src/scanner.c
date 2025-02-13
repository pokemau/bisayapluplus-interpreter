#include "scanner.h"

#include <stdio.h>

struct Scanner scanner_create(scanner_source *source) {

    struct Scanner self;
    self.source = source;
    // TODO:
    // allocate memory for tokens(?)

    self.start = 0;
    self.current = 0;
    self.line = 1;

    return self;
}

void scanner_token(struct Scanner *self) {

    while (self->current <= self->source->len) {
        printf("CURR: %c\n", self->source->data[self->current]);
        self->current++;
    }
}
