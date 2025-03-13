#include "stmt.h"
#include <stdlib.h>

Stmt *stmt_make_var_decl(Token *name, Expr *initializer) {
    Stmt *self = malloc(sizeof(Stmt));
    self->type = STMT_VAR_DECL;
    self->Var_decl.name = name;
    self->Var_decl.initializer = initializer;
    return self;
}
