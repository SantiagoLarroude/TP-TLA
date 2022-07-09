#ifndef MEM_MANAGEMENT_H
#define MEM_MANAGEMENT_H

#include "../frontend/syntactic-analysis/node.h"

void free_program(program_t *root_node);

void free_node_function_call(node_function_call *fun_call);
void free_main_function(program_t *main_function);
void free_node_function(node_function *function);
void free_node_list(node_list *args);
void free_node_expression_list(node_expression_list *expression_list);
void free_node_list(node_list *args);
void free_variable(variable *variable);
void free_node_expression(node_expression *exprs);
void free_node_file_block(node_file_block *file_handler);
void free_node_loop(node_loop *loop_expr);

#endif /* MEM_MANAGEMENT_H */
