#include <stdbool.h>            /* bool type */
#include <stdlib.h>             /* malloc, calloc */
#include <string.h>             /* strlen, strcpy */

/* Backend */
#include "../../backend/logger.h"

/* Frontend */
#include "../lexical-analysis/flex-actions.h"   /* enum TokenID */

/* This file */
#include "bison-actions.h"


/* Macros and constants */
// Debug

// Log function call with prototype: function(char* val)
#define log_debug_single_value(val)                                         \
        LogDebug("%s(%s)\n\tFile: %s", __func__, val, __FILE__);

// Log function call with prototype: function(int op, char* lv, char* rv)
#define log_debug_operator_lvalue_rvalue(op_pretty_name, op, lv, rv)        \
        LogDebug("{%s} %s(%d, %s, %s)\n\tFile: %s",                         \
                op_pretty_name, __func__, op, lv, rv, __FILE__);


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
        LogDebug("grammar_program(%s)", value->value);

        state.succeed = true;
        state.result = 5;

        programblock = malloc(sizeof(node_block));
        if (programblock == NULL) {
                log_error_no_mem();
                return;
        }

        programblock->exp = (node_expression*) value;

        return;
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



node_expression*
grammar_constant_number(const char* value)
{
        log_debug_single_value(value);

        node_expression* exp = malloc(sizeof(node_expression));
        if (exp == NULL) {
                log_error_no_mem();
                return NULL;
        }

        if (const_to_expr_cpy(value, exp) == false) {
                return NULL;
        }

        exp->type = NUMBER;

        return exp;
}

node_expression*
grammar_constant_string(const char* value)
{
        log_debug_single_value(value);

        node_expression* exp = malloc(sizeof(node_expression));
        if (exp == NULL) {
                log_error_no_mem();
                return NULL;
        }

        if (const_to_expr_cpy(value, exp) == false) {
                return NULL;
        }

        exp->type = NUMBER;
        return exp;
}

void
free_programblock(struct node_block* program)
{
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