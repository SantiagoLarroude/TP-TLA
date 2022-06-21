#include <stdbool.h> /* bool type */
#include <stdlib.h> /* malloc, calloc */
#include <string.h> /* strlen, strcpy */

/* Backend */
#include "../../backend/logger.h" /* LogDebug */

/* Frontend */
#include "../lexical-analysis/flex-actions.h" /* enum TokenID; free_yylval */

/* This file */
#include "bison-actions.h"

/* Prototypes */
// static bool const_to_expr_cp(const char* value,
//                              node_expression* expression);

/**
 * Implementación de "bison-grammar.h".
 */

void yyerror(const char *string)
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


grammar_expression_from_funcall() {

}



constant_node *
grammar_new_return_node(const char *id) {
        LogDebug("%s(%s)\n", __func__, id);
        
        constant_node * node = malloc(sizeof(constant_node));
        if (id == NULL) {
                node->my_id = NULL;
                node->constant_node_type = RETURN_NULL_CONSTANT_NODE_TYPE;
        } else {
                node->my_id = malloc(strlen(id)+1);
                strcpy(node->my_id, id);
                node->constant_node_type = RETURN_CONSTANT_NODE_TYPE;
        }
        node->value = NULL;
        
        return node;
}

node_expression*
grammar_new_assignment_expression(const node_expression *expr,
                                  const char *id) {
        
        LogDebug("%s(%p, %s)\n", __func__, expr, id);

        node_expression * expression = malloc(sizeof(node_expression));
        expression = expr;
        expression->my_id = malloc(strlen(id)+1);
        strcpy(expression->my_id, id);

        return expression;
}

char*
grammar_identifier(const char * id) {
        
        LogDebug("%s(%s)\n", __func__, id);

        return id;
}

constant_node*
grammar_constant_bool(const variable_value *value) {

        LogDebug("%s(%s)\n", __func__, value);

        constant_node* node = malloc(sizeof(constant_node));
        node->value = value;
        node->my_id = NULL;
        node->constant_node_type = BOOLEAN_CONSTANT_NODE_TYPE;

        return node;
}

constant_node*
grammar_constant_number(const variable_value *value) {
        
        LogDebug("%s(%s)\n", __func__, value);

        constant_node* node = malloc(sizeof(constant_node));
        node->value = value;
        node->my_id = NULL;
        node->constant_node_type = NUMBER_CONSTANT_NODE_TYPE;

        return node;

}

constant_node *
grammar_constant_string(const variable_value *value) {
        
        LogDebug("%s(%s)\n", __func__, value);

        constant_node* node = malloc(sizeof(constant_node));
        node->value = value;
        node->my_id = NULL;
        node->constant_node_type = STRING_CONSTANT_NODE_TYPE;

        return node;
}