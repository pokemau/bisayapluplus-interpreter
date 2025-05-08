#include "environment.h"
#include "util/arena.h"
#include "value.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void env_error(int line, const char *msg) {
    fprintf(stderr, "Semantic Error at Line [%d]: %s\n", line, msg);
    exit(1);
}

environment *env_create(environment *parent, arena *arena) {
    environment *self = arena_alloc(arena, sizeof(environment));
    self->list = NULL;
    self->parent = parent;
    self->arena = arena;
    initialize_value_hashmap(arena);
    return self;
}

void env_free(environment *self) {
    env_elem *e = self->list;
    while (e) {
        env_elem *next = e->next;
        free(e->name);
        value_free(e->value);
        free(e);
        e = next;
    }
    free(self);
}

void env_define(environment *self, const char *name, value val, int line, int scope) {
    // printf("Defining variable %s, scope %d\n", name, scope);

    // Check for redeclaration in the current scope
    environment *curr = self;
    while (curr) {
        env_elem *elem = curr->list;
        while (elem) {
            if (strcmp(elem->name, name) == 0 && elem->scope == scope) {
                char buf[128];
                snprintf(buf, 128, "Redeclaration of variable %s",
                        elem->name);
                env_error(line, buf);
            }
            elem = elem->next;
        }
        curr = curr->parent;
    }

    env_elem *elem = arena_alloc(self->arena, sizeof(env_elem));
    size_t name_len = strlen(name) + 1;
    elem->name = arena_alloc(self->arena, name_len);
    memcpy(elem->name, name, name_len);
    elem->value = val;
    elem->next = self->list;
    elem->scope = scope;
    self->list = elem;
}

bool env_get(environment *self, const char *name, value *out, int scope) {
    // printf("Getting variable %s, scope %d\n", name, scope);
    environment *curr = self;
    env_elem *found = NULL;

    while (curr) {
        env_elem *elem = curr->list;
        while (elem) {
            if (strcmp(elem->name, name) == 0 ) {
                if (!found || elem->scope > found->scope) {
                    found = elem;
                }
            }
            elem = elem->next;
        }
        curr = curr->parent;
    }
    if (found) {
        *out = found->value;
        return true;
    }
    return false;
}

bool env_assign(environment *self, const char *name, value val, int scope) {
    // printf("Assigning variable %s, scope %d\n", name, scope);
    environment *curr = self;
    env_elem *found = NULL;

    while (curr) {
        env_elem *elem = curr->list;
        while (elem) {
            if (strcmp(elem->name, name) == 0 ) {
                if (!found || elem->scope > found->scope) {
                    found = elem;
                }
            }
            elem = elem->next;
        }
        curr = curr->parent;
    }
    if (found) {
        found->value = val;
        return true;
    }
    return false;
}

void env_free(environment *self);

value_type env_get_variable_type(environment *self, const char *name, int scope) {
    // printf("Getting variable type %s, scope %d\n", name, scope);
    environment *curr = self;
    env_elem *found = NULL;

    while (curr) {
        env_elem *elem = curr->list;
        while (elem) {
            if (strcmp(elem->name, name) == 0 ) {
                if (!found || elem->scope > found->scope) {
                    found = elem;
                }
            }
            elem = elem->next;
        }
        curr = curr->parent;
    }
    if (found) {
        return get_variable_type(found->value);
    }
    return VAL_NOT_EXIST;
}

void env_unlink_by_scope(environment *self, int scope) {
    // printf("Unlinking scope %d\n", scope);
    env_elem **current = &self->list;

    while (*current) {
        env_elem *elem = *current;

        if (elem->scope == scope) {
            *current = elem->next;
            continue;
        }
        current = &elem->next;
    }
}