#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "util.h"

struct Scanner s;


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
    data[total-1] = '\0';

    // scanner stuff here
    s = scanner_create(&(scanner_source) {
        .len = total,
        .data = data
    });

    scanner_scan_tokens(&s);


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
