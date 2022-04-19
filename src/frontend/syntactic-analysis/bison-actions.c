#include <stdbool.h>            /* bool type */
#include <stdlib.h>             /* malloc, calloc */
#include <string.h>             /* strlen, strcpy */

/* Backend */
#include "../../backend/logger.h"       /* LogDebug */

/* Frontend */
#include "../lexical-analysis/flex-actions.h"   /* enum TokenID; free_yylval */

/* This file */
#include "bison-actions.h"


/* Macros and constants */
// Debug

// Return "NULL" string when value is NULL. Ideal to use with log_* macros
#define gen_null_str(value) ((value == NULL) ? "NULL" : value)

// Log function call with prototype: function(char* val)
#define log_debug_single_arg_char(val)                                      \
        LogDebug("%s(%s)\n\tFile: %s", __func__, gen_null_str(val), __FILE__);

// Log function call with prototype: function(any* val)
#define log_debug_single_arg(type, ptr)                                     \
        LogDebug("%s(%s: %p)\n\tFile: %s", __func__, type, ptr, __FILE__);

// Log function call with prototype: function(any* val, any* val)
#define log_debug_dual_arg(type1, ptr1, type2, ptr2)                        \
        LogDebug("%s(%s: %p, %s: %p)\n\tFile: %s",                          \
        __func__, type1, (void*) ptr1, type2, (void*) ptr2, __FILE__);

// Log function call with prototype: function(int op, char* lv, char* rv)
#define log_debug_operator_lvalue_rvalue(op_pretty_name, op, lv, rv)        \
        LogDebug("{%s} %s(%d, %s, %s)\n\tFile: %s",                         \
                op_pretty_name, __func__, op, gen_null_str(lv),             \
                gen_null_str(rv), __FILE__);

/* Prototypes */
static bool const_to_expr_cpy(const char* value, node_expression* expression);


/**
 * Implementación de "bison-grammar.h".
 */

void yyerror(const char* string)
{
        const int length = strlen(yytext);

        LogError("Mensaje: '%s' debido a '%s' (linea %d).", string, yytext,
                 yylineno);
        LogError("Mensaje: '%s.", string);
        LogError("En ASCII es:");
        LogErrorRaw("\t");

        for (int i = 0; i < length; ++i) {
                LogErrorRaw("[%d]", yytext[i]);
        }

        LogErrorRaw("\n\n");
}


void
grammar_program(const node_expression* value)
{
        // TODO: Implement

        log_debug_single_arg("const node_expression*", &value);

        state.succeed = true;
        state.result = 5;

        /* programblock = malloc(sizeof(node_block));
        if (programblock == NULL) {
                log_error_no_mem();
                return;
        }

        programblock->exp = (node_expression*) value; */

        return;
}

node_expression* grammar_expression_assignment(const node_expression* lvalue,
                                               const node_identifier* id)
{
        // TODO: Implement.

        log_debug_dual_arg("node_expression", &lvalue, "node_identifier", &id);

        return (node_expression*) lvalue; // TODO: Change when implemented
}

node_expression*
grammar_expression_assignment_list(const node_expression* lvalue,
                                   const node_expression* list)
{
        // TODO: Implement.

        log_debug_dual_arg("node_expression", &lvalue,
                           "node_expression", &list);

        return (node_expression*) lvalue; // TODO: Change when implemented
}


