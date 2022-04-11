#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../backend/shared.h"

#include "node.h"                                       /* node_* types */

/**
 * Se definen las acciones a ejecutar sobre cada regla de producción de la
 * gramática. El objetivo de cada acción debe ser el de construir el nodo
 * adecuado que almacene la información requerida en el árbol de sintaxis
 * abstracta (i.e., el AST).
 */

 /* Program */
void grammar_program(const node_expression* value);


/* Expressions */
node_expression* grammar_expression_logic(const token_t operator,
                                          const node_expression* lvalue,
                                          const node_expression* rvalue);

node_expression* grammar_expression_comparison(const token_t operator,
                                               const node_expression* lvalue,
                                               const node_expression* rvalue);

node_expression*
grammar_expression_arithmetic_numeric(const token_t operator,
                                      const node_expression* lvalue,
                                      const node_expression* rvalue);

node_expression*
grammar_expression_arithmetic_string(const token_t operator,
                                     const node_expression* lvalue,
                                     const node_expression* rvalue);

/* Constant */
node_expression* grammar_constant_number(const char* value);
node_expression* grammar_constant_string(const char* value);

void free_programblock(struct node_block* program);

#endif
