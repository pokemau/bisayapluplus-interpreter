#include "environment.h"
#include "value.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void env_error(int line, const char *msg) {
    fprintf(stderr, "Line [%d]: %s\n", line, msg);
    exit(1);
}

environment *env_create(environment *parent) {
    environment *self = malloc(sizeof(environment));
    self->list = NULL;
    self->parent = parent;
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
    env_elem *elem = malloc(sizeof(env_elem));
    elem->name = strdup(name);
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
                value_free(elem->value);
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
