#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../backend/shared.h"

#include "node.h" /* node_*; variable; token_t */

/**
 * Se definen las acciones a ejecutar sobre cada regla de producción de la
 * gramática. El objetivo de cada acción debe ser el de construir el nodo
 * adecuado que almacene la información requerida en el árbol de sintaxis
 * abstracta (i.e., el AST).
 */

 /* Program */
program_t* new_program();

int grammar_program(program_t* root_node, node_function* fun);

/* Functions */
node_function*
grammar_new_function(const char* name,
                     const node_list* args,
                     const node_expression_list* list,
                     const variable* return_value);

node_expression_list*
grammar_new_expression_list(const node_expression* expr);

node_expression_list*
grammar_concat_expressions(node_expression_list* list,
                           const node_expression* expr);

node_function_call* grammar_function_call(const char* fun_id,
                                          const node_list* args);
node_function_call*
grammar_concat_function_call(const node_function_call* fun_list,
                             const node_function_call* fun_append);

node_function_call*
grammar_function_call_from_id(const char* id,
                              const node_function_call* fun);

node_expression*
grammar_expression_from_funcall(const node_function_call* fn_calls);

variable* grammar_new_return_node(const char* id);

/* Assignment */
node_expression*
grammar_expression_from_assignment(const node_expression* assignment);

node_expression*
grammar_new_assignment_expression(const node_expression* expr,
                                  const char* id);

node_expression*
grammar_new_assignment_from_id(const char* from, const char* to);

node_expression*
grammar_new_assignment_identifier(const node_expression* from_id,
                                  const node_expression* to_id);

/* File */
node_expression*
grammar_expression_from_filehandler(const node_file_block* fhandler);

node_expression*
grammar_concat_expressions_filehandler(const node_expression* exprs,
                                       const node_file_block* fhandler);

node_expression* grammar_new_declaration_file_node(const char* fpath,
                                                   const char* id,
                                                   const node_list* separators);

node_expression*
grammar_new_declaration_stdout_node(const char* id,
                                    const node_list* separators);

node_file_block* grammar_using_file(const char* id,
                                    const node_expression_list* expr);

/* Variable declaration */
node_expression* grammar_new_variable(const char* id,
                                      const node_expression* expr);

/* Conditional */
node_expression* grammar_new_conditional(const node_expression* condition,
                                         const node_expression* expr_true,
                                         const node_expression* expr_false);

/* Loop */
node_expression* grammar_new_loop(const char* id,
                                  const node_expression* iterable,
                                  const node_expression* action);

/* List */
node_expression* grammar_expression_from_list(const node_list* list);

node_list* grammar_new_list(const node_expression* expr);

node_list* grammar_concat_list_expr(node_list* list,
                                    const node_expression* expr);

node_list* grammar_concat_list_list(node_list* head_list,
                                    const node_list* tail_list);

node_list* grammar_new_list_from_range(const char* start,
                                       const char* end);

node_list* grammar_new_list_args(const node_expression* expr);

node_list* grammar_concat_list_args(node_list* list,
                                    const node_expression* expr);

/* Arithmetic */
// Numeric
node_expression*
grammar_expression_arithmetic_num_add(const node_expression* lvalue,
                                      const node_expression* rvalue);
node_expression*
grammar_expression_arithmetic_num_sub(const node_expression* lvalue,
                                      const node_expression* rvalue);
node_expression*
grammar_expression_arithmetic_num_mul(const node_expression* lvalue,
                                      const node_expression* rvalue);
node_expression*
grammar_expression_arithmetic_num_div(const node_expression* lvalue,
                                      const node_expression* rvalue);
node_expression*
grammar_expression_arithmetic_num_mod(const node_expression* lvalue,
                                      const node_expression* rvalue);

// String
node_expression*
grammar_expression_arithmetic_str_add(const node_expression* lvalue,
                                      const node_expression* rvalue);
node_expression*
grammar_expression_arithmetic_str_sub(const node_expression* lvalue,
                                      const node_expression* rvalue);

/* Bool */
node_expression* grammar_expression_bool_and(const node_expression* lvalue,
                                             const node_expression* rvalue);

node_expression* grammar_expression_bool_or(const node_expression* lvalue,
                                            const node_expression* rvalue);

node_expression* grammar_expression_bool_not(const node_expression* value);

node_expression* grammar_expression_cmp_equals(const node_expression* lvalue,
                                               const node_expression* rvalue);

// Compare
node_expression*
grammar_expression_cmp_not_equals(const node_expression* lvalue,
                                  const node_expression* rvalue);
node_expression*
grammar_expression_cmp_greater_than(const node_expression* lvalue,
                                    const node_expression* rvalue);
node_expression*
grammar_expression_cmp_greater_equal(const node_expression* lvalue,
                                     const node_expression* rvalue);
node_expression*
grammar_expression_cmp_less_than(const node_expression* lvalue,
                                 const node_expression* rvalue);
node_expression*
grammar_expression_cmp_less_equal(const node_expression* lvalue,
                                  const node_expression* rvalue);

node_expression* grammar_expression_cmp_by_type(const char* id,
                                                const token_t type);

/* Constant */
node_expression* grammar_constant_number(const char* rval);
node_expression* grammar_constant_string(const char* rval);
node_expression* grammar_constant_bool(const char* rval);

// void free_programblock(struct node_block* program);

#endif
