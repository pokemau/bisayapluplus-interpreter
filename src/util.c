// #include "error.h"

#include <stdio.h>
#include <stdlib.h>

void bpp_error(int line, const char *msg) {
    fprintf(stderr, "Line: [%d], ERROR: %s\n", line, msg);
    exit(1);
}