node_expression* grammar_expression_logic(const token_t operator,
                                          const node_expression* lvalue,
                                          const node_expression* rvalue)
{
        // TODO: Implement

        switch (operator) {
        case AND:
                log_debug_operator_lvalue_rvalue("AND", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case OR:
                log_debug_operator_lvalue_rvalue("OR", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case NOT:
                log_debug_operator_lvalue_rvalue("NOT", operator,
                                                 lvalue->value, rvalue->value);
                break;
        default:
                log_debug_operator_lvalue_rvalue("Invalid", operator,
                                                 lvalue->value, rvalue->value);
                break;
        }
        return (node_expression*) lvalue; // TODO: Change when implemented
}

node_expression* grammar_expression_comparison(const token_t operator,
                                               const node_expression* lvalue,
                                               const node_expression* rvalue)
{
        // TODO: Implement

        switch (operator) {
        case EQUALS:
                log_debug_operator_lvalue_rvalue("EQUALS", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case NOT_EQUALS:
                log_debug_operator_lvalue_rvalue("NOT_EQUALS", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case GREATER_THAN:
                log_debug_operator_lvalue_rvalue("GREATER_THAN", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case GREATER_EQUAL:
                log_debug_operator_lvalue_rvalue("GREATER_EQUAL", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case LESS_THAN:
                log_debug_operator_lvalue_rvalue("LESS_THAN", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case LESS_EQUAL:
                log_debug_operator_lvalue_rvalue("LESS_EQUAL", operator,
                                                 lvalue->value, rvalue->value);
                break;
        default:
                log_debug_operator_lvalue_rvalue("Invalid", operator,
                                                 lvalue->value, rvalue->value);
                break;
        }
        return (node_expression*) lvalue; // TODO: Change when implemented
}

node_expression*
grammar_expression_arithmetic_numeric(const token_t operator,
                                      const node_expression* lvalue,
                                      const node_expression* rvalue)
{
        // TODO: Implement

        switch (operator) {
        case ADD:
                log_debug_operator_lvalue_rvalue("ADD", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case SUB:
                log_debug_operator_lvalue_rvalue("SUB", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case MUL:
                log_debug_operator_lvalue_rvalue("MUL", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case DIV:
                log_debug_operator_lvalue_rvalue("DIV", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case MOD:
                log_debug_operator_lvalue_rvalue("MOD", operator,
                                                 lvalue->value, rvalue->value);
                break;
        default:
                log_debug_operator_lvalue_rvalue("Invalid", operator,
                                                 lvalue->value, rvalue->value);
                break;
        }

        return (node_expression*) lvalue; // TODO: Change when implemented
}

node_expression*
grammar_expression_arithmetic_string(const token_t operator,
                                     const node_expression* lvalue,
                                     const node_expression* rvalue)
{
        // TODO: Implement

        switch (operator) {
        case STR_ADD:
                log_debug_operator_lvalue_rvalue("STR_ADD", operator,
                                                 lvalue->value, rvalue->value);
                break;
        case STR_SUB:
                log_debug_operator_lvalue_rvalue("STR_SUB", operator,
                                                 lvalue->value, rvalue->value);
                break;
        default:
                log_debug_operator_lvalue_rvalue("Invalid", operator,
                                                 lvalue->value, rvalue->value);
                break;
        }

        return (node_expression*) lvalue; // TODO: Change when implemented
}

node_expression* grammar_expression_list_new(const node_expression* expression)
{
        // TODO: Implement
        log_debug_single_arg("node_expression", expression);

        return (node_expression*) expression; // TODO: Change when implemented
}

node_expression* grammar_expression_list_append(const node_expression* list,
                                                const node_expression* expression)
{
        // TODO: Implement

        log_debug_dual_arg("node_expression", &list,
                           "node_expression", &expression);

        return (node_expression*) expression; // TODO: Change when implemented
}

node_expression*
grammar_identifier_list_new(const char* id)
{
        // TODO: Implement

        log_debug_single_arg("char", id);

        return NULL; // TODO: Change when implemented
}

node_expression*
grammar_identifier_list_append(const node_expression* list,
                               const char* id)
{
        // TODO: Implement

        log_debug_dual_arg("node_expression", &list,
                           "char", &id);

        return NULL; // TODO: Change when implemented
}

node_identifier* grammar_identifier(const char* id)
{
        // TODO: Implement
        log_debug_single_arg_char(id);

        /* node_identifier* nid = malloc(sizeof(node_identifier));
        if (nid == NULL) {
                log_error_no_mem();
                return NULL;
        }

        size_t len = strlen(id);
        nid->name = calloc(sizeof(char), len);
        if (nid->name == NULL) {
                log_error_no_mem();
                return NULL;
        }

        nid->length = len;
        strcpy(nid->name, id);

        return nid; */
        return NULL;
}

node_expression*
grammar_constant_number(const char* value)
{
        // TODO: Implement

        log_debug_single_arg_char(value);

        /* node_expression* exp = malloc(sizeof(node_expression));
        if (exp == NULL) {
                log_error_no_mem();
                return NULL;
        }

        if (const_to_expr_cpy(value, exp) == false) {
                return NULL;
        }

        exp->type = NUMBER;

        return exp; */
        return NULL;
}

node_expression*
grammar_constant_string(const char* value)
{
        // TODO: Implement

        log_debug_single_arg_char(value);

        /* node_expression* exp = malloc(sizeof(node_expression));
        if (exp == NULL) {
                log_error_no_mem();
                return NULL;
        }

        if (const_to_expr_cpy(value, exp) == false) {
                return NULL;
        }

        exp->type = NUMBER;
        return exp; */
        return NULL;
}

void
free_programblock(struct node_block* program)
{
        free_yylval();

        if (program == NULL)
                return;

        free(program->exp);
        free(program);
}

static bool const_to_expr_cpy(const char* value, node_expression* expression)
{
        size_t len = strlen(value);

        expression->value = calloc(sizeof(char), len);
        if (expression->value == NULL) {
                log_error_no_mem();
                return false;
        }

        expression->length = len;
        strcpy(expression->value, value);
        return true;
}