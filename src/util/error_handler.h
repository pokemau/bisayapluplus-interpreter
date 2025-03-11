#ifndef _ERR_HANDLER_H_
#define _ERR_HANDLER_H_

#include <stdbool.h>
#include <stdio.h>
#include "../token.h"

extern bool had_error;
extern bool had_runtime_error;

void report(const char *text, int line, const char *file);
void error(token *tok, const char *msg);

#endif

