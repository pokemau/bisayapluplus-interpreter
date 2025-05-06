#include "interpreter.h"
#include "ast.h"
#include "environment.h"
#include "parser.h"
#include "token.h"
#include "value.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define equality(a, b) ((a) > (b) ? 1 : (a) < (b) ? -1 : 0)

// TODO:
// fix evaluate AST_BINARY

bool interp_has_error = false;

static value evaluate(interpreter *self, ast_node *node);
static void execute_statement(interpreter *self, ast_node *node);

static void interp_error(int line, const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

interpreter *interp_create(arena *arena) {
    interpreter *self = arena_alloc(arena, sizeof(interpreter));
    self->env = env_create(NULL, arena);
    self->arena = arena;
    return self;
}

void interp_free(interpreter *self) {
    environment *env = self->env;
    while (env) {
        environment *parent = env->parent;
        env_free(env);
        env = parent;
    }
    free(self);
}

void interp_execute(interpreter *self, ast_node *node) {
    if (!node)
        return;

    switch (node->type) {
    case AST_PROGRAM:
        for (int i = 0; i < node->program.stmt_count; i++) {
            execute_statement(self, &node->program.statements[i]);
        }
        break;
    default:
        fprintf(stderr, "Invalid Node!");
        exit(1);
        break;
    }
}

static void execute_statement(interpreter *self, ast_node *node) {
    if (!node)
        return;

    value val;
    value_type d_type;

    switch (node->type) {
    case AST_PROGRAM:
        break;
    case AST_VAR_DECL:
        for (int i = 0; i < node->var_decl.name_count; i++) {
            d_type = value_get_d_type(node->var_decl.d_type);
            value val = node->var_decl.inits[i]
                            ? evaluate(self, node->var_decl.inits[i])
                            : value_create_null(d_type);
            if (val.type != VAL_NULL && val.type != d_type) {
                char buf[128];
                snprintf(buf, 128, "Type '%s' can't be of type '%s'",
                         get_string_from_value_type(val.type),
                         get_string_from_value_type(d_type));
                env_error(node->var_decl.names[i].line, buf);
            }
            env_define(self->env, node->var_decl.names[i].lexeme, val);
        }
        break;

    case AST_ASSIGNMENT:
        // TODO by JOROsh: 
        // check if the datatype is correct with the newly assigned value?
        val = evaluate(self, node->assignment.expr);
        if (!env_assign(self->env, node->assignment.var->lexeme, val)) {

            char buf[128];
            snprintf(buf, 128, "Undefined variable '%s'",
                     node->variable.name->lexeme);
            env_error(node->variable.name->line, buf);
        }
        break;
    case AST_PRINT:
        for (int i = 0; i < node->print.expr_count; i++) {
            ast_node *curr = node->print.exprs[i];
            if (curr->type == AST_LITERAL) {
                if (curr->literal.value->type == DOLLAR) {
                    printf("\n");
                } else {
                    printf("%s", curr->literal.value->lexeme);
                }
                continue;
            }
            value val = evaluate(self, curr);

            switch (val.type) {

            case VAL_NUMERO:
                printf("%ld", val.as.numero);
                break;
            case VAL_LETRA:
                printf("%c", val.as.letra);
                break;
            case VAL_TIPIK:
                printf("%f", val.as.tipik);
                break;
            case VAL_TINUOD:
                printf("%s", val.as.tinuod ? "OO" : "DILI");
                break;
            case VAL_NULL:
                printf("NULL");
                break;
            }
        }
        break;
    case AST_INPUT:
        // 100 characters limit
        char *input_string = arena_alloc(self->arena, 100);
        if (fgets(input_string, 100, stdin) != NULL) {
            // Remove the trailing newline character, if present
            size_t len = strlen(input_string);
            if (len > 0 && input_string[len - 1] == '\n') {
                input_string[len - 1] = '\0';
            }
        }
        if (strlen(input_string) == 0) {
            interp_error(node->input.vars[0].line, "User input has a length of zero. Please input properly!");
        }

        // expecting at least 1 token in an input
        size_t expression_count = 1;

        int token_start = 0;
        int string_len = 0;     // iteration counter and records the strings length after the while loop below
        while (input_string[string_len] != '\0' && expression_count < 10) {
            if (input_string[string_len] == ',') {
                token_start = string_len + 1;    // resets the token_start to point at the next lexeme's starting location
                expression_count++;
            }
            string_len++;
        }
        // printf("string len: %d\n", string_len);
        if (node->input.var_count != expression_count) {
            char error_message[100];
            sprintf(error_message, "User input count does not match with variable count - inputs: %d - variables: %d", expression_count, node->input.var_count);
            env_error(node->input.vars[0].line, error_message);
            break;
        }
        lexer sub_lexer = lexer_create(&(lexer_src) {
            .len = string_len, .data = input_string
        });
        lexer_gen_input_tokens(&sub_lexer);
        parser sub_parser = parser_create(&sub_lexer.tokens, self->arena);
        ast_node **sub_ast_nodes = sub_parser_parse(&sub_parser, expression_count, node->input.vars);
        value val;
        for (int i = 0; i < expression_count; i++) {
            val = evaluate(self, sub_ast_nodes[i]);
            value_type variable_type = env_get_variable_type(self->env, node->input.vars[i].lexeme);
            // printf("Variable %s datatype: %s\n", node->input.vars[i].lexeme, get_string_from_value_type(variable_type));
            if (!(val.type == VAL_LETRA && variable_type == VAL_LETRA) &&
                !(val.type == VAL_TIPIK && variable_type == VAL_TIPIK) &&
                !(val.type == VAL_NUMERO && variable_type == VAL_NUMERO) &&
                !(val.type == VAL_TINUOD && variable_type == VAL_TINUOD)) {
                    char error_message[100];
                    sprintf(error_message, "User input datatype does not match with variable datatype - input: %s - variable \'%s\': %s ", 
                        get_string_from_value_type(val.type),
                        node->input.vars[i].lexeme,
                        get_string_from_value_type(variable_type));
                    env_error(node->input.vars[0].line, error_message);
                    break;
            }
            env_assign(self->env, node->input.vars[i].lexeme, val);
        }

        // token_list *sub_lexer_tokens = &sub_lexer.tokens;
    
        break;

    case AST_IF:
    case AST_ELSE_IF:
        val = evaluate(self, node->if_stmt.condition);
        if (val.type != VAL_TINUOD)
            bpp_error(node->if_stmt.condition->binary.op->line,
                      "Condition must be boolean");
        if (val.as.tinuod) {
            if (node->if_stmt.then_block)
                execute_statement(self, node->if_stmt.then_block);
        } else if (node->if_stmt.else_block) {
            execute_statement(self, node->if_stmt.else_block);
        }
        break;
    case AST_ELSE:
        if (node->if_stmt.then_block)
            execute_statement(self, node->if_stmt.then_block);
        break;
    case AST_BLOCK:
        for (int i = 0; i < node->block.stmt_count; i++) {
            execute_statement(self, &node->block.statements[i]);
        }
        break;
    case AST_FOR:
        if (node->for_stmt.init) {
            execute_statement(self, node->for_stmt.init);
        }

        while (true) {
            val = evaluate(self, node->for_stmt.condition);

            if (!val.as.tinuod) {
                break;
            }

            if (node->for_stmt.body) {
                execute_statement(self, node->for_stmt.body);
            }

            if (node->for_stmt.update) {
                execute_statement(self, node->for_stmt.update);
            }
        }
        break;

    case AST_SWITCH: {
        value switch_val = evaluate(self, node->switch_stmt.expression);
        for (int i = 0; i < node->switch_stmt.case_count; i++) {
            ast_node *case_node = node->switch_stmt.cases[i];
            value case_val = evaluate(self, case_node->case_stmt.value);
            // simple equality check
            if (switch_val.type == case_val.type && (
                (switch_val.type == VAL_NUMERO && switch_val.as.numero == case_val.as.numero) ||
                (switch_val.type == VAL_TIPIK && switch_val.as.tipik == case_val.as.tipik) ||
                (switch_val.type == VAL_LETRA && switch_val.as.letra == case_val.as.letra) ||
                (switch_val.type == VAL_TINUOD && switch_val.as.tinuod == case_val.as.tinuod)
            )) {
                execute_statement(self, case_node->case_stmt.block);
                break;
            }
        }
    } break;

    default:
        break;
    }
}

static value evaluate(interpreter *self, ast_node *node) {
    if (!node) {
        printf("(INTERPRETER: evaluate[1])Created a true null value...\n");
        return value_create_null(VAL_NULL);
    }

    value val, left, right;
    switch (node->type) {
    case AST_LITERAL:

        switch (node->literal.value->type) {

        case NUMBER:
            return value_create_numero(
                *((double *)node->literal.value->literal));
        case CHAR:
            return value_create_letra(*(char *)node->literal.value->literal);
        case TRUE:
            return value_create_tinuod(true);
        case FALSE:
            return value_create_tinuod(false);
        default:
            interp_error(node->literal.value->line, "Invalid type");
        }
    case AST_VARIABLE:
        if (!env_get(self->env, node->variable.name->lexeme, &val)) {
            char buf[128];
            snprintf(buf, 128, "Undefined variable '%s'",
                     node->variable.name->lexeme);
            env_error(node->variable.name->line, buf);
        }
        return val;
    case AST_ASSIGNMENT:
        val = evaluate(self, node->assignment.expr);
        env_assign(self->env, node->assignment.var->lexeme, val);
        return val;
    case AST_UNARY:
        // token *op;
        // ast_node *expr;
        val = evaluate(self, node->unary.expr);
        switch (node->unary.op->type) {
        case MINUS:
            switch (val.type) {
            case VAL_NUMERO:
                return value_create_numero(-val.as.numero);
            case VAL_TIPIK:
                return value_create_tipik(-val.as.tipik);
            default:
                interp_error(node->unary.op->line, "Operand NaN");
            }
            break;
        case DILI:
            switch (val.type) {
            case VAL_TINUOD:
                return value_create_tinuod(!val.as.tinuod);
            default:
                interp_error(node->unary.op->line,
                             "Operand of 'DILI' must be of type 'TINUOD'");
            }
        default:
            break;
        }
        break;
    case AST_BINARY:
        left = evaluate(self, node->binary.left);
        right = evaluate(self, node->binary.right);

        if (left.type == VAL_LETRA || right.type == VAL_LETRA) {
            char buf[128];
            snprintf(buf, 128, "Invalid operand '%c'",
                     left.type == VAL_LETRA ? left.as.letra : right.as.letra);
            interp_error(node->binary.op->line, buf);
            return value_create_null(VAL_NULL);
        }
        value_precedence_convert(&left, &right);

        switch (node->binary.op->type) {
        case PLUS:
            if (left.type == VAL_TIPIK)
                return value_create_tipik(left.as.tipik + right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_numero(left.as.numero + right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod + right.as.tinuod);

        case MINUS:
            if (left.type == VAL_TIPIK)
                return value_create_tipik(left.as.tipik - right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_numero(left.as.numero - right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod - right.as.tinuod);
        case STAR:
            if (left.type == VAL_TIPIK)
                return value_create_tipik(left.as.tipik * right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_numero(left.as.numero * right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod * right.as.tinuod);
        case MODULO:
            if (left.type != VAL_NUMERO)
                interp_error(node->binary.op->line,
                             "Modulo only works with type 'NUMERO'");
            return value_create_numero(left.as.numero % right.as.numero);
        case SLASH:
            if (right.as.numero == 0) {
                interp_error(node->binary.op->line, "Division by zero");
            }
            if (left.type == VAL_TIPIK)
                return value_create_tipik(left.as.tipik / right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_numero(left.as.numero / right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod / right.as.tinuod);
        case EQUAL_EQUAL:
            if (left.type == VAL_TIPIK)
                return value_create_tinuod(left.as.tipik == right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_tinuod(left.as.numero == right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod == right.as.tinuod);
        case NOT_EQUAL:
            if (left.type == VAL_TIPIK)
                return value_create_tinuod(left.as.tipik != right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_tinuod(left.as.numero != right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod != right.as.tinuod);
        case LESS:
            if (left.type == VAL_TIPIK)
                return value_create_tinuod(left.as.tipik < right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_tinuod(left.as.numero < right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod < right.as.tinuod);
        case LESS_EQUAL:
            if (left.type == VAL_TIPIK)
                return value_create_tinuod(left.as.tipik <= right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_tinuod(left.as.numero <= right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod <= right.as.tinuod);
        case GREATER:
            if (left.type == VAL_TIPIK)
                return value_create_tinuod(left.as.tipik > right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_tinuod(left.as.numero > right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod > right.as.tinuod); 
        case GREATER_EQUAL:
            if (left.type == VAL_TIPIK)
                return value_create_tinuod(left.as.tipik >= right.as.tipik);
            else if (left.type == VAL_NUMERO)
                return value_create_tinuod(left.as.numero >= right.as.numero);
            else if (left.type == VAL_TINUOD)
                return value_create_tinuod(left.as.tinuod >= right.as.tinuod);
        case UG:
            // other dataytypes might be allowed to be UG but idk
            if (left.type != VAL_TINUOD) {
                interp_error(node->binary.op->line, "UG operation only works with TINUODs");
            }
            return value_create_tinuod(left.as.tinuod && right.as.tinuod);
        case O:
            if (left.type != VAL_TINUOD) {
                interp_error(node->binary.op->line, "O operation only works with TINUODs");
            }
            return value_create_tinuod(left.as.tinuod || right.as.tinuod);
        default:
            break;
        }
        break;
    default:
        break;
    }

    printf("(INTERPRETER: evaluate[2])Created a true null value...\n");
    return value_create_null(VAL_NULL);
}
