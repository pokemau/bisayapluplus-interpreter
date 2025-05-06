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

void env_define(environment *self, const char *name, value val) {
    env_elem *elem = arena_alloc(self->arena, sizeof(env_elem));
    size_t name_len = strlen(name) + 1;
    elem->name = arena_alloc(self->arena, name_len);
    memcpy(elem->name, name, name_len);
    elem->value = val;
    elem->next = self->list;
    self->list = elem;
}

bool env_get(environment *self, const char *name, value *out) {
    environment *curr = self;

    while (curr) {
        env_elem *elem = curr->list;
        while (elem) {
            if (strcmp(elem->name, name) == 0) {
                *out = elem->value;
                return true;
            }
            elem = elem->next;
        }
        curr = curr->parent;
    }
    return false;
}

bool env_assign(environment *self, const char *name, value val) {

    environment *curr = self;

    while (curr) {
        env_elem *elem = curr->list;
        while (elem) {
            if (strcmp(elem->name, name) == 0) {
                elem->value = val;
                return true;
            }
            elem = elem->next;
        }
        curr = curr->parent;
    }
    return false;
}

void env_free(environment *self);

value_type env_get_variable_type(environment *self, const char *name) {

    environment *curr = self;

    while (curr) {
        env_elem *elem = curr->list;
        while (elem) {
            if (strcmp(elem->name, name) == 0) {
                return get_variable_type(elem->value);
            }
            elem = elem->next;
        }
        curr = curr->parent;
    }
    return VAL_NOT_EXIST;
}
